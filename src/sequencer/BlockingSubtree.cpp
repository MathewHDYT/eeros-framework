#include <eeros/sequencer/BlockingSubtree.hpp>
#include <eeros/sequencer/Common.hpp>

namespace eeros {
namespace sequencer {

BlockingSubtree::BlockingSubtree(const std::string& name, const BT::NodeConfig& config,
                                  BT::BehaviorTreeFactory* factory)
    : RestartableAction(name, config), factory_(factory)
{}

BT::PortsList BlockingSubtree::providedPorts()
{
  return { BT::InputPort<std::string>("tree_id", "", "ID of the <BehaviorTree> to run as a sub-sequence"),
           BT::InputPort<std::string>("restart_flag_key", "",
               "optional blackboard key (written by a sibling ExceptionSequence) "
               "-- if set and true at the moment this node is halted, discards "
               "sub_tree_ instead of preserving it, so the next start creates a "
               "genuinely fresh sub-tree. Empty/unset = always preserve (this "
               "node's original behavior)") };
}

BT::NodeStatus BlockingSubtree::onStartAction()
{
  if (sub_tree_ == nullptr) {
    std::string tree_id;
    getInput("tree_id", tree_id);

    std::string caller = config().blackboard->get<std::string>("sequence_name");

    AbortFlag         abort_flag      = config().blackboard->get<AbortFlag>("abort_flag");
    ThreadRegistryPtr thread_registry = config().blackboard->get<ThreadRegistryPtr>("thread_registry");

    sub_tree_ = std::make_unique<BT::Tree>(factory_->createTree(tree_id));
    SeedSharedState(sub_tree_->rootBlackboard(), abort_flag, thread_registry);
    sub_tree_->rootBlackboard()->set<std::string>("sequence_name", name());

    log_.info() << "create sequence '" << name() << "' (blocking), caller sequence: '" << caller << "'";
    log_.info() << "start '" << name() << "'";
  }
  // else: resuming

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus BlockingSubtree::onRunningAction()
{
  BT::NodeStatus st = sub_tree_->tickOnce();
  if (st == BT::NodeStatus::RUNNING) {
    return BT::NodeStatus::RUNNING;
  }

  log_.info() << "sequence '" << name() << "' terminated";
  sub_tree_.reset();
  return st;
}

void BlockingSubtree::onRestartAction()
{
  log_.info() << "sequence '" << name() << "' terminated";
  sub_tree_.reset();
}

}
}
