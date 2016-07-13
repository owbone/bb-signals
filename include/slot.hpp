#ifndef SLOT_HPP
#define SLOT_HPP

#include "detail/slot_state.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <utility>

//------------------------------------------------------------------------------

namespace bb {

//------------------------------------------------------------------------------

template <class... Params>
class signal;

///
/// \brief The slot class owns a connection to a signal. The signal will be
/// disconnected when the slot goes out of scope.
///
template <class... Params>
class slot
{
public:
  ///
  /// \brief The function type which can be attached to this slot.
  ///
  using function_t = std::function<void(Params...)>;

  ///
  /// \brief Construct an empty slot.
  ///
  slot();

  ///
  /// \brief Construct a slot which will call the given function when a signal
  /// is received.
  /// \param fn The function to be invoked with the signal parameters.
  ///
  slot(function_t fn);

  ///
  /// \brief Construct a slot which will post the given function to the given
  /// executor when a signal is received.
  /// \tparam Executor A type implementing the Executor concept
  /// \param executor A reference to the executor to which the fn will be
  /// submitted.
  /// \param fn The function to be invoked with the signal parameters.
  ///
  template <class Executor>
  slot(Executor& executor, function_t fn);

  ///
  /// \brief Copy constructor is deleted.
  ///
  slot(const slot&) = delete;

  ///
  /// \brief Copy assignment operator is deleted.
  ///
  auto operator=(const slot&) -> slot& = delete;

  ///
  /// \brief Move constructor.
  ///
  slot(slot&&);

  ///
  /// \brief Move assignment operator.
  ///
  auto operator=(slot&&) -> slot&;

  ///
  /// \brief The destructor will disconnect from the signal.
  /// \note If the function is currently being invoked in another thread then
  /// the destructor will block until it is finished.
  ///
  ~slot();

private:
  template <class... T>
  friend void
  connect(const signal<T...>& signal, slot<T...>& slot);

  using state_t = detail::slot_state<Params...>;
  using shared_state_t = std::shared_ptr<state_t>;

  shared_state_t state;
};

//------------------------------------------------------------------------------

template <class... Params>
slot<Params...>::slot() = default;

template <class... Params>
slot<Params...>::slot(function_t fn)
  : state{std::make_shared<state_t>(std::move(fn))}
{
}

template <class... Params>
template <class Executor>
slot<Params...>::slot(Executor& executor, function_t fn)
 : state{std::make_shared<state_t>(executor, std::move(fn))}
{
}

template <class... Params>
slot<Params...>::slot(slot&&) = default;

template <class... Params>
auto slot<Params...>::operator=(slot&&) -> slot& = default;

template <class... Params>
slot<Params...>::~slot()
{
  // This will block until the internal mutex is locked and the function has
  // been cleared. This is essential because the state itself might outlive
  // "this", but we don't want that to mean that the function might be invoked
  // after this destructor has returned.
  if (state) state->reset();
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif  // SLOT_HPP
