#include <eeros/sequencer/ExceptionSequence.hpp>
#include <eeros/core/Fault.hpp>

namespace eeros {
namespace sequencer {

ExceptionSequence::ExceptionSequence(const std::string& name, const BT::NodeConfig& config,
                                      BT::BehaviorTreeFactory* factory)
    : StatefulActionNode(name, config), factory_(factory), log_(logger::Logger::getLogger('X'))
{}

BT::PortsList ExceptionSequence::providedPorts()
{
  return { BT::InputPort<std::string>("work_tree_id", "",
               "REQUIRED: ID of a <BehaviorTree> ticked INLINE (blocking) as this "
               "exception handler's work. For a simple fixed backoff, this can be "
               "as small as a single <Wait msec=\"3000\"/>. To decide restart-vs-"
               "abort, have the work-tree write a MonitorBehavior to its own "
               "blackboard key \"monitor_behavior\" (default RESUME if never set) "
               "BEFORE any Wait -- e.g. <Script code=\"monitor_behavior:=RESTART\"/> "
               "for an uncapped restart, or Counter + CounterExceeded + Script for "
               "a capped one before switching to ABORT"),
           BT::InputPort<std::string>("reset_keys", "",
               "comma-separated list of blackboard keys to reset after the work "
               "(empty = none) -- same convention as elsewhere in this library"),
           BT::InputPort<std::string>("restart_flag_key", "",
               "optional blackboard key to write true/false to -- read by "
               "sibling Wait/ThreadedSubtree nodes to decide whether a halt "
               "means 'discard progress, this is a genuine restart' (true) or "
               "'just pause' (false). Written in onStart(), BEFORE the work "
               "begins, since interrupted children are halted the moment this "
               "node starts running, not when it finishes") };
}

BT::NodeStatus ExceptionSequence::onStart()
{
  std::string owner = config().blackboard->get<std::string>("sequence_name");

  log_.info() << "create sequence '" << name() << "' (blocking), caller sequence: '" << owner << "'";
  log_.info() << "start '" << name() << "'";

  std::string work_tree_id;
  getInput("work_tree_id", work_tree_id);
  if (work_tree_id.empty())
  {
    throw eeros::Fault("ExceptionSequence: work_tree_id is required (e.g. a "
                            "<BehaviorTree> containing just <Wait msec=\"...\"/>)");
  }

  AbortFlag         abort_flag      = config().blackboard->get<AbortFlag>("abort_flag");
  ThreadRegistryPtr thread_registry = config().blackboard->get<ThreadRegistryPtr>("thread_registry");

  work_tree_ = std::make_unique<BT::Tree>(factory_->createTree(work_tree_id));
  SeedSharedState(work_tree_->rootBlackboard(), abort_flag, thread_registry);
  work_tree_->rootBlackboard()->set<std::string>("sequence_name", name());

  work_tree_->rootBlackboard()->set<std::shared_ptr<Resettable>>(
      "restart_count", config().blackboard->get<std::shared_ptr<Resettable>>("restart_count"));

  work_tree_->rootBlackboard()->set<int>("monitor_behavior", static_cast<int>(MonitorBehavior::RESUME)); // default

  // Tick once is required here to ensure monitor_behavior has the correct inital value already.
  // This is what makes it possible to define the monitor_behavior in a Script node on top of the subtree spawned by this tree node.
  work_tree_->tickOnce();

  int monitor_behavior = static_cast<int>(MonitorBehavior::RESUME);
  (void)work_tree_->rootBlackboard()->get<int>("monitor_behavior", monitor_behavior);

  will_abort_ = (monitor_behavior == static_cast<int>(MonitorBehavior::ABORT));
  is_restart_ = (monitor_behavior == static_cast<int>(MonitorBehavior::RESTART));

  std::string restart_flag_key;
  getInput("restart_flag_key", restart_flag_key);
  if (!restart_flag_key.empty())
  {
    WriteRestartFlag(config().blackboard, restart_flag_key, is_restart_);
  }

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus ExceptionSequence::onRunning()
{
  BT::NodeStatus st = work_tree_->tickOnce();
  if (st == BT::NodeStatus::RUNNING)
  {
    return BT::NodeStatus::RUNNING;
  }

  log_.info() << "sequence '" << name() << "' terminated";

  std::string reset_keys_csv;
  getInput("reset_keys", reset_keys_csv);
  for (const BT::StringView& key : BT::splitString(reset_keys_csv, ','))
  {
    config().blackboard->get<std::shared_ptr<Resettable>>(std::string(key))->reset();
  }

  if (will_abort_)
  {
    return BT::NodeStatus::FAILURE;
  }

  if (is_restart_)
  {
    std::string owner = config().blackboard->get<std::string>("sequence_name");
    log_.info() << "restart sequence '" << owner << "'";
    log_.info() << "start '" << owner << "'";
  }

  return BT::NodeStatus::SUCCESS;
}

void ExceptionSequence::onHalted() {}

}
}
