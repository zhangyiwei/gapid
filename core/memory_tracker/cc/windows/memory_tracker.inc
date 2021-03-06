/*
 * Copyright (C) 2017 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <functional>
namespace gapii {
namespace track_memory {

class WindowsMemoryTracker {
  public:
    WindowsMemoryTracker(std::function<bool(void*)> segfault_function):
      vectored_exception_handler_(nullptr),
      handle_segfault_(segfault_function) {
    }

    bool IsInstalled() const { return vectored_exception_handler_; }
  protected:
  // A static wrapper of HandleSegfault() as VectoredException() asks for a static
  // function.
  static long int __stdcall VectoredExceptionHandler(void *info);

  // EnableMemoryTrackerImpl calls sigaction() to register the new segfault
  // handler to the thread (and affects all the child threads), stores the
  // original segfault handler. This method sets the static pointer:
  // unique_tracker to |this| pointer. The signal handler will not be set again
  // if the signal handler has already been set by the same memory tracker
  // instance before, all following calls to this function will just return
  // true.
  bool EnableMemoryTrackerImpl();

  // DisableMemoryTrackerImpl recovers the original segfault signal
  // handler. Returns true if the handler is recovered successfully,
  // othwerwise returns false.
  bool DisableMemoryTrackerImpl();

  private:
  void* vectored_exception_handler_; // The currently registered vectored exception
                                     // handler. Nullptr if this is none.
   std::function<bool(void*)> handle_segfault_; // The function to be called when there is a segfault.
};

typedef MemoryTrackerImpl<WindowsMemoryTracker> MemoryTracker;
extern WindowsMemoryTracker* unique_tracker;

} // namespace track_memory
} // namespace gapii
