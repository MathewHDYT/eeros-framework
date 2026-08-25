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
        <Sequence name="phases">
          <Repeat num_cycles="3"><Wait name="step A" msec="1000"/></Repeat>
          <Parallel success_count="2" failure_count="1">
            <ThreadedSubtree name="seq S" tree_id="SeqSTree"/>
            <Repeat num_cycles="3"><Wait name="step A" msec="1000"/></Repeat>
          </Parallel>
        </Sequence>
      </ReactiveSequence>
    </SequenceWrapper>
  </BehaviorTree>

  <BehaviorTree ID="SeqSTree">
    <Sequence name="seqS_setup">
      <Script code="watchdog_threshold := 1500"/>
      <ReactiveSequence name="seqS_body">
        <AbortGuard/>
        <ReactiveFallback>
          <MonitorWrapper monitor_name="timeout">
            <ClockExceeded state_key="watchdog" threshold_msec="{watchdog_threshold}"/>
          </MonitorWrapper>
          <Sequence name="exception_and_retime">
            <ExceptionSequence name="exception sequence" work_tree_id="ExceptionWaitTree"
                                reset_keys="watchdog"/>
            <Script code="watchdog_threshold := 5000"/>
          </Sequence>
        </ReactiveFallback>
        <Wait name="step B" msec="1000"/>
        <Counter state_keys="loop_state" target="5"/>
      </ReactiveSequence>
    </Sequence>
  </BehaviorTree>

  <BehaviorTree ID="ExceptionWaitTree">
    <Wait name="wait" msec="3000"/>
  </BehaviorTree>
</root>
)";

void signalHandler(int signum) {
  Sequencer::Instance().Abort();
}

int main(int argc, char **argv) {
  BT::BehaviorTreeFactory factory;
  RegisterAll(factory);

  Sequencer::Instance().Init(factory, xml_text, "MainTree");

  std::signal(SIGINT, signalHandler);
  Logger::setDefaultStreamLogger(std::cout);
  Logger log = Logger::getLogger('M');
  log.info() << "Sequencer example started...";

  Sequencer::Instance().Run();
  Sequencer::Instance().Wait();
  log.info() << "Simple sequencer example finished...";
  Sequencer::Instance().Shutdown();
}
