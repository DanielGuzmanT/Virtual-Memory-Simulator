cmake_minimum_required(VERSION 3.15)
project(vmsim C)

set(CMAKE_C_STANDARD 11)

add_executable(vmsim main.c vmsim.c vmsim.h options.c options.h fault.c fault.h pagetable.c pagetable.h physmem.c physmem.h stats.c stats.h util.c util.h)