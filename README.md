
# C++11/17 loki

## build with C++11

`make CXXFLAGS=-std=c++11`

## build with C++17

`make CXXFLAGS=-std=c++17`

## build for Windows with mingw64

`make OS=Windows`

## Change

- Remove ide support files
- Necessary change to run with C++11
- Remove AssocVector
- Remove LevelMutex

## TODO

1. ☐ fix all warnings
2. ☐ Mutex optimize
3. ☐ SmallObj check
4. ☐ How to destroy object cached in ChacedFactory.h
