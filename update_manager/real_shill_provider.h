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

#ifndef UPDATE_ENGINE_UPDATE_MANAGER_REAL_SHILL_PROVIDER_H_
#define UPDATE_ENGINE_UPDATE_MANAGER_REAL_SHILL_PROVIDER_H_

// TODO(garnold) Much of the functionality in this module was adapted from the
// update engine's connection_manager.  We need to make sure to deprecate use of
// connection manager when the time comes.

#include <memory>
#include <string>

#include <base/time/time.h>
#include <dbus/object_path.h>

#include "update_engine/common/system_state.h"
#include "update_engine/cros/shill_proxy_interface.h"
#include "update_engine/update_manager/generic_variables.h"
#include "update_engine/update_manager/shill_provider.h"

namespace chromeos_update_manager {

// ShillProvider concrete implementation.
class RealShillProvider : public ShillProvider {
 public:
  explicit RealShillProvider(
      chromeos_update_engine::ShillProxyInterface* shill_proxy)
      : shill_proxy_(shill_proxy) {}
  RealShillProvider(const RealShillProvider&) = delete;
  RealShillProvider& operator=(const RealShillProvider&) = delete;

  ~RealShillProvider() override = default;

  // Initializes the provider and returns whether it succeeded.
  bool Init();

  Variable<bool>* var_is_connected() override { return &var_is_connected_; }

  Variable<chromeos_update_engine::ConnectionType>* var_conn_type() override {
    return &var_conn_type_;
  }

  Variable<bool>* var_is_metered() override { return &var_is_metered_; }

  Variable<base::Time>* var_conn_last_changed() override {
    return &var_conn_last_changed_;
  }

 private:
  // A handler for ManagerProxy.PropertyChanged signal.
  void OnManagerPropertyChanged(const std::string& name,
                                const brillo::Any& value);

  // Called when the signal in ManagerProxy.PropertyChanged is connected.
  void OnSignalConnected(const std::string& interface_name,
                         const std::string& signal_name,
                         bool successful);

  // Get the connection and populate the type of the given default connection.
  bool ProcessDefaultService(const dbus::ObjectPath& default_service_path);

  // The current default service path, if connected. "/" means not connected.
  dbus::ObjectPath default_service_path_{"uninitialized"};

  // The mockable interface to access the shill DBus proxies.
  std::unique_ptr<chromeos_update_engine::ShillProxyInterface> shill_proxy_;

  // The provider's variables.
  AsyncCopyVariable<bool> var_is_connected_{"is_connected"};
  AsyncCopyVariable<chromeos_update_engine::ConnectionType> var_conn_type_{
      "conn_type"};
  AsyncCopyVariable<bool> var_is_metered_{"is_metered"};
  AsyncCopyVariable<base::Time> var_conn_last_changed_{"conn_last_changed"};
};

}  // namespace chromeos_update_manager

#endif  // UPDATE_ENGINE_UPDATE_MANAGER_REAL_SHILL_PROVIDER_H_
