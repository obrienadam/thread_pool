#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

class ThreadPool {
public:
  using Task = std::function<void()>;

  ThreadPool(int num_threads);

  ~ThreadPool();

  void add_task(Task task);

  void add_tasks(std::vector<Task> tasks);

  void wait() const;

private:
  std::vector<std::thread> threads_;
  mutable std::mutex m;
  mutable std::condition_variable thread_queue, wait_queue;
  std::deque<Task> task_queue;

  // Used to terminate
  bool run = true;

  // Use to keep track of running tasks.
  int num_tasks_running = 0;
};
