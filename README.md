# ARC

## About

ARC implementation

## How to use

### To use in your project

Simply include `arc.h` in your code

### To try example main

1) `cd example`

2) `mkdir build`

3) `cd build`

4) `cmake ..`

5) `make`

6) `./arc.x`

### To run tests

`env CTEST_OUTPUT_ON_FAILURE=1 cmake --build ./ --target test`

### Logging

To enable logging run:
```
cmake .. -DENABLE_LOGGING
```






