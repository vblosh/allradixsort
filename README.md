# About allradixsort library
It is a radix sort implementation for all primitive data types :
uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double

It is ~10-20x times faster as a std::sort for small data types on arrays of 1.000.000 elements, see test results.

It is a header only library, simply add header files to your project.

# Using

1. Use std::sort like interface for arrays of primitive data types.
```
  allradixsort::sort(arr.begin(), arr.end()); 
```


2. Use extract key field lambda function for arrays of complex data types, for example
```
  struct Item 
  {
      uint32_t Id; // Key field in the complex type to sort on
      // Some additional data
      float data;
      // some more data ...
  }
  
  std::vector<Item> arr;
  // fill array
  // sort it using extract key field lambda function : [](auto& element) -> uint32_t& { return element.Id; }
  allradixsort::sort<uint32_t>(arr.begin(), arr.end(), [](auto& element) -> uint32_t& { return element.Id; });
```

# Hacking

## Building

This project uses the CMake build-system. To build it, simply run the following commands:

```console
$ mkdir build
$ cd build/
$ cmake ..
$ make
```

## Running perfomance tests

1. Build the project
2. `cd build/ &&  perftests/runPerfomanceTests`

## Running unit tests

This project uses *GoogleTest*.

1. Build the project
2. `cd build/ && unittests/runUnitTests`

# Test results on Intel(R) Core(TM) i5-12500H 2.50 GHz
```
Using uint8_t data
 std::sort 1000000 elements 10 times,  total 10000000 duration 264 ms
radix sort 1000000 elements 10 times,  total 10000000 duration 28 ms
radix sort is 9.42857 times faster for 1000000 elements
Using uint16_t data
 std::sort 1000000 elements 10 times,  total 10000000 duration 596 ms
radix sort 1000000 elements 10 times,  total 10000000 duration 31 ms
radix sort is 19.2258 times faster for 1000000 elements
Using uint32_t data
 std::sort 1000000 elements 10 times,  total 10000000 duration 674 ms
radix sort 1000000 elements 10 times,  total 10000000 duration 69 ms
radix sort is 9.76812 times faster for 1000000 elements
Using uint64_t data
 std::sort 1000000 elements 10 times,  total 10000000 duration 689 ms
radix sort 1000000 elements 10 times,  total 10000000 duration 182 ms
radix sort is 3.78571 times faster for 1000000 elements
Using int8_t data
 std::sort 1000000 elements 10 times,  total 10000000 duration 259 ms
radix sort 1000000 elements 10 times,  total 10000000 duration 34 ms
radix sort is 7.61765 times faster for 1000000 elements
Using int16_t data
 std::sort 1000000 elements 10 times,  total 10000000 duration 576 ms
radix sort 1000000 elements 10 times,  total 10000000 duration 32 ms
radix sort is 18 times faster for 1000000 elements
Using int32_t data
 std::sort 1000000 elements 10 times,  total 10000000 duration 677 ms
radix sort 1000000 elements 10 times,  total 10000000 duration 74 ms
radix sort is 9.14865 times faster for 1000000 elements
Using int64_t data
 std::sort 1000000 elements 10 times,  total 10000000 duration 685 ms
radix sort 1000000 elements 10 times,  total 10000000 duration 182 ms
radix sort is 3.76374 times faster for 1000000 elements
Using float data
 std::sort 1000000 elements 10 times,  total 10000000 duration 736 ms
radix sort 1000000 elements 10 times,  total 10000000 duration 85 ms
radix sort is 8.65882 times faster for 1000000 elements
Using double data
 std::sort 1000000 elements 10 times,  total 10000000 duration 744 ms
radix sort 1000000 elements 10 times,  total 10000000 duration 297 ms
radix sort is 2.50505 times faster for 1000000 elements
```
