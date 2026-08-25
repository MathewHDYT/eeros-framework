#ifndef ORG_EEROS_SEQUENCER_STEP_HPP_
#define ORG_EEROS_SEQUENCER_STEP_HPP_
#include <eeros/sequencer/RestartableAction.hpp>

namespace eeros {
namespace sequencer {

/**
 * @brief Used for leaf conditions, whose completion is decided by an externally polled condition, rather than fixed-time deadline.
 */
class Step : public RestartableAction {
public:
  Step(const std::string& name, const BT::NodeConfig& config);

protected:
  BT::NodeStatus onStartAction() final;
  BT::NodeStatus onRunningAction() final;
  void onRestartAction() override;

  /**
   * @brief The one-time kickoff (e.g. "start moving toward this target").
   */
  virtual void doAction() = 0;

  /**
   * @brief Polled every tick after @ref doAction() runs, until it returns true.
   */
  virtual bool checkExitCondition() = 0;

private:
  bool has_started_ = false;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_STEP_HPP_
