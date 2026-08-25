#ifndef ORG_EEROS_SEQUENCER_RESTARTABLE_ACTION_HPP_
#define ORG_EEROS_SEQUENCER_RESTARTABLE_ACTION_HPP_
#include <eeros/sequencer/PreconditionedAction.hpp>

namespace eeros {
namespace sequencer {

/**
 * @brief Wrapper for the precondition action adding a seperate virtual method that is executed if the node should be restarted instead of only resumed.
 */
class RestartableAction : public PreconditionedAction {
public:
  RestartableAction(const std::string& name, const BT::NodeConfig& config)
      : PreconditionedAction(name, config)
  {}

protected:
  /**
   * @brief If the node is halted and instead of resuming should reset internal state this method is called instead.
   */
  virtual void onRestartAction() = 0;

private:
  void onHaltedAction() final
  {
    std::string restart_flag_key;
    getInput("restart_flag_key", restart_flag_key);
    if (!restart_flag_key.empty() && ReadRestartFlag(config().blackboard, restart_flag_key))
    {
      onRestartAction();
    }
    // else: resume (default) -- do nothing, preserve state as-is.
  }
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_RESTARTABLE_ACTION_HPP_
