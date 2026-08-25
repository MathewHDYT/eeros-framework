#ifndef ORG_EEROS_SEQUENCER_CONDITIONS_HPP_
#define ORG_EEROS_SEQUENCER_CONDITIONS_HPP_
#include <behaviortree_cpp/bt_factory.h>

namespace eeros {
namespace sequencer {

/**
 * @brief Predicate that fails as soon as the timeout stored in the blackboard was exceeded.
 */
class ClockExceeded : public BT::ConditionNode
{
public:
  ClockExceeded(const std::string& name, const BT::NodeConfig& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
};

/**
 * @brief Predicate that fails as soon as the counter stored in the blackboard was exceeded.
 */
class CounterExceeded : public BT::ConditionNode
{
public:
  CounterExceeded(const std::string& name, const BT::NodeConfig& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_CONDITIONS_HPP_
