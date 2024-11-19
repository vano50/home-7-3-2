#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>
#include <functional>

template<typename Iterator, typename Function>
void parallel_for_each(Iterator first, Iterator last, Function f, size_t num_threads = std::thread::hardware_concurrency())
{
    size_t length = std::distance(first, last);
    if (length == 0 || num_threads == 0) {
        return;
    }

    if (num_threads > length) {
        num_threads = length;
    }

    size_t block_size = length / num_threads;
    size_t remainder = length % num_threads;

    std::vector<std::future<void>> futures;

    for (size_t i = 0; i < num_threads; ++i) {
        Iterator begin = first + i * block_size;
        Iterator end = first + (i + 1) * block_size;

        if (remainder > 0) {
            ++end;
            --remainder;
        }

        futures.emplace_back(std::async([begin, end, &f]() {
            std::for_each(begin, end, f);
            }));
    }

    for (auto& fut : futures) {
        fut.wait();
    }
}

int main()
{
    std::vector<int> v(1000);
    std::iota(v.begin(), v.end(), 1);

    auto print = [](int& x) {
        std::cout << x << ' ';
        };

    parallel_for_each(v.begin(), v.end(), print);

    return 0;
}