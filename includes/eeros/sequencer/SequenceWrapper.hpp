#ifndef ORG_EEROS_SEQUENCER_SEQUENCE_WRAPPER_HPP_
#define ORG_EEROS_SEQUENCER_SEQUENCE_WRAPPER_HPP_
#include <behaviortree_cpp/bt_factory.h>
#include <eeros/logger/Logger.hpp>

namespace eeros {
namespace sequencer {

/**
 * @brief Wrapper node handling the logging for arbitrary StatefulActionNode implementations.
 */
class SequenceWrapper : public BT::DecoratorNode
{
public:
  SequenceWrapper(const std::string& name, const BT::NodeConfig& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;

private:
  logger::Logger log_;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_SEQUENCE_WRAPPER_HPP_
