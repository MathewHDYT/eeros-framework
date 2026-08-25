#include <eeros/sequencer/Conditions.hpp>
#include <eeros/sequencer/Common.hpp>

namespace eeros {
namespace sequencer {

ClockExceeded::ClockExceeded(const std::string& name, const BT::NodeConfig& config)
    : ConditionNode(name, config)
{}

BT::PortsList ClockExceeded::providedPorts()
{
  return { BT::InputPort<std::string>("state_key", "watchdog", "WatchedValue<ClockTime> to check"),
           BT::InputPort<int>("threshold_msec", 2500,
               "elapsed time (ms) that counts as exceeded. Bind to a blackboard "
               "entry via \"{key}\" if the threshold needs to change at runtime "
               "(e.g. threshold_msec=\"{watchdog_threshold}\") -- see example 20") };
}

BT::NodeStatus ClockExceeded::tick()
{
  std::string key = "watchdog";
  getInput("state_key", key);
  int threshold = 2500;
  getInput("threshold_msec", threshold);

  auto cell = GetWatched<ClockTime>(config().blackboard, key);
  auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        Clock::now() - cell->value)
                        .count();

  return elapsed_ms > threshold ? BT::NodeStatus::FAILURE : BT::NodeStatus::SUCCESS;
}

CounterExceeded::CounterExceeded(const std::string& name, const BT::NodeConfig& config)
    : ConditionNode(name, config)
{}

BT::PortsList CounterExceeded::providedPorts()
{
  return { BT::InputPort<std::string>("state_key", "monitor_state", "WatchedValue<AtomicInt> to check"),
           BT::InputPort<int>("threshold", 2, "value that counts as exceeded") };
}

BT::NodeStatus CounterExceeded::tick()
{
  std::string key = "monitor_state";
  getInput("state_key", key);
  int threshold = 2;
  getInput("threshold", threshold);

  auto cell = GetWatched<AtomicInt>(config().blackboard, key);
  return cell->value > threshold ? BT::NodeStatus::FAILURE : BT::NodeStatus::SUCCESS;
}

}
}
