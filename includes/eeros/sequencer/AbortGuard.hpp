#ifndef ORG_EEROS_SEQUENCER_ABORT_GUARD_HPP_
#define ORG_EEROS_SEQUENCER_ABORT_GUARD_HPP_
#include <behaviortree_cpp/bt_factory.h>

namespace eeros {
namespace sequencer {

/**
 * @brief ConditionNode allowing to fail the entire SubTree if the @ref AbortFlag on the blackboard is set to true.
 */
class AbortGuard : public BT::ConditionNode {
public:
  AbortGuard(const std::string& name, const BT::NodeConfig& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_ABORT_GUARD_HPP_
