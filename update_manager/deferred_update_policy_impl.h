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

#ifndef UPDATE_ENGINE_UPDATE_MANAGER_DEFERRED_UPDATE_POLICY_IMPL_H_
#define UPDATE_ENGINE_UPDATE_MANAGER_DEFERRED_UPDATE_POLICY_IMPL_H_

#include <string>

#include "update_engine/update_manager/policy_interface.h"

namespace chromeos_update_manager {

class DeferredUpdatePolicyImpl : public PolicyInterface {
 public:
  DeferredUpdatePolicyImpl() = default;
  DeferredUpdatePolicyImpl(const DeferredUpdatePolicyImpl&) = delete;
  DeferredUpdatePolicyImpl& operator=(const DeferredUpdatePolicyImpl&) = delete;

  ~DeferredUpdatePolicyImpl() override = default;

  std::string PolicyName() const override { return "DeferredUpdatePolicyImpl"; }

  // Policy overrides.
  EvalStatus Evaluate(EvaluationContext* ec,
                      State* state,
                      std::string* error,
                      PolicyDataInterface* data) const override;
};

}  // namespace chromeos_update_manager

#endif  // UPDATE_ENGINE_UPDATE_MANAGER_DEFERRED_UPDATE_POLICY_IMPL_H_
