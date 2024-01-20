# Cache Simulation Models

Created simulation models for different types of cache and replacement policies:

- N-way Associative Cache
  - File Path: `src/nway_cache.cpp`

- LRU (Least Recently Used) Cache
  - File Path: `src/lru_cache.cpp`

- FIFO (First-In-First-Out) Cache
  - File Path: `src/fifo_cache.cpp`

- LFU (Least Frequently Used) Cache
  - File Path: `src/lfu_cache.cpp`

- Random Replacement Cache
  - File Path: `src/rr_cache.cpp`

- Time-to-Live (TTL) Cache
  - File Path: `src/ttl_cache.cpp`

Additionally, the project includes the following files:

- Builder (Dependency Injection)
  - File Path: `src/builder.cpp`

- Cache Dependency Injection
  - File Path: `src/cache_depend_inj.cpp`

- Prototype (Design Pattern)
  - File Path: `src/prototype.cpp`

The project emphasizes the use of dependency injection and other design patterns to create highly abstracted code. Concurrency has been implemented in the LRU cache by instantiating multiple threads and adding synchronization primitives (locks, semaphores, atomics) to gracefully handle concurrent cache accesses.
