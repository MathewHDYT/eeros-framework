#include <eeros/sequencer/Wait.hpp>

namespace eeros {
namespace sequencer {

Wait::Wait(const std::string& name, const BT::NodeConfig& config)
    : RestartableAction(name, config)
{}

BT::PortsList Wait::providedPorts()
{
  return { BT::InputPort<int>("msec", 1000, "how long this step takes"),
           BT::InputPort<int>("skip_after", -1,
               "if >= 0, this step's precondition fails once it has already "
               "run this many times. -1 (default) = unlimited, matching "
               "EEROS's default checkPreCondition() == true"),
           BT::InputPort<std::string>("restart_flag_key", "",
               "optional blackboard key (written by a sibling ExceptionSequence) "
               "-- if set and true at the moment this node is halted, discards "
               "the in-progress deadline instead of preserving it, so the next "
               "start is genuinely fresh. Empty/unset = always preserve (this "
               "node's original behavior)") };
}

bool Wait::checkPreCondition()
{
  int skip_after = -1;
  getInput("skip_after", skip_after);
  if (skip_after < 0) {
    return true;
  }
  return invocation_count_ < skip_after;
}

BT::NodeStatus Wait::onStartAction()
{
  if (!has_deadline_) {
    ++invocation_count_;
    log_.info() << "start '" << name() << "'";
    int msec = 1000;
    getInput("msec", msec);
    deadline_     = Clock::now() + std::chrono::milliseconds(msec);
    has_deadline_ = true;
  }
  // else: resuming
  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus Wait::onRunningAction()
{
  if (Clock::now() < deadline_)
  {
    return BT::NodeStatus::RUNNING;
  }
  has_deadline_ = false;
  return BT::NodeStatus::SUCCESS;
}

void Wait::onRestartAction()
{
  has_deadline_ = false;
}

}
}
