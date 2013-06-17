/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#ifndef ADDRESS_SPACE_ADDON_H_
#define ADDRESS_SPACE_ADDON_H_

#include <opc/common/addons_core/addon.h>

#include "address_space_internal.h"

namespace OpcUa
{
  namespace Internal
  {

    class AddressSpaceAddon
      : public Common::Addon
      , public AddressSpaceMultiplexor
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpaceAddon);
    };

    class AddressSpaceAddonFactory : public Common::AddonFactory
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpaceAddonFactory);

    public:
      virtual Common::Addon::UniquePtr CreateAddon();
    };

  }
} // namespace OpcUa

#endif /* ADDRESS_SPACE_ADDON_H_ */