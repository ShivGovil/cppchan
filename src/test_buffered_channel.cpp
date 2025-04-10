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
    complex{.n = start_val++, .c = 'a'},
    complex{.n = start_val++, .c = 'b'},
    complex{.n = start_val, .c = 'c'}
  };

  for (auto i = 0; i < 3; ++i) {
    chan.send(values_to_send[i]);
    std::cout << sender_id << " Sent complex | n: " << values_to_send[i].n 
      << " c: " << values_to_send[i].c << '\n';
  }
}

void receiver() {
  for (auto i = 0; i < 9; ++i) {
    auto val = chan.recv();
    std::cout << "Received complex | n: " << val.n 
      << " c: " << val.c << '\n';
  }
}

int main() {
  std::array<std::thread, 4> thread_pool;
  for (auto sender_id = 0; sender_id < 3; ++sender_id) {
    thread_pool[sender_id] = std::thread(sender, sender_id);
  }
  thread_pool[3] = std::thread(receiver);

  std::cout << "All threads spawned\n";

  for (auto &t : thread_pool) {
    t.join();
  }

  return 0;
}
