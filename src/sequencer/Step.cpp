#include <eeros/sequencer/Step.hpp>

namespace eeros {
namespace sequencer {

Step::Step(const std::string& name, const BT::NodeConfig& config)
    : RestartableAction(name, config)
{}

BT::NodeStatus Step::onStartAction()
{
  if (!has_started_) {
    doAction();
    has_started_ = true;
  }
  // else: resuming after a pause
  return checkExitCondition() ? BT::NodeStatus::SUCCESS : BT::NodeStatus::RUNNING;
}

BT::NodeStatus Step::onRunningAction()
{
  if (checkExitCondition()) {
    has_started_ = false; // consumed -- the next onStartAction() is a genuinely fresh call
    return BT::NodeStatus::SUCCESS;
  }
  return BT::NodeStatus::RUNNING;
}

void Step::onRestartAction()
{
  has_started_ = false; // discard -- next onStartAction() calls doAction() fresh, not a resume
}

}
}
