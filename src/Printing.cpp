#include"../h/Printing.hpp"
void printString(const char* string) {
    while (*string!='\0') {
        putc(*string);
        string++;
    }
}