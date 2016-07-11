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
  /// \brief create_signal creates a new signal which is associated with this
  /// emitter.
  /// \return A new signal.
  ///
  auto create_signal() const -> signal<Params...>;

  ///
  /// \brief emit any signals which have been created.
  /// \param args The arguments with which to emit the signal.
  ///
  template <class... Args>
  void operator()(Args&&... args);

private:
  using state_t = detail::signal_state<Params...>;
  using shared_state_t = std::shared_ptr<state_t>;

  shared_state_t state;
};

//------------------------------------------------------------------------------

template <class... Params>
emitter<Params...>::emitter()
  : state(std::make_shared<state_t>())
{
}

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
auto emitter<Params...>::create_signal() const -> signal<Params...>
{
  return signal<Params...>{state};
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
