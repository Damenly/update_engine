//
// Copyright 2021 The Android Open Source Project
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

#include "update_engine/cros/fake_system_state.h"
#include "update_engine/update_manager/omaha_request_params_policy.h"
#include "update_engine/update_manager/policy_test_utils.h"

using chromeos_update_engine::FakeSystemState;

namespace chromeos_update_manager {

class UmOmahaRequestParamsPolicyTest : public UmPolicyTestBase {
 protected:
  UmOmahaRequestParamsPolicyTest() : UmPolicyTestBase() {
    policy_data_.reset();
    policy_2_.reset(new OmahaRequestParamsPolicy());
  }

  void SetUp() override {
    UmPolicyTestBase::SetUp();
    fake_state_.device_policy_provider()->var_device_policy_is_loaded()->reset(
        new bool(true));
  }
};

TEST_F(UmOmahaRequestParamsPolicyTest, PolicyIsLoaded) {
  EXPECT_EQ(EvalStatus::kSucceeded, evaluator_->Evaluate());
}

TEST_F(UmOmahaRequestParamsPolicyTest, DefaultMarketSegment) {
  EXPECT_EQ(EvalStatus::kSucceeded, evaluator_->Evaluate());
  EXPECT_EQ(FakeSystemState::Get()->request_params()->market_segment(),
            "consumer");
}

TEST_F(UmOmahaRequestParamsPolicyTest, FooMarketSegment) {
  fake_state_.device_policy_provider()->var_market_segment()->reset(
      new std::string("foo-segment"));

  EXPECT_EQ(EvalStatus::kSucceeded, evaluator_->Evaluate());
  EXPECT_EQ(FakeSystemState::Get()->request_params()->market_segment(),
            "foo-segment");
}

TEST_F(UmOmahaRequestParamsPolicyTest, MarketSegmentDisabledNopolicy) {
  fake_state_.device_policy_provider()->var_device_policy_is_loaded()->reset(
      new bool(false));
  fake_state_.updater_provider()->var_market_segment_disabled()->reset(
      new bool(true));
  fake_state_.device_policy_provider()->var_market_segment()->reset(
      new std::string("foo-segment"));

  EXPECT_EQ(EvalStatus::kContinue, evaluator_->Evaluate());
  EXPECT_EQ(FakeSystemState::Get()->request_params()->market_segment(), "");
}

TEST_F(UmOmahaRequestParamsPolicyTest, MarketSegmentDisabledWithPolicy) {
  fake_state_.updater_provider()->var_market_segment_disabled()->reset(
      new bool(true));
  fake_state_.device_policy_provider()->var_market_segment()->reset(
      new std::string("foo-segment"));

  EXPECT_EQ(EvalStatus::kSucceeded, evaluator_->Evaluate());
  EXPECT_EQ(FakeSystemState::Get()->request_params()->market_segment(), "");
}

TEST_F(UmOmahaRequestParamsPolicyTest, QuickFixBuildToken) {
  constexpr char kToken[] = "token";
  fake_state_.device_policy_provider()->var_quick_fix_build_token()->reset(
      new std::string(kToken));
  EXPECT_EQ(EvalStatus::kSucceeded, evaluator_->Evaluate());
  EXPECT_EQ(FakeSystemState::Get()->request_params()->quick_fix_build_token(),
            kToken);
}

TEST_F(UmOmahaRequestParamsPolicyTest, ReleaseLtsTag) {
  constexpr char kReleaseTag[] = "foo-release-tag";
  fake_state_.device_policy_provider()->var_release_lts_tag()->reset(
      new std::string(kReleaseTag));
  EXPECT_EQ(EvalStatus::kSucceeded, evaluator_->Evaluate());
  EXPECT_EQ(FakeSystemState::Get()->request_params()->release_lts_tag(),
            kReleaseTag);
}

}  // namespace chromeos_update_manager
