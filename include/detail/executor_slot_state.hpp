#ifndef EXECUTOR_SLOT_STATE_HPP
#define EXECUTOR_SLOT_STATE_HPP

#include "slot_state.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <utility>

//------------------------------------------------------------------------------

namespace bb {

//------------------------------------------------------------------------------

namespace detail {

//------------------------------------------------------------------------------

template <class Executor, class... Params>
class executor_slot_state
    : public slot_state<Params...>
    , std::enable_shared_from_this<executor_slot_state<Executor, Params...>>
{
public:
  using function_t = std::function<void(Params...)>;

  executor_slot_state(Executor& executor, function_t fn);

private:
  void post(Params... args) const;
  void reset();

  void execute(Params...);
  static void execute_weak(std::weak_ptr<executor_slot_state>, Params...);

  mutable std::mutex mutex;
  Executor& executor;
  function_t fn;
};

//------------------------------------------------------------------------------

template <class Executor, class... Params>
executor_slot_state<Executor, Params...>::executor_slot_state(
    Executor& executor,
    function_t fn
    )
  : executor(executor)
  , fn(std::move(fn))
{
}

template <class Executor, class... Params>
void executor_slot_state<Executor, Params...>::post(Params... args) const
{
  std::weak_ptr<executor_slot_state> wimpl{this->shared_from_this()};
  executor.submit(std::bind(&executor_slot_state::execute_weak,
                            wimpl,
                            std::forward<Params>(args)...));
}

template <class Executor, class... Params>
void executor_slot_state<Executor, Params...>::reset()
{
  std::unique_lock<std::mutex> lock(mutex);
  fn = nullptr;
}

template <class Executor, class... Params>
void executor_slot_state<Executor, Params...>::execute(Params... args)
{
  std::unique_lock<std::mutex> lock(mutex);
  if (fn)
    fn(std::forward<Params>(args)...);
}

template <class Executor, class... Params>
void executor_slot_state<Executor, Params...>::execute_weak(
    std::weak_ptr<executor_slot_state> wimpl,
    Params... args
    )
{
  if (auto impl = wimpl.lock())
    impl->execute(std::forward<Params>(args)...);
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // EXECUTOR_SLOT_STATE_HPP
