#include <iostream>
#include <cassert>
#include "buffered_channel.hpp"
#include <thread>

struct complex {
  int n;
  char c;
};

buffered_channel<complex, 3> chan;

void sender(int sender_id) {
  int start_val = sender_id * 3;
  std::array<complex, 3> values_to_send = {
    complex{start_val++, 'a'},

  }

  for (auto i = 0; i < 3; ++i) {
    chan.send();

  }


}

void receiver() {


}

int main() {
  std::array<std::thread, 3> thread_pool;
  for (auto sender_id = 0; sender_id < 3; ++sender_id) {
    thread_pool[sender_id] = std::thread(sender, sender_id);
  }

  for (auto &t : thread_pool) {
    t.join();
  }

  return 0;
}
