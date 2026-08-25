#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/sequencer/RegisterAll.hpp>
#include <eeros/sequencer/PredicateCondition.hpp>
#include <eeros/sequencer/Common.hpp>
#include <eeros/sequencer/Sequencer.hpp>
#include <csignal>

using namespace eeros::sequencer;
using namespace eeros::logger;

static const char* xml_text = R"(
<root BTCPP_format="4">
  <BehaviorTree ID="MainTree">
    <SequenceWrapper name="Main Sequence">
      <ReactiveSequence name="main_body">
        <AbortGuard/>

        <ReactiveFallback>
          <MonitorWrapper monitor_name="timeout">
            <ClockExceeded state_key="watchdog" threshold_msec="10000"/>
          </MonitorWrapper>
          <ExceptionSequence name="timeout exception sequence"
                              work_tree_id="ExceptionWaitTree" reset_keys="watchdog"/>
        </ReactiveFallback>

        <ReactiveFallback>
          <MonitorWrapper monitor_name="myMonitor">
            <MyCondition state_key="monitor_state"/>
          </MonitorWrapper>
          <ExceptionSequence name="counter exception sequence"
                              work_tree_id="ExceptionWaitTree" reset_keys="monitor_state"/>
        </ReactiveFallback>

        <Wait name="step A" msec="2000"/>
        <Counter state_keys="loop_state,monitor_state" target="10"/>
      </ReactiveSequence>
    </SequenceWrapper>
  </BehaviorTree>

  <BehaviorTree ID="ExceptionWaitTree">
    <Wait name="wait" msec="500"/>
  </BehaviorTree>
</root>
)";

void signalHandler(int signum) {
  Sequencer::Instance().Abort();
}

int main(int argc, char **argv) {
  BT::BehaviorTreeFactory factory;
  RegisterAll(factory);

  RegisterPredicateCondition(factory, "MyCondition",
      [](BT::Blackboard::Ptr bb, const std::string& key)
      {
        auto cell = GetWatched<AtomicInt>(bb, key);
        return cell->value > 2; // return count > 2;
      });

  Sequencer::Instance().Init(factory, xml_text, "MainTree");

  std::signal(SIGINT, signalHandler);
  Logger::setDefaultStreamLogger(std::cout);
  Logger log = Logger::getLogger();
  log.info() << "Sequencer example started...";

  Sequencer::Instance().Run();
  Sequencer::Instance().Wait();
  log.info() << "Simple sequencer example finished...";
  Sequencer::Instance().Shutdown();
}
