#ifndef ORG_EEROS_SEQUENCER_WAIT_HPP_
#define ORG_EEROS_SEQUENCER_WAIT_HPP_
#include <eeros/sequencer/RestartableAction.hpp>

namespace eeros {
namespace sequencer {

/**
 * @brief Action implementing waiting for a certain amount of time before returning success and returning running otherwise.
 */
class Wait : public RestartableAction {
public:
  Wait(const std::string& name, const BT::NodeConfig& config);
  static BT::PortsList providedPorts();

protected:
  bool checkPreCondition() override;
  BT::NodeStatus onStartAction() override;
  BT::NodeStatus onRunningAction() override;
  void onRestartAction() override;

private:
  bool              has_deadline_ = false;
  Clock::time_point deadline_;
  int               invocation_count_ = 0;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_WAIT_HPP_
