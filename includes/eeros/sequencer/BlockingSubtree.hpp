#ifndef ORG_EEROS_SEQUENCER_BLOCKING_SUBTREE_HPP_
#define ORG_EEROS_SEQUENCER_BLOCKING_SUBTREE_HPP_
#include <eeros/sequencer/RestartableAction.hpp>
#include <memory>

namespace eeros {
namespace sequencer {

/**
 * @brief Subtree running on the same thread as the tree that spawned it.
 */
class BlockingSubtree : public RestartableAction {
public:
  BlockingSubtree(const std::string& name, const BT::NodeConfig& config,
                   BT::BehaviorTreeFactory* factory);

  static BT::PortsList providedPorts();

protected:
  BT::NodeStatus onStartAction() override;
  BT::NodeStatus onRunningAction() override;
  void onRestartAction() override;

private:
  BT::BehaviorTreeFactory*  factory_;
  std::unique_ptr<BT::Tree> sub_tree_;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_ABORT_GUARD_HPP_
