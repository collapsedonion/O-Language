//
// Created by Роман  Тимофеев on 01.03.2023.
//

#include "Geneerator.h"


#define TORET std::vector<int> toRet;
#define HEADER(name, paramOne, paramTwo) std::vector<int> Geneerator::name(paramOne, paramTwo)
#define ADDVECTORTOVECTOR(target, source) target.insert(target.end(), source.begin(), source.end());
#define LOADREG(paramOut, paramIn) auto paramOut = generateRegister(paramIn);
#define LOADVALUE(paramOut, paramIn) auto paramOut = generateValue(paramIn);
#define MADINIT(name0, name1, name2) std::string name0, int name1, Registers name2
#define LOADMAD(paramOut, sector, anchor, offset) auto paramOut = generateMad(sector, anchor, offset);
#define REGREG(name) HEADER(name, Registers op1, Registers op2)
#define REGVAL(name) HEADER(name, Registers op1, int value)
#define REGMAD(name) HEADER(name, Registers op1, MADINIT(sect, off, anchor))
#define MADVAL(name) HEADER(name, MADINIT(sectOP1, offsetOP1, anchorOP1), int value)
#define MADREG(name) HEADER(name, MADINIT(sectOP1, offsetOP1, anchorOP1), Registers source)
#define MADMAD(name) HEADER(name, MADINIT(sectOP1, offsetOP1, anchorOP1), MADINIT(sectOP2, offsetOP2, anchorOP2))

#define CREATEFIRSTOPREG2op(name, index)REGREG(name){std::vector<int> toRet;toRet.push_back(index); LOADREG(o1, op1) LOADREG(o2, op2) ADDVECTORTOVECTOR(toRet, o1) ADDVECTORTOVECTOR(toRet, o2) return toRet;} REGVAL(name){TORET toRet.push_back(index);LOADREG(r0, op1)LOADVALUE(o2, value)ADDVECTORTOVECTOR(toRet, r0)ADDVECTORTOVECTOR(toRet, o2)return toRet;}REGMAD(name){TORET toRet.push_back(index); LOADREG(r0, op1) LOADMAD(m0, sect, anchor, off) ADDVECTORTOVECTOR(toRet, r0) ADDVECTORTOVECTOR(toRet, m0) return toRet;}

#define CREATEFIRSTOPMAD2op(name, index) MADVAL(name){TORET toRet.push_back(index);LOADMAD(m0, sectOP1, anchorOP1, offsetOP1)LOADVALUE(v0, value)ADDVECTORTOVECTOR(toRet, m0)ADDVECTORTOVECTOR(toRet, v0)return toRet;}MADREG(name){TORET toRet.push_back(index);LOADMAD(m0, sectOP1, anchorOP1, offsetOP1)LOADREG(r0, source);ADDVECTORTOVECTOR(toRet, m0)ADDVECTORTOVECTOR(toRet, r0)return toRet;}MADMAD(name){TORET toRet.push_back(index);LOADMAD(m0, sectOP1, anchorOP1, offsetOP1)LOADMAD(m1, sectOP2, anchorOP2, offsetOP2)ADDVECTORTOVECTOR(toRet, m0)ADDVECTORTOVECTOR(toRet, m1)return toRet;}

#define CREATEZEROPARAM(name, index)std::vector<int> Geneerator::name(){return {index,0,0,0,0};}

#define CREATEONEPARAM(name, index) std::vector<int> Geneerator::name(int value){return {index, 0, value,0,0};}std::vector<int> Geneerator::name(Geneerator::Registers value){TORET toRet.push_back(index);LOADREG(r0, value)ADDVECTORTOVECTOR(toRet, r0)toRet.push_back(0);toRet.push_back(0);return toRet;}std::vector<int> Geneerator::name(std::string sector, int offset, Registers anchor){TORET toRet.push_back(index);LOADMAD(m0, sector, anchor, offset)ADDVECTORTOVECTOR(toRet, m0)toRet.push_back(0);toRet.push_back(0);return toRet;}

std::vector<int> Geneerator::generateMad(std::string sectorName, Geneerator::Registers anchor, int offset) {
    std::vector<int> toRet;

    toRet.push_back(2);

    for(auto c : sectorName){
        toRet.push_back(c);
    }

    toRet.push_back(0);

    toRet.push_back((int)anchor);
    toRet.push_back(offset);

    return toRet;
}

std::vector<int> Geneerator::generateRegister(Geneerator::Registers reg) {
    return {1, (int)reg};
}

std::vector<int> Geneerator::generateValue(int value) {
    return {0, value};
}

CREATEFIRSTOPREG2op(add, 1)

CREATEFIRSTOPMAD2op(add, 1)

CREATEFIRSTOPREG2op(mov, 0)

CREATEFIRSTOPMAD2op(mov, 0)

CREATEFIRSTOPREG2op(sub, 2)

CREATEFIRSTOPMAD2op(sub, 2)

CREATEZEROPARAM(ret, 5)

CREATEONEPARAM(call, 6)

CREATEONEPARAM(pop, 3)

CREATEONEPARAM(push, 4)

CREATEZEROPARAM(pushs, 7)

CREATEZEROPARAM(pops, 8)

CREATEFIRSTOPREG2op(cmp, 9)

CREATEFIRSTOPMAD2op(cmp, 9)

CREATEFIRSTOPREG2op(movg, 10)

CREATEFIRSTOPMAD2op(movg, 10)

CREATEFIRSTOPREG2op(movl, 11)

CREATEFIRSTOPMAD2op(movl, 11)

CREATEFIRSTOPREG2op(move, 12)

CREATEFIRSTOPMAD2op(move, 12)

CREATEONEPARAM(jmp, 13)

CREATEONEPARAM(jme, 14)

CREATEONEPARAM(malloc, 15)

CREATEONEPARAM(free, 16)

CREATEONEPARAM(interrupt, 17)

CREATEFIRSTOPMAD2op(mul, 18)

CREATEFIRSTOPREG2op(mul, 18)

CREATEFIRSTOPMAD2op(div, 19)

CREATEFIRSTOPREG2op(div, 19)

CREATEFIRSTOPMAD2op(mod, 20)

CREATEFIRSTOPREG2op(mod, 20)

CREATEFIRSTOPREG2op(AND, 21)

CREATEFIRSTOPMAD2op(AND, 21)

CREATEFIRSTOPREG2op(OR, 22)

CREATEFIRSTOPMAD2op(OR, 22)

CREATEFIRSTOPREG2op(addf, 23)

CREATEFIRSTOPMAD2op(addf, 23)

CREATEFIRSTOPREG2op(subf, 24)

CREATEFIRSTOPMAD2op(subf, 24)

CREATEFIRSTOPREG2op(mulf, 25)

CREATEFIRSTOPMAD2op(mulf, 25)

CREATEFIRSTOPREG2op(divf, 26)

CREATEFIRSTOPMAD2op(divf, 26)

CREATEFIRSTOPREG2op(cmpf, 27)

CREATEFIRSTOPMAD2op(cmpf, 27)

