/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa binary protocol connection processor.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "opc_tcp_processor.h"

#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/session.h>

#include <iostream>
#include <stdexcept>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;
  using namespace OpcUa::Server;


  class OpcTcp : public IncomingConnectionProcessor
  {
  public:
    OpcTcp(std::shared_ptr<OpcUa::Remote::Computer> computer, bool debug)
      : Computer(computer)
      , Debug(debug)
      , ChannelID(1)
      , TokenID(2)
    {
    }

    virtual void Process(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
      if (!clientChannel)
      {
        if (Debug) std::cerr << "Empty channel passed to endpoints opc binary protocol processor." << std::endl;
        return;
      }

      if (Debug) std::clog << "Hello client!" << std::endl;
      OpcUa::Binary::IOStream stream(clientChannel);
      while(ProcessChunk(stream));
    }

    virtual void StopProcessing(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
    }

  private:
    bool ProcessChunk(OpcUa::Binary::IOStream& stream)
    {
      using namespace OpcUa::Binary;

      Header hdr;
      stream >> hdr;
      switch (hdr.Type)
      {
        case MT_HELLO:
        {
          if (Debug) std::clog << "Accepted hello message." << std::endl;
          HelloClient(stream);
          break;
        }


        case MT_SECURE_OPEN:
        {
          if (Debug) std::clog << "Opening securechannel." << std::endl;
          OpenChannel(stream);
          break;
        }

        case MT_SECURE_CLOSE:
        {
          if (Debug) std::clog << "Closing secure channel." << std::endl;
          CloseChannel(stream);
          return false;
        }

        case MT_SECURE_MESSAGE:
        {
          ProcessMessage(stream);
          break;
        }

        case MT_ACKNOWLEDGE:
        {
          if (Debug) std::clog << "Received acknowledge from client. He mustn't do that.." << std::endl;
          throw std::logic_error("Thank to client about acknowledge.");
        }
        case MT_ERROR:
        {
          if (Debug) std::clog << "There is an error happend in the client!" << std::endl;
          throw std::logic_error("It is very nice get to know server about error in the client.");
        }
        default:
        {
          if (Debug) std::clog << "Unknown message received!" << std::endl;
          throw std::logic_error("Invalid message type received.");
        }
      }
      return true;
    }

    void HelloClient(OpcUa::Binary::IOStream& stream)
    {
      using namespace OpcUa::Binary;

      if (Debug) std::clog << "Reading hello message." << std::endl;
      Hello hello;
      stream >> hello;

      Acknowledge ack;
      ack.ReceiveBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.SendBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.MaxMessageSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.MaxChunkCount = 1;

      Header ackHeader(MT_ACKNOWLEDGE, CHT_SINGLE);
      ackHeader.AddSize(RawSize(ack));
      if (Debug) std::clog << "Sending answer to client." << std::endl;
      stream << ackHeader << ack << flush; 
    }

    void OpenChannel(IOStream& stream)
    {
      uint32_t channelID = 0;
      stream >> channelID;
      AsymmetricAlgorithmHeader algorithmHeader;
      stream >> algorithmHeader;

      if (algorithmHeader.SecurityPolicyURI != "http://opcfoundation.org/UA/SecurityPolicy#None")
      {
        throw std::logic_error(std::string("Client want to create secure channel with invalid policy '") + algorithmHeader.SecurityPolicyURI + std::string("'"));
      }

      SequenceHeader sequence;
      stream >> sequence;

      OpenSecureChannelRequest request;
      stream >> request;

      if (request.RequestType != STR_ISSUE)
      {
        throw std::logic_error("Client have to create secure channel!");
      }

      if (request.SecurityMode != MSM_NONE)
      {
        throw std::logic_error("Unsupported security mode.");
      }

      OpenSecureChannelResponse response;
      FillResponseHeader(request.Header, response.Header);
      response.ChannelSecurityToken.SecureChannelID = ChannelID;
      response.ChannelSecurityToken.TokenID = TokenID;
      response.ChannelSecurityToken.CreatedAt = OpcUa::CurrentDateTime();
      response.ChannelSecurityToken.RevisedLifetime = 3600;


      SecureHeader responseHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
      responseHeader.AddSize(RawSize(algorithmHeader));
      responseHeader.AddSize(RawSize(sequence));
      responseHeader.AddSize(RawSize(response));
      stream << responseHeader << algorithmHeader << sequence << response << flush;
    }

    void CloseChannel(IOStream& stream)
    {
      uint32_t channelID = 0;
      stream >> channelID;
     
      SymmetricAlgorithmHeader algorithmHeader;
      stream >> algorithmHeader;

      SequenceHeader sequence;
      stream >> sequence;

      CloseSecureChannelRequest request;
      stream >> request;
    }

    void ProcessMessage(IOStream& stream)
    {
      uint32_t channelID = 0;
      stream >> channelID;
     
      SymmetricAlgorithmHeader algorithmHeader;
      stream >> algorithmHeader;

      SequenceHeader sequence;
      stream >> sequence;

      NodeID typeID;
      stream >> typeID;

      RequestHeader requestHeader;
      stream >> requestHeader;

      switch (GetMessageID(typeID))
      {
        case OpcUa::GET_ENDPOINTS_REQUEST:
        {
          if (Debug) std::clog << "Processing get endpoints request." << std::endl;
          EndpointsFilter filter;
          stream >> filter;

          GetEndpointsResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Endpoints = Computer->Endpoints()->GetEndpoints(filter);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case OpcUa::BROWSE_REQUEST:
        {
          if (Debug) std::clog << "Processing browse request." << std::endl;
          NodesQuery query;
          stream >> query;

          BrowseResponse response;
          FillResponseHeader(requestHeader, response.Header);

          for (auto node : query.NodesToBrowse)
          {
            BrowseResult result;
            OpcUa::Remote::BrowseParameters browseParams;
            browseParams.Description = node;
            browseParams.MaxReferenciesCount = query.MaxReferenciesPerNode;

            result.Referencies = Computer->Views()->Browse(browseParams);
            response.Results.push_back(result);
          }

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case CREATE_SESSION_REQUEST:
        {
          if (Debug) std::clog << "Processing create session request." << std::endl;
          SessionParameters params;
          stream >> params;

          CreateSessionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          response.Session.SessionID = SessionID;
          response.Session.AuthenticationToken = SessionID;
          response.Session.RevisedSessionTimeout = params.RequestedSessionTimeout;
          response.Session.MaxRequestMessageSize = 65536;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;

          return;
        }
        case ACTIVATE_SESSION_REQUEST:
        {
          if (Debug) std::clog << "Processing activate session request." << std::endl;
          UpdatedSessionParameters params;
          stream >> params;

          ActivateSessionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case CLOSE_SESSION_REQUEST:
        {
          if (Debug) std::clog << "Processing close session request." << std::endl;
          bool deleteSubscriptions = false;
          stream >> deleteSubscriptions;

          CloseSessionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }


        default:
        {
          // TODO add code of message ID to the text of exception
          throw std::logic_error("Unknown message type was recieved.");
        }
      }
    }

  private:
    void FillResponseHeader(const RequestHeader& requestHeader, ResponseHeader& responseHeader)
    {
       responseHeader.InnerDiagnostics.push_back(DiagnosticInfo());
       responseHeader.Timestamp = CurrentDateTime();
       responseHeader.RequestHandle = requestHeader.RequestHandle;
    }

  private:
    std::shared_ptr<OpcUa::Remote::Computer> Computer;
    bool Debug;
    uint32_t ChannelID;
    uint32_t TokenID;
    NodeID SessionID;
    NodeID AuthenticationToken;
  };

}

namespace OpcUa
{
  namespace Internal
  {

    std::unique_ptr<IncomingConnectionProcessor> CreateOpcTcpProcessor(std::shared_ptr<OpcUa::Remote::Computer> computer, bool debug)
    {
      return std::unique_ptr<IncomingConnectionProcessor>(new OpcTcp(computer, debug));
    }

  }
}
