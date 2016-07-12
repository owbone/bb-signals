#ifndef SIGNAL_STATE_HPP
#define SIGNAL_STATE_HPP

#include "slot_state.hpp"

#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <utility>

//------------------------------------------------------------------------------

namespace bb {

//------------------------------------------------------------------------------

namespace detail {

//------------------------------------------------------------------------------

template <class... Params>
class signal_state
{
public:
  using slot_state_t = slot_state<Params...>;
  using connection_t = std::shared_ptr<slot_state_t>;
  using weak_connection_t = std::weak_ptr<slot_state_t>;
  using function_t = std::function<void(Params...)>;

  signal_state();
  signal_state(const signal_state&) = delete;
  signal_state& operator=(const signal_state&) = delete;
  signal_state(signal_state&&) = delete;
  signal_state& operator=(signal_state&&) = delete;

  void connect(weak_connection_t connection) const;

  template <class... Args>
  void emit(Args&&... args) const;

private:
  using connection_list_t = std::list<weak_connection_t>;

  void splice_new_connections(connection_list_t&) const;

  mutable std::mutex new_connections_mutex;
  mutable connection_list_t new_connections;

  mutable std::mutex connections_mutex;
  mutable connection_list_t connections;
};

//------------------------------------------------------------------------------

template <class... Params>
signal_state<Params...>::signal_state() = default;

template <class... Params>
void signal_state<Params...>::connect(weak_connection_t connection) const
{
  std::unique_lock<std::mutex> lock(new_connections_mutex);
  new_connections.push_back(std::move(connection));
}

template <class... Params>
template <class... Args>
void signal_state<Params...>::emit(Args&&... args) const
{
  std::unique_lock<std::mutex> lock(connections_mutex);

  splice_new_connections(connections);

  // TODO: Argument forwarding shortcut for single connections.

  auto it = connections.begin();
  auto end = connections.end();

  while (it != end)
  {
    if (auto connection = it->lock())
    {
      connection->post(args...);
      ++it;
    }
    else
    {
      it = connections.erase(it);
    }
  }
}

template <class... Params>
void signal_state<Params...>::splice_new_connections(connection_list_t& list) const
{
  std::unique_lock<std::mutex> lock(new_connections_mutex);
  list.splice(list.end(), new_connections);
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // SIGNAL_STATE_HPP
