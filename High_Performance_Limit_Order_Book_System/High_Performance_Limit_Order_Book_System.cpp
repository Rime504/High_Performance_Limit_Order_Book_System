#include "OrderBook.h"
#include <chrono>
#include <random>

uint64_t getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void demonstrateOrderBook() {
    OrderBook book("AAPL");

    std::cout << "=================================================\n";
    std::cout << "  High-Performance Limit Order Book System\n";
    std::cout << "=================================================\n";

    // Add initial orders
    std::cout << "\n[1] Adding initial orders...\n";

    // Buy orders (bids)
    uint64_t bid1 = book.addOrder(Side::BUY, 150.50, 100, getCurrentTimestamp());
    uint64_t bid2 = book.addOrder(Side::BUY, 150.45, 200, getCurrentTimestamp());
    uint64_t bid3 = book.addOrder(Side::BUY, 150.40, 150, getCurrentTimestamp());
    uint64_t bid4 = book.addOrder(Side::BUY, 150.50, 50, getCurrentTimestamp());  // Same price level

    // Sell orders (asks)
    uint64_t ask1 = book.addOrder(Side::SELL, 150.55, 100, getCurrentTimestamp());
    uint64_t ask2 = book.addOrder(Side::SELL, 150.60, 150, getCurrentTimestamp());
    uint64_t ask3 = book.addOrder(Side::SELL, 150.65, 200, getCurrentTimestamp());
    uint64_t ask4 = book.addOrder(Side::SELL, 150.55, 75, getCurrentTimestamp()); // Same price level

    book.printBook();

    // Show market statistics
    std::cout << "Market Statistics:\n";
    std::cout << "  Best Bid: $" << book.getBestBid() << "\n";
    std::cout << "  Best Ask: $" << book.getBestAsk() << "\n";
    std::cout << "  Spread: $" << book.getSpread() << "\n";
    std::cout << "  Mid Price: $" << book.getMidPrice() << "\n";
    std::cout << "  Bid Levels: " << book.getBidLevels() << "\n";
    std::cout << "  Ask Levels: " << book.getAskLevels() << "\n";

    // Modify an order
    std::cout << "\n[2] Modifying order " << bid1 << " quantity from 100 to 250...\n";
    book.modifyOrder(bid1, 250);
    book.printBook();

    // Cancel an order
    std::cout << "\n[3] Canceling order " << ask2 << "...\n";
    book.cancelOrder(ask2);
    book.printBook();

    // Add aggressive orders that narrow the spread
    std::cout << "\n[4] Adding aggressive orders to narrow spread...\n";
    book.addOrder(Side::BUY, 150.52, 80, getCurrentTimestamp());
    book.addOrder(Side::SELL, 150.53, 90, getCurrentTimestamp());
    book.printBook();

    std::cout << "New Spread: $" << book.getSpread() << "\n\n";
}

void performanceTest() {
    std::cout << "\n=================================================\n";
    std::cout << "  Performance Test: 100,000 Orders\n";
    std::cout << "=================================================\n\n";

    OrderBook book("SPY");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> priceDist(400.0, 405.0);
    std::uniform_int_distribution<> qtyDist(10, 1000);
    std::uniform_int_distribution<> sideDist(0, 1);

    const int numOrders = 100000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numOrders; ++i) {
        Side side = sideDist(gen) == 0 ? Side::BUY : Side::SELL;
        double price = std::round(priceDist(gen) * 100) / 100; // Round to 2 decimals
        uint32_t qty = qtyDist(gen);
        book.addOrder(side, price, qty, getCurrentTimestamp());
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Added " << numOrders << " orders in " << duration.count() << "ms\n";
    std::cout << "Average: " << (duration.count() * 1000.0 / numOrders) << " microseconds per order\n";
    std::cout << "\nFinal Book State:\n";
    std::cout << "  Bid Levels: " << book.getBidLevels() << "\n";
    std::cout << "  Ask Levels: " << book.getAskLevels() << "\n";
    std::cout << "  Best Bid: $" << book.getBestBid() << "\n";
    std::cout << "  Best Ask: $" << book.getBestAsk() << "\n";
    std::cout << "  Spread: $" << book.getSpread() << "\n\n";

    book.printBook(10);
}

int main() {
    try {
        demonstrateOrderBook();
        performanceTest();

        std::cout << "=================================================\n";
        std::cout << "  System Features Demonstrated:\n";
        std::cout << "=================================================\n";
        std::cout << "  ✓ O(log n) order insertion/deletion\n";
        std::cout << "  ✓ O(1) best bid/ask retrieval\n";
        std::cout << "  ✓ Price-time priority\n";
        std::cout << "  ✓ Order modification support\n";
        std::cout << "  ✓ Real-time spread calculation\n";
        std::cout << "  ✓ Market depth visualization\n";
        std::cout << "  ✓ High-throughput capability\n\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}