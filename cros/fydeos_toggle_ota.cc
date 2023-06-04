#include "update_engine/cros/fydeos_toggle_ota.h"
#include <base/files/file_util.h>

#include "update_engine/common/subprocess.h"

namespace fydeos {
namespace {
  // keep it the same with chromium src fydeos/misc/fydeos_toggle_ota.cc
  const char kFydeOSOTAIndicatorFile[] = "/mnt/stateful_partition/unencrypted/preserve/.disable_fydeos_ota";

  const char kShellCmd[] = "/usr/share/fydeos_shell/ota-checker.sh";
}

bool OTAEnabled() {
  const bool enabled = !base::PathExists(base::FilePath(kFydeOSOTAIndicatorFile));
  return enabled;
}

bool OTANeeded(std::string version) {
  int exit_code = 0;
  std::string out, err;
  std::vector<std::string> cmd = {kShellCmd, version.c_str()};

  if (!chromeos_update_engine::Subprocess::SynchronousExec(cmd, &exit_code, &out, &err)) {
    LOG(ERROR) << kShellCmd << " error";
    return true;
  }

  LOG(INFO) << kShellCmd << " exit code: " << exit_code
            << "\n stdout: \n"<< out
            << "\n stderr: \n"<< err;

  if (exit_code == EEXIST)
    return false;

  return true;
}

} // fydeos
