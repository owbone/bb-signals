#ifndef FUNCTION_SLOT_STATE_HPP
#define FUNCTION_SLOT_STATE_HPP

#include "slot_state.hpp"

#include <functional>
#include <mutex>
#include <utility>

//------------------------------------------------------------------------------

namespace bb {

//------------------------------------------------------------------------------

namespace detail {

//------------------------------------------------------------------------------

template <class... Params>
class function_slot_state : public slot_state<Params...>
{
public:
  using function_t = std::function<void(Params...)>;

  function_slot_state(function_t fn);

private:
  void post(Params... args) const;
  void reset();

  mutable std::mutex mutex;
  function_t fn;
};

//------------------------------------------------------------------------------

template <class... Params>
function_slot_state<Params...>::function_slot_state(function_t fn)
  : fn(std::move(fn))
{
}

template <class... Params>
void function_slot_state<Params...>::post(Params... args) const
{
  std::unique_lock<std::mutex> lock(mutex);
  if (fn)
  {
    fn(std::forward<Params>(args)...);
  }
}

template <class... Params>
void function_slot_state<Params...>::reset()
{
  std::unique_lock<std::mutex> lock(mutex);
  fn = nullptr;
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // FUNCTION_SLOT_STATE_HPP
