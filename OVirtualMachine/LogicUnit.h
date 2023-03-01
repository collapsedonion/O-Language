//
// Created by Роман  Тимофеев on 28.02.2023.
//

#ifndef OVIRTUALMACHINE_LOGICUNIT_H
#define OVIRTUALMACHINE_LOGICUNIT_H

#include "Memory.h"

#define DOUBLEOPINSTRUCTIONDESCRIPTION(instName) void instName(O::Memory::Registers destReg, int value);void instName(O::Memory::Registers destReg, O::Memory::Registers sourceReg);void instName(O::Memory::MemoryAddressDescriptor dest, int value);void instName(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source);void instName(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source);void instName(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source);
#define SINGLEOPINSTRUCTIONDESCRIPTION(instName) void instName(O::Memory::Registers destReg); void instName(O::Memory::MemoryAddressDescriptor mad); void instName(int value);
#define ZEROOPINSTRUCTIONDESCRTIPTION(instName) void instName();

namespace O {
    class LogicUnit {
    private:
        Memory* _mem = nullptr;
    public:
        LogicUnit(Memory* memoryUnit);
        DOUBLEOPINSTRUCTIONDESCRIPTION(mov)

        DOUBLEOPINSTRUCTIONDESCRIPTION(add)

        DOUBLEOPINSTRUCTIONDESCRIPTION(sub)

        ZEROOPINSTRUCTIONDESCRTIPTION(ret);

        SINGLEOPINSTRUCTIONDESCRIPTION(call);
    };
}

#endif //OVIRTUALMACHINE_LOGICUNIT_H