#ifndef ORG_EEROS_SEQUENCER_COMMON_HPP_
#define ORG_EEROS_SEQUENCER_COMMON_HPP_
#include <behaviortree_cpp/bt_factory.h>
#include <eeros/core/Fault.hpp>
#include <chrono>
#include <atomic>
#include <string>
#include <memory>
#include <vector>
#include <future>
#include <mutex>

namespace eeros {
namespace sequencer {

using Clock = std::chrono::steady_clock;

/**
 * @brief Interface to allow for @ref ExceptionSequence to reset an arbitrary, mixed-type lsit of blackboard entries.
 * Without knowing each entrys concrete value type. 
 */
struct Resettable
{
  virtual ~Resettable() = default;
  virtual void reset() = 0;
};

/**
 * @brief Wrapper for @ref Clock::time_point to allow for default construction to return now instead of the clock's epoch. 
 * Allows to use the same generic @ref WatchedValue<T> template without specialization, where resetting the value means setting it to the value of default construction.
 */
struct ClockTime
{
  Clock::time_point value = Clock::now();
  operator Clock::time_point() const { return value; }
};

inline Clock::duration operator-(Clock::time_point lhs, const ClockTime& rhs)
{
  return lhs - rhs.value;
}

/**
 * @brief Wrapper for @ref std::atomic<int> to allow for default construction assignment, which is not possible using a normal atomic.
 * Allows to use the same generic @ref WatchedValue<T> template without specialization, where resetting the value means setting it to the value of default construction.
 */
struct AtomicInt
{
  std::atomic<int> value{0};

  AtomicInt() = default;
  AtomicInt(const AtomicInt& other) : value(other.value.load()) {}
  AtomicInt& operator=(const AtomicInt& other)
  {
    value.store(other.value.load());
    return *this;
  }

  operator int() const { return value.load(); }
  int operator++() { return ++value; }
};

/**
 * @brief Single purpose, single value state cell holding a value that should be reset by the "reset_keys" list of the @ref ExceptionSequence.
 * 
 * @tparam T Type that should be reset to it's default constructed value.
 */
template <typename T>
struct WatchedValue : public Resettable
{
  T value{};
  void reset() override { value = T{}; }
};

/**
 * @brief Fetches the WatchedValue<T> with the specific key from the blackboard and attempts to downcast to the specifc type to actual read the underlying value.
 * 
 * @tparam T Underlying type of the value.
 * @param bb Pointer to the blackboard containg the value.
 * @param key Name of the specific blackboard entry.
 * @return Shared pointer to the actual blackboard entry value.
 */
template <typename T>
std::shared_ptr<WatchedValue<T>> GetWatched(BT::Blackboard::Ptr bb, const std::string& key)
{
  auto base  = bb->get<std::shared_ptr<Resettable>>(key);
  auto typed = std::dynamic_pointer_cast<WatchedValue<T>>(base);
  if (!typed)
  {
    throw eeros::Fault("blackboard key '" + key +
                            "' does not hold a WatchedValue of the requested type");
  }
  return typed;
}

/**
 * @brief Abort mechanism being blackboard scoped, so that all blackboards share the same global AbortFlag.
 */
using AbortFlag = std::shared_ptr<std::atomic<bool>>;

/**
 * @brief Write the flag to differentiate between restart and resume, when ExceptionSequence does not abort.
 * 
 * @param bb Pointer to the blackboard we want to write into.
 * @param key Name of the specific blackboard entry we want to write.
 * @param restarting True when the blackboard task should be reset.
 */
void WriteRestartFlag(BT::Blackboard::Ptr bb, const std::string& key, bool restarting);

/**
 * @brief Read the flag to differentiate between restart and resume, when ExceptionSequence does not abort.
 * 
 * @param bb Pointer to the blackboard we want to read from.
 * @param key Name of the specific blackboard entry we want to read.
 * @return True when the blackboard task should be reset.
 */
bool ReadRestartFlag(BT::Blackboard::Ptr bb, const std::string& key);

/**
 * @brief Registry of all running sequences, to allow for the execution of @ref ThreadedSubtree to finish.
 */
class ThreadRegistry
{
public:
  void Track(std::shared_future<BT::NodeStatus> fut);
  void WaitForAll();

private:
  std::mutex                                       mutex_;
  std::vector<std::shared_future<BT::NodeStatus>>  futures_;
};

using ThreadRegistryPtr = std::shared_ptr<ThreadRegistry>;

/**
 * @brief Current behavior of the @ref ExceptionSequence
 * 
 */
enum class MonitorBehavior
{
  RESUME  = 0,
  RESTART = 1,
  ABORT   = 2,
};

/**
 * @brief Common setup of all Subtrees to share teh same base blackboard variables, so that the thread registry and abort flag are shared.
 * 
 * @param bb Blackboard that needs the shared state values.
 * @param abort_flag Sahred abort flag.
 * @param thread_registry Shared thread registry.
 */
void SeedSharedState(BT::Blackboard::Ptr bb, AbortFlag abort_flag,
                      ThreadRegistryPtr thread_registry);

} // namespace sequencer
} // namespace eeros

#endif // ORG_EEROS_SEQUENCER_COMMON_HPP_
