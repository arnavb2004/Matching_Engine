#include "dispatcher.hpp"
#include "benchmark.hpp"
#include <iostream>
#include <chrono>

int main() {
    using namespace std;

    cout << "=== Matching Engine CLI ===\n";
    cout << "Commands:\n"
         << "SUBMIT <user> <symbol> <BUY|SELL> <price> <qty>\n"
         << "PRINT\n"
         << "BENCHMARK <N>\n"
         << "EXIT\n\n";

    Dispatcher dispatcher(thread::hardware_concurrency()); // automatically use all CPU cores

    string cmd;
    int total_orders = 0;
    auto start = chrono::high_resolution_clock::now();

    while (cin >> cmd) {
        if (cmd == "SUBMIT") {
            string user, symbol, side_str;
            int64_t price, qty;
            cin >> user >> symbol >> side_str >> price >> qty;

            Side side = (side_str == "BUY" ? Side::BUY : Side::SELL);
            Order order(user, symbol, side, price, qty);
            dispatcher.submitOrder(order);

            cout << "Order submitted: " << order.id << "\n";
            total_orders++;
        }

        else if (cmd == "PRINT") {
            dispatcher.printAllBooks();
        }

        else if (cmd == "BENCHMARK") {
            int N; cin >> N;
            unordered_map<string, OrderBook> temp_market; // Benchmark uses standalone map
            Benchmark::runBenchmark(temp_market, N);
            total_orders += N;
        }

        else if (cmd == "EXIT") break;
        else cout << "Unknown command.\n";
    }

    auto end = chrono::high_resolution_clock::now();
    double dur = chrono::duration<double>(end - start).count();

    cout << "\nProcessed " << total_orders << " orders in "
         << dur << " seconds ("
         << (dur > 0 ? total_orders / dur : 0)
         << " orders/sec)\n";

    return 0;
}
