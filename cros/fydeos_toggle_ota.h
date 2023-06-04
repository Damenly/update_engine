#ifndef FYDEOS_TOGGLE_OTA_H_
#define FYDEOS_TOGGLE_OTA_H_

#include <base/files/file_util.h>

namespace fydeos {
  bool OTAEnabled();

  bool OTANeeded(std::string version);
}

#endif // ifndef FYDEOS_TOGGLE_OTA_H_
