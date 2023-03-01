//
// Created by Роман  Тимофеев on 01.03.2023.
//

#ifndef O_BYTECODE_GENERATOR_GENEERATOR_H
#define O_BYTECODE_GENERATOR_GENEERATOR_H

#include <vector>
#include <string>

#define GENERATETWOPARAMETERSDEF(NAME) static std::vector<int> NAME(Registers op1, int op2); static std::vector<int> NAME(Registers op1, Registers op2); static std::vector<int> NAME(Registers op1, std::string sector, int offset, Registers anchor); static std::vector<int> NAME(std::string sectorDest, int offsetDest, Registers anchorDest, int op2); static std::vector<int> NAME(std::string sectorDest, int offsetDest, Registers anchorDest, Registers op2); static std::vector<int> NAME(std::string sectorD, int offsetD, Registers anchorD, std::string sector, int offset, Registers anchor);
#define GENERATEZEROPARAMDEF(NAME) static std::vector<int> NAME();
#define GENERATEONEPARAMDEF(NAME) static std::vector<int> NAME(int value); static std::vector<int> NAME(Registers op); static std::vector<int> NAME(std::string sector, int offset, Registers anchor);

static class Geneerator {
public:
    enum class Registers{
        NULLREG= -1,
        eax = 0,
        ebx = 1,
        ecx = 2,
        edx = 3,
        eip = 4,
        esp = 5,
        edp = 6,
        esi = 7,
        edi = 8,
        mc1 = 10,
        mc2 = 11,
        mc3 = 12,
        aa0 = 13,
        aa1 = 14,
        ebp = 15
    };

    static std::vector<int> generateMad(std::string sectorName, Registers anchor, int offset);
    static std::vector<int> generateRegister(Registers reg);
    static std::vector<int> generateValue(int value);

    GENERATETWOPARAMETERSDEF(add)
    GENERATETWOPARAMETERSDEF(mov)
    GENERATETWOPARAMETERSDEF(sub)
    GENERATEZEROPARAMDEF(ret);
    GENERATEONEPARAMDEF(call);
    GENERATEONEPARAMDEF(push);
    GENERATEONEPARAMDEF(pop);
};


#endif //O_BYTECODE_GENERATOR_GENEERATOR_H
