#pragma once
#include <string>
#include <chrono>

using int64 = long long;

// BUY or SELL
enum class Side { BUY, SELL };

// Unique order ID generator
inline int64 generate_order_id() {
    static int64 global_order_id = 1;
    return global_order_id++;
}

// Timestamp in microseconds
inline int64 now_ts() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// Represents a single order
struct Order {
    int64 id;
    std::string user;
    std::string symbol;
    Side side;
    int64 price;
    int64 qty;
    int64 ts;

    Order(const std::string& u, const std::string& s, Side sd, int64 p, int64 q)
        : id(generate_order_id()), user(u), symbol(s), side(sd), price(p),
          qty(q), ts(now_ts()) {}
};

// Represents a trade
struct Trade {
    int64 buy_id;
    int64 sell_id;
    int64 price;
    int64 qty;
    int64 ts;
};
