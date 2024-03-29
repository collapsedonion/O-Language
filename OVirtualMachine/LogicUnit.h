//
// Created by Роман  Тимофеев on 28.02.2023.
//

#ifndef OVIRTUALMACHINE_LOGICUNIT_H
#define OVIRTUALMACHINE_LOGICUNIT_H

#include "Memory.h"
#include <map>
#include <OVM_SDK.h>

#define DOUBLEOPINSTRUCTIONDESCRIPTION(instName) void instName(O::Memory::Registers destReg, long long value);void instName(O::Memory::Registers destReg, O::Memory::Registers sourceReg);void instName(O::Memory::MemoryAddressDescriptor dest, long long value);void instName(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source);void instName(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source);void instName(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source);
#define SINGLEOPINSTRUCTIONDESCRIPTION(instName) void instName(O::Memory::Registers destReg); void instName(O::Memory::MemoryAddressDescriptor mad); void instName(long long value);
#define ZEROOPINSTRUCTIONDESCRTIPTION(instName) void instName();

namespace O {
    class LogicUnit {
    private:
        struct DebugSymbol{
            int line = 0;
            std::u32string file = U"";
            int esp_min = 0;
            int esp_max = 0;
            std::string sector = "";
        };

        struct VariableSymbol
        {
            std::string name;
            O::Memory::Registers offset_registor;
            int offset;
            std::string sector;
        };

        struct StopLine{
            std::string sector;
            long long stop_address;
        };
        

    private:
        Memory* _mem = nullptr;

        std::vector<Interrupt> registeredInterrupts;

        std::vector<std::pair<std::u32string, int>> break_points;

        std::vector<StopLine> stop_lines;

        std::vector<DebugSymbol> debug_symbols;

        std::map<std::string, std::vector<VariableSymbol>> components;

    public:
        LogicUnit(Memory* memoryUnit);

        void LoadNewInterrupts(std::string path);

        void RunAtSector(std::string sector_name);

        void LoadDebugSymbols(std::string path);

        void AddBreakPoint(std::u32string path, int line);

        void DebugMode(StopLine sl);

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
