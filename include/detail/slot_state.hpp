#ifndef SLOT_STATE_HPP
#define SLOT_STATE_HPP

#include <functional>
#include <memory>
#include <mutex>
#include <utility>

//------------------------------------------------------------------------------

namespace bb {

//------------------------------------------------------------------------------

namespace detail {

//------------------------------------------------------------------------------

class slot_state_base
{
public:
  virtual ~slot_state_base() { }
  virtual void reset() = 0;
};

//------------------------------------------------------------------------------

template <class... Params>
class slot_state
    : public slot_state_base
    , public std::enable_shared_from_this<slot_state<Params...>>
{
public:
  using function_t = std::function<void(Params...)>;

  template <class Executor>
  slot_state(Executor& executor, function_t fn)
    : executor(std::make_unique<executor_model>(executor))
    , fn(std::move(fn))
  { }

  slot_state(function_t fn)
    : executor(std::make_unique<inline_executor>())
    , fn(std::move(fn))
  { }

  void reset() override
  {
    std::unique_lock<std::mutex> lock{mutex};
    fn = nullptr;
  }

  template <class... Args>
  void post(Args&&... args) const
  {
    std::weak_ptr<const slot_state> weak_state(this->shared_from_this());

    auto lock_and_execute = [weak_state](Args... args)
    {
      if (auto state = weak_state.lock())
        state->execute(std::forward<Args>(args)...);
    };

    executor->submit(std::bind(lock_and_execute, std::forward<Args>(args)...));
  }

private:
  struct executor_concept
  {
    virtual void submit(std::function<void()>) = 0;
  };

  template <class Executor>
  struct executor_model : executor_concept
  {
    Executor& executor;

    void submit(std::function<void()> closure) override
    {
      executor.submit(std::move(closure));
    }
  };

  struct inline_executor : executor_concept
  {
    void submit(std::function<void()> closure) override
    {
      closure();
    }
  };

  template <class... Args>
  void execute(Args&&... args) const
  {
    std::unique_lock<std::mutex> lock{mutex};
    if (fn)
      fn(std::forward<Args>(args)...);
  }

  std::unique_ptr<executor_concept> executor;
  mutable std::mutex mutex;
  function_t fn;
};

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // SLOT_STATE_HPP
