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

#include "update_engine/common/connection_utils.h"

#include <shill/dbus-constants.h>

namespace {
// Not defined by shill since we don't use this outside of UE.
constexpr char kTypeDisconnected[] = "Disconnected";
constexpr char kTypeUnknown[] = "Unknown";
}  // namespace

namespace chromeos_update_engine {
namespace connection_utils {

ConnectionType ParseConnectionType(const std::string& type_str) {
  if (type_str == shill::kTypeEthernet) {
    return ConnectionType::kEthernet;
  } else if (type_str == shill::kTypeWifi) {
    return ConnectionType::kWifi;
  } else if (type_str == shill::kTypeCellular) {
    return ConnectionType::kCellular;
  } else if (type_str == kTypeDisconnected) {
    return ConnectionType::kDisconnected;
  }
  return ConnectionType::kUnknown;
}

const char* StringForConnectionType(ConnectionType type) {
  switch (type) {
    case ConnectionType::kEthernet:
      return shill::kTypeEthernet;
    case ConnectionType::kWifi:
      return shill::kTypeWifi;
    case ConnectionType::kCellular:
      return shill::kTypeCellular;
    case ConnectionType::kDisconnected:
      return kTypeDisconnected;
    case ConnectionType::kUnknown:
      return kTypeUnknown;
  }
  return kTypeUnknown;
}

}  // namespace connection_utils

}  // namespace chromeos_update_engine
