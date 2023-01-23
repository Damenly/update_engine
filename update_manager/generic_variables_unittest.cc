//
// Copyright (C) 2014 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "update_engine/update_manager/generic_variables.h"

#include <memory>

#include <base/functional/callback.h>
#include <brillo/message_loops/fake_message_loop.h>
#include <brillo/message_loops/message_loop.h>
#include <brillo/message_loops/message_loop_utils.h>
#include <gtest/gtest.h>

#include "update_engine/update_manager/umtest_utils.h"

using brillo::MessageLoop;
using brillo::MessageLoopRunMaxIterations;
using std::unique_ptr;

namespace chromeos_update_manager {

class UmPollCopyVariableTest : public ::testing::Test {};

TEST_F(UmPollCopyVariableTest, SimpleTest) {
  // Tests that copies are generated as intended.
  int source = 5;
  PollCopyVariable<int> var("var", source);

  // Generate and validate a copy.
  unique_ptr<const int> copy_1(
      var.GetValue(UmTestUtils::DefaultTimeout(), nullptr));
  ASSERT_NE(nullptr, copy_1.get());
  EXPECT_EQ(5, *copy_1);

  // Assign a different value to the source variable.
  source = 42;

  // Check that the content of the copy was not affected (distinct instance).
  EXPECT_EQ(5, *copy_1);

  // Generate and validate a second copy.
  UmTestUtils::ExpectVariableHasValue(42, &var);
}

TEST_F(UmPollCopyVariableTest, SetFlagTest) {
  // Tests that the set flag is being referred to as expected.
  int source = 5;
  bool is_set = false;
  PollCopyVariable<int> var("var", source, &is_set);

  // Flag marked unset, nothing should be returned.
  UmTestUtils::ExpectVariableNotSet(&var);

  // Flag marked set, we should be getting a value.
  is_set = true;
  UmTestUtils::ExpectVariableHasValue(5, &var);
}

class CopyConstructorTestClass {
 public:
  CopyConstructorTestClass(void) : copied_(false) {}
  CopyConstructorTestClass(const CopyConstructorTestClass& other)
      : copied_(true), val_(other.val_ * 2) {}

  // Tells if the instance was constructed using the copy-constructor.
  const bool copied_;

  // An auxiliary internal value.
  int val_ = 0;
};

TEST_F(UmPollCopyVariableTest, UseCopyConstructorTest) {
  // Ensures that CopyVariables indeed uses the copy constructor.
  const CopyConstructorTestClass source;
  ASSERT_FALSE(source.copied_);

  PollCopyVariable<CopyConstructorTestClass> var("var", source);
  unique_ptr<const CopyConstructorTestClass> copy(
      var.GetValue(UmTestUtils::DefaultTimeout(), nullptr));
  ASSERT_NE(nullptr, copy.get());
  EXPECT_TRUE(copy->copied_);
}

class UmConstCopyVariableTest : public ::testing::Test {};

TEST_F(UmConstCopyVariableTest, SimpleTest) {
  int source = 5;
  ConstCopyVariable<int> var("var", source);
  UmTestUtils::ExpectVariableHasValue(5, &var);

  // Ensure the value is cached.
  source = 42;
  UmTestUtils::ExpectVariableHasValue(5, &var);
}

class UmCallCopyVariableTest : public ::testing::Test {};

CopyConstructorTestClass test_func(CopyConstructorTestClass* obj) {
  obj->val_++;  // So we can check that the function was called.
  return *obj;
}

TEST_F(UmCallCopyVariableTest, SimpleTest) {
  // Tests that the returned value is generated by copying the value returned by
  // the function call.

  CopyConstructorTestClass test_obj;
  ASSERT_FALSE(test_obj.copied_);
  test_obj.val_ = 5;

  base::Callback<CopyConstructorTestClass(void)> cb =
      base::Bind(test_func, &test_obj);
  CallCopyVariable<CopyConstructorTestClass> var("var", cb);

  unique_ptr<const CopyConstructorTestClass> copy(
      var.GetValue(UmTestUtils::DefaultTimeout(), nullptr));
  EXPECT_EQ(6, test_obj.val_);  // Check that the function was called.
  ASSERT_NE(nullptr, copy.get());
  EXPECT_TRUE(copy->copied_);
  EXPECT_EQ(12, copy->val_);  // Check that copying occurred once.
}

TEST_F(UmCallCopyVariableTest, NullTest) {
  // Ensures that the variable returns null when the callback is null.

  base::Callback<bool(void)> cb;
  CallCopyVariable<bool> var("var", cb);
  UmTestUtils::ExpectVariableNotSet(&var);
}

class UmAsyncCopyVariableTest : public ::testing::Test {
 protected:
  void SetUp() override { loop_.SetAsCurrent(); }

  void TearDown() override {
    // No remaining event on the main loop.
    EXPECT_FALSE(loop_.PendingTasks());
  }

  brillo::FakeMessageLoop loop_{nullptr};
};

TEST_F(UmAsyncCopyVariableTest, ConstructorTest) {
  AsyncCopyVariable<int> var("var");
  UmTestUtils::ExpectVariableNotSet(&var);
  EXPECT_EQ(kVariableModeAsync, var.GetMode());
}

TEST_F(UmAsyncCopyVariableTest, SetValueTest) {
  AsyncCopyVariable<int> var("var");
  var.SetValue(5);
  UmTestUtils::ExpectVariableHasValue(5, &var);
  // Execute all the pending observers.
  MessageLoopRunMaxIterations(MessageLoop::current(), 100);
}

TEST_F(UmAsyncCopyVariableTest, UnsetValueTest) {
  AsyncCopyVariable<int> var("var", 42);
  var.UnsetValue();
  UmTestUtils::ExpectVariableNotSet(&var);
  // Execute all the pending observers.
  MessageLoopRunMaxIterations(MessageLoop::current(), 100);
}

class CallCounterObserver : public BaseVariable::ObserverInterface {
 public:
  void ValueChanged(BaseVariable* variable) { calls_count_++; }

  int calls_count_ = 0;
};

TEST_F(UmAsyncCopyVariableTest, ObserverCalledTest) {
  AsyncCopyVariable<int> var("var", 42);
  CallCounterObserver observer;
  var.AddObserver(&observer);
  EXPECT_EQ(0, observer.calls_count_);

  // Check that a different value fires the notification.
  var.SetValue(5);
  MessageLoopRunMaxIterations(MessageLoop::current(), 100);
  EXPECT_EQ(1, observer.calls_count_);

  // Check the same value doesn't.
  var.SetValue(5);
  MessageLoopRunMaxIterations(MessageLoop::current(), 100);
  EXPECT_EQ(1, observer.calls_count_);

  // Check that unsetting a previously set value fires the notification.
  var.UnsetValue();
  MessageLoopRunMaxIterations(MessageLoop::current(), 100);
  EXPECT_EQ(2, observer.calls_count_);

  // Check that unsetting again doesn't.
  var.UnsetValue();
  MessageLoopRunMaxIterations(MessageLoop::current(), 100);
  EXPECT_EQ(2, observer.calls_count_);

  var.RemoveObserver(&observer);
}

}  // namespace chromeos_update_manager
