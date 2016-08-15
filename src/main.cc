#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <core/exec_manager.h>
#include <util/factory.h>

namespace po = boost::program_options;
namespace pt = boost::property_tree;

int main(int argc, char** argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "print help message")
    ("config", po::value<std::string>(), "configuration file")
  ;
  po::positional_options_description pos;
  pos.add("config", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
      options(desc).positional(pos).run(), vm);
  po::notify(vm);

  if (vm.count("help") || !vm.count("config")) {
    std::cout << desc << std::endl;
    exit(1);
  }

  auto config_file = vm["config"].as<std::string>();

  pt::ptree properties;
  pt::read_json(config_file, properties);

  // TODO
  auto exec_manager = aser::util::create<aser::exec_manager>(
      properties.get<std::string>("exec_manager.type"),
      static_cast<const pt::ptree&>(properties));
  exec_manager->start();

  return 0;
}

