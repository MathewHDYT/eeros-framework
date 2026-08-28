#include <eeros/sequencer/RegisterAll.hpp>
#include <eeros/sequencer/AbortGuard.hpp>
#include <eeros/sequencer/Wait.hpp>
#include <eeros/sequencer/SequenceWrapper.hpp>
#include <eeros/sequencer/ResumableSequence.hpp>
#include <eeros/sequencer/BlockingSubtree.hpp>
#include <eeros/sequencer/ThreadedSubtree.hpp>
#include <eeros/sequencer/MonitorWrapper.hpp>
#include <eeros/sequencer/Conditions.hpp>
#include <eeros/sequencer/ExceptionSequence.hpp>
#include <eeros/sequencer/Counter.hpp>
#include <memory>

namespace eeros {
namespace sequencer {

void RegisterAll(BT::BehaviorTreeFactory& factory)
{
  factory.registerScriptingEnums<MonitorBehavior>();

  factory.registerNodeType<SequenceWrapper>("SequenceWrapper");
  factory.registerNodeType<ResumableSequence>("ResumableSequence");
  factory.registerNodeType<Wait>("Wait");
  factory.registerNodeType<AbortGuard>("AbortGuard");
  factory.registerNodeType<MonitorWrapper>("MonitorWrapper");
  factory.registerNodeType<ClockExceeded>("ClockExceeded");
  factory.registerNodeType<CounterExceeded>("CounterExceeded");
  factory.registerNodeType<Counter>("Counter");

  factory.registerBuilder<ExceptionSequence>(
      "ExceptionSequence",
      [&factory](const std::string& name, const BT::NodeConfig& config)
      {
        return std::make_unique<ExceptionSequence>(name, config, &factory);
      });

  factory.registerBuilder<BlockingSubtree>(
      "BlockingSubtree",
      [&factory](const std::string& name, const BT::NodeConfig& config)
      {
        return std::make_unique<BlockingSubtree>(name, config, &factory);
      });

  factory.registerBuilder<ThreadedSubtree>(
      "ThreadedSubtree",
      [&factory](const std::string& name, const BT::NodeConfig& config)
      {
        return std::make_unique<ThreadedSubtree>(name, config, &factory);
      });
}

}
}
