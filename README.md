# Matching_Engine
# Overview
This is a simple stock matching engine implemented in C++. It simulates the core functionality of a trading system where buy and sell orders are matched based on price and time priority.

The engine supports:

1. Placing buy and sell orders

2. Partial fills (an order can be partially executed)

3. Order cancellation

4. Maintaining an order book

NOTE: The stock symbols currently are arbitary strings with no validation with real stock system.

# FEATURES 

1. Add a new order (buy/sell) with specified quantity and price

2. Cancel an order, with any unfilled quantity returned to the user

3. Automatic matching of buy and sell orders based on price and time

4. Basic reporting of executed trades and order book state

# USAGE

1. Once you run the code in matching_engine.cpp (the main file), you will be shown the syntax in which the code expects the input.

2. Now you can place orders by specifying stock symbol, order type (buy/ sell), quantity and price.

3. Cancel orders by providing order id.

4. You can also view the current order book for a particular stock and also the trades done are logged once done.

5. Once you exit, it shows the number of orders performed in the given duration.

# FUTURE IMPROVEMENTS

1. Validate stock symbols against a predefined list.

2. Add market orders and a stop loss order.

3. Persist order book to a database for real time applications.

4. Web interface for easier access.

# LICENSE 

This project is licensed under the MIT License.