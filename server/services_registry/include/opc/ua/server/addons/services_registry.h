/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Computer wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/addons_core/addon.h>
#include <opc/ua/computer.h>

namespace OpcUa
{
  namespace Server
  {

    class ServicesRegistryAddon : public Common::Addon
    {
    public:
      DEFINE_CLASS_POINTERS(ServicesRegistryAddon);

    public:
      virtual std::shared_ptr<OpcUa::Remote::Computer> GetComputer() const = 0;

      virtual void RegisterEndpointsServices(std::shared_ptr<OpcUa::Remote::EndpointServices> endpoints) = 0;
      virtual void UnregisterEndpointsServices() = 0;

      virtual void RegisterViewServices(std::shared_ptr<OpcUa::Remote::ViewServices> views) = 0;
      virtual void UnregisterViewServices() = 0;

      virtual void RegisterAttributeServices(std::shared_ptr<OpcUa::Remote::AttributeServices> attributes) = 0;
      virtual void UnregisterAttributeServices() = 0;
    };

    const char ServicesRegistryAddonID[] = "services_registry";

  }
}
