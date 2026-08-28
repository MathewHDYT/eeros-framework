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
        <ExceptionSequence name="exception sequence" work_tree_id="ExceptionAbortTree"/>
      </ReactiveFallback>
      <Wait name="step B" msec="1000"/>
      <Counter state_keys="loop_state" target="5"/>
    </ReactiveSequence>
  </BehaviorTree>

  <BehaviorTree ID="ExceptionAbortTree">
    <Sequence name="exception_body">
      <Script code="monitor_behavior:=ABORT"/>
      <Wait name="wait" msec="3000"/>
    </Sequence>
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
