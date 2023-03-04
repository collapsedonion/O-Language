//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "Scenary.h"

#define INSTSETUPTWOPARAM(instName) case InstructionType::instName:{switch (sw.top1) {case OperandType::registerO:{switch (sw.top2) {case OperandType::value:{lu->instName(sw.op1.reg, sw.op2.value);break;}case OperandType::registerO:{lu->instName(sw.op1.reg, sw.op2.reg);break;}case OperandType::MAD:{lu->instName(sw.op1.reg, sw.op2.mad);break;}}break;}case OperandType::MAD:{switch (sw.top2) {case OperandType::value:{lu->instName(sw.op1.mad, sw.op2.value);break;}case OperandType::registerO:{lu->instName(sw.op1.mad, sw.op2.reg);break;}case OperandType::MAD:{lu->instName(sw.op1.mad, sw.op2.mad);break;}}break;}}break;}
#define INSTSETUPONEPARAM(instName, unit) case InstructionType::instName:{switch (sw.top1){case OperandType::value:{unit->instName(sw.op1.value); break;} case OperandType::registerO: {unit->instName(sw.op1.reg); break;} case OperandType::MAD:{unit->instName(sw.op1.mad); break;}} break;}
#define INSTSETUPZEROPARAM(instName, unit)case InstructionType::instName:{unit->instName();break;}

namespace O{

    std::pair<int, Scenary::ScriptWord> Scenary::generateScript(int *a, int id) {
        int readSize = 0;
        auto type = (InstructionType)a[id];
        readSize += 1;
        auto opT1 = (OperandType)a[id + readSize];
        readSize += 1;
        auto op1 = readOperand(opT1, a, id + readSize);
        readSize += op1.first;
        auto opT2 = (OperandType)a[id + readSize];
        readSize += 1;
        auto op2 = readOperand(opT2, a, id + readSize);
        readSize += op2.first;

        Scenary::ScriptWord sw;
        sw.instruction = type;
        sw.op1 = op1.second;
        sw.op2 = op2.second;
        sw.top1 = opT1;
        sw.top2 = opT2;
        return {readSize, sw};
    }

    std::pair<int, Scenary::Operand> Scenary::readOperand(Scenary::OperandType opT, int *a, int id) {
        int readSize = 0;
        Scenary::Operand toRet;
        switch (opT) {
            case OperandType::value:
                toRet.value = a[id];
                readSize += 1;
                break;
            case OperandType::registerO:
                toRet.reg = (O::Memory::Registers)a[id];
                readSize++;
                break;
            case OperandType::MAD:
                std::string str;
                while(a[id + readSize] != 0){
                    char c = (char)a[id+readSize];
                    str += c;
                    readSize+=1;
                }
                readSize += 1;
                O::Memory::Registers reg = (O::Memory::Registers)a[id + readSize];
                readSize++;
                int offset = a[id+readSize];
                readSize++;
                O::Memory::MemoryAddressDescriptor mad;
                mad.sectorName = str;
                mad.offset = offset;
                mad.anchor = reg;
                toRet.mad = mad;
                break;
        }

        return {readSize, toRet};
    }

    void Scenary::EvaluateWord(Scenary::ScriptWord sw, LogicUnit *lu, Memory *memoryUnit) {
        switch (sw.instruction) {
            INSTSETUPTWOPARAM(mov);
            INSTSETUPTWOPARAM(add)
            INSTSETUPTWOPARAM(sub)
            INSTSETUPONEPARAM(pop, memoryUnit)
            INSTSETUPONEPARAM(push, memoryUnit)
            INSTSETUPZEROPARAM(ret, lu)
            INSTSETUPONEPARAM(call, lu)
            INSTSETUPZEROPARAM(pushs, memoryUnit)
            INSTSETUPZEROPARAM(pops, memoryUnit)
            INSTSETUPTWOPARAM(cmp)
            INSTSETUPTWOPARAM(move)
            INSTSETUPTWOPARAM(movl)
            INSTSETUPTWOPARAM(movg)
            INSTSETUPONEPARAM(jmp, lu)
            INSTSETUPONEPARAM(jme, lu)
            INSTSETUPONEPARAM(malloc, memoryUnit)
        }
    }
}