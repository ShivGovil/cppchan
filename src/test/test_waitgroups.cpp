#include <random>
#include <thread>
#include <cstddef>
#include <iostream>
#include <unbuffered_channel.hpp>
#include <vector>
#include <waitgroup.hpp>

constexpr size_t THREAD_COUNT{30};

void worker(unbuffered_channel<int> *chan, int ID) {
  static thread_local std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<> dis(0, 499);

  std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
  chan->send(ID);
}

void waiter(waitgroup *wg) {
  wg->wait();
  std::cout << "SUCCESSFULLY WAITED\n";
}

int main() {
  std::vector<std::thread> pool(THREAD_COUNT);
  unbuffered_channel<int> chan;
  waitgroup wg;

  wg.add(THREAD_COUNT);
  std::thread w(waiter, &wg);

  for (size_t tr = 0; tr < THREAD_COUNT; ++tr) {
    pool[tr] = std::thread(worker, &chan, tr+1);
  }

  for (size_t i = 0; i < THREAD_COUNT; ++i) {
    int stars = chan.recv();
    for (auto j = 0; j < stars; ++j) {
      std::cout << '*';
    }
    std::cout << '\n';
    wg.done();
  }

  w.join();

  for (auto &tr : pool) {
    tr.join();
  }
}
