#include <condition_variable>
#include <cstddef>
#include <cstring>
#include <optional>
#include <semaphore>
#include <mutex>

template <typename T>
class unbuffered_channel {
  std::mutex m;
  std::condition_variable cv_senders;
  std::condition_variable cv_recvers;

  std::optional<T> value{};
  std::binary_semaphore *sender_sem{};

public:
  void send(const T &e) {
    std::binary_semaphore me(0);

    {
      std::unique_lock<std::mutex> lock(m);

      cv_senders.wait(lock, [this]{ return !value.has_value(); });

      value = e;
      sender_sem = &me;

      cv_recvers.notify_one();
    }

    me.acquire();
  }

  T recv() {
    std::unique_lock<std::mutex> lock(m);

    cv_recvers.wait(lock, [this]{ return value.has_value(); });

    T res = std::move(value.value());
    value = std::nullopt;

    std::binary_semaphore *signal = sender_sem;
    sender_sem = nullptr;

    cv_senders.notify_one();
    signal->release();

    return res;
  }

};
