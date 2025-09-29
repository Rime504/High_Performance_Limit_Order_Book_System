#pragma once
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include <iomanip>

enum class Side { BUY, SELL };

struct Order {
    uint64_t orderId;
    Side side;
    double price;
    uint32_t quantity;
    uint64_t timestamp;

    Order(uint64_t id, Side s, double p, uint32_t q, uint64_t ts)
        : orderId(id), side(s), price(p), quantity(q), timestamp(ts) {
    }
};

class PriceLevel {
private:
    double price;
    uint32_t totalQuantity;
    std::map<uint64_t, std::shared_ptr<Order>> orders; // orderId -> Order

public:
    explicit PriceLevel(double p) : price(p), totalQuantity(0) {}

    void addOrder(std::shared_ptr<Order> order) {
        orders[order->orderId] = order;
        totalQuantity += order->quantity;
    }

    bool removeOrder(uint64_t orderId) {
        auto it = orders.find(orderId);
        if (it != orders.end()) {
            totalQuantity -= it->second->quantity;
            orders.erase(it);
            return true;
        }
        return false;
    }

    bool modifyOrder(uint64_t orderId, uint32_t newQty) {
        auto it = orders.find(orderId);
        if (it != orders.end()) {
            totalQuantity = totalQuantity - it->second->quantity + newQty;
            it->second->quantity = newQty;
            return true;
        }
        return false;
    }

    uint32_t getTotalQuantity() const { return totalQuantity; }
    double getPrice() const { return price; }
    bool isEmpty() const { return orders.empty(); }
    size_t getOrderCount() const { return orders.size(); }
};

class OrderBook {
private:
    std::string symbol;
    // Buy side: higher prices first (descending)
    std::map<double, std::shared_ptr<PriceLevel>, std::greater<double>> bids;
    // Sell side: lower prices first (ascending)
    std::map<double, std::shared_ptr<PriceLevel>, std::less<double>> asks;

    std::unordered_map<uint64_t, std::pair<Side, double>> orderIndex; // orderId -> (side, price)
    uint64_t nextOrderId;

public:
    explicit OrderBook(const std::string& sym) : symbol(sym), nextOrderId(1) {}

    uint64_t addOrder(Side side, double price, uint32_t quantity, uint64_t timestamp) {
        uint64_t orderId = nextOrderId++;
        auto order = std::make_shared<Order>(orderId, side, price, quantity, timestamp);

        if (side == Side::BUY) {
            if (bids.find(price) == bids.end()) {
                bids[price] = std::make_shared<PriceLevel>(price);
            }
            bids[price]->addOrder(order);
        }
        else {
            if (asks.find(price) == asks.end()) {
                asks[price] = std::make_shared<PriceLevel>(price);
            }
            asks[price]->addOrder(order);
        }

        orderIndex[orderId] = { side, price };
        return orderId;
    }

    bool cancelOrder(uint64_t orderId) {
        auto it = orderIndex.find(orderId);
        if (it == orderIndex.end()) return false;

        Side side = it->second.first;
        double price = it->second.second;

        bool removed = false;
        if (side == Side::BUY) {
            auto bidIt = bids.find(price);
            if (bidIt != bids.end()) {
                removed = bidIt->second->removeOrder(orderId);
                if (bidIt->second->isEmpty()) {
                    bids.erase(bidIt);
                }
            }
        }
        else {
            auto askIt = asks.find(price);
            if (askIt != asks.end()) {
                removed = askIt->second->removeOrder(orderId);
                if (askIt->second->isEmpty()) {
                    asks.erase(askIt);
                }
            }
        }

        if (removed) {
            orderIndex.erase(it);
        }
        return removed;
    }

    bool modifyOrder(uint64_t orderId, uint32_t newQuantity) {
        auto it = orderIndex.find(orderId);
        if (it == orderIndex.end()) return false;

        Side side = it->second.first;
        double price = it->second.second;

        if (side == Side::BUY) {
            auto bidIt = bids.find(price);
            if (bidIt != bids.end()) {
                return bidIt->second->modifyOrder(orderId, newQuantity);
            }
        }
        else {
            auto askIt = asks.find(price);
            if (askIt != asks.end()) {
                return askIt->second->modifyOrder(orderId, newQuantity);
            }
        }
        return false;
    }

    double getBestBid() const {
        return bids.empty() ? 0.0 : bids.begin()->first;
    }

    double getBestAsk() const {
        return asks.empty() ? 0.0 : asks.begin()->first;
    }

    double getSpread() const {
        if (bids.empty() || asks.empty()) return 0.0;
        return getBestAsk() - getBestBid();
    }

    double getMidPrice() const {
        if (bids.empty() || asks.empty()) return 0.0;
        return (getBestBid() + getBestAsk()) / 2.0;
    }

    void printBook(int depth = 5) const {
        std::cout << "\n=== Order Book for " << symbol << " ===\n";
        std::cout << std::fixed << std::setprecision(2);

        std::cout << "\nASKS (Sell Orders):\n";
        std::cout << "Price\t\tQty\tOrders\n";
        std::cout << "-----\t\t---\t------\n";

        int count = 0;
        for (auto it = asks.rbegin(); it != asks.rend() && count < depth; ++it, ++count) {
            std::cout << "$" << it->first << "\t\t"
                << it->second->getTotalQuantity() << "\t"
                << it->second->getOrderCount() << "\n";
        }

        std::cout << "\n--- SPREAD: $" << getSpread() << " | MID: $" << getMidPrice() << " ---\n\n";

        std::cout << "BIDS (Buy Orders):\n";
        std::cout << "Price\t\tQty\tOrders\n";
        std::cout << "-----\t\t---\t------\n";

        count = 0;
        for (auto it = bids.begin(); it != bids.end() && count < depth; ++it, ++count) {
            std::cout << "$" << it->first << "\t\t"
                << it->second->getTotalQuantity() << "\t"
                << it->second->getOrderCount() << "\n";
        }
        std::cout << "\n";
    }

    const std::string& getSymbol() const { return symbol; }
    size_t getBidLevels() const { return bids.size(); }
    size_t getAskLevels() const { return asks.size(); }
};