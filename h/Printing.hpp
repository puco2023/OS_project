<<<<<<< Updated upstream
#ifndef PRINTING_HPP
#define PRINTING_HPP
#include"../h/Syscall_c_api.hpp"
void printString(const char* string);
=======
#ifndef PRINTING_HPP_
#define PRINTING_HPP
#include "../lib/hw.h"
#include"../h/syscall_cpp.hpp"
void printString(const char* string);
void printInteger(uint64 integer);
>>>>>>> Stashed changes
#endif