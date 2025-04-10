// Need to think about how this would work in a switch() {case} thing

#include <condition_variable>
#include <cstddef>
#include <cstring>
#include <semaphore>
#include <mutex>

template <typename T, uint32_t C>
requires(C > 0)
class buffered_channel {
  struct item {
    T value;
    std::binary_semaphore *sender;
  };

  item *buffer;
  size_t size{};
  size_t start{}, end{};
  std::mutex m;
  std::condition_variable cv_senders;
  std::condition_variable cv_recvers;

public:
  buffered_channel() : buffer(new item[C]) {}

  void send(const T &e) {
    std::binary_semaphore me(0);

    {
      std::unique_lock<std::mutex> lock(m);

      cv_senders.wait(lock, [this]{ return size < C; });

      buffer[end] = {e, &me}; // We should automatically copy here right?
      end = (end + 1) % C;
      ++size;

      cv_recvers.notify_one();
    }

    me.acquire();
  }

  T recv() {
    std::unique_lock<std::mutex> lock(m);

    cv_recvers.wait(lock, [this]{ return size > 0; });

    auto res = buffer[start++];
    start %= C;
    --size;

    cv_senders.notify_one();
    res.sender->release();

    return res.value;
  }

  ~buffered_channel() {
    delete[] buffer;
  }

};
