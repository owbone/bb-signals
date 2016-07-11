#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include "detail/signal_state.hpp"
#include "slot.hpp"

#include <functional>
#include <list>
#include <memory>

//------------------------------------------------------------------------------

namespace bb {

//------------------------------------------------------------------------------

template <class... Params>
class emitter;

///
/// \brief The signal class represents a signal to which client can connect
/// functions which receive the signals when they are emitted.
/// \tparam Params... The signal parameters.
///
template <class... Params>
class signal
{
public:
  ///
  /// \brief The function type which can connect to this type of signal.
  ///
  using function_t = std::function<void(Params...)>;

  ///
  /// \brief Deleted copy constructor.
  ///
  signal(const signal&) = delete;

  ///
  /// \brief Deleted copy assignment operator.
  ///
  auto operator=(const signal&) -> signal& = delete;

  ///
  /// \brief Move constructor.
  ///
  signal(signal&&);

  ///
  /// \brief Move assignment operator.
  ///
  auto operator=(signal&&) -> signal&;

  ///
  /// \brief connect registers the given function to be called when the signal
  /// is emitted.
  /// \param fn The function object to be called.
  /// \return A new slot which owns this connection.
  ///
  auto connect(function_t fn) const -> slot;

  ///
  /// \brief connect registers the given function to be submitted to the given
  /// executor when the signal is emitted.
  /// \tparam Executor A type which implements the Executor concept.
  /// \param executor A reference to the executor on which to call fn.
  /// \param fn The function object to be called.
  /// \return A new slot which owns this connection.
  ///
  template <class Executor>
  auto connect(Executor& executor, function_t fn) const -> slot;

private:
  friend class emitter<Params...>;

  using state_t = detail::signal_state<Params...>;
  using shared_state_t = std::shared_ptr<state_t>;

  signal(shared_state_t);

  shared_state_t state;
};

//------------------------------------------------------------------------------

template <class... Params>
signal<Params...>::signal(shared_state_t state)
  : state(std::move(state))
{
}

template <class... Params>
signal<Params...>::signal(signal&&) = default;

template <class... Params>
signal<Params...>& signal<Params...>::operator=(signal&&) = default;

template <class... Params>
auto signal<Params...>::connect(function_t fn) const -> slot
{
  using slot_state_t = detail::function_slot_state<Params...>;
  auto connection = std::make_shared<slot_state_t>(std::move(fn));
  state->connect(connection);
  return slot{connection};
}

template <class... Params>
template <class Executor>
auto signal<Params...>::connect(Executor& executor, function_t fn) const -> slot
{
  using slot_state_t = detail::executor_slot_state<Executor, Params...>;
  auto connection = std::make_shared<slot_state_t>(executor, std::move(fn));
  state->connect(connection);
  return slot{connection};
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // SIGNAL_HPP
