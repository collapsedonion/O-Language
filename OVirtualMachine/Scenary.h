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
            pushs = 7,
            pops = 8,
            cmp = 9,
            movg = 10,
            movl = 11,
            move = 12,
            jmp = 13,
            jme = 14,
            malloc = 15,
            free = 16,
            interrupt = 17,
            mul = 18,
            div = 19,
            mod = 20,
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
