#ifndef EMITTER_HPP
#define EMITTER_HPP

#include "detail/signal_state.hpp"
#include "signal.hpp"

#include <memory>
#include <utility>

//------------------------------------------------------------------------------

namespace bb {

//------------------------------------------------------------------------------

///
/// \brief The emitter class is a function object which is used to create and
/// emit signals.
///
template <class... Params>
class emitter
{
public:
  ///
  /// \brief Construct a new emitter.
  ///
  emitter();

  ///
  /// \brief Copy constructor.
  ///
  emitter(const emitter&);

  ///
  /// \brief Copy assignment operator.
  ///
  emitter& operator=(const emitter&);

  ///
  /// \brief Move constructor.
  ///
  emitter(emitter&&);

  ///
  /// \brief Move assignment operator.
  ///
  emitter& operator=(emitter&&);

  ///
  /// \brief emit any signals which have been created.
  /// \param args The arguments with which to emit the signal.
  ///
  template <class... Args>
  void operator()(Args&&... args);

  ///
  /// \brief Connect an emitter to a signal, so that calling the emitter will
  /// trigger any slots connected to the signal.
  /// \param emitter The sending emitter.
  /// \param signal The receiving signal.
  /// \note Existing connections from the emitter and the signal will be
  /// destroyed.
  ///
  template <class... T>
  friend void connect(emitter<T...>& emitter, signal<T...>& signal);

private:
  using state_t = detail::signal_state<Params...>;
  using weak_state_t = std::weak_ptr<state_t>;

  emitter(weak_state_t);

  weak_state_t weak_state;
};

//------------------------------------------------------------------------------

template <class... Params>
emitter<Params...>::emitter(weak_state_t weak_state)
  : weak_state{std::move(weak_state)}
{
}

template <class... Params>
emitter<Params...>::emitter() = default;

template <class... Params>
emitter<Params...>::emitter(const emitter&) = default;

template <class... Params>
emitter<Params...>& emitter<Params...>::operator=(const emitter&) = default;

template <class... Params>
emitter<Params...>::emitter(emitter&&) = default;

template <class... Params>
emitter<Params...>& emitter<Params...>::operator=(emitter&&) = default;

template <class... Params>
void connect(emitter<Params...>& emitter_, signal<Params...>& signal_)
{
  using state_t = typename emitter<Params...>::state_t;
  using weak_state_t = typename emitter<Params...>::weak_state_t;
  auto state = std::make_shared<state_t>();
  emitter_ = emitter<Params...>{weak_state_t{state}};
  signal_ = signal<Params...>{state};
}

template <class... Params>
template <class... Args>
void emitter<Params...>::operator()(Args&&... args)
{
  if (auto state = weak_state.lock())
    state->emit(std::forward<Args>(args)...);
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // EMITTER_HPP
