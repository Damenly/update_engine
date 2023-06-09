//
// Copyright (C) 2022 The Android Open Source Project
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

#include "update_engine/cros/install_action.h"

#include <utility>
#include <vector>

#include <base/files/file_util.h>
#include <base/strings/stringprintf.h>
#include <brillo/message_loops/fake_message_loop.h>
#include <gtest/gtest.h>

#include "update_engine/common/action_processor.h"
#include "update_engine/common/mock_http_fetcher.h"
#include "update_engine/common/test_utils.h"
#include "update_engine/cros/fake_system_state.h"

namespace chromeos_update_engine {

namespace {
constexpr char kDefaultOffset[] = "1024";
constexpr char kDefaultSha[] =
    "5f70bf18a086007016e948b04aed3b82103a36bea41755b6cddfaf10ace3c6ef";

constexpr char kManifestTemplate[] =
    R"({
  "critical-update": false,
  "days-to-purge": 5,
  "description": "A FOOBAR DLC",
  "factory-install": false,
  "fs-type": "squashfs",
  "id": "sample-dlc",
  "image-sha256-hash": "%s",
  "image-type": "dlc",
  "is-removable": true,
  "loadpin-verity-digest": false,
  "manifest-version": 1,
  "mount-file-required": false,
  "name": "Sample DLC",
  "package": "package",
  "pre-allocated-size": "4194304",
  "preload-allowed": true,
  "reserved": false,
  "size": "%s",
  "table-sha256-hash": )"
    R"("44a4e688209bda4e06fd41aadc85a51de7d74a641275cb63b7caead96a9b03b7",
  "used-by": "system",
  "version": "1.0.0-r1"
})";
constexpr char kProperties[] = R"(
CHROMEOS_RELEASE_APPID={DEB6CEFD-4EEE-462F-AC21-52DF1E17B52F}
CHROMEOS_BOARD_APPID={DEB6CEFD-4EEE-462F-AC21-52DF1E17B52F}
CHROMEOS_CANARY_APPID={90F229CE-83E2-4FAF-8479-E368A34938B1}
DEVICETYPE=CHROMEBOOK
CHROMEOS_RELEASE_NAME=Chrome OS
CHROMEOS_AUSERVER=https://tools.google.com/service/update2
CHROMEOS_DEVSERVER=
CHROMEOS_ARC_VERSION=9196679
CHROMEOS_ARC_ANDROID_SDK_VERSION=30
CHROMEOS_RELEASE_BUILDER_PATH=brya-release/R109-15201.0.0
CHROMEOS_RELEASE_KEYSET=devkeys
CHROMEOS_RELEASE_TRACK=testimage-channel
CHROMEOS_RELEASE_BUILD_TYPE=Official Build
CHROMEOS_RELEASE_DESCRIPTION=15201.0.0 (Official Build) dev-channel brya test
CHROMEOS_RELEASE_BOARD=brya
CHROMEOS_RELEASE_BRANCH_NUMBER=0
CHROMEOS_RELEASE_BUILD_NUMBER=15201
CHROMEOS_RELEASE_CHROME_MILESTONE=109
CHROMEOS_RELEASE_PATCH_NUMBER=0
CHROMEOS_RELEASE_VERSION=15201.0.0
GOOGLE_RELEASE=15201.0.0
CHROMEOS_RELEASE_UNIBUILD=1
)";

class InstallActionTestProcessorDelegate : public ActionProcessorDelegate {
 public:
  InstallActionTestProcessorDelegate() : expected_code_(ErrorCode::kSuccess) {}
  ~InstallActionTestProcessorDelegate() override = default;

  void ProcessingDone(const ActionProcessor* processor,
                      ErrorCode code) override {
    brillo::MessageLoop::current()->BreakLoop();
  }

  void ActionCompleted(ActionProcessor* processor,
                       AbstractAction* action,
                       ErrorCode code) override {
    EXPECT_EQ(InstallAction::StaticType(), action->Type());
    EXPECT_EQ(expected_code_, code);
  }

  ErrorCode expected_code_{ErrorCode::kSuccess};
};
}  // namespace

class InstallActionTest : public ::testing::Test {
 protected:
  InstallActionTest() : data_(1024) {}
  ~InstallActionTest() override = default;

  void SetUp() override {
    loop_.SetAsCurrent();

    ASSERT_TRUE(tempdir_.CreateUniqueTempDir());
    EXPECT_TRUE(base::CreateDirectory(tempdir_.GetPath().Append("etc")));
    EXPECT_TRUE(base::CreateDirectory(
        tempdir_.GetPath().Append("dlc/foobar-dlc/package")));
    test::SetImagePropertiesRootPrefix(tempdir_.GetPath().value().c_str());
    FakeSystemState::CreateInstance();

    auto http_fetcher =
        std::make_unique<MockHttpFetcher>(data_.data(), data_.size(), nullptr);
    install_action_ = std::make_unique<InstallAction>(
        std::move(http_fetcher),
        "foobar-dlc",
        /*slotting=*/"",
        /*manifest_dir=*/tempdir_.GetPath().Append("dlc").value());
  }

