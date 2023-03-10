#include "Memory.h"
#include "LogicUnit.h"
#include "Scenary.h"
#include <fstream>
#include <iostream>
#include <string>
#include <OVM_SDK.h>
#include <mach-o/dyld.h>

void GetHighFloat(MEM_POINTER mem){
    long* origin = &(*mem._mem)[mem.ebp];
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

    uint32_t execPathBufSize;
    char* execPathBuf = (char*)malloc(execPathBufSize);

    while(_NSGetExecutablePath(execPathBuf, &execPathBufSize) == -1){
        free(execPathBuf);
        execPathBufSize += 1024;
        execPathBuf = (char*)malloc(execPathBufSize);
    }

    std::string execPath(execPathBuf);
    free(execPathBuf);

    execPath = execPath.substr(0, execPath.rfind("/"));

    std::string loadFile = args[1];

    std::ifstream f(loadFile);

    std::vector<long> fileRep;

    int size;
    f.read((char*)&size, 4);
    int bodyStart;
    f.read((char*)&bodyStart, 4);

    for(int i = 0; i < size; i++){
        long newE = 0;
        f.read((char*)&newE, 4);
        fileRep.push_back(newE);
    }

    f.close();

    O::Memory mem(200);
    O::LogicUnit lu(&mem);

    lu.AddNewInterrupt("getHighFloat", GetHighFloat);

    LoadDL(execPath + "/stdbin/libs.conf", &lu);

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
