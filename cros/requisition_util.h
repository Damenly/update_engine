//
// Copyright (C) 2020 The Android Open Source Project
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

#ifndef UPDATE_ENGINE_CROS_REQUISITION_UTIL_H_
#define UPDATE_ENGINE_CROS_REQUISITION_UTIL_H_

#include <memory>
#include <string>

#include <base/files/file_path.h>
#include <base/values.h>

namespace chromeos_update_engine {

// Checks the VPD and Local State for the device's requisition and returns it,
// or an empty string if the device has no requisition.
std::string ReadDeviceRequisition(const base::Value* local_state);

}  // namespace chromeos_update_engine

#endif  //  UPDATE_ENGINE_CROS_REQUISITION_UTIL_H_
