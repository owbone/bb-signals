# bb-signals

A simple, header-only signals and slots libary for C++14.

## Features

 * Header-only.
 * No external dependencies.
 * [Executors](http://www.boost.org/doc/libs/1_58_0/doc/html/thread/synchronization.html#thread.synchronization.executors.ref.concept_executor) are
supported, so a slot's execution context can be closely controlled.
 * `signals` are separate from `emitters`, so classes have more fine-grained
control over who can connect and who can emit signals.

## Requirements

 * CMake (v3.4 and above).
 * A C++14 standard-compliant compiler.
 * For unit tests, see [googletest](https://github.com/google/googletest)
requirements.
