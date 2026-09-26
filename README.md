# Limit Order Book / Matching Engine

A console-based limit order book in C++, implementing price-time-priority
matching between buy and sell orders.

## Features

- **Price-time priority matching** — the highest-paying buyer is matched
  against the lowest-asking seller first; ties at the same price are broken
  by arrival order.
- **Partial fills** — a single order can be matched against multiple
  counterparties across separate trades in one pass.
- **Persistent order book** — buy and sell orders are saved to disk
  (`buy_Orders.txt`, `sell_Orders.txt`) and reloaded on the next run, so the
  book survives a restart.
- **Input validation** — rejects non-numeric, negative, zero, or malformed
  input at every prompt without crashing or corrupting later reads.
- **Defensive file parsing** — a corrupted or malformed saved line is
  skipped rather than crashing the program.

## How it works

Each order is stored as one line: `name||price||quantity`.

On every `transaction()` call:
1. Fully-filled orders (quantity `0`) are removed.
2. Buy orders are sorted by price descending; sell orders ascending — so the
   most eager buyer always meets the cheapest seller first.
3. Each buy is matched against sells for the same stock while
   `buy.price >= sell.price`, executing a partial fill for
   `min(buy.quantity, sell.quantity)` shares at a time, until the buy order
   is filled or no more sells qualify.

## Build & run

Requires a C++11 (or later) compiler.

```bash
g++ -std=c++17 main.cpp -o stock_exchange
./stock_exchange
```

## Menu

```
1) Buy a Stock
2) Sell a Stock
3) View Buy Orders
4) View Sell Orders
5) Exit
```
