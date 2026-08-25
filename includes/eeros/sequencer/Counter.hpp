#ifndef ORG_EEROS_SEQUENCER_COUNTER_HPP_
#define ORG_EEROS_SEQUENCER_COUNTER_HPP_
#include <behaviortree_cpp/bt_factory.h>

namespace eeros {
namespace sequencer {

// Increments every WatchedValue<int> listed in state_keys (comma-
// separated, same convention as reset_keys), each tick. The FIRST key
// listed is checked against target to decide when this node succeeds;
// any additional keys are incremented alongside it with no target check
// of their own -- e.g. one driving loop termination, another tracking a
// separate Monitor's own count. Covers both single-counter usage (one
// key listed) and multi-counter usage (several) with the same port.
class Counter : public BT::StatefulActionNode
{
public:
  Counter(const std::string& name, const BT::NodeConfig& config);
  static BT::PortsList providedPorts();

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

private:
  BT::NodeStatus checkProgress();
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_COUNTER_HPP_
