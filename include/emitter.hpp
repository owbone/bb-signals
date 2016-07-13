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
  auto operator=(const emitter&) -> emitter&;

  ///
  /// \brief Move constructor.
  ///
  emitter(emitter&&);

  ///
  /// \brief Move assignment operator.
  ///
  auto operator=(emitter&&) -> emitter&;

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
  friend void
  connect(emitter<T...>& emitter, signal<T...>& signal);

private:
  using state_t = detail::signal_state<Params...>;
  using shared_state_t = std::shared_ptr<state_t>;

  emitter(shared_state_t);

  shared_state_t state;
};

//------------------------------------------------------------------------------

template <class... Params>
emitter<Params...>::emitter(shared_state_t state)
  : state{std::move(state)}
{
}

template <class... Params>
emitter<Params...>::emitter() = default;

template <class... Params>
emitter<Params...>::emitter(const emitter&) = default;

template <class... Params>
auto
emitter<Params...>::operator=(const emitter&) -> emitter<Params...>& = default;

template <class... Params>
emitter<Params...>::emitter(emitter&&) = default;

template <class... Params>
auto
emitter<Params...>::operator=(emitter&&) -> emitter<Params...>& = default;

template <class... Params>
void connect(emitter<Params...>& emitter_, signal<Params...>& signal_)
{
  auto state = std::make_shared<typename emitter<Params...>::state_t>();
  emitter_ = emitter<Params...>{state};
  signal_ = signal<Params...>{state};
}

template <class... Params>
template <class... Args>
void emitter<Params...>::operator()(Args&&... args)
{
  state->emit(std::forward<Args>(args)...);
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // EMITTER_HPP
