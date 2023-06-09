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

#ifndef UPDATE_ENGINE_COMMON_HIBERNATE_STUB_H_
#define UPDATE_ENGINE_COMMON_HIBERNATE_STUB_H_

#include <string>

#include "update_engine/common/hibernate_interface.h"

namespace chromeos_update_engine {

// An implementation of the DlcServiceInterface that does nothing.
class HibernateStub : public HibernateInterface {
 public:
  HibernateStub() = default;
  HibernateStub(const HibernateStub&) = delete;
  HibernateStub& operator=(const HibernateStub&) = delete;

  ~HibernateStub() = default;

  bool IsResuming() override;
  bool AbortResume(const std::string& reason) override;
};

}  // namespace chromeos_update_engine

#endif  // UPDATE_ENGINE_COMMON_HIBERNATE_STUB_H_
