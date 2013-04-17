/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "internal/uri_facade.h"
#include "stream_computer.h"

#include <opc/ua/client/binary_computer.h>


std::unique_ptr<OpcUa::Remote::Computer> OpcUa::Remote::CreateBinaryComputer(std::shared_ptr<IOChannel> channel)
{
  return std::unique_ptr<OpcUa::Remote::Computer>(new OpcUa::Internal::Computer<OpcUa::Binary::IOStream>(channel));
}
