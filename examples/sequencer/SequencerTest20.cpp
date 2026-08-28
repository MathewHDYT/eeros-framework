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
          <BlockingSubtree name="seq S" tree_id="SeqSTree"/>
          <Repeat num_cycles="3"><Wait name="step A" msec="1000"/></Repeat>
        </Sequence>
      </ReactiveSequence>
    </SequenceWrapper>
  </BehaviorTree>

  <BehaviorTree ID="SeqSTree">
    <ReactiveSequence name="seqS_body">
      <AbortGuard/>
      <Repeat num_cycles="5"><Wait name="step B" msec="1000"/></Repeat>
    </ReactiveSequence>
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
  Logger log = Logger::getLogger('M');
  log.info() << "Sequencer example started...";

  sequencer.Run();
  sequencer.Wait();
  log.info() << "Simple sequencer example finished...";
  sequencer.Shutdown();
}
