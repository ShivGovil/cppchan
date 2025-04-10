// Includes

// What is a channel?
// Is it not just a thread safe queue? -- how do we get native blocking without a context switch?
// We need to atomically push and pull from an unbuffered channel
// If buffered, we must block if pulling when empty
// If not, we just pull
// Must block on push until it is not full
//
// We can't push or pop references if those values go out of scope
// Need to think about how this would work in a switch() {case} thing

#include <condition_variable>
#include <cstddef>
#include <cstring>
#include <semaphore>
#include <mutex>

// This is literally a thread safe queue implementation with more waiting
template <typename T, uint32_t C>
class channel {
  std::pair<T, std::binary_semaphore*> *buffer;
  size_t size;
  size_t start{}, end{};
  std::mutex m;
  std::condition_variable cv;

  // void grow(size_t new_cap) {
    // T *new_buffer = new T[new_cap];
    // memcpy(new_buffer, buffer, capacity);
    // capacity = new_cap;
    // delete[] buffer;
    // buffer = new_buffer;
  // }

public:
  channel(size_t size_ = 0)
  : buffer(new T[size_]),
  size(size_) {}

  // Imagine buffered for now
  void send(T e) {
    std::unique_lock<std::mutex> lock(m);

    cv.wait(lock, [this]{ return size < C; });

    std::binary_semaphore me(0);

    buffer[end] = {e, &me};
    end = (end + 1) % C;

    me.acquire();
  }

  T recv() {
    std::unique_lock<std::mutex> lock(m);

    cv.wait(lock, [this]{ return size > 0; });

    auto res = buffer[start];
    start = (start + 1) % C;

    res.second->release();


  }

  ~channel() {
    delete[] buffer;
  }


};
