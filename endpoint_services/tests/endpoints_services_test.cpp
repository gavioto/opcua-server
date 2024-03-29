/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "endpoints_services_test.h"
#include "../../services_registry/tests/services_registry_test.h"

#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>


using namespace testing;

class EndpointsRegistry :public Test
{
protected:
  virtual void SetUp()
  {
    Addons = Common::CreateAddonsManager();
    OpcUa::Test::RegisterServicesRegistry(*Addons);
    OpcUa::Test::RegisterEndpointsServicesAddon(*Addons);
    Addons->Start();
  }

  virtual void TearDown()
  {
    Addons->Stop();
    Addons.reset();
  }

protected:
  std::unique_ptr<Common::AddonsManager> Addons;
};

TEST_F(EndpointsRegistry, CanBeCreated)
{
  OpcUa::UaServer::EndpointsServicesAddon::SharedPtr endpoints;

  ASSERT_NO_THROW(endpoints = Addons->GetAddon<OpcUa::UaServer::EndpointsServicesAddon>(OpcUa::UaServer::EndpointsServicesAddonID));
  ASSERT_TRUE(static_cast<bool>(endpoints));
}

TEST_F(EndpointsRegistry, RegisterEndpoints)
{
  OpcUa::EndpointDescription desc;
  desc.EndpointURL = "url";

  OpcUa::UaServer::EndpointsServicesAddon::SharedPtr endpoints = Addons->GetAddon<OpcUa::UaServer::EndpointsServicesAddon>(OpcUa::UaServer::EndpointsServicesAddonID);
  endpoints->AddEndpoints(std::vector<OpcUa::EndpointDescription>(1, desc));

  OpcUa::UaServer::ServicesRegistryAddon::SharedPtr services = Addons->GetAddon<OpcUa::UaServer::ServicesRegistryAddon>(OpcUa::UaServer::ServicesRegistryAddonID);
  OpcUa::Remote::Server::SharedPtr computer = services->GetServer();
  std::vector<OpcUa::EndpointDescription> descriptions;
  ASSERT_NO_THROW(descriptions = computer->Endpoints()->GetEndpoints(OpcUa::EndpointsFilter()));
  ASSERT_EQ(descriptions.size(), 1);
  ASSERT_EQ(descriptions[0].EndpointURL, "url");
}
