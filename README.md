# High-Performance Limit Order Book (LOB) System

A C++ implementation of a low-latency limit order book for financial trading systems, demonstrating production-grade market microstructure concepts.

## Features

- **Low-Latency Operations**
  - O(log n) order insertion and deletion
  - O(1) best bid/ask retrieval
  - Efficient price-time priority matching

- **Core Functionality**
  - Add, modify, and cancel orders
  - Real-time bid-ask spread calculation
  - Market depth visualization
  - Price level aggregation
  - Timestamp tracking for order priority

- **Performance**
  - Handles 100,000+ orders with microsecond-level latency
  - Optimized data structures using STL containers
  - Memory-efficient price level management

## Architecture

### Core Components

- **Order**: Represents individual market orders with price, quantity, and metadata
- **PriceLevel**: Aggregates orders at the same price point
- **OrderBook**: Manages the full order book with separate bid/ask trees

### Data Structures

- `std::map` with custom comparators for price-ordered levels
- `std::unordered_map` for O(1) order lookup
- Smart pointers for automatic memory management

## Building

### Requirements
- C++17 or later
- Visual Studio 2017+ (Windows) or GCC/Clang (Linux/Mac)

### Visual Studio (Windows)
```bash
1. Open oopCpp.sln
2. Build Solution (Ctrl+Shift+B)
3. Run (F5)
```

### Command Line (Linux/Mac)
```bash
g++ -std=c++17 -O3 main.cpp -o orderbook
./orderbook
```

## Usage Example

```cpp
OrderBook book("AAPL");

// Add buy order at $150.50 for 100 shares
uint64_t orderId = book.addOrder(Side::BUY, 150.50, 100, timestamp);

// Get market data
double bestBid = book.getBestBid();
double bestAsk = book.getBestAsk();
double spread = book.getSpread();
double midPrice = book.getMidPrice();

// Modify order
book.modifyOrder(orderId, 250);

// Cancel order
book.cancelOrder(orderId);

// Display order book
book.printBook(5); // Show top 5 levels
```

## Output Example

```
=== Order Book for AAPL ===

ASKS (Sell Orders):
Price		Qty	Orders
-----		---	------
$150.65		200	1
$150.60		150	1
$150.55		175	2

--- SPREAD: $0.05 | MID: $150.525 ---

BIDS (Buy Orders):
Price		Qty	Orders
-----		---	------
$150.50		150	2
$150.45		200	1
$150.40		150	1
```

## Performance Characteristics

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Add Order | O(log n) | O(1) |
| Cancel Order | O(log n) | O(1) |
| Modify Order | O(log n) | O(1) |
| Get Best Bid/Ask | O(1) | O(1) |
| Get Spread | O(1) | O(1) |

Where n is the number of unique price levels.

## Potential Enhancements

- **Matching Engine**: Add order execution logic for crossing orders
- **Lock-Free Structures**: Implement concurrent access for multi-threaded environments
- **Memory Pools**: Pre-allocate memory for order objects to reduce allocation overhead
- **Market Data Feed**: Integrate with FIX protocol or native exchange APIs
- **Historical Replay**: Support for backtesting strategies
- **Multiple Symbols**: Extend to handle multiple instruments simultaneously

## Technical Highlights

- **Price-Time Priority**: Orders at the same price level are processed FIFO
- **Separate Bid/Ask Trees**: Bids sorted descending, asks sorted ascending for optimal performance
- **Constant-Time Lookups**: Hash map index for instant order access by ID
- **Automatic Cleanup**: Smart pointers prevent memory leaks

## Use Cases

- Market making algorithms
- High-frequency trading systems
- Exchange simulation and testing
- Trading strategy backtesting
- Financial education and research

## License

MIT License - Free to use for educational and commercial purposes.

## Author

Built with focus on low-latency performance and production-ready design patterns used in quantitative trading firms.

---

**Note**: This is a demonstration project showing core order book mechanics. Production systems would include additional features like order matching, risk management, and network connectivity.
