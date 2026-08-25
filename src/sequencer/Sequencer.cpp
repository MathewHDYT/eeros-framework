#include <eeros/sequencer/Sequencer.hpp>
#include <eeros/sequencer/SequencerUI.hpp>
#include <eeros/core/Fault.hpp>

namespace eeros {
namespace sequencer {

Sequencer& Sequencer::Instance()
{
  static Sequencer instance;
  return instance;
}

void Sequencer::Init(BT::BehaviorTreeFactory& factory, const std::string& xml_text,
                       const std::string& main_tree_id)
{
  factory.registerBehaviorTreeFromText(xml_text);
  tree_ = std::make_unique<BT::Tree>(factory.createTree(main_tree_id));
  SeedSharedState(tree_->rootBlackboard(), abort_flag_, thread_registry_);
}

BT::NodeStatus Sequencer::Run()
{
  return tree_->tickWhileRunning();
}

void Sequencer::Wait()
{
  thread_registry_->WaitForAll();
}

void Sequencer::Abort()
{
  *abort_flag_ = true;
}

BT::Blackboard::Ptr Sequencer::RootBlackboard()
{
  return tree_->rootBlackboard();
}

void Sequencer::Shutdown()
{
  tree_.reset();
}

} // namespace sequencer
} // namespace eeros
