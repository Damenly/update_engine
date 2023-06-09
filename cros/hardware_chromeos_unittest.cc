//
// Copyright (C) 2016 The Android Open Source Project
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

#include "update_engine/cros/hardware_chromeos.h"

#include <memory>

#include <base/files/file_util.h>
#include <base/files/scoped_temp_dir.h>
#include <base/json/json_string_value_serializer.h>
#include <brillo/file_utils.h>
#include <gtest/gtest.h>

#include "update_engine/common/constants.h"
#include "update_engine/common/fake_hardware.h"
#include "update_engine/common/platform_constants.h"
#include "update_engine/common/test_utils.h"
#include "update_engine/update_manager/umtest_utils.h"

using chromeos_update_engine::test_utils::WriteFileString;
using std::string;

namespace {

constexpr char kEnrollmentReCoveryTrueJSON[] = R"({
  "the_list": [ "val1", "val2" ],
  "EnrollmentRecoveryRequired": true,
  "some_String": "1337",
  "some_int": 42
})";

constexpr char kEnrollmentReCoveryFalseJSON[] = R"({
  "the_list": [ "val1", "val2" ],
  "EnrollmentRecoveryRequired": false,
  "some_String": "1337",
  "some_int": 42
})";

constexpr char kNoEnrollmentRecoveryJSON[] = R"({
  "the_list": [ "val1", "val2" ],
  "some_String": "1337",
  "some_int": 42
})";

}  // namespace

namespace chromeos_update_engine {

class HardwareChromeOSTest : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_TRUE(root_dir_.CreateUniqueTempDir()); }

  void WriteStatefulConfig(const string& config) {
    base::FilePath kFile(root_dir_.GetPath().value() + kStatefulPartition +
                         "/etc/update_manager.conf");
    ASSERT_TRUE(base::CreateDirectory(kFile.DirName()));
    ASSERT_TRUE(WriteFileString(kFile.value(), config));
  }

  void WriteRootfsConfig(const string& config) {
    base::FilePath kFile(root_dir_.GetPath().value() +
                         "/etc/update_manager.conf");
    ASSERT_TRUE(base::CreateDirectory(kFile.DirName()));
    ASSERT_TRUE(WriteFileString(kFile.value(), config));
  }

  // Helper method to call HardwareChromeOS::LoadConfig with the test directory.
  void CallLoadConfig(bool normal_mode) {
    hardware_.LoadConfig(root_dir_.GetPath().value(), normal_mode);
  }

  std::unique_ptr<base::Value> JSONToUniquePtrValue(const string& json_string) {
    int error_code;
    std::string error_msg;

    JSONStringValueDeserializer deserializer(json_string);

    return deserializer.Deserialize(&error_code, &error_msg);
  }

  HardwareChromeOS hardware_;
  base::ScopedTempDir root_dir_;
};

TEST_F(HardwareChromeOSTest, NoLocalFile) {
  std::unique_ptr<base::Value> root = nullptr;

  EXPECT_FALSE(hardware_.IsEnrollmentRecoveryModeEnabled(root.get()));
}

TEST_F(HardwareChromeOSTest, LocalFileWithEnrollmentRecoveryTrue) {
  std::unique_ptr<base::Value> root =
      JSONToUniquePtrValue(kEnrollmentReCoveryTrueJSON);
  EXPECT_TRUE(hardware_.IsEnrollmentRecoveryModeEnabled(root.get()));
}

TEST_F(HardwareChromeOSTest, LocalFileWithEnrollmentRecoveryFalse) {
  std::unique_ptr<base::Value> root =
      JSONToUniquePtrValue(kEnrollmentReCoveryFalseJSON);
  EXPECT_FALSE(hardware_.IsEnrollmentRecoveryModeEnabled(root.get()));
}

TEST_F(HardwareChromeOSTest, LocalFileWithNoEnrollmentRecoveryPath) {
  std::unique_ptr<base::Value> root =
      JSONToUniquePtrValue(kNoEnrollmentRecoveryJSON);
  EXPECT_FALSE(hardware_.IsEnrollmentRecoveryModeEnabled(root.get()));
}

TEST_F(HardwareChromeOSTest, NoFileFoundReturnsDefault) {
  CallLoadConfig(true /* normal_mode */);
  EXPECT_TRUE(hardware_.IsOOBEEnabled());
}

TEST_F(HardwareChromeOSTest, DontReadStatefulInNormalMode) {
  WriteStatefulConfig("is_oobe_enabled=false");

  CallLoadConfig(true /* normal_mode */);
  EXPECT_TRUE(hardware_.IsOOBEEnabled());
}

