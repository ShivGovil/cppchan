#include <cstring>
#include <iostream>
#include "buffered_channel.hpp"
#include "unbuffered_channel.hpp"
#include <thread>

struct complex {
  int n;
  char c;
};

template<typename chan_type>
void run_test(chan_type& channel) {
  std::array<std::thread, 4> thread_pool;
  std::mutex cout_mutex;

  auto sender_func = [&channel, &cout_mutex](int sender_id) {
    int start_val = (sender_id) * 3;
    std::array<complex, 3> values_to_send = {
      complex{.n = start_val++, .c = 'a'},
      complex{.n = start_val++, .c = 'b'},
      complex{.n = start_val, .c = 'c'}
    };
    for (auto i = 0; i < 3; ++i) {
      channel.send(values_to_send[i]);
      std::lock_guard<std::mutex> lg(cout_mutex);
      std::cout << sender_id << " Sent complex | n: " << values_to_send[i].n 
        << " c: " << values_to_send[i].c << '\n';
    }
  };

  auto receiver_func = [&channel, &cout_mutex]() {
    for (auto i = 0; i < 9; ++i) {
      auto val = channel.recv();
      std::lock_guard<std::mutex> lg(cout_mutex);
      std::cout << "Received complex | n: " << val.n 
        << " c: " << val.c << '\n';
    }
  };

  for (auto sender_id = 0; sender_id < 3; ++sender_id) {
    thread_pool[sender_id] = std::thread(sender_func, sender_id);
  }
  thread_pool[3] = std::thread(receiver_func);

  for (auto &t : thread_pool) {
    t.join();
  }
}

int main(int argc, char* argv[]) {
  bool b = false;

  if (argc > 1) {
    std::string arg = argv[1];
    if (arg == "-b") {
      b = true;
    }
  }

  if (b) {
    std::cout << "Testing buffered channel\n";
    buffered_channel<complex, 3> chan;
    run_test(chan);
  } else {
    std::cout << "Testing unbuffered channel\n";
    unbuffered_channel<complex> chan;
    run_test(chan);
  }

  return 0;
}
