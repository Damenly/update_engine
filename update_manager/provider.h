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

#ifndef UPDATE_ENGINE_UPDATE_MANAGER_PROVIDER_H_
#define UPDATE_ENGINE_UPDATE_MANAGER_PROVIDER_H_

namespace chromeos_update_manager {

// Abstract base class for a policy provider.
class Provider {
 public:
  Provider(const Provider&) = delete;
  Provider& operator=(const Provider&) = delete;
  virtual ~Provider() {}

 protected:
  Provider() {}
};

}  // namespace chromeos_update_manager

#endif  // UPDATE_ENGINE_UPDATE_MANAGER_PROVIDER_H_
