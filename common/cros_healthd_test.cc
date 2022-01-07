//
// Copyright (C) 2021 The Android Open Source Project
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

#include "update_engine/common/cros_healthd.h"

#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace chromeos_update_engine {

class CrosHealthdTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  CrosHealthd cros_healthd_;
};

TEST_F(CrosHealthdTest, ParseSystemResultV2Check) {
  {
    TelemetryInfo telemetry_info{};
    auto telemetry_info_ptr =
        chromeos::cros_healthd::mojom::TelemetryInfo::New();
    cros_healthd_.ParseSystemResultV2(&telemetry_info_ptr, &telemetry_info);
    EXPECT_EQ("", telemetry_info.system_v2_info.dmi_info.board_vendor);
    EXPECT_EQ("", telemetry_info.system_v2_info.dmi_info.board_name);
    EXPECT_EQ("", telemetry_info.system_v2_info.dmi_info.board_version);
    EXPECT_EQ("", telemetry_info.system_v2_info.dmi_info.bios_version);
    EXPECT_EQ(TelemetryInfo::SystemV2Info::OsInfo::BootMode::kUnknown,
              telemetry_info.system_v2_info.os_info.boot_mode);
  }
  {
    TelemetryInfo telemetry_info{};
    auto&& telemetry_info_ptr =
        chromeos::cros_healthd::mojom::TelemetryInfo::New();
    telemetry_info_ptr->system_result_v2 =
        chromeos::cros_healthd::mojom::SystemResultV2::New();
    auto& system_result_v2_ptr = telemetry_info_ptr->system_result_v2;
    system_result_v2_ptr->set_system_info_v2(
        chromeos::cros_healthd::mojom::SystemInfoV2::New());
    auto& system_info_v2_ptr = system_result_v2_ptr->get_system_info_v2();

    system_info_v2_ptr->dmi_info =
        chromeos::cros_healthd::mojom::DmiInfo::New();
    auto& dmi_info_ptr = system_info_v2_ptr->dmi_info;
    // Missing values.
    dmi_info_ptr->board_name = "fake-board-name";
    dmi_info_ptr->bios_version = "fake-bios-version";

    system_info_v2_ptr->os_info = chromeos::cros_healthd::mojom::OsInfo::New();
    auto& os_info_ptr = system_info_v2_ptr->os_info;
    os_info_ptr->boot_mode = chromeos::cros_healthd::mojom::BootMode::kCrosEfi;

    cros_healthd_.ParseSystemResultV2(&telemetry_info_ptr, &telemetry_info);
    EXPECT_EQ("fake-board-name",
              telemetry_info.system_v2_info.dmi_info.board_name);
    EXPECT_EQ("fake-bios-version",
              telemetry_info.system_v2_info.dmi_info.bios_version);
    EXPECT_EQ(TelemetryInfo::SystemV2Info::OsInfo::BootMode::kCrosEfi,
              telemetry_info.system_v2_info.os_info.boot_mode);
  }
  {
    TelemetryInfo telemetry_info{};
    auto&& telemetry_info_ptr =
        chromeos::cros_healthd::mojom::TelemetryInfo::New();
    telemetry_info_ptr->system_result_v2 =
        chromeos::cros_healthd::mojom::SystemResultV2::New();
    auto& system_result_v2_ptr = telemetry_info_ptr->system_result_v2;
    system_result_v2_ptr->set_system_info_v2(
        chromeos::cros_healthd::mojom::SystemInfoV2::New());
    auto& system_info_v2_ptr = system_result_v2_ptr->get_system_info_v2();

    system_info_v2_ptr->dmi_info =
        chromeos::cros_healthd::mojom::DmiInfo::New();
    auto& dmi_info_ptr = system_info_v2_ptr->dmi_info;
    dmi_info_ptr->board_vendor = "fake-board-vendor";
    dmi_info_ptr->board_name = "fake-board-name";
    dmi_info_ptr->board_version = "fake-board-version";
    dmi_info_ptr->bios_version = "fake-bios-version";

    system_info_v2_ptr->os_info = chromeos::cros_healthd::mojom::OsInfo::New();
    auto& os_info_ptr = system_info_v2_ptr->os_info;
    os_info_ptr->boot_mode = chromeos::cros_healthd::mojom::BootMode::kCrosEfi;

    cros_healthd_.ParseSystemResultV2(&telemetry_info_ptr, &telemetry_info);
    EXPECT_EQ("fake-board-vendor",
              telemetry_info.system_v2_info.dmi_info.board_vendor);
    EXPECT_EQ("fake-board-name",
              telemetry_info.system_v2_info.dmi_info.board_name);
    EXPECT_EQ("fake-board-version",
              telemetry_info.system_v2_info.dmi_info.board_version);
    EXPECT_EQ("fake-bios-version",
              telemetry_info.system_v2_info.dmi_info.bios_version);
    EXPECT_EQ(TelemetryInfo::SystemV2Info::OsInfo::BootMode::kCrosEfi,
              telemetry_info.system_v2_info.os_info.boot_mode);
  }
}

