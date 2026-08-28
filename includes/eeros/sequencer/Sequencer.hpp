#ifndef ORG_EEROS_SEQUENCER_SEQUENCER_HPP_
#define ORG_EEROS_SEQUENCER_SEQUENCER_HPP_
#include <eeros/sequencer/SequencerUI.hpp>
#include <eeros/logger/Logger.hpp>
#include <eeros/sequencer/Common.hpp>
#include <behaviortree_cpp/bt_factory.h>
#include <memory>

namespace eeros {
namespace sequencer {

class SequencerUI;

/**
 * @brief Singleton owning the running BT::Tree, allows to abort all running sequences and can wait for all sequences to finish running.
 * 
 * @since v1.0
 */
class Sequencer {
  friend class SequencerUI;

public:

  /**
   * Returns a sequencer instance. When first called it will initially create such an instance.
   * Subsequent calls will return this initial instance.
   * 
   * @return - sequencer instance
   */
  static Sequencer& Instance();

  /**
   * @brief Registers and initalizes the given main tree defined in the xml text as the BehaviorTree to be executed.
   * 
   * @param factory BT factory used to intialize the sequence.
   * @param xml_text XML text that defines the exact sequence to register.
   * @param main_tree_id Name of the root behavior tree.
   */
  void Init(BT::BehaviorTreeFactory& factory, const std::string& xml_text,
            const std::string& main_tree_id = "MainTree");

  /**
   * @brief Tick the main tree until it reaches success or failure.
   * Ignores any spawned subtrees that are still running use @ref Wait for that.
   * 
   * @return BT::NodeStatus 
   */
  BT::NodeStatus Run();

  /**
   * @brief Halts the current tree allows to tick individually using the @ref Step method.
   */
  void SingleStepping();

  /**
   * @brief Tick the main tree once.
   * 
   * @return BT::NodeStatus 
   */
  BT::NodeStatus Step();

  /**
   * @brief Waits for all spawned subtrees running on another thread to complete.
   */
  void Wait();

  /**
   * @brief Aborts all currently running sequences.
   */
  void Abort();

  /**
   * @brief Get access to the root blackboard.
   */
  BT::Blackboard::Ptr RootBlackboard();

  /**
   * @brief Explicitly destroys all running sequences
   */
  void Shutdown();

private:
  Sequencer() = default;
  Sequencer(const Sequencer&)            = delete;
  Sequencer& operator=(const Sequencer&) = delete;

  std::unique_ptr<BT::Tree> tree_;
  AbortFlag                 abort_flag_      = std::make_shared<std::atomic<bool>>(false);
  ThreadRegistryPtr         thread_registry_ = std::make_shared<ThreadRegistry>();
  SequencerUI ui_;
};

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_SEQUENCER_HPP_
