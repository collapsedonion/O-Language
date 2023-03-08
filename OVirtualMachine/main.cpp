#include "Memory.h"
#include "LogicUnit.h"
#include "Scenary.h"
#include <fstream>
#include <iostream>
#include <string>
#include <OVM_SDK.h>

void GetHighFloat(MEM_POINTER mem){
    int* origin = &(*mem._mem)[mem.ebp];
    *mem.eax = (int)(*(float*)origin);
}

void LoadDL(std::string path, O::LogicUnit* lu){
    std::ifstream f(path);

    std::string orgPath = path.substr(0, path.rfind('/'));

    if(f.good()){
        while (!f.eof()){
            std::string newF;
            f >> newF;
            newF = orgPath + "/" + newF;
            lu->LoadNewInterrupts(newF);
        }
    }
}

int main(int argc, char* args[]) {

    if(argc == 1){
        return -1;
    }

    std::string loadFile = args[1];

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
    O::LogicUnit lu(&mem);

    lu.AddNewInterrupt("getHighFloat", GetHighFloat);

    if(argc == 3){
        LoadDL(args[2], &lu);
    }

    int start = mem.LoadProgram("main", fileRep);
    lu.mov(O::Memory::Registers::eip, start + bodyStart);


    while (*(mem.GetRegisterAccess(O::Memory::Registers::eip)) < start + fileRep.size()){
        auto scenary = O::Scenary::generateScript(mem.getMem(), *(mem.GetRegisterAccess(O::Memory::Registers::eip)));
        *(mem.GetRegisterAccess(O::Memory::Registers::eip)) += scenary.first;
        O::Scenary::EvaluateWord(scenary.second, &lu, &mem);
    }

    return 0;
}
