#ifndef ORG_EEROS_SEQUENCER_REGISTER_ALL_HPP_
#define ORG_EEROS_SEQUENCER_REGISTER_ALL_HPP_
#include <behaviortree_cpp/bt_factory.h>

namespace eeros {
namespace sequencer {

// Registers every node type in this library with the given factory,
// including ThreadedSubtree (which needs the factory pointer itself to
// instantiate sub-trees by ID). Call once per BehaviorTreeFactory, before
// registerBehaviorTreeFromText()/createTree().
void RegisterAll(BT::BehaviorTreeFactory& factory);

} // namespace eeros
} // namespace sequencer

#endif // ORG_EEROS_SEQUENCER_REGISTER_ALL_HPP_
