/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Common test utls.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "common.h"

#include <opc/common/addons_core/dynamic_addon_factory.h>
#include <src/server_options.h>

std::unique_ptr<Common::AddonsManager> OpcUa::Tests::LoadAddons(const std::string& configPath)
{
  std::string config = "--config=";
  config += configPath;

  char* argv[2];
  argv[0] =  "test.exe";
  argv[1] =   const_cast<char*>(config.c_str());
  OpcUa::UaServer::CommandLine cmdline(2, argv);
  OpcUa::UaServer::ModulesConfiguration modules = cmdline.GetModules();
  Common::AddonsManager::UniquePtr addons = Common::CreateAddonsManager();
  for (auto module : modules)
  {
    Common::AddonInformation config;
    config.ID = module.ID;
    config.Factory = Common::CreateDynamicAddonFactory(module.Path.c_str());
    config.Dependencies = module.DependsOn;
    config.Parameters = module.Parameters;
    addons->Register(config);
  }
  addons->Start();
  return addons;
}