  base::ScopedTempDir tempdir_;

  brillo::Blob data_;
  std::unique_ptr<InstallAction> install_action_;

  InstallActionTestProcessorDelegate delegate_;

  ActionProcessor processor_;
  brillo::FakeMessageLoop loop_{nullptr};
};

TEST_F(InstallActionTest, ManifestReadFailure) {
  processor_.set_delegate(&delegate_);
  processor_.EnqueueAction(std::move(install_action_));

  ASSERT_TRUE(test_utils::WriteFileString(
      tempdir_.GetPath()
          .Append("dlc/foobar-dlc/package/imageloader.json")
          .value(),
      ""));
  delegate_.expected_code_ = ErrorCode::kScaledInstallationError;

  loop_.PostTask(
      FROM_HERE,
      base::BindOnce(
          [](ActionProcessor* processor) { processor->StartProcessing(); },
          base::Unretained(&processor_)));
  loop_.Run();
  EXPECT_FALSE(loop_.PendingTasks());
}

TEST_F(InstallActionTest, PerformSuccessfulTest) {
  processor_.set_delegate(&delegate_);
  processor_.EnqueueAction(std::move(install_action_));

  auto manifest =
      base::StringPrintf(kManifestTemplate, kDefaultSha, kDefaultOffset);
  ASSERT_TRUE(test_utils::WriteFileString(
      tempdir_.GetPath()
          .Append("dlc/foobar-dlc/package/imageloader.json")
          .value(),
      manifest));
  ASSERT_TRUE(test_utils::WriteFileString(
      tempdir_.GetPath().Append("etc/lsb-release").value(), kProperties));
  delegate_.expected_code_ = ErrorCode::kSuccess;

  ASSERT_TRUE(test_utils::WriteFileString(
      tempdir_.GetPath().Append("foobar-dlc-device").value(), ""));
  FakeSystemState::Get()->fake_boot_control()->SetPartitionDevice(
      "dlc/foobar-dlc/package",
      0,
      tempdir_.GetPath().Append("foobar-dlc-device").value());

  loop_.PostTask(
      FROM_HERE,
      base::BindOnce(
          [](ActionProcessor* processor) { processor->StartProcessing(); },
          base::Unretained(&processor_)));
  loop_.Run();
  EXPECT_FALSE(loop_.PendingTasks());
}

// This also tests backup URLs.
TEST_F(InstallActionTest, PerformInvalidOffsetTest) {
  processor_.set_delegate(&delegate_);
  processor_.EnqueueAction(std::move(install_action_));

  auto manifest = base::StringPrintf(kManifestTemplate, kDefaultSha, "1025");
  ASSERT_TRUE(test_utils::WriteFileString(
      tempdir_.GetPath()
          .Append("dlc/foobar-dlc/package/imageloader.json")
          .value(),
      manifest));
  ASSERT_TRUE(test_utils::WriteFileString(
      tempdir_.GetPath().Append("etc/lsb-release").value(), kProperties));
  delegate_.expected_code_ = ErrorCode::kScaledInstallationError;

  ASSERT_TRUE(test_utils::WriteFileString(
      tempdir_.GetPath().Append("foobar-dlc-device").value(), ""));
  FakeSystemState::Get()->fake_boot_control()->SetPartitionDevice(
      "dlc/foobar-dlc/package",
      0,
      tempdir_.GetPath().Append("foobar-dlc-device").value());

  loop_.PostTask(
      FROM_HERE,
      base::BindOnce(
          [](ActionProcessor* processor) { processor->StartProcessing(); },
          base::Unretained(&processor_)));
  loop_.Run();
  EXPECT_FALSE(loop_.PendingTasks());
}

// This also tests backup URLs.
TEST_F(InstallActionTest, PerformInvalidShaTest) {
  processor_.set_delegate(&delegate_);
  processor_.EnqueueAction(std::move(install_action_));

  auto manifest = base::StringPrintf(
      kManifestTemplate,
      "5f70bf18a086007016e948b04aed3b82103a36bea41755b6cddfaf10deadbeef",
      kDefaultOffset);
  ASSERT_TRUE(test_utils::WriteFileString(
      tempdir_.GetPath()
          .Append("dlc/foobar-dlc/package/imageloader.json")
          .value(),
      manifest));
  ASSERT_TRUE(test_utils::WriteFileString(
      tempdir_.GetPath().Append("etc/lsb-release").value(), kProperties));
  delegate_.expected_code_ = ErrorCode::kScaledInstallationError;

  ASSERT_TRUE(test_utils::WriteFileString(
      tempdir_.GetPath().Append("foobar-dlc-device").value(), ""));
  FakeSystemState::Get()->fake_boot_control()->SetPartitionDevice(
      "dlc/foobar-dlc/package",
      0,
      tempdir_.GetPath().Append("foobar-dlc-device").value());

  loop_.PostTask(
      FROM_HERE,
      base::BindOnce(
          [](ActionProcessor* processor) { processor->StartProcessing(); },
          base::Unretained(&processor_)));
  loop_.Run();
  EXPECT_FALSE(loop_.PendingTasks());
}

}  // namespace chromeos_update_engine
