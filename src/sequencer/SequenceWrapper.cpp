#include <eeros/sequencer/SequenceWrapper.hpp>
#include <eeros/sequencer/Common.hpp>

namespace eeros {
namespace sequencer {

SequenceWrapper::SequenceWrapper(const std::string& name, const BT::NodeConfig& config)
    : DecoratorNode(name, config), log_(logger::Logger::getLogger('X'))
{}

BT::PortsList SequenceWrapper::providedPorts()
{
  return { BT::InputPort<std::string>("caller", "no caller", "logs a creation line naming the caller"),
           BT::InputPort<bool>("blocking", true,
               "true: 'create sequence ... (blocking)' phrasing. "
               "false: 'start thread for sequence ... (non-blocking)' phrasing") };
}

BT::NodeStatus SequenceWrapper::tick()
{
  config().blackboard->set<std::string>("sequence_name", name());

  if (status() == BT::NodeStatus::IDLE)
  {
    std::string caller;
    getInput("caller", caller);
    if (!caller.empty())
    {
      bool blocking = true;
      getInput("blocking", blocking);
      if (blocking)
      {
        log_.info() << "create sequence '" << name() << "' (blocking), caller sequence: '" << caller << "'";
      }
      else
      {
        log_.info() << "start thread for sequence '" << name() << "' (non-blocking), caller sequence: '" << caller << "'";
      }
    }
    log_.info() << "start '" << name() << "'";
  }

  BT::NodeStatus child_status = child_node_->executeTick();

  if (child_status == BT::NodeStatus::SUCCESS || child_status == BT::NodeStatus::FAILURE)
  {
    log_.info() << "sequence '" << name() << "' terminated";
    resetChild();
  }
  return child_status;
}

}
}
