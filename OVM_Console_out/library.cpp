
#include <iostream>
#include <OVM_SDK.h>

void PutChar(MEM_POINTER mem){
    std::cout << (char)(*mem._mem)[mem.ebp];
}

extern "C" std::vector<Interrupt> _Omain(int last_id){
    Interrupt putChar;
    putChar.id = last_id;
    putChar.name = "putChar";
    putChar.hInt = PutChar;

    return {putChar};
}