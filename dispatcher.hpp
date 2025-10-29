#pragma once
#include "thread_pool.hpp"
#include "orderbook.hpp"
#include <unordered_map>
#include <string>
#include <mutex>
#include <memory>
#include <iostream>

class Dispatcher {
private:
    ThreadPool pool;

    struct MarketEntry {
        OrderBook book;
        std::mutex mtx;
    };

    std::unordered_map<std::string, std::shared_ptr<MarketEntry>> markets;
    std::mutex market_map_mutex; // only guards access to the map itself

public:
    Dispatcher(size_t num_threads) : pool(num_threads) {}

    // Handle incoming orders concurrently
    void submitOrder(Order order) {
        pool.enqueue([this, order = std::move(order)]() mutable {
            std::shared_ptr<MarketEntry> entry;

            // Lock the market map only while finding/creating the entry
            {
                std::lock_guard<std::mutex> map_lock(market_map_mutex);
                if (markets.find(order.symbol) == markets.end()) {
                    markets[order.symbol] = std::make_shared<MarketEntry>();
                }
                entry = markets[order.symbol];
            }

            // Lock only this symbol's book
            std::lock_guard<std::mutex> book_lock(entry->mtx);
            entry->book.processIncoming(order, [](const Trade& t) {
                std::cout << "TRADE: " << t.buy_id << " bought from "
                          << t.sell_id << " @ " << t.price
                          << " for qty " << t.qty << "\n";
            });
        });
    }

    bool cancelOrder(int64_t order_id) {
        std::lock_guard<std::mutex> lock(market_map_mutex);
        for (auto& [sym, entry] : markets) {
            std::lock_guard<std::mutex> book_lock(entry->mtx);
            if (entry->book.cancelOrder(order_id)) {
                std::cout << "CANCELLED " << order_id << "\n";
                return true;
            }
        }
        return false;
    }

    void printAllBooks() {
        std::lock_guard<std::mutex> lock(market_map_mutex);
        for (auto& [sym, entry] : markets) {
            std::lock_guard<std::mutex> book_lock(entry->mtx);
            std::cout << "\n=== Order Book for " << sym << " ===\n";
            entry->book.printTop();
        }
    }
};