TEST_F(CrosHealthdTest, ParseMemoryResultCheck) {
  {
    TelemetryInfo telemetry_info{};
    auto telemetry_info_ptr =
        chromeos::cros_healthd::mojom::TelemetryInfo::New();
    cros_healthd_.ParseMemoryResult(&telemetry_info_ptr, &telemetry_info);
    EXPECT_EQ(uint32_t(0), telemetry_info.memory_info.total_memory_kib);
  }
  {
    TelemetryInfo telemetry_info{};
    auto&& telemetry_info_ptr =
        chromeos::cros_healthd::mojom::TelemetryInfo::New();
    telemetry_info_ptr->memory_result =
        chromeos::cros_healthd::mojom::MemoryResult::New();
    auto& memory_result_ptr = telemetry_info_ptr->memory_result;
    memory_result_ptr->set_memory_info(
        chromeos::cros_healthd::mojom::MemoryInfo::New());

    auto& memory_info_ptr = memory_result_ptr->get_memory_info();
    memory_info_ptr->total_memory_kib = uint32_t(123);

    cros_healthd_.ParseMemoryResult(&telemetry_info_ptr, &telemetry_info);
    EXPECT_EQ(uint32_t(123), telemetry_info.memory_info.total_memory_kib);
  }
}

TEST_F(CrosHealthdTest, ParseNonRemovableBlockDeviceResultCheck) {
  {
    TelemetryInfo telemetry_info{};
    auto telemetry_info_ptr =
        chromeos::cros_healthd::mojom::TelemetryInfo::New();
    cros_healthd_.ParseNonRemovableBlockDeviceResult(&telemetry_info_ptr,
                                                     &telemetry_info);
    EXPECT_TRUE(telemetry_info.block_device_info.empty());
  }
  {
    TelemetryInfo telemetry_info{};
    auto&& telemetry_info_ptr =
        chromeos::cros_healthd::mojom::TelemetryInfo::New();
    telemetry_info_ptr->block_device_result =
        chromeos::cros_healthd::mojom::NonRemovableBlockDeviceResult::New();
    auto& block_device_result_ptr = telemetry_info_ptr->block_device_result;
    std::vector<chromeos::cros_healthd::mojom::NonRemovableBlockDeviceInfoPtr>
        block_device_info;
    block_device_info.emplace_back(
        chromeos::cros_healthd::mojom::NonRemovableBlockDeviceInfo::New());
    block_device_result_ptr->set_block_device_info(
        std::move(block_device_info));

    auto& block_device_info_ptr =
        block_device_result_ptr->get_block_device_info();
    block_device_info_ptr.front()->size = uint64_t(123);

    cros_healthd_.ParseNonRemovableBlockDeviceResult(&telemetry_info_ptr,
                                                     &telemetry_info);
    EXPECT_EQ(uint64_t(123), telemetry_info.block_device_info.front().size);
  }
}

