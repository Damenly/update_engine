#include "update_engine/cros/fydeos_license_checker.h"

#include <base/logging.h>
#include <base/strings/stringprintf.h>
#include <base/json/json_reader.h>
#include <base/base64.h>
#include <base/values.h>
#include <crypto/signature_verifier.h>

#include "update_engine/common/subprocess.h"

namespace fydeos {

namespace {
  const char kShellCmd[] = "/usr/share/fydeos_shell/license-utils.sh";
  const char kParamId[] = "id";
} // namespace

FydeLicenseChecker::FydeLicenseChecker() = default;
FydeLicenseChecker::~FydeLicenseChecker() {
  if (license_checker_singleton_ == this) {
    license_checker_singleton_ = nullptr;
  }
}

void FydeLicenseChecker::Init() {
  if (license_checker_singleton_ == this)
    return;
  CHECK(license_checker_singleton_ == nullptr);
  license_checker_singleton_ = this;

  int exit_code = 0;
  std::string value, error;
  std::vector<std::string> cmd = {kShellCmd, kParamId};
  if (!chromeos_update_engine::Subprocess::SynchronousExec(cmd, &exit_code, &value, &error) || exit_code) {
    LOG(ERROR) << "Get fydeos license id error, exit_code: " << exit_code;
    return;
  }
  base::TrimWhitespaceASCII(value, base::TRIM_ALL, &license_id_);
  LOG(INFO) << "FydeOS license id: " << license_id_;
}

FydeLicenseChecker* FydeLicenseChecker::license_checker_singleton_ = nullptr;

} // namespace fydeos
