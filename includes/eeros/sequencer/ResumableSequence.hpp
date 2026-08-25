#ifndef ORG_EEROS_SEQUENCER_RESUMABLE_SEQUENCE_HPP_
#define ORG_EEROS_SEQUENCER_RESUMABLE_SEQUENCE_HPP_
#include <behaviortree_cpp/bt_factory.h>
#include <cstddef>

namespace eeros {
namespace sequencer {

/**
 * @brief Mimicks the default @ref BT::Sequence implementation, but safes the index of children that were run to completion.
 * Allows to resume without restarting internal nodes. Necessary if multiple Actions can be interrupted by a MonitorCondition.
 */
class ResumableSequence : public BT::ControlNode {
public:
  ResumableSequence(const std::string& name, const BT::NodeConfig& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void halt() override;

private:
  size_t current_child_ = 0;
  bool   started_       = false;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_EXCEPTION_SEQUENCE_HPP_
