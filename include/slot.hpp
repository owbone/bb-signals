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

///
/// \brief The slot class owns a connection to a signal. The signal will be
/// disconnected when the slot goes out of scope.
///
class slot
{
public:
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
  template <class... Params>
  friend class signal;

  using state_t = detail::slot_state_base;
  using shared_state_t = std::shared_ptr<state_t>;

  slot(shared_state_t);

  shared_state_t state;
};

//------------------------------------------------------------------------------

slot::slot(shared_state_t state)
  : state(std::move(state))
{
}

slot::slot(slot&&) = default;
auto slot::operator=(slot&&) -> slot& = default;

slot::~slot()
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