TEST_F(CrosHealthdTest, ParseCpuResultCheck) {
  {
    TelemetryInfo telemetry_info{};
    auto&& telemetry_info_ptr =
        chromeos::cros_healthd::mojom::TelemetryInfo::New();
    cros_healthd_.ParseCpuResult(&telemetry_info_ptr, &telemetry_info);
    EXPECT_TRUE(telemetry_info.cpu_info.physical_cpus.empty());
  }
  {
    TelemetryInfo telemetry_info{};
    auto&& telemetry_info_ptr =
        chromeos::cros_healthd::mojom::TelemetryInfo::New();
    telemetry_info_ptr->cpu_result =
        chromeos::cros_healthd::mojom::CpuResult::New();
    auto& cpu_result_ptr = telemetry_info_ptr->cpu_result;
    cpu_result_ptr->set_cpu_info(chromeos::cros_healthd::mojom::CpuInfo::New());
    auto& cpu_info_ptr = cpu_result_ptr->get_cpu_info();

    std::vector<chromeos::cros_healthd::mojom::PhysicalCpuInfoPtr>
        physical_cpus;
    physical_cpus.emplace_back(
        chromeos::cros_healthd::mojom::PhysicalCpuInfo::New());
    cpu_info_ptr->physical_cpus = std::move(physical_cpus);

    // Missing values, don't set any values.

    cros_healthd_.ParseCpuResult(&telemetry_info_ptr, &telemetry_info);
    EXPECT_TRUE(telemetry_info.cpu_info.physical_cpus.empty());
  }
  {
    TelemetryInfo telemetry_info{};
    auto&& telemetry_info_ptr =
        chromeos::cros_healthd::mojom::TelemetryInfo::New();
    telemetry_info_ptr->cpu_result =
        chromeos::cros_healthd::mojom::CpuResult::New();
    auto& cpu_result_ptr = telemetry_info_ptr->cpu_result;
    cpu_result_ptr->set_cpu_info(chromeos::cros_healthd::mojom::CpuInfo::New());
    auto& cpu_info_ptr = cpu_result_ptr->get_cpu_info();

    std::vector<chromeos::cros_healthd::mojom::PhysicalCpuInfoPtr>
        physical_cpus;
    physical_cpus.emplace_back(
        chromeos::cros_healthd::mojom::PhysicalCpuInfo::New());
    cpu_info_ptr->physical_cpus = std::move(physical_cpus);

    auto& physical_cpus_ptr = cpu_info_ptr->physical_cpus;
    physical_cpus_ptr.front()->model_name = "fake-model-name";

    cros_healthd_.ParseCpuResult(&telemetry_info_ptr, &telemetry_info);
    EXPECT_EQ("fake-model-name",
              telemetry_info.cpu_info.physical_cpus.front().model_name);
  }
}

TEST_F(CrosHealthdTest, ParseBusResultCheckMissingBusResult) {
  TelemetryInfo telemetry_info{};
  auto&& telemetry_info_ptr =
      chromeos::cros_healthd::mojom::TelemetryInfo::New();
  cros_healthd_.ParseBusResult(&telemetry_info_ptr, &telemetry_info);
  EXPECT_TRUE(telemetry_info.bus_devices.empty());
}

TEST_F(CrosHealthdTest, ParseBusResultCheckMissingBusInfo) {
  TelemetryInfo telemetry_info{};
  auto&& telemetry_info_ptr =
      chromeos::cros_healthd::mojom::TelemetryInfo::New();
  telemetry_info_ptr->bus_result =
      chromeos::cros_healthd::mojom::BusResult::New();
  auto& bus_result_ptr = telemetry_info_ptr->bus_result;
  bus_result_ptr->set_bus_devices({});
  auto& bus_devices_ptr = bus_result_ptr->get_bus_devices();

  bus_devices_ptr.emplace_back(chromeos::cros_healthd::mojom::BusDevice::New());

  cros_healthd_.ParseBusResult(&telemetry_info_ptr, &telemetry_info);
  EXPECT_TRUE(telemetry_info.bus_devices.empty());
}

TEST_F(CrosHealthdTest, ParseBusResultCheckPciBusDefault) {
  TelemetryInfo telemetry_info{};
  auto&& telemetry_info_ptr =
      chromeos::cros_healthd::mojom::TelemetryInfo::New();
  telemetry_info_ptr->bus_result =
      chromeos::cros_healthd::mojom::BusResult::New();
  auto& bus_result_ptr = telemetry_info_ptr->bus_result;
  bus_result_ptr->set_bus_devices({});
  auto& bus_devices_ptr = bus_result_ptr->get_bus_devices();

  bus_devices_ptr.emplace_back(chromeos::cros_healthd::mojom::BusDevice::New());
  auto& bus_device_ptr = bus_devices_ptr.back();
  auto& bus_info_ptr = bus_device_ptr->bus_info;
  bus_info_ptr = chromeos::cros_healthd::mojom::BusInfo::New();

  // Create PCI bus info.
  bus_info_ptr->set_pci_bus_info(
      chromeos::cros_healthd::mojom::PciBusInfo::New());

  cros_healthd_.ParseBusResult(&telemetry_info_ptr, &telemetry_info);
  EXPECT_FALSE(telemetry_info.bus_devices.empty());
}

