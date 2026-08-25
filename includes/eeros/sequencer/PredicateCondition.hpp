#ifndef ORG_EEROS_SEQUENCER_PREDICATE_CONDITION_HPP_
#define ORG_EEROS_SEQUENCER_PREDICATE_CONDITION_HPP_
#include <behaviortree_cpp/bt_factory.h>
#include <functional>
#include <string>

namespace eeros {
namespace sequencer {

/**
 * @brief Generic @ref BT::ConditionNode wrapping arbitrary user-supplied predicate. To allow for the writing of one-off custom conditions.
 */
class PredicateCondition : public BT::ConditionNode {
public:
  using Predicate = std::function<bool(BT::Blackboard::Ptr, const std::string& state_key)>;

  PredicateCondition(const std::string& name, const BT::NodeConfig& config, Predicate predicate);

  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;

private:
  Predicate predicate_;
};

/**
 * @brief Register a custom predicate condition with the given behavior tree factory.
 * 
 * @param factory Factory that we want to register our custom condition with.
 * @param tag_name Name of the condition.
 * @param predicate Predicate that should be called.
 */
void RegisterPredicateCondition(BT::BehaviorTreeFactory& factory, const std::string& tag_name,
                                 PredicateCondition::Predicate predicate);

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_PREDICATE_CONDITION_HPP_
