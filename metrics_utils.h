//
// Copyright (C) 2015 The Android Open Source Project
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

#ifndef UPDATE_ENGINE_METRICS_UTILS_H_
#define UPDATE_ENGINE_METRICS_UTILS_H_

#include <string>

#include <base/time/time.h>

#include "update_engine/common/clock_interface.h"
#include "update_engine/common/connection_utils.h"
#include "update_engine/common/error_code.h"
#include "update_engine/common/metrics_constants.h"
#include "update_engine/common/metrics_reporter_interface.h"
#include "update_engine/common/prefs_interface.h"

namespace chromeos_update_engine {

namespace metrics_utils {

// Transforms a ErrorCode value into a metrics::DownloadErrorCode.
// This obviously only works for errors related to downloading so if |code|
// is e.g. |ErrorCode::kFilesystemCopierError| then
// |kDownloadErrorCodeInputMalformed| is returned.
metrics::DownloadErrorCode GetDownloadErrorCode(ErrorCode code);

// Transforms a ErrorCode value into a metrics::AttemptResult.
//
// If metrics::AttemptResult::kPayloadDownloadError is returned, you
// can use utils::GetDownloadError() to get more detail.
metrics::AttemptResult GetAttemptResult(ErrorCode code);

// Calculates the internet connection type given |type| and |metered|.
metrics::ConnectionType GetConnectionType(ConnectionType type, bool metered);

// Returns the persisted value from prefs for the given key. It also
// validates that the value returned is non-negative.
int64_t GetPersistedValue(const std::string& key, PrefsInterface* prefs);

// Persists the reboot count of the update attempt to |kPrefsNumReboots|.
void SetNumReboots(int64_t num_reboots, PrefsInterface* prefs);

// Persists the payload attempt number to |kPrefsPayloadAttemptNumber|.
void SetPayloadAttemptNumber(int64_t payload_attempt_number,
                             PrefsInterface* prefs);

// Persists the finished time of an update to the |kPrefsSystemUpdatedMarker|.
void SetSystemUpdatedMarker(ClockInterface* clock, PrefsInterface* prefs);

// Persists the start monotonic time of an update to
// |kPrefsUpdateTimestampStart|.
void SetUpdateTimestampStart(const base::Time& update_start_time,
                             PrefsInterface* prefs);

// Persists the start boot time of an update to
// |kPrefsUpdateBootTimestampStart|.
void SetUpdateBootTimestampStart(const base::Time& update_start_boot_time,
                                 PrefsInterface* prefs);

// Called at program startup if the device booted into a new update.
// The |time_to_reboot| parameter contains the (monotonic-clock) duration
// from when the update successfully completed (the value in
// |kPrefsSystemUpdatedMarker|) until the device was booted into the update
// (current monotonic-clock time).
bool LoadAndReportTimeToReboot(MetricsReporterInterface* metrics_reporter,
                               PrefsInterface* prefs,
                               ClockInterface* clock);

}  // namespace metrics_utils
}  // namespace chromeos_update_engine

#endif  // UPDATE_ENGINE_METRICS_UTILS_H_
