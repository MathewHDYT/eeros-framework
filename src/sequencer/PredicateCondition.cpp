#include <eeros/sequencer/PredicateCondition.hpp>

namespace eeros {
namespace sequencer {

PredicateCondition::PredicateCondition(const std::string& name, const BT::NodeConfig& config,
                                        Predicate predicate)
    : ConditionNode(name, config), predicate_(std::move(predicate))
{}

BT::PortsList PredicateCondition::providedPorts()
{
  return { BT::InputPort<std::string>("state_key", "monitor_state",
               "blackboard key passed to the predicate") };
}

BT::NodeStatus PredicateCondition::tick()
{
  std::string key = "monitor_state";
  getInput("state_key", key);

  bool triggered = predicate_(config().blackboard, key);
  return triggered ? BT::NodeStatus::FAILURE : BT::NodeStatus::SUCCESS;
}

void RegisterPredicateCondition(BT::BehaviorTreeFactory& factory, const std::string& tag_name,
                                 PredicateCondition::Predicate predicate)
{
  factory.registerBuilder<PredicateCondition>(
      tag_name,
      [predicate](const std::string& name, const BT::NodeConfig& config)
      {
        return std::make_unique<PredicateCondition>(name, config, predicate);
      });
}

}
}
