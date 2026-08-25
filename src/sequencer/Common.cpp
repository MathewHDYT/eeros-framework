#include <eeros/sequencer/Common.hpp>
#include <ctime>
#include <mutex>

namespace eeros {
namespace sequencer {

void ThreadRegistry::Track(std::shared_future<BT::NodeStatus> fut)
{
  std::lock_guard<std::mutex> lock(mutex_);
  futures_.push_back(std::move(fut));
}

void ThreadRegistry::WaitForAll()
{
  // Create snapshot to ensure that no tracked task spawns another ThreadedSubtree before finishing would cause deadlock.
  std::vector<std::shared_future<BT::NodeStatus>> snapshot;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot = futures_;
  }
  for (auto& fut : snapshot)
  {
    if (fut.valid())
    {
      fut.wait();
    }
  }
}

void SeedSharedState(BT::Blackboard::Ptr bb, AbortFlag abort_flag, ThreadRegistryPtr thread_registry)
{
  bb->set<std::shared_ptr<Resettable>>("watchdog", std::make_shared<WatchedValue<ClockTime>>());
  bb->set<std::shared_ptr<Resettable>>("loop_state", std::make_shared<WatchedValue<AtomicInt>>());
  bb->set<std::shared_ptr<Resettable>>("monitor_state", std::make_shared<WatchedValue<AtomicInt>>());
  bb->set<std::shared_ptr<Resettable>>("restart_count", std::make_shared<WatchedValue<AtomicInt>>());
  bb->set<AbortFlag>("abort_flag", abort_flag);
  bb->set<ThreadRegistryPtr>("thread_registry", thread_registry);
}

void WriteRestartFlag(BT::Blackboard::Ptr bb, const std::string& key, bool restarting)
{
  bb->set<bool>(key, restarting);
}

bool ReadRestartFlag(BT::Blackboard::Ptr bb, const std::string& key)
{
  bool restarting = false;
  (void)bb->get<bool>(key, restarting); // non-throwing overload; false if key doesn't exist yet
  return restarting;
}

}
}
