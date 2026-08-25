#include <eeros/sequencer/ResumableSequence.hpp>
#include <eeros/sequencer/Common.hpp>

namespace eeros {
namespace sequencer {

ResumableSequence::ResumableSequence(const std::string& name, const BT::NodeConfig& config)
    : ControlNode(name, config)
{}

BT::PortsList ResumableSequence::providedPorts()
{
  return { BT::InputPort<std::string>("restart_flag_key", "",
               "optional blackboard key (written by a sibling ExceptionSequence) "
               "-- if set and true at the moment this node is halted, resets "
               "current_child_ to 0 AND cascades haltChildren(), so this is a "
               "genuine restart, not a pause. Empty/unset = always preserve "
               "position (this node's original behavior)") };
}

BT::NodeStatus ResumableSequence::tick()
{
  if (!started_) {
    current_child_ = 0;
    started_       = true;
  }

  while (current_child_ < children_nodes_.size())   {
    BT::NodeStatus child_status = children_nodes_[current_child_]->executeTick();

    if (child_status == BT::NodeStatus::RUNNING) {
      return BT::NodeStatus::RUNNING;
    }
    if (child_status == BT::NodeStatus::FAILURE) {
      current_child_ = 0;
      started_        = false;
      haltChildren(); // a genuine failure: clean up fully, unlike a pause
      return BT::NodeStatus::FAILURE;
    }
    ++current_child_; // SUCCESS -- advance and keep going within this same tick
  }

  // All children succeeded.
  current_child_ = 0;
  started_        = false;
  return BT::NodeStatus::SUCCESS;
}

void ResumableSequence::halt()
{
  std::string restart_flag_key;
  getInput("restart_flag_key", restart_flag_key);

  if (!restart_flag_key.empty() && ReadRestartFlag(config().blackboard, restart_flag_key)) {
    // Genuine restart
    current_child_ = 0;
    started_        = false;
    haltChildren();
  }
  // else: resume (default) -- preserve current_child_/started_

  resetStatus();
}

}
}
