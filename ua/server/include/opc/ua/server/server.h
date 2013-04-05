/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef _OPC_UA_BINARY_SERVER_H
#define _OPC_UA_BINARY_SERVER_H

#include <opc/ua/channel.h>

#include <memory>

namespace OpcUa
{
  namespace Server
  {

    class IncomingConnectionProcessor : private Interface
    {
    public:
      virtual void Process(std::unique_ptr<IOChannel> clientChannel) = 0;
    };

    class ConnectionListener : private Interface
    {
    public:
      virtual void Start(std::unique_ptr<IncomingConnectionProcessor> connectionProcssor) = 0;
      virtual void Stop() = 0;
    };

  } // namespace Server
}  // namespace OpcUA

#endif // _OPC_UA_BINARY_SERVER_H
