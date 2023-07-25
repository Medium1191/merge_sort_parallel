#include <iostream>
#include <vector>
#include <future>
#include <algorithm>

template<typename RandomIt>
void merge_sort(RandomIt beg, RandomIt end) {
    if (end - beg > 1) {
        RandomIt mid = beg + (end - beg) / 2;
        std::future<void> left = std::async(std::launch::async, merge_sort<RandomIt>, beg, mid);
        std::future<void> right = std::async(std::launch::async, merge_sort<RandomIt>, mid, end);
        left.get();
        right.get();
        std::inplace_merge(beg, mid, end);
    }
}

template<typename RandomIt>
void parallel_merge_sort(RandomIt beg, RandomIt end) {
    if (end - beg > 1) {
        RandomIt mid = beg + (end - beg) / 2;
        std::future<void> left = std::async(std::launch::async, parallel_merge_sort<RandomIt>, beg, mid);
        std::future<void> right = std::async(std::launch::async, parallel_merge_sort<RandomIt>, mid, end);
        left.get();
        right.get();
        std::inplace_merge(beg, mid, end);
    }
}

template<typename RandomIt>
void parallel_merge_sort_limited_threads(RandomIt beg, RandomIt end) {
    if (end - beg > 1) {
        RandomIt mid = beg + (end - beg) / 2;
        if (std::distance(beg, mid) > 1000000 && std::distance(mid, end) > 1000000) {
            std::promise<void> p;
            auto f = p.get_future();
            std::thread t([&] {
                parallel_merge_sort_limited_threads(beg, mid);
                p.set_value();
                });
            parallel_merge_sort_limited_threads(mid, end);
            f.wait();
            t.join();
        }
        else {
            parallel_merge_sort_limited_threads(beg, mid);
            parallel_merge_sort_limited_threads(mid, end);
        }
        std::inplace_merge(beg, mid, end);
    }
}

int main() {
    std::vector<int> v{3, 1, 4, 1, 5, 9};
    parallel_merge_sort_limited_threads(v.begin(), v.end());
    for (int i : v) {
        std::cout << i << ' ';
    }
}
 // В данном примере используется функция parallel_merge_sort_limited_threads,
// которая рекурсивно разбивает вектор на две части и запускает их вычисление в отдельных потоках.
// Если размеры частей больше 1000000 элементов2, то запускается дополнительный поток.
// Синхронизация результатов происходит через std::promise и std::future.