#include <eeros/sequencer/ThreadedSubtree.hpp>
#include <eeros/sequencer/Common.hpp>

namespace eeros {
namespace sequencer {

ThreadedSubtree::ThreadedSubtree(const std::string& name, const BT::NodeConfig& config,
                                  BT::BehaviorTreeFactory* factory)
    : PreconditionedAction(name, config), factory_(factory)
{}

BT::PortsList ThreadedSubtree::providedPorts()
{
  return { BT::InputPort<std::string>("tree_id", "", "ID of the <BehaviorTree> to run as a sub-sequence") };
}

BT::NodeStatus ThreadedSubtree::onStartAction()
{
  if (fut_.valid() && fut_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
    fut_.get();
    fut_ = std::shared_future<BT::NodeStatus>{};
  }

  if (!fut_.valid()) {
    std::string tree_id;
    getInput("tree_id", tree_id);

    std::string               caller          = config().blackboard->get<std::string>("sequence_name");
    AbortFlag                 abort_flag      = config().blackboard->get<AbortFlag>("abort_flag");
    ThreadRegistryPtr         thread_registry = config().blackboard->get<ThreadRegistryPtr>("thread_registry");
    std::string               node_name       = name();
    BT::BehaviorTreeFactory*  factory         = factory_;

    log_.info() << "start thread for sequence '" << node_name << "' (non-blocking), caller sequence: '" << caller << "'";
    log_.info() << "start '" << node_name << "'";

    fut_ = std::async(std::launch::async,
        [factory, tree_id, abort_flag, thread_registry, node_name]() -> BT::NodeStatus
        {
          BT::Tree sub_tree = factory->createTree(tree_id);
          SeedSharedState(sub_tree.rootBlackboard(), abort_flag, thread_registry);
          sub_tree.rootBlackboard()->set<std::string>("sequence_name", node_name);
          return sub_tree.tickWhileRunning();
        }).share();

    thread_registry->Track(fut_);
  }

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus ThreadedSubtree::onRunningAction()
{
  if (fut_.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
    return BT::NodeStatus::RUNNING;
  }

  BT::NodeStatus st = fut_.get();
  fut_ = std::shared_future<BT::NodeStatus>{};
  log_.info() << "sequence '" << name() << "' terminated";
  return st;
}

}
}