TEST_F(CrosHealthdTest, ParseBusResultCheckPciBus) {
  TelemetryInfo telemetry_info{};
  auto&& telemetry_info_ptr =
      chromeos::cros_healthd::mojom::TelemetryInfo::New();
  telemetry_info_ptr->bus_result =
      chromeos::cros_healthd::mojom::BusResult::New();
  auto& bus_result_ptr = telemetry_info_ptr->bus_result;
  bus_result_ptr->set_bus_devices({});
  auto& bus_devices_ptr = bus_result_ptr->get_bus_devices();

  bus_devices_ptr.emplace_back(chromeos::cros_healthd::mojom::BusDevice::New());
  auto& bus_device_ptr = bus_devices_ptr.back();
  auto& bus_info_ptr = bus_device_ptr->bus_info;
  bus_info_ptr = chromeos::cros_healthd::mojom::BusInfo::New();

  // Create PCI bus info.
  uint8_t class_id = 1;
  uint8_t subclass_id = 2;
  uint8_t protocol_id = 3;
  uint16_t vendor_id = 4;
  uint16_t device_id = 5;
  // Don't use driver after std::move.
  absl::optional<std::string> driver = "some-driver";
  bus_info_ptr->set_pci_bus_info(
      chromeos::cros_healthd::mojom::PciBusInfo::New(class_id,
                                                     subclass_id,
                                                     protocol_id,
                                                     vendor_id,
                                                     device_id,
                                                     std::move(driver)));

  cros_healthd_.ParseBusResult(&telemetry_info_ptr, &telemetry_info);
  ASSERT_EQ(telemetry_info.bus_devices.size(), 1);

  const auto* pci_bus_info = std::get_if<TelemetryInfo::BusDevice::PciBusInfo>(
      &telemetry_info.bus_devices[0].bus_type_info);
  ASSERT_NE(pci_bus_info, nullptr);
  EXPECT_EQ(pci_bus_info->vendor_id, vendor_id);
  EXPECT_EQ(pci_bus_info->device_id, device_id);
  EXPECT_EQ(pci_bus_info->driver, "some-driver");
}

TEST_F(CrosHealthdTest, ParseBusResultCheckUsbBusDefault) {
  TelemetryInfo telemetry_info{};
  auto&& telemetry_info_ptr =
      chromeos::cros_healthd::mojom::TelemetryInfo::New();
  telemetry_info_ptr->bus_result =
      chromeos::cros_healthd::mojom::BusResult::New();
  auto& bus_result_ptr = telemetry_info_ptr->bus_result;
  bus_result_ptr->set_bus_devices({});
  auto& bus_devices_ptr = bus_result_ptr->get_bus_devices();

  bus_devices_ptr.emplace_back(chromeos::cros_healthd::mojom::BusDevice::New());
  auto& bus_device_ptr = bus_devices_ptr.back();
  auto& bus_info_ptr = bus_device_ptr->bus_info;
  bus_info_ptr = chromeos::cros_healthd::mojom::BusInfo::New();

  // Create USB bus info.
  bus_info_ptr->set_usb_bus_info(
      chromeos::cros_healthd::mojom::UsbBusInfo::New());

  cros_healthd_.ParseBusResult(&telemetry_info_ptr, &telemetry_info);
  EXPECT_FALSE(telemetry_info.bus_devices.empty());
}

