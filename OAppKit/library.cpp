
#include <OVM_SDK.h>
#include "InterruptDefinitions.h"

extern "C" std::vector<Interrupt> _Omain(int last_id){
    Interrupt createApplication;
    createApplication.name = "initApplication";
    createApplication.hInt = CreateApplication;
    createApplication.id = last_id;

    Interrupt getSharedApplication;
    getSharedApplication.id = last_id+1;
    getSharedApplication.name = "getSharedApplication";
    getSharedApplication.hInt = GetSharedApplication;

    return {createApplication, getSharedApplication};
}