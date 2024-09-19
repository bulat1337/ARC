# ARC

## About

ARC implementation

## How to use

### To use in your project

Simply include `arc.h` or `perfect_cache.h` in your code

### To try example main

1) `mkdir build`

2) `cd build`

3) `cmake ..`

4) `make`

5) `./arc.x` or `./perfect.x`

### To run tests

1) `cmake ..`
2) `make`
3) `ctest`

#### CMake options

- If you want to run big data tests build the project like this first
```
cmake .. -DENABLE_ARC_BD_TESTS=ON DENABLE_PERFECT_BD_TESTS=ON
```

-  You can turn off arc or perfect_cache construction like this:
`cmake .. -DBUILD_ARC=OFF` or `cmake .. -DBUILD_PERFECT=OFF`

-  To enable logging run:
```
cmake .. -DENABLE_LOGGING
```

- To build the debug version:
```
cmake .. -DDEBUG=ON
```