TEST_F(CrosHealthdTest, ParseBusResultCheckUsbBus) {
  TelemetryInfo telemetry_info{};
  auto&& telemetry_info_ptr =
      chromeos::cros_healthd::mojom::TelemetryInfo::New();
  telemetry_info_ptr->bus_result =
      chromeos::cros_healthd::mojom::BusResult::New();
  auto& bus_result_ptr = telemetry_info_ptr->bus_result;
  bus_result_ptr->set_bus_devices({});
  auto& bus_devices_ptr = bus_result_ptr->get_bus_devices();

  bus_devices_ptr.emplace_back(chromeos::cros_healthd::mojom::BusDevice::New());
  auto& bus_device_ptr = bus_devices_ptr.back();
  auto& bus_info_ptr = bus_device_ptr->bus_info;
  bus_info_ptr = chromeos::cros_healthd::mojom::BusInfo::New();

  // Create USB bus info.
  uint8_t class_id = 1;
  uint8_t subclass_id = 2;
  uint8_t protocol_id = 3;
  uint16_t vendor_id = 4;
  uint16_t product_id = 5;
  // Don't use interface after std::move.
  std::vector<chromeos::cros_healthd::mojom::UsbBusInterfaceInfoPtr> interfaces;
  bus_info_ptr->set_usb_bus_info(
      chromeos::cros_healthd::mojom::UsbBusInfo::New(class_id,
                                                     subclass_id,
                                                     protocol_id,
                                                     vendor_id,
                                                     product_id,
                                                     std::move(interfaces)));

  cros_healthd_.ParseBusResult(&telemetry_info_ptr, &telemetry_info);
  ASSERT_EQ(telemetry_info.bus_devices.size(), 1);

  const auto* usb_bus_info = std::get_if<TelemetryInfo::BusDevice::UsbBusInfo>(
      &telemetry_info.bus_devices[0].bus_type_info);
  ASSERT_NE(usb_bus_info, nullptr);
  EXPECT_EQ(usb_bus_info->vendor_id, vendor_id);
  EXPECT_EQ(usb_bus_info->product_id, product_id);
}

TEST_F(CrosHealthdTest, ParseBusResultCheckThunderboltBusDefault) {
  TelemetryInfo telemetry_info{};
  auto&& telemetry_info_ptr =
      chromeos::cros_healthd::mojom::TelemetryInfo::New();
  telemetry_info_ptr->bus_result =
      chromeos::cros_healthd::mojom::BusResult::New();
  auto& bus_result_ptr = telemetry_info_ptr->bus_result;
  bus_result_ptr->set_bus_devices({});
  auto& bus_devices_ptr = bus_result_ptr->get_bus_devices();

  bus_devices_ptr.emplace_back(chromeos::cros_healthd::mojom::BusDevice::New());
  auto& bus_device_ptr = bus_devices_ptr.back();
  auto& bus_info_ptr = bus_device_ptr->bus_info;
  bus_info_ptr = chromeos::cros_healthd::mojom::BusInfo::New();

  // Create Thunderbolt bus info.
  bus_info_ptr->set_thunderbolt_bus_info(
      chromeos::cros_healthd::mojom::ThunderboltBusInfo::New());

  cros_healthd_.ParseBusResult(&telemetry_info_ptr, &telemetry_info);
  // Thunderbolt is not parsed yet.
  EXPECT_TRUE(telemetry_info.bus_devices.empty());
}

TEST_F(CrosHealthdTest, ParseBusResultCheckThunderboltBus) {
  TelemetryInfo telemetry_info{};
  auto&& telemetry_info_ptr =
      chromeos::cros_healthd::mojom::TelemetryInfo::New();
  telemetry_info_ptr->bus_result =
      chromeos::cros_healthd::mojom::BusResult::New();
  auto& bus_result_ptr = telemetry_info_ptr->bus_result;
  bus_result_ptr->set_bus_devices({});
  auto& bus_devices_ptr = bus_result_ptr->get_bus_devices();

  bus_devices_ptr.emplace_back(chromeos::cros_healthd::mojom::BusDevice::New());
  auto& bus_device_ptr = bus_devices_ptr.back();
  auto& bus_info_ptr = bus_device_ptr->bus_info;
  bus_info_ptr = chromeos::cros_healthd::mojom::BusInfo::New();

  // Create Thunderbolt bus info.
  bus_info_ptr->set_thunderbolt_bus_info(
      chromeos::cros_healthd::mojom::ThunderboltBusInfo::New());
  auto& thunderbolt_bus_info_ptr = bus_info_ptr->get_thunderbolt_bus_info();
  thunderbolt_bus_info_ptr->security_level =
      chromeos::cros_healthd::mojom::ThunderboltSecurityLevel::kNone;
  thunderbolt_bus_info_ptr->thunderbolt_interfaces.emplace_back(
      chromeos::cros_healthd::mojom::ThunderboltBusInterfaceInfo::New());

  cros_healthd_.ParseBusResult(&telemetry_info_ptr, &telemetry_info);
  // Thunderbolt is not parsed yet.
  EXPECT_TRUE(telemetry_info.bus_devices.empty());
}

