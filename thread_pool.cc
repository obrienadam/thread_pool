#include "thread_pool.h"
#include <cstddef>
#include <iterator>
#include <mutex>

ThreadPool::ThreadPool(int num_threads) {
  threads_.reserve(num_threads);

  for (int i = 0; i < num_threads; ++i) {
    threads_.emplace_back([this]() {
      for (;;) {
        std::unique_lock lock(m);
        thread_queue.wait(lock,
                          [this]() { return !task_queue.empty() || !run; });

        if (!run) {
          return;
        }

        auto task = std::move(task_queue.front());
        task_queue.pop_front();
        num_tasks_running += 1;
        lock.unlock();

        task();

        lock.lock();
        num_tasks_running -= 1;
        wait_queue.notify_one();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::lock_guard lock(m);
    run = false;
  }

  thread_queue.notify_all();

  // Join all threads.
  for (auto &thread : threads_) {
    thread.join();
  }
}

void ThreadPool::add_task(Task task) {
  {
    std::lock_guard lock(m);
    task_queue.emplace_back(std::move(task));
  }

  // May need to wake-up a thread!
  thread_queue.notify_one();
}

void ThreadPool::add_tasks(std::vector<Task> tasks) {
  {
    std::lock_guard lock(m);
    std::move(tasks.begin(), tasks.end(), std::back_inserter(task_queue));
  }

  thread_queue.notify_all();
}

size_t ThreadPool::num_tasks_pending() const {
  std::lock_guard lock(m);
  return num_tasks_running + task_queue.size();
}

void ThreadPool::wait() const {
  std::unique_lock lock(m);
  wait_queue.wait(
      lock, [this]() { return task_queue.empty() && num_tasks_running == 0; });
}
