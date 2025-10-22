#pragma once
#include "order.hpp"
#include "orderbook.hpp"
#include <random>
#include <chrono>
#include <iomanip>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <algorithm>

class Benchmark {
public:
    static Order generateRandomOrder() {
        static std::vector<std::string> users = {"U1", "U2", "U3", "U4", "U5"};
        static std::vector<std::string> symbols = {"INFY", "TCS", "RELIANCE", "HDFCBANK", "ICICI"};
        static std::mt19937 rng(std::random_device{}());

        std::uniform_int_distribution<int> userDist(0, (int)users.size() - 1);
        std::uniform_int_distribution<int> symDist(0, (int)symbols.size() - 1);
        std::uniform_int_distribution<int> sideDist(0, 1);

        // Simulate realistic clustered price levels (like a market spread)
        std::normal_distribution<double> priceDist(100.0, 2.0); // mean ₹100, std ₹2
        std::uniform_int_distribution<int> qtyDist(1, 100);

        std::string user = users[userDist(rng)];
        std::string sym = symbols[symDist(rng)];
        Side side = (sideDist(rng) == 0 ? Side::BUY : Side::SELL);
        
        // Clip to a reasonable price range (₹90–₹110)
        int price = std::max(90, std::min(110, (int)priceDist(rng)));
        int qty = qtyDist(rng);

        static int64_t id_counter = 1; // keep internal ID generator
        int64_t id = id_counter++;

        return Order(user, sym, side, price, qty);
    }

    static void runBenchmark(std::unordered_map<std::string, OrderBook>& market, int N) {
        using namespace std::chrono;
        auto start = high_resolution_clock::now();

        std::unordered_map<std::string, int> perSymbolCount;

        for (int i = 0; i < N; i++) {
            Order o = generateRandomOrder();
            perSymbolCount[o.symbol]++;
            market[o.symbol].processIncoming(std::move(o), [](const Trade&) {});
        }

        auto end = high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "\n--- Benchmark Results ---\n";
        std::cout << "Orders processed: " << N << "\n";
        std::cout << "Total time: " << std::fixed << std::setprecision(2) << ms << " ms\n";
        std::cout << "Throughput: " << std::fixed << std::setprecision(2)
                  << (N / (ms / 1000.0)) << " orders/sec\n\n";

        std::cout << "Order distribution per symbol:\n";
        for (auto& it : perSymbolCount) {
            const std::string& sym = it.first;
            int count = it.second;
            std::cout << "  " << std::setw(10) << std::left << sym << ": " << count << "\n";
        }
        std::cout << std::endl;
    }
};
