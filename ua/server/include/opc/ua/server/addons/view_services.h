/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief View services addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opc_ua_view_service_addon_h
#define opc_ua_view_service_addon_h

#include <opc/ua/protocol/types.h>
#include <opccore/common/addons_core/addon.h>

namespace OpcUa
{
  namespace Server
  {

    class ViewServicesAddon : public Common::Addon
    {
    };

    const char EndpointsServicesAddonID[] = "view_services";

  } // namespace Server
} // nmespace OpcUa

#endif // opc_ua_get_view_service_addon_h
