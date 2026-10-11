# Challenge 6: LRU Cache Design

## Difficulty: Hard | Points: 150 | Topic: Design Patterns & Systems

### Problem Statement
Implement a Least Recently Used (LRU) Cache class of fixed capacity with two operations:
- `get key`: Returns the value of `key` if it exists, otherwise `-1`. Updates recency.
- `put key value`: Inserts or updates the key-value pair. If capacity is exceeded, evicts the least recently used key.

### Input Format
Line 1: Capacity C and number of operations N.
Next N lines:
- `get <key>`
- `put <key> <val>`

### Output Format
Print the return value for each `get` operation.