TEST_F(CrosHealthdTest, ParseBusResultCheckAllBus) {
  TelemetryInfo telemetry_info{};
  auto&& telemetry_info_ptr =
      chromeos::cros_healthd::mojom::TelemetryInfo::New();
  telemetry_info_ptr->bus_result =
      chromeos::cros_healthd::mojom::BusResult::New();
  auto& bus_result_ptr = telemetry_info_ptr->bus_result;
  bus_result_ptr->set_bus_devices({});
  auto& bus_devices_ptr = bus_result_ptr->get_bus_devices();

  // Common usage across creating bus.
  uint8_t class_id = 1;
  uint8_t subclass_id = 2;
  uint8_t protocol_id = 3;
  uint16_t vendor_id = 4;

  // Create PCI bus info.
  uint16_t device_id = 7;
  {
    bus_devices_ptr.emplace_back(
        chromeos::cros_healthd::mojom::BusDevice::New());
    auto& bus_device_ptr = bus_devices_ptr.back();
    auto& bus_info_ptr = bus_device_ptr->bus_info;
    bus_info_ptr = chromeos::cros_healthd::mojom::BusInfo::New();
    // Don't use driver after std::move.
    absl::optional<std::string> driver = "some-driver";
    bus_info_ptr->set_pci_bus_info(
        chromeos::cros_healthd::mojom::PciBusInfo::New(class_id,
                                                       subclass_id,
                                                       protocol_id,
                                                       vendor_id,
                                                       device_id,
                                                       std::move(driver)));
  }

  // Create USB bus info.
  uint16_t product_id = 8;
  {
    bus_devices_ptr.emplace_back(
        chromeos::cros_healthd::mojom::BusDevice::New());
    auto& bus_device_ptr = bus_devices_ptr.back();
    auto& bus_info_ptr = bus_device_ptr->bus_info;
    bus_info_ptr = chromeos::cros_healthd::mojom::BusInfo::New();
    // Don't use interface after std::move.
    std::vector<chromeos::cros_healthd::mojom::UsbBusInterfaceInfoPtr>
        interfaces;
    bus_info_ptr->set_usb_bus_info(
        chromeos::cros_healthd::mojom::UsbBusInfo::New(class_id,
                                                       subclass_id,
                                                       protocol_id,
                                                       vendor_id,
                                                       product_id,
                                                       std::move(interfaces)));
  }

  // Create Thunderbolt bus info.
  {
    bus_devices_ptr.emplace_back(
        chromeos::cros_healthd::mojom::BusDevice::New());
    auto& bus_device_ptr = bus_devices_ptr.back();
    auto& bus_info_ptr = bus_device_ptr->bus_info;
    bus_info_ptr = chromeos::cros_healthd::mojom::BusInfo::New();
    bus_info_ptr->set_thunderbolt_bus_info(
        chromeos::cros_healthd::mojom::ThunderboltBusInfo::New());
    auto& thunderbolt_bus_info_ptr = bus_info_ptr->get_thunderbolt_bus_info();
    thunderbolt_bus_info_ptr->security_level =
        chromeos::cros_healthd::mojom::ThunderboltSecurityLevel::kNone;
    thunderbolt_bus_info_ptr->thunderbolt_interfaces.emplace_back(
        chromeos::cros_healthd::mojom::ThunderboltBusInterfaceInfo::New());
  }

  cros_healthd_.ParseBusResult(&telemetry_info_ptr, &telemetry_info);
  ASSERT_EQ(telemetry_info.bus_devices.size(), 2);

  // Check PCI bus info.
  const auto* pci_bus_info = std::get_if<TelemetryInfo::BusDevice::PciBusInfo>(
      &telemetry_info.bus_devices[0].bus_type_info);
  ASSERT_NE(pci_bus_info, nullptr);
  EXPECT_EQ(pci_bus_info->vendor_id, vendor_id);
  EXPECT_EQ(pci_bus_info->device_id, device_id);
  EXPECT_EQ(pci_bus_info->driver, "some-driver");
  // Check USB bus info.
  const auto* usb_bus_info = std::get_if<TelemetryInfo::BusDevice::UsbBusInfo>(
      &telemetry_info.bus_devices[1].bus_type_info);
  ASSERT_NE(usb_bus_info, nullptr);
  EXPECT_EQ(usb_bus_info->vendor_id, vendor_id);
  EXPECT_EQ(usb_bus_info->product_id, product_id);
  // Thunderbolt is not parsed yet.
}

}  // namespace chromeos_update_engine
