//
// Created by Роман  Тимофеев on 05.03.2023.
//

#include <OVM_SDK.h>
#include <stdio.h>

void testH(MEM_POINTER mp){
    printf("Hello, lib");
    *mp.eax = 273;
}

extern "C" std::vector<Interrupt> _Omain(int last_id){
    Interrupt testInt;
    testInt.id = last_id;
    testInt.name = "test";
    testInt.hInt = testH;

    return {testInt};
}