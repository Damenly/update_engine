#ifndef UPDATE_ENGINE_FYDEOS_LICENSE_CHECKER_H
#define UPDATE_ENGINE_FYDEOS_LICENSE_CHECKER_H

#include <string>
#include <memory>

namespace fydeos {

class FydeLicenseChecker {
  public:
    explicit FydeLicenseChecker();
    ~FydeLicenseChecker();

    static FydeLicenseChecker& Get() { return *license_checker_singleton_; }
    void Init();
    std::string GetLicenseId() const { return license_id_; };

  private:
    static FydeLicenseChecker* license_checker_singleton_;

    std::string license_id_;
};

} // namespace fydeos
#endif /* ifndef UPDATE_ENGINE_FYDEOS_LICENSE_CHECKER_H */
