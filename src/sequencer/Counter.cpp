#include <eeros/sequencer/Counter.hpp>
#include <eeros/sequencer/Common.hpp>
#include <eeros/core/Fault.hpp>
#include <cstddef>

namespace eeros {
namespace sequencer {

Counter::Counter(const std::string& name, const BT::NodeConfig& config)
    : StatefulActionNode(name, config)
{}

BT::PortsList Counter::providedPorts()
{
  return { BT::InputPort<std::string>("state_keys", "loop_state",
               "comma-separated list of WatchedValue<AtomicInt> keys to increment each "
               "tick (e.g. \"loop_state,monitor_state\") -- same list convention "
               "as reset_keys. The FIRST key listed is the one checked against "
               "target to decide when this node succeeds; any further keys are "
               "just incremented alongside it, no target check of their own"),
           BT::InputPort<int>("target", 5,
               "how many successes complete the loop, checked against the FIRST "
               "key in state_keys") };
}

BT::NodeStatus Counter::onStart() { return checkProgress(); }
BT::NodeStatus Counter::onRunning() { return checkProgress(); }
void Counter::onHalted() {}

BT::NodeStatus Counter::checkProgress()
{
  std::string keys_csv = "loop_state";
  getInput("state_keys", keys_csv);
  int target = 5;
  getInput("target", target);

  // BT::StringView (non-owning) -- no heap copy for the split itself.
  // Each element still gets converted to std::string exactly once, at
  // the GetWatched() call below -- that boundary is unavoidable without
  // knowing whether Blackboard's internal lookup supports a
  // transparent/heterogeneous string_view key, which wasn't verified.
  std::vector<BT::StringView> keys = BT::splitString(keys_csv, ',');
  if (keys.empty())
  {
    throw eeros::Fault("Counter: state_keys must list at least one key");
  }

  int primary_value = 0;
  for (size_t i = 0; i < keys.size(); ++i)
  {
    // Explicit std::string(...) here: std::string_view's conversion to
    // std::string is an EXPLICIT constructor, not implicit -- passing a
    // StringView directly where GetWatched expects const std::string&
    // wouldn't compile via an implicit conversion.
    auto cell  = GetWatched<AtomicInt>(config().blackboard, std::string(keys[i]));
    int  value = ++cell->value;
    if (i == 0)
    {
      primary_value = value;
    }
  }

  return primary_value >= target ? BT::NodeStatus::SUCCESS : BT::NodeStatus::RUNNING;
}

}
}
