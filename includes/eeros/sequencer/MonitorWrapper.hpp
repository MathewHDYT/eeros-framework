#ifndef ORG_EEROS_SEQUENCER_MONITOR_WRAPPER_HPP_
#define ORG_EEROS_SEQUENCER_MONITOR_WRAPPER_HPP_
#include <behaviortree_cpp/bt_factory.h>
#include <eeros/logger/Logger.hpp>

namespace eeros {
namespace sequencer {

/**
 * @brief Wrapper node handling the logging for arbitrary ConditionNode implementations.
 */
class MonitorWrapper : public BT::DecoratorNode {
public:
  MonitorWrapper(const std::string& name, const BT::NodeConfig& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;

private:
  bool already_logged_ = false;
  logger::Logger log_;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_MONITOR_WRAPPER_HPP_
