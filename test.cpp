#include <iostream>
#include <future>
#include <thread>

int calculate_value() {
  // Simulate some work
  std::this_thread::sleep_for(std::chrono::seconds(2));
  return 42;
}

int main() {
  // Create a promise
  std::promise<int> promise;

  // Get the future associated with the promise
  std::future<int> future = promise.get_future();

  // Launch a thread to perform work
  std::thread worker_thread([&promise] {
    int result = calculate_value();
    promise.set_value(result);
  });

  // Get the result from the future
  int value = future.get();
  std::cout << "Result: " << value << std::endl;

  worker_thread.join();

  return 0;
}