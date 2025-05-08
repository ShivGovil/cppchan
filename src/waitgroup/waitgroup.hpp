#include <condition_variable>
#include <mutex>

class waitgroup {
  size_t threads{};
  std::condition_variable cv;
  std::mutex thread_mtx;

public:
  waitgroup(const waitgroup& other) = delete;
  waitgroup& operator=(const waitgroup& other) = delete;

  void add(size_t n = 1) {
    std::unique_lock<std::mutex> lk(thread_mtx);
    threads += n;
  }

  void done() {
    std::unique_lock<std::mutex> lk(thread_mtx);
    --threads;
    cv.notify_all();
  }

  void wait() {
    std::unique_lock<std::mutex> lk(thread_mtx);
    cv.wait(lk, [this]{ return !threads; });
  }
};
