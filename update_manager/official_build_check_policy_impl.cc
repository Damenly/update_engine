//
// Copyright (C) 2017 The Android Open Source Project
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

#include "update_engine/update_manager/official_build_check_policy_impl.h"

#include <base/logging.h>

namespace chromeos_update_manager {

// Unofficial builds should not perform periodic update checks.
EvalStatus OnlyUpdateOfficialBuildsPolicyImpl::Evaluate(
    EvaluationContext* ec,
    State* state,
    std::string* error,
    PolicyDataInterface* data) const {
// ---***FYDEOS BEGIN***---
  /*
  const bool* is_official_build_p =
      ec->GetValue(state->system_provider()->var_is_official_build());
  if (is_official_build_p != nullptr && !(*is_official_build_p)) {
    const int64_t* interval_timeout_p = ec->GetValue(
        state->updater_provider()->var_test_update_check_interval_timeout());
    // The |interval_timeout | is used for testing only to test periodic
    // update checks on unofficial images.
    if (interval_timeout_p == nullptr) {
      LOG(INFO) << "Unofficial build, blocking periodic update checks.";
      return EvalStatus::kAskMeAgainLater;
    }
    LOG(INFO) << "Unofficial build, but periodic update check interval "
              << "timeout is defined, so update is not blocked.";
  }
  */
// ---***FYDEOS END***---
  return EvalStatus::kContinue;
}

}  // namespace chromeos_update_manager
