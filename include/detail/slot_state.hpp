#ifndef SLOT_STATE_HPP
#define SLOT_STATE_HPP

//------------------------------------------------------------------------------

namespace bb {

//------------------------------------------------------------------------------

namespace detail {

//------------------------------------------------------------------------------

class slot_state_base
{
public:
  virtual ~slot_state_base() { }
  virtual void reset() = 0;
};

template <class... Params>
class slot_state : public slot_state_base
{
public:
  virtual ~slot_state() { }
  virtual void post(Params... args) const = 0;
};

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

#endif // SLOT_STATE_HPP
