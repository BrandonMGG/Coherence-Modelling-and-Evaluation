#include <stdio.h>
#include <stdlib.h>
#include "cpu.c"
#include "../include/bus.h"
#include "../include/cpu.h"

struct Channel {
    char op[10];
    char address[10];
    char action[10];
};


int main(){

    struct CPU cpu0,cpu1,cpu2;
    struct Channel channel;
    

}