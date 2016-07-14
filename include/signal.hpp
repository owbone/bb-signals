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
  /// \brief Construct an inactive signal.
  ///
  signal();

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
  /// \brief Connect an existing signal to an existing slot so that the slot is
  /// invoked when the signal is emitted.
  /// \param signal A const reference to an existing signal to listen to.
  /// \param slot A reference to an existing slot to receive signals.
  ///
  template <class... T>
  friend void connect(const signal<T...>& signal, slot<T...>& slot);

  ///
  /// \brief Connect an existing signal to a function so that the function is
  /// called when the signal is emitted.
  /// \param signal A const reference to an existing signal to listen to.
  /// \param fn A function to receive signals.
  ///
  template <class Fn, class... T>
  friend void connect(const signal<T...>& signal, Fn fn);

private:
  template <class... T>
  friend void connect(emitter<T...>& emitter, signal<T...>& signal);

  using state_t = detail::signal_state<Params...>;
  using shared_state_t = std::shared_ptr<state_t>;

  signal(shared_state_t);

  shared_state_t state;
};

//------------------------------------------------------------------------------

template <class... Params>
signal<Params...>::signal(shared_state_t state)
  : state{std::move(state)}
{
}

template <class... Params>
signal<Params...>::signal() = default;

template <class... Params>
signal<Params...>::signal(signal&&) = default;

template <class... Params>
signal<Params...>& signal<Params...>::operator=(signal&&) = default;

template <class... Params>
void connect(const signal<Params...>& signal, slot<Params...>& slot)
{
  if (signal.state)
    signal.state->connect(slot.state);
}

template <class Fn, class... Params>
void connect(const signal<Params...>& signal, Fn fn)
{
  using function_t = typename signal<Params...>::function_t;
  if (signal.state)
    signal.state->connect(function_t{std::move(fn)});

}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // SIGNAL_HPP
