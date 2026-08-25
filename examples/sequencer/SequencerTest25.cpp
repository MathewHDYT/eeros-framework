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
          <ForceSuccess>
            <BlockingSubtree name="seq S" tree_id="SeqSTree"/>
          </ForceSuccess>
          <Repeat num_cycles="3"><Wait name="step A" msec="1000"/></Repeat>
        </Sequence>
      </ReactiveSequence>
    </SequenceWrapper>
  </BehaviorTree>

  <BehaviorTree ID="SeqSTree">
    <ReactiveSequence name="seqS_body">
      <AbortGuard/>
      <ReactiveFallback>
        <MonitorWrapper monitor_name="timeout">
          <ClockExceeded state_key="watchdog" threshold_msec="2500"/>
        </MonitorWrapper>
        <ExceptionSequence name="exception sequence" work_tree_id="ExceptionWaitTree"
                            reset_keys="watchdog"/>
      </ReactiveFallback>
      <Wait name="step B" msec="1000"/>
      <Counter state_keys="loop_state" target="5"/>
    </ReactiveSequence>
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
