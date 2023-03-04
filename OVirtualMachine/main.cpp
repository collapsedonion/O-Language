#include "Memory.h"
#include "LogicUnit.h"
#include "Scenary.h"
#include <fstream>
#include <iostream>
#include <string>

int main() {

    std::string loadFile;

    std::cin >> loadFile;

    std::ifstream f(loadFile);

    std::vector<int> fileRep;

    int size;
    f.read((char*)&size, 4);
    int bodyStart;
    f.read((char*)&bodyStart, 4);

    for(int i = 0; i < size; i++){
        int newE;
        f.read((char*)&newE, 4);
        fileRep.push_back(newE);
    }

    f.close();

    O::Memory mem(200);
    int start = mem.LoadProgram("main", fileRep) + bodyStart;
    O::LogicUnit lu(&mem);
    lu.mov(O::Memory::Registers::eip, start);


    while (*(mem.GetRegisterAccess(O::Memory::Registers::eip)) < start + fileRep.size()){
        auto scenary = O::Scenary::generateScript(mem.getMem(), *(mem.GetRegisterAccess(O::Memory::Registers::eip)));
        *(mem.GetRegisterAccess(O::Memory::Registers::eip)) += scenary.first;
        O::Scenary::EvaluateWord(scenary.second, &lu, &mem);
    }

    return 0;
}
