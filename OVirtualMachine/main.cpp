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

void LoadPackage(MEM_POINTER mem){
    long* destPackage = &((*mem._mem)[(*mem._mem)[mem.ebp]]);
    long sourceData = (*mem._mem)[mem.ebp - 1];
    unsigned char elementIndex = (*mem._mem)[mem.ebp - 2];
    unsigned char length = (*mem._mem)[mem.ebp - 3];

    switch (length) {
        case 1: {
            long val = (char)sourceData & 0b1;
            long clearMask = ~(1 << elementIndex);
            val <<= elementIndex;
            *destPackage &= clearMask;
            *destPackage |= val;
            break;
        }
        case 8: {
            long val = (char)sourceData;
            long shift = elementIndex * 8;

            long clearMask = 0b11111111;
            clearMask <<= shift;
            clearMask = ~clearMask;

            val <<= shift;
            *destPackage &= clearMask;
            *destPackage |= val;
            break;
        }
        case 32: {
            long value = (int)sourceData;
            long shift = elementIndex * 32;

            long clearMask = 0xFFFFFFFF;
            clearMask <<= shift;
            clearMask = ~clearMask;
            value <<= shift;
            *destPackage &= clearMask;
            *destPackage |= value;
            break;
        }
        default:
            throw std::exception();
    }
}

void ExtractPackage(MEM_POINTER mem){
    long sourceData = (*mem._mem)[mem.ebp];
    unsigned char elementIndex = (*mem._mem)[mem.ebp - 1];
    unsigned char length = (*mem._mem)[mem.ebp - 2];

    switch (length) {
        case 1: {
            long value = sourceData & (0b1 << elementIndex);
            value >>= elementIndex;
            *mem.eax = value;
            break;
        }
        case 8: {
            long shift = elementIndex * 8;
            long mask = 0xFF;
            mask <<= shift;

            long value = sourceData & mask;
            value >>= elementIndex * 8;
            *mem.eax = value;
            break;
        }
        case 32: {
            long shift = elementIndex * 32;
            long mask = 0xFFFFFFFF;
            mask <<= shift;

            long value = sourceData & mask;
            value >>= elementIndex * 32;
            *mem.eax = value;
            break;
        }
        default:
            throw std::exception();
    }
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
    lu.AddNewInterrupt("packSend", LoadPackage);
    lu.AddNewInterrupt("packExtract", ExtractPackage);

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
