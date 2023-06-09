//
// Copyright (C) 2012 The Android Open Source Project
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

#ifndef UPDATE_ENGINE_CROS_MOCK_CONNECTION_MANAGER_H_
#define UPDATE_ENGINE_CROS_MOCK_CONNECTION_MANAGER_H_

#include <gmock/gmock.h>

#include "update_engine/cros/connection_manager_interface.h"

namespace chromeos_update_engine {

// This class mocks the generic interface to the connection manager
// (e.g FlimFlam, Shill, etc.) to consolidate all connection-related
// logic in update_engine.
class MockConnectionManager : public ConnectionManagerInterface {
 public:
  MockConnectionManager() = default;

  MOCK_METHOD(bool,
              GetConnectionProperties,
              (ConnectionType * out_type, bool* out_metered));

  MOCK_CONST_METHOD0(IsUpdateAllowedOverMetered, bool());
  MOCK_CONST_METHOD0(IsAllowedConnectionTypesForUpdateSet, bool());
};

}  // namespace chromeos_update_engine

#endif  // UPDATE_ENGINE_CROS_MOCK_CONNECTION_MANAGER_H_
