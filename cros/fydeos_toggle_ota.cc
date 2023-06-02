#include "update_engine/cros/fydeos_toggle_ota.h"
#include <base/files/file_util.h>

namespace fydeos {
namespace {
  // keep it the same with chromium src fydeos/misc/fydeos_toggle_ota.cc
  const char kFydeOSOTAIndicatorFile[] = "/mnt/stateful_partition/unencrypted/preserve/.disable_fydeos_ota";
}

bool OTAEnabled() {
  const bool enabled = !base::PathExists(base::FilePath(kFydeOSOTAIndicatorFile));
  return enabled;
}

} // fydeos
