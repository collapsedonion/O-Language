//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "Scenary.h"

#define INSTSETUPTWOPARAM(instName) case InstructionType::instName:{switch (sw.top1) {case OperandType::registerO:{switch (sw.top2) {case OperandType::value:{lu->instName(sw.op1.reg, sw.op2.value);break;}case OperandType::registerO:{lu->instName(sw.op1.reg, sw.op2.reg);break;}case OperandType::MAD:{lu->instName(sw.op1.reg, sw.op2.mad);break;}}break;}case OperandType::MAD:{switch (sw.top2) {case OperandType::value:{lu->instName(sw.op1.mad, sw.op2.value);break;}case OperandType::registerO:{lu->instName(sw.op1.mad, sw.op2.reg);break;}case OperandType::MAD:{lu->instName(sw.op1.mad, sw.op2.mad);break;}}break;}}break;}
#define INSTSETUPONEPARAM(instName, unit) case InstructionType::instName:{switch (sw.top1){case OperandType::value:{unit->instName(sw.op1.value); break;} case OperandType::registerO: {unit->instName(sw.op1.reg); break;} case OperandType::MAD:{unit->instName(sw.op1.mad); break;}} break;}
#define INSTSETUPZEROPARAM(instName, unit)case InstructionType::instName:{unit->instName();break;}

namespace O{

    const std::vector<std::string> Scenary::instruction_string_name = {
        "mov",
        "add",
        "sub",
        "pop",
        "push",
        "ret",
        "call",
        "pushs",
        "pops",
        "cmp",
        "movg",
        "movl",
        "move",
        "jmp",
        "jme",
        "malloc",
        "free",
        "interrupt",
        "mul",
        "div",
        "mod",
        "and",
        "or",
        "addf",
        "subf",
        "mulf",
        "divf",
        "cmpf",
        "get adress",
    };

    std::pair<Scenary::InstructionType, std::pair<Scenary::OperandType, Scenary::OperandType>> Scenary::breakInstType(int type){
        int dType = type >> 16;
        short opTypes = (short)type;
        char opType = opTypes >> 8;
        char opType1 = (char)opTypes;
        
        return {(Scenary::InstructionType)dType, {(OperandType)opType, (OperandType)opType1}};
    }

    std::pair<int, Scenary::ScriptWord> Scenary::generateScript(long long *a, long long id) {
        long long readSize = 0;
        int encodedType = (int)a[id];
        readSize += 1;
        auto decoded = breakInstType(encodedType);
        auto op1 = readOperand(decoded.second.first, a, id + readSize);
        readSize += op1.first;
        auto op2 = readOperand(decoded.second.second, a, id + readSize);
        readSize += op2.first;
        Scenary::ScriptWord sw;
        sw.instruction = decoded.first;
        sw.op1 = op1.second;
        sw.op2 = op2.second;
        sw.top1 = decoded.second.first;
        sw.top2 = decoded.second.second;
        return {readSize, sw};
    }

    std::string Scenary::ScriptWord::toString(){
        std::string result;
        if(this->instruction < instruction_string_name.size()){
            result += instruction_string_name[this->instruction];
            result += ": ";
        }else{
            result += this->instruction;
            result += ": ";
        }
        
        switch (this->top1) {
            case value:
                result+="V(";
                result+= std::to_string(this->op1.value);
                result+="):";
                break;
            case registerO:
                result+="R(";
                result+=O::toString(this->op1.reg);
                result+="):";
                break;
            case MAD:
                result+="M(";
                result+=O::toString(this->op1.mad);
                result+="):";
                break;
            default:
                break;
        }
        
        switch (this->top2) {
            case value:
                result+="V(";
                result+= std::to_string(this->op2.value);
                result+=")";
                break;
            case registerO:
                result+="R(";
                result+=O::toString(this->op2.reg);
                result+=")";
                break;
            case MAD:
                result+="M(";
                result+=O::toString(this->op2.mad);
                result+=")";
                break;
            default:
                break;
        }
        
        return result;
    }

    std::pair<int, Scenary::Operand> Scenary::readOperand(Scenary::OperandType opT, long long *a, long long id) {
        long long readSize = 0;
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
                long long offset = a[id+readSize];
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
            INSTSETUPONEPARAM(free, memoryUnit)
            INSTSETUPONEPARAM(interrupt, lu)
            INSTSETUPTWOPARAM(mul)
            INSTSETUPTWOPARAM(div)
            INSTSETUPTWOPARAM(mod)
            INSTSETUPTWOPARAM(AND)
            INSTSETUPTWOPARAM(OR)
            INSTSETUPTWOPARAM(addf)
            INSTSETUPTWOPARAM(subf)
            INSTSETUPTWOPARAM(mulf)
            INSTSETUPTWOPARAM(divf)
            INSTSETUPTWOPARAM(cmpf)
            INSTSETUPONEPARAM(ga, lu);
        }
    }
}
