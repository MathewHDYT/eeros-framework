#ifndef ORG_EEROS_SEQUENCER_PRECONDITION_ACTION_HPP_
#define ORG_EEROS_SEQUENCER_PRECONDITION_ACTION_HPP_
#include <behaviortree_cpp/bt_factory.h>
#include <eeros/sequencer/Common.hpp>
#include <eeros/logger/Logger.hpp>

namespace eeros {
namespace sequencer {

/**
 * @brief Wrapper for an arbitrary @ref BT::StatefulActionNode that additionally checks a precondition before actually running.
 */
class PreconditionedAction : public BT::StatefulActionNode {
public:
  PreconditionedAction(const std::string& name, const BT::NodeConfig& config)
      : StatefulActionNode(name, config), log_(logger::Logger::getLogger('X'))
  {}

  BT::NodeStatus onStart() final
  {
    if (!checkPreCondition())
    {
      log_.error() << "precondition failed for '" << name() << "' -- skipped";
      return BT::NodeStatus::FAILURE;
    }
    return onStartAction();
  }

  BT::NodeStatus onRunning() final { return onRunningAction(); }
  void onHalted() final { onHaltedAction(); }

protected:
  virtual bool checkPreCondition() { return true; }

  virtual BT::NodeStatus onStartAction()   = 0;
  virtual BT::NodeStatus onRunningAction() { return BT::NodeStatus::SUCCESS; }
  virtual void           onHaltedAction()  {}

  logger::Logger log_;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_PRECONDITION_ACTION_HPP_
