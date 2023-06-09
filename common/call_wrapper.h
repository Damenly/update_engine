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

#ifndef UPDATE_ENGINE_COMMON_CALL_WRAPPER_H_
#define UPDATE_ENGINE_COMMON_CALL_WRAPPER_H_

#include "update_engine/common/call_wrapper_interface.h"

#include <base/files/file_path.h>

namespace chromeos_update_engine {

class CallWrapper : public CallWrapperInterface {
 public:
  CallWrapper() = default;
  CallWrapper(const CallWrapper&) = delete;
  CallWrapper& operator=(const CallWrapper&) = delete;

  ~CallWrapper() = default;

  // CallWrapperInterface overrides.
  int64_t AmountOfFreeDiskSpace(const base::FilePath& path) override;
};

}  // namespace chromeos_update_engine

#endif  // UPDATE_ENGINE_COMMON_CALL_WRAPPER_H_
