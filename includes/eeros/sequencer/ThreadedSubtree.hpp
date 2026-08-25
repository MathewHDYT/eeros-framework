#ifndef ORG_EEROS_SEQUENCER_THREADED_SUBTREE_HPP_
#define ORG_EEROS_SEQUENCER_THREADED_SUBTREE_HPP_
#include <eeros/sequencer/PreconditionedAction.hpp>
#include <future>

namespace eeros {
namespace sequencer {

/**
 * @brief Subtree running on a different thread as the tree that spawned it, subscribed itself to the @ref ThreadRegistry to ensure it is possible to wait for it's execution to finish.
 */
class ThreadedSubtree : public PreconditionedAction {
public:
  ThreadedSubtree(const std::string& name, const BT::NodeConfig& config,
                   BT::BehaviorTreeFactory* factory);

  static BT::PortsList providedPorts();

protected:
  BT::NodeStatus onStartAction() override;
  BT::NodeStatus onRunningAction() override;

private:
  BT::BehaviorTreeFactory*           factory_;
  std::shared_future<BT::NodeStatus> fut_;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_THREADED_SUBTREE_HPP_
