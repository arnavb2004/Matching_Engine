#include "OrderBook.hpp"
#include <unordered_map>
#include <iostream>
#include <chrono>

int main() {
    using namespace std;
    unordered_map<string, OrderBook> market;

    string cmd;
    cout << "Matching Engine CLI\nCommands:\n"
         << "SUBMIT <user> <symbol> <BUY|SELL> <price> <qty>\n"
         << "CANCEL <order_id>\nPRINT <symbol>\nEXIT\n";

    auto start = chrono::high_resolution_clock::now();
    int total_orders = 0;

    auto tradeLogger = [](const Trade& tr){
        cout << "TRADE " << tr.buy_id << " " << tr.sell_id
             << " " << tr.price << " " << tr.qty << "\n";
    };

    while (cin >> cmd) {
        if (cmd == "SUBMIT") {
            string user, sym, sside; int64 price, qty;
            cin >> user >> sym >> sside >> price >> qty;
            Side sd = (sside == "BUY" ? Side::BUY : Side::SELL);
            Order o(user, sym, sd, price, qty);
            market[sym].processIncoming(o, tradeLogger);
            cout << "ORDER_ID " << o.id << "\n";
            total_orders++;
        } else if (cmd == "CANCEL") {
            int64 oid; cin >> oid;
            bool found = false;
            for (auto& p : market) {
                if (p.second.cancelOrder(oid)) { cout << "CANCELLED " << oid << "\n"; found = true; break; }
            }
            if (!found) cout << "NOTFOUND " << oid << "\n";
        } else if (cmd == "PRINT") {
            string sym; cin >> sym;
            if (market.find(sym) == market.end()) { cout << "Empty\n"; continue; }
            market[sym].printTop();
        } else if (cmd == "EXIT") break;
        else cout << "Unknown command\n";
    }

    auto end = chrono::high_resolution_clock::now();
    double duration = chrono::duration<double>(end - start).count();
    if (duration > 0)
        cout << "Processed " << total_orders << " orders in "
             << duration << " seconds ("
             << total_orders / duration << " orders/sec)\n";

    return 0;
}
