//
// Created by Роман  Тимофеев on 28.02.2023.
//

#ifndef OVIRTUALMACHINE_LOGICUNIT_H
#define OVIRTUALMACHINE_LOGICUNIT_H

#include "Memory.h"
#include <OVM_SDK.h>
#include <dlfcn.h>

#define DOUBLEOPINSTRUCTIONDESCRIPTION(instName) void instName(O::Memory::Registers destReg, long long value);void instName(O::Memory::Registers destReg, O::Memory::Registers sourceReg);void instName(O::Memory::MemoryAddressDescriptor dest, long long value);void instName(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source);void instName(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source);void instName(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source);
#define SINGLEOPINSTRUCTIONDESCRIPTION(instName) void instName(O::Memory::Registers destReg); void instName(O::Memory::MemoryAddressDescriptor mad); void instName(long long value);
#define ZEROOPINSTRUCTIONDESCRTIPTION(instName) void instName();

namespace O {
    class LogicUnit {
    private:
        Memory* _mem = nullptr;

        std::vector<Interrupt> registeredInterrupts;
    public:
        LogicUnit(Memory* memoryUnit);

        void LoadNewInterrupts(std::string path);

        void AddNewInterrupt(std::string name, InterruptHandler interrupt);

        DOUBLEOPINSTRUCTIONDESCRIPTION(mov)

        DOUBLEOPINSTRUCTIONDESCRIPTION(add)

        DOUBLEOPINSTRUCTIONDESCRIPTION(sub)

        DOUBLEOPINSTRUCTIONDESCRIPTION(mul)

        DOUBLEOPINSTRUCTIONDESCRIPTION(div)

        DOUBLEOPINSTRUCTIONDESCRIPTION(mod)

        ZEROOPINSTRUCTIONDESCRTIPTION(ret);

        SINGLEOPINSTRUCTIONDESCRIPTION(call);

        DOUBLEOPINSTRUCTIONDESCRIPTION(cmp);

        DOUBLEOPINSTRUCTIONDESCRIPTION(movg);

        DOUBLEOPINSTRUCTIONDESCRIPTION(movl);

        DOUBLEOPINSTRUCTIONDESCRIPTION(move);

        SINGLEOPINSTRUCTIONDESCRIPTION(jmp);

        SINGLEOPINSTRUCTIONDESCRIPTION(jme);

        SINGLEOPINSTRUCTIONDESCRIPTION(interrupt);

        DOUBLEOPINSTRUCTIONDESCRIPTION(AND);

        DOUBLEOPINSTRUCTIONDESCRIPTION(OR);

        DOUBLEOPINSTRUCTIONDESCRIPTION(addf);

        DOUBLEOPINSTRUCTIONDESCRIPTION(subf);

        DOUBLEOPINSTRUCTIONDESCRIPTION(mulf)

        DOUBLEOPINSTRUCTIONDESCRIPTION(divf);

        DOUBLEOPINSTRUCTIONDESCRIPTION(cmpf);

	SINGLEOPINSTRUCTIONDESCRIPTION(ga);
    };
}

#endif //OVIRTUALMACHINE_LOGICUNIT_H
