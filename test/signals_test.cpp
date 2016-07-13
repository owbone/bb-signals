#include "emitter.hpp"
#include "signal.hpp"
#include "slot.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <memory>
#include <vector>

using namespace std;

//------------------------------------------------------------------------------

namespace {

//------------------------------------------------------------------------------

// Check that a single slot can connect to and receive a signal with no
// arguments.
TEST(signals_test, slot_receives_void_signal)
{
  bool received = false;
  bb::emitter<> emit_signal;
  bb::signal<> signal;
  bb::connect(emit_signal, signal);

  bb::slot<> slot{[&received]{ received = true; }};
  bb::connect(signal, slot);

  ASSERT_FALSE(received);

  emit_signal();

  ASSERT_TRUE(received);
}

// Check that a slot which has been destroyed no longer receives signals from
// the signal to which it was connected.
TEST(signals_test, destroyed_slot_doesnt_receive_signal)
{
  bool received = false;
  bb::emitter<> emit_signal;
  bb::signal<> signal;
  bb::connect(emit_signal, signal);

  {
    bb::slot<> slot{[&received]() { received = true; }};
    bb::connect(signal, slot);

    emit_signal();

    EXPECT_TRUE(received);

    received = false;
  }

  emit_signal();

  EXPECT_FALSE(received);
}

// Check that a single slot can connect to and receive a signal with a single
// argument.
TEST(signals_test, slot_receives_single_argument)
{
  bb::emitter<int> emit_signal;
  bb::signal<int> signal;
  bb::connect(emit_signal, signal);

  int received = 0;
  bb::slot<int> slot{[&](int value){ received = value; }};
  bb::connect(signal, slot);

  ASSERT_EQ(0, received);

  for (int value = 0; value < 1000; ++value)
  {
    emit_signal(value);

    EXPECT_EQ(value, received);
  }
}

// Check that multiple slots can connect to and receive a signal with no
// arguments.
TEST(signals_test, multiple_slots_receive_void_signal)
{
  bb::emitter<> emit_signal;
  bb::signal<> signal;
  bb::connect(emit_signal, signal);

  std::array<bool, 10> results = {};
  vector<bb::slot<>> slots_;

  for (bool& received : results)
  {
    bb::slot<> slot{[&](){ received = true; }};
    bb::connect(signal, slot);
    slots_.push_back(std::move(slot));
    ASSERT_FALSE(received);
  }

  emit_signal();

  for (bool received : results)
  {
    EXPECT_TRUE(received);
  }
}

// Check that multiple slots can connect to and receive a signal with a single
// argument.
TEST(signals_test, multiple_slots_receive_single_argument)
{
  bb::emitter<int> emit_signal;
  bb::signal<int> signal;
  bb::connect(emit_signal, signal);

  std::array<int, 10> results = {};
  vector<bb::slot<int>> slots_;

  for (int& result : results)
  {
    bb::slot<int> slot{[&](int value){ result = value; }};
    bb::connect(signal, slot);
    slots_.push_back(std::move(slot));
    ASSERT_EQ(0, result);
  }

  for (int value = 0; value < 100; ++value)
  {
    emit_signal(value);

    for (int result : results)
    {
      EXPECT_EQ(value, result);
    }
  }
}

// Check that values are fully forwarded to the receiving functions and no
// copies are leaked whilst emitting a signal.
TEST(signals_test, parameters_are_released)
{
  bb::emitter<std::shared_ptr<void>> emit_signal;
  bb::signal<std::shared_ptr<void>> signal;
  bb::connect(emit_signal, signal);

  // Check the use_count() of a shared_ptr to make sure that only a single
  // extra copy is made per-function.
  std::shared_ptr<void> result;
  bb::slot<std::shared_ptr<void>> slot{[&](auto value){ result = value; }};
  bb::connect(signal, slot);

  std::shared_ptr<void> counter = std::make_shared<bool>();
  ASSERT_FALSE(result);
  ASSERT_EQ(1u, counter.use_count());
  emit_signal(counter);
  ASSERT_EQ(result, counter);
  ASSERT_EQ(2u, counter.use_count());
}

//------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
