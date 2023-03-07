
#include <iostream>
#include <stdio.h>
#include <OVM_SDK.h>

void PutChar(MEM_POINTER mem){
    std::cout << (char)(*mem._mem)[mem.ebp];
}

void TookChar(MEM_POINTER mem){
    char c;

    std::cin.get(c);
    *mem.eax = (int)c;
}

extern "C" std::vector<Interrupt> _Omain(int last_id){
    Interrupt putChar;
    putChar.id = last_id;
    putChar.name = "putChar";
    putChar.hInt = PutChar;

    Interrupt tookChar;
    tookChar.id = last_id + 1;
    tookChar.name = "tookChar";
    tookChar.hInt = TookChar;


    return {putChar, tookChar};
}