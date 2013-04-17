/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <opc/ua/client/remote_connection.h>
#include <opc/ua/server/addons/builtin_computer.h>
#include <opc/ua/server/addons/endpoints.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/common/addons_core/dynamic_addon_factory.h>

#include <chrono>
#include <thread>

#include "common.h"

using namespace testing;

class EndpointsAddonTest : public Test
{
public:
  void SetUp()
  {
    Addons = OpcUa::LoadAddons(OpcUa::GetEndpointsConfigPath());
  }

  void TearDown()
  {
    Addons->Stop();
    Addons.reset();
  }

protected:
  std::unique_ptr<Common::AddonsManager> Addons;
};

TEST_F(EndpointsAddonTest, Loads)
{
  ASSERT_TRUE(static_cast<bool>(Addons->GetAddon(OpcUa::Server::EndpointsAddonID)));
}

TEST_F(EndpointsAddonTest, CanListEndpoints)
{
  std::shared_ptr<OpcUa::Server::BuiltinComputerAddon> computerAddon = Common::GetAddon<OpcUa::Server::BuiltinComputerAddon>(*Addons, OpcUa::Server::BuiltinComputerAddonID);
  ASSERT_TRUE(static_cast<bool>(computerAddon));
  std::shared_ptr<OpcUa::Remote::Computer> computer = computerAddon->GetComputer();
  ASSERT_TRUE(static_cast<bool>(computer));
  std::vector<OpcUa::EndpointDescription> endpoints;
  ASSERT_NO_THROW(endpoints = computer->Endpoints()->GetEndpoints(OpcUa::Remote::EndpointFilter()));
  EXPECT_FALSE(endpoints.empty());
}
