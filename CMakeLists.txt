cmake_minimum_required(VERSION 3.21)
project(ddc-bus-analyzer)

# Bus Monitor
add_subdirectory(src/bm/)

# Bus Controller
add_subdirectory(src/bc/)

# Remote Terminal
# add_subdirectory(src/rt/)