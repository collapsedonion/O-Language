//
// Created by Роман  Тимофеев on 28.02.2023.
//

#ifndef OVIRTUALMACHINE_SCENARY_H
#define OVIRTUALMACHINE_SCENARY_H

#include "LogicUnit.h"
#include "Memory.h"

namespace O {
    class Scenary {
    public:
        enum OperandType{
            value = 0,
            registerO = 1,
            MAD = 2
        };

        enum InstructionType{
            mov = 0,
            add = 1,
            sub = 2,
            pop = 3,
            push = 4,
            ret = 5,
            call = 6,
        };

        struct Operand{
            int value = 0;
            O::Memory::Registers reg;
            O::Memory::MemoryAddressDescriptor mad;
        };

        struct ScriptWord{
            InstructionType instruction;
            OperandType top1;
            Operand op1;
            OperandType top2;
            Operand op2;
        };

        static std::pair<int, Operand> readOperand(OperandType opT, int* a, int id);
        static std::pair<int, ScriptWord> generateScript(int* a, int id);
        static void EvaluateWord(ScriptWord sw, LogicUnit* lu, Memory* memoryUnit);

    };
}


#endif //OVIRTUALMACHINE_SCENARY_H
