#include <cmath>
#include <cstddef>
#include <iostream>
#include <mutex>

#include "thread_pool.h"

std::vector<size_t> prime_factors(size_t n) {
  std::vector<size_t> result;

  while (n % 2 == 0) {
    result.push_back(2);
    n = n / 2;
  }

  for (size_t i = 3; i <= std::sqrt(n); i = i + 2) {
    while (n % i == 0) {
      result.push_back(i);
      n = n / i;
    }
  }

  if (n > 2) {
    result.push_back(n);
  }

  return result;
}

int main(int argc, const char **argv) {
  ThreadPool thread_pool(8);

  // Find numbers that are the product of two prime numbers.
  std::mutex m;

  size_t num_results = 0;
  size_t largest_number = 0;
  size_t start = 90000000;
  size_t end = 100000000;
  for (size_t i = start; i < end; ++i) {
    thread_pool.add_task([i, &m, &num_results, &largest_number]() {
      auto result = prime_factors(i);

      if (result.size() == 20) {
        std::lock_guard lock(m);
        ++num_results;
        largest_number = std::max(largest_number, i);
      }
    });
  }

  std::cout << "Finished adding all tasks to queue..." << std::endl;
  thread_pool.wait();

  std::cout << "Results found: " << num_results << " / " << end << " ("
            << double(num_results) / end * 100 << "% )"
            << "\nLargest number: " << largest_number << std::endl;

  return 0;
}