TEST_F(HardwareChromeOSTest, ReadStatefulInDevMode) {
  WriteRootfsConfig("is_oobe_enabled=true");
  // Since the stateful is present, we should read that one.
  WriteStatefulConfig("is_oobe_enabled=false");

  CallLoadConfig(false /* normal_mode */);
  EXPECT_FALSE(hardware_.IsOOBEEnabled());
}

TEST_F(HardwareChromeOSTest, ReadRootfsIfStatefulNotFound) {
  WriteRootfsConfig("is_oobe_enabled=false");

  CallLoadConfig(false /* normal_mode */);
  EXPECT_FALSE(hardware_.IsOOBEEnabled());
}

TEST_F(HardwareChromeOSTest, RunningInMiniOs) {
  base::FilePath test_path = root_dir_.GetPath();
  hardware_.SetRootForTest(test_path);
  std::string cmdline =
      " loglevel=7    root=/dev cros_minios \"noinitrd "
      "panic=60   version=14018.0\" \'kern_guid=78 ";
  brillo::TouchFile(test_path.Append("proc").Append("cmdline"));
  EXPECT_TRUE(
      base::WriteFile(test_path.Append("proc").Append("cmdline"), cmdline));
  EXPECT_TRUE(hardware_.IsRunningFromMiniOs());

  cmdline = " loglevel=7    root=/dev cros_minios";
  EXPECT_TRUE(
      base::WriteFile(test_path.Append("proc").Append("cmdline"), cmdline));
  EXPECT_TRUE(hardware_.IsRunningFromMiniOs());

  // Search all matches for key.
  cmdline = "cros_minios_version=1.1.1 cros_minios";
  EXPECT_TRUE(
      base::WriteFile(test_path.Append("proc").Append("cmdline"), cmdline));
  EXPECT_TRUE(hardware_.IsRunningFromMiniOs());

  // Ends with quotes.
  cmdline =
      "dm_verity.dev_wait=1  \"noinitrd panic=60 "
      "cros_minios_version=14116.0.2021_07_28_1259 cros_minios\"";
  EXPECT_TRUE(
      base::WriteFile(test_path.Append("proc").Append("cmdline"), cmdline));
  EXPECT_TRUE(hardware_.IsRunningFromMiniOs());

  // Search all matches for key, reject multiple partial matches.
  cmdline = "cros_minios_version=1.1.1 cros_minios_mode";
  EXPECT_TRUE(
      base::WriteFile(test_path.Append("proc").Append("cmdline"), cmdline));
  EXPECT_FALSE(hardware_.IsRunningFromMiniOs());

  // Reject a partial match.
  cmdline = " loglevel=7    root=/dev cros_minios_version=1.1.1";
  EXPECT_TRUE(
      base::WriteFile(test_path.Append("proc").Append("cmdline"), cmdline));
  EXPECT_FALSE(hardware_.IsRunningFromMiniOs());
}

TEST_F(HardwareChromeOSTest, NotRunningInMiniOs) {
  EXPECT_FALSE(hardware_.IsRunningFromMiniOs());
}

TEST_F(HardwareChromeOSTest, RecoveryKeyVersionMissingFile) {
  base::FilePath test_path = root_dir_.GetPath();
  hardware_.SetNonVolatileDirectoryForTest(test_path);

  base::FilePath non_volatile_directory;
  ASSERT_TRUE(hardware_.GetNonVolatileDirectory(&non_volatile_directory));
  ASSERT_TRUE(base::CreateDirectory(non_volatile_directory));

  std::string version;
  EXPECT_FALSE(hardware_.GetRecoveryKeyVersion(&version));
}

TEST_F(HardwareChromeOSTest, RecoveryKeyVersionBadKey) {
  base::FilePath test_path = root_dir_.GetPath();
  hardware_.SetNonVolatileDirectoryForTest(test_path);

  base::FilePath non_volatile_directory;
  ASSERT_TRUE(hardware_.GetNonVolatileDirectory(&non_volatile_directory));
  ASSERT_TRUE(base::CreateDirectory(non_volatile_directory));

  EXPECT_TRUE(base::WriteFile(
      non_volatile_directory.Append(constants::kRecoveryKeyVersionFileName),
      "foobar"));

  std::string version;
  EXPECT_FALSE(hardware_.GetRecoveryKeyVersion(&version));
}

TEST_F(HardwareChromeOSTest, RecoveryKeyVersion) {
  base::FilePath test_path = root_dir_.GetPath();
  hardware_.SetNonVolatileDirectoryForTest(test_path);

  base::FilePath non_volatile_directory;
  ASSERT_TRUE(hardware_.GetNonVolatileDirectory(&non_volatile_directory));
  ASSERT_TRUE(base::CreateDirectory(non_volatile_directory));

  EXPECT_TRUE(base::WriteFile(
      non_volatile_directory.Append(constants::kRecoveryKeyVersionFileName),
      "123"));

  std::string version;
  EXPECT_TRUE(hardware_.GetRecoveryKeyVersion(&version));
  EXPECT_EQ(std::string("123"), version);
}

