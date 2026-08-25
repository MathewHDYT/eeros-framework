#include <eeros/sequencer/MonitorWrapper.hpp>
#include <eeros/sequencer/Common.hpp>

namespace eeros {
namespace sequencer {

MonitorWrapper::MonitorWrapper(const std::string& name, const BT::NodeConfig& config)
    : DecoratorNode(name, config), log_(logger::Logger::getLogger('X'))
{}

BT::PortsList MonitorWrapper::providedPorts()
{
  return { BT::InputPort<std::string>("monitor_name", "monitor", "name reported in the fired-log line") };
}

BT::NodeStatus MonitorWrapper::tick()
{
  BT::NodeStatus child_status = child_node_->executeTick();

  if (child_status == BT::NodeStatus::FAILURE) {
    if (!already_logged_) {
      std::string owner = config().blackboard->get<std::string>("sequence_name");
      std::string mon_name = "monitor";
      getInput("monitor_name", mon_name);
      log_.info() << "monitor '" << mon_name << "' of '" << owner << "' fired";
      already_logged_ = true;
    }
  }
  else {
    already_logged_ = false;
  }

  return child_status;
}

}
}
