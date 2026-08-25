#include <eeros/sequencer/AbortGuard.hpp>
#include <eeros/sequencer/Common.hpp>

namespace eeros {
namespace sequencer {

AbortGuard::AbortGuard(const std::string& name, const BT::NodeConfig& config)
    : ConditionNode(name, config)
{}

BT::PortsList AbortGuard::providedPorts() { return {}; }

BT::NodeStatus AbortGuard::tick()
{
  auto flag = config().blackboard->get<AbortFlag>("abort_flag");
  return flag->load() ? BT::NodeStatus::FAILURE : BT::NodeStatus::SUCCESS;
}

}
}
