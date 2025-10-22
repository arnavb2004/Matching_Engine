#pragma once
#include "Order.hpp"
#include <map>
#include <list>
#include <unordered_map>
#include <functional>
#include <iostream>

class OrderBook {
private:
    // Price -> list of orders
    std::map<int64, std::list<Order>> asks; // ascending
    std::map<int64, std::list<Order>, std::greater<int64>> bids; // descending
    std::unordered_map<int64, std::pair<int64, std::list<Order>::iterator>> order_index;

    // Helper for matching
    template<typename MapType>
void matchOrder(Order& incoming, MapType& oppBook, bool isBuy,
                const std::function<void(const Trade&)>& tradeCallback) {
    while (incoming.qty > 0 && !oppBook.empty()) {
        auto it_level = oppBook.begin();
        if ((isBuy && it_level->first > incoming.price) || (!isBuy && it_level->first < incoming.price))
            break;

        auto& lst = it_level->second;
        while (incoming.qty > 0 && !lst.empty()) {
            Order& resting = lst.front();
            int64 traded = std::min(incoming.qty, resting.qty);

            Trade tr;
            tr.buy_id = isBuy ? incoming.id : resting.id;
            tr.sell_id = isBuy ? resting.id : incoming.id;
            tr.price = resting.price;
            tr.qty = traded;
            tr.ts = now_ts();

            tradeCallback(tr);

            incoming.qty -= traded;
            resting.qty -= traded;

            if (resting.qty == 0) {
                order_index.erase(resting.id);
                lst.pop_front();
            }
        }
        if (lst.empty()) oppBook.erase(it_level);
    }
    if (incoming.qty > 0) addOrder(incoming);
}


public:
    // Add order to book (without matching)
    void addOrder(const Order& order) {
        if (order.side == Side::BUY) {
            auto& lst = bids[order.price];
            lst.push_back(order);
            order_index[order.id] = {order.price, std::prev(lst.end())};
        } else {
            auto& lst = asks[order.price];
            lst.push_back(order);
            order_index[order.id] = {order.price, std::prev(lst.end())};
        }

    }

    // Process incoming order with trade callback
    void processIncoming(Order incoming, const std::function<void(const Trade&)>& tradeCallback) {
        if (incoming.side == Side::BUY)
            matchOrder(incoming, asks, true, tradeCallback);
        else
            matchOrder(incoming, bids, false, tradeCallback);
    }

    // Cancel an order by ID
    bool cancelOrder(int64 order_id) {
        auto it = order_index.find(order_id);
        if (it == order_index.end()) return false;

        int64 price = it->second.first;
        auto lit = it->second.second;

        auto bit = bids.find(price);
        if (bit != bids.end()) {
            bit->second.erase(lit);
            if (bit->second.empty()) bids.erase(bit);
        } else {
            auto ait = asks.find(price);
            if (ait != asks.end()) {
                ait->second.erase(lit);
                if (ait->second.empty()) asks.erase(ait);
            }
        }
        order_index.erase(it);
        return true;
    }

    // Print top N levels of order book
    void printTop(int levels = 5) const {
        std::cout << "--- Top Asks ---\n";
        int c = 0;
        for (auto it = asks.begin(); it != asks.end() && c < levels; ++it, ++c) {
            int64 total = 0;
            for (auto& o : it->second) total += o.qty;
            std::cout << it->first << " : " << total << "\n";
        }
        std::cout << "--- Top Bids ---\n";
        c = 0;
        for (auto it = bids.begin(); it != bids.end() && c < levels; ++it, ++c) {
            int64 total = 0;
            for (auto& o : it->second) total += o.qty;
            std::cout << it->first << " : " << total << "\n";
        }
    }
};
