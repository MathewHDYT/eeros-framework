#include <eeros/logger/StreamLogWriter.hpp>
#include <eeros/sequencer/RegisterAll.hpp>
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
          <ExceptionSequence name="exception sequence" work_tree_id="ExceptionAbortTree"
                              reset_keys="monitor_state"/>
        </ReactiveFallback>
        <ReactiveFallback>
          <MonitorWrapper monitor_name="myMonitor">
            <CounterExceeded state_key="monitor_state" threshold="2"/>
          </MonitorWrapper>
          <ExceptionSequence name="exception sequence" work_tree_id="ExceptionWaitTree"
                              reset_keys="monitor_state"/>
        </ReactiveFallback>
        <Wait name="step A" msec="2000"/>
        <Counter state_keys="loop_state,monitor_state" target="10"/>
      </ReactiveSequence>
    </SequenceWrapper>
  </BehaviorTree>

  <BehaviorTree ID="ExceptionAbortTree">
    <Sequence name="exception_body">
      <Script code="monitor_behavior:=ABORT"/>
      <Wait name="wait" msec="500"/>
    </Sequence>
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

  std::signal(SIGINT, signalHandler);
  Logger::setDefaultStreamLogger(std::cout);

  auto& sequencer = Sequencer::Instance();
  sequencer.Init(factory, xml_text, "MainTree");
  Logger log = Logger::getLogger();
  log.info() << "Sequencer example started...";

  sequencer.Run();
  sequencer.Wait();
  log.info() << "Simple sequencer example finished...";
  sequencer.Shutdown();
}
