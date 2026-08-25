#ifndef ORG_EEROS_SEQUENCER_EXCEPTION_SEQUENCE_HPP_
#define ORG_EEROS_SEQUENCER_EXCEPTION_SEQUENCE_HPP_
#include <behaviortree_cpp/bt_factory.h>
#include <eeros/sequencer/Common.hpp>
#include <eeros/logger/Logger.hpp>
#include <memory>

namespace eeros {
namespace sequencer {

/**
 * @brief Generic exception handling spawning a new subtree, meant to handle failure cases inside the behavior tree and changes behavior dependening on the currently configured @ref MonitorBehavior.
 */
class ExceptionSequence : public BT::StatefulActionNode {
public:
  ExceptionSequence(const std::string& name, const BT::NodeConfig& config,
                     BT::BehaviorTreeFactory* factory);
  static BT::PortsList providedPorts();

  BT::NodeStatus onStart() override;
  BT::NodeStatus onRunning() override;
  void onHalted() override;

private:
  BT::BehaviorTreeFactory*  factory_;
  std::unique_ptr<BT::Tree> work_tree_;
  bool                      will_abort_ = false;
  bool                      is_restart_ = false;
  logger::Logger            log_;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_EXCEPTION_SEQUENCE_HPP_