TEST_F(HardwareChromeOSTest, RecoveryKeyVersionTrimWhitespaces) {
  base::FilePath test_path = root_dir_.GetPath();
  hardware_.SetNonVolatileDirectoryForTest(test_path);

  base::FilePath non_volatile_directory;
  ASSERT_TRUE(hardware_.GetNonVolatileDirectory(&non_volatile_directory));
  ASSERT_TRUE(base::CreateDirectory(non_volatile_directory));

  EXPECT_TRUE(base::WriteFile(
      non_volatile_directory.Append(constants::kRecoveryKeyVersionFileName),
      "\n888\n"));

  std::string version;
  EXPECT_TRUE(hardware_.GetRecoveryKeyVersion(&version));
  EXPECT_EQ(std::string("888"), version);
}

TEST_F(HardwareChromeOSTest, IsRootfsVerificationEnabled) {
  base::FilePath test_path = root_dir_.GetPath();
  hardware_.SetRootForTest(test_path);
  {
    std::string cmdline =
        R"(cros_secure console= loglevel=7 init=/sbin/init )"
        R"(cros_secure drm.trace=0x106 )"
        R"(root=PARTUUID=dc3f3c92-18db-744b-a2c2-7b0eb696b879/PARTNROFF=1 )"
        R"(rootwait rw dm_verity.error_behavior=3 dm_verity.max_bios=-1 )"
        R"(dm_verity.dev_wait=0 )"
        R"(dm="1 vroot none ro 1,0 6348800 verity payload=ROOT_DEV )"
        R"(hashtree=HASH_DEV hashstart=6348800 alg=sha256 )"
        R"(root_hexdigest=ebc3199685b6f9217c59016b3d4a82ce066c2087a3b99c9c38e9)"
        R"(772281288fb1 )"
        R"(salt=00d2f004a524773dd1c69aada1cc91b9a5de7701ffbfd4fec89ff34469a47)"
        R"(cf0" noinitrd cros_debug vt.global_cursor_default=0 )"
        R"(kern_guid=dc3f3c92-18db-744b-a2c2-7b0eb696b879 add_efi_memmap )"
        R"(boot=local noresume noswap i915.modeset=1 ramoops.ecc=1 )"
        R"(tpm_tis.force=0 intel_pmc_core.warn_on_s0ix_failures=1 )"
        R"(i915.enable_guc=3 i915.enable_dc=4 xdomain=0 swiotlb=65536 )"
        R"(intel_iommu=on i915.enable_psr=1 usb-storage.quirks=13fe:6500:u)";
    brillo::TouchFile(test_path.Append("proc").Append("cmdline"));
    EXPECT_TRUE(
        base::WriteFile(test_path.Append("proc").Append("cmdline"), cmdline));
    EXPECT_FALSE(hardware_.IsRootfsVerificationEnabled());
  }

  {
    std::string cmdline =
        R"(cros_secure console= loglevel=7 init=/sbin/init )"
        R"(cros_secure drm.trace=0x106 )"
        R"(root=PARTUUID=dc3f3c92-18db-744b-a2c2-7b0eb696b879/PARTNROFF=1 )"
        R"(rootwait rw dm_verity.error_behavior=3 dm_verity.max_bios=-1 )"
        R"(dm_verity.dev_wait=1 )"
        R"(dm="1 vroot none ro 1,0 6348800 verity payload=ROOT_DEV )"
        R"(hashtree=HASH_DEV hashstart=6348800 alg=sha256 )"
        R"(root_hexdigest=ebc3199685b6f9217c59016b3d4a82ce066c2087a3b99c9c38e9)"
        R"(772281288fb1 )"
        R"(salt=00d2f004a524773dd1c69aada1cc91b9a5de7701ffbfd4fec89ff34469a47)"
        R"(cf0" noinitrd cros_debug vt.global_cursor_default=0 )"
        R"(kern_guid=dc3f3c92-18db-744b-a2c2-7b0eb696b879 add_efi_memmap )"
        R"(boot=local noresume noswap i915.modeset=1 ramoops.ecc=1 )"
        R"(tpm_tis.force=0 intel_pmc_core.warn_on_s0ix_failures=1 )"
        R"(i915.enable_guc=3 i915.enable_dc=4 xdomain=0 swiotlb=65536 )"
        R"(intel_iommu=on i915.enable_psr=1 usb-storage.quirks=13fe:6500:u)";
    EXPECT_TRUE(
        base::WriteFile(test_path.Append("proc").Append("cmdline"), cmdline));
    EXPECT_TRUE(hardware_.IsRootfsVerificationEnabled());
  }
}

}  // namespace chromeos_update_engine
