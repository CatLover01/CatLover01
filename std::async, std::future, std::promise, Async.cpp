#include <iostream>
#include <future>
#include <thread>

// Using  std::promise   --> Okay
void calculation(std::exception_ptr p, std::promise<long long int>& promise, long long int x, long long int y) {
	std::cout << "\nCalculation Stared...\n";
	long long int sum{};
	for (long long int i{x}; i <= y; i++) {
		sum += i;
	}

	if (sum < 0) { promise.set_exception(p) ; } // Error Handeling
	promise.set_value(sum);
} 

// using  std::async   --> Good
long long int Calculation(long long int x, long long int y) {
	long long int sum{};
	for (long long int i{ x }; i <= y; i++) {
		sum += i;
	}
	return sum;
}

int main()
{
	// using std::async (2x less code)
	long long int x = 0;
	long long int y = 1900000000ll;

	std::future<long long int>future = std::async(std::launch::async, Calculation, x, y);

	// Heavy calculation here
	std::this_thread::sleep_for(std::chrono::milliseconds(1200));

	std::cout << "Sum is " << future.get() << '\n';

// ----------------------------------

	// using std::promise
	long long int x = 0;
	long long int y = 1900000000ll;

	std::exception_ptr p;
	std::promise<long long int>promise;
	std::future<long long int>future = promise.get_future();
	std::thread thread{ calculation, p, std::ref(promise), x, y};

	// Heavy calculation/code could be here while calculating on another thread
	std::this_thread::sleep_for(std::chrono::milliseconds(1500));

	std::cout << "Sum is " << future.get() << '\n';

	thread.join();
}
