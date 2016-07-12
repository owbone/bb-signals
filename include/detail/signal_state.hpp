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

  signal_state() = default;
  signal_state(const signal_state&) = delete;
  signal_state& operator=(const signal_state&) = delete;
  signal_state(signal_state&&) = delete;
  signal_state& operator=(signal_state&&) = delete;

  void connect(weak_connection_t connection) const
  {
    std::unique_lock<std::mutex> lock(new_connections_mutex);
    new_connections.push_back(std::move(connection));
  }

  template <class... Args>
  void emit(Args&&... args) const
  {
    std::unique_lock<std::mutex> lock(connections_mutex);

    splice_new_connections(connections);

    if (connections.size() == 1)
    {
      // If there's only a single connection then we can forward the arguments
      // directly to it without copying.
      try_post(connections.begin(), std::forward<Args>(args)...);
    }
    else
    {
      auto it = connections.begin();
      auto end = connections.end();

      while (it != end)
      {
        it = try_post(it, args...);
      }
    }
  }

private:
  using connection_list_t = std::list<weak_connection_t>;

  template <class... Args>
  typename connection_list_t::iterator
  try_post(typename connection_list_t::iterator it, Args&&... args) const
  {
    if (auto connection = it->lock())
    {
      connection->post(std::forward<Args>(args)...);
      return ++it;
    }
    else
    {
      return connections.erase(it);
    }
  }

  void
  splice_new_connections(connection_list_t& list) const
  {
    std::unique_lock<std::mutex> lock(new_connections_mutex);
    list.splice(list.end(), new_connections);
  }

  mutable std::mutex new_connections_mutex;
  mutable connection_list_t new_connections;

  mutable std::mutex connections_mutex;
  mutable connection_list_t connections;
};

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // SIGNAL_STATE_HPP
