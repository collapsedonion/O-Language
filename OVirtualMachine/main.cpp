#include "Memory.h"
#include "LogicUnit.h"
#include "Scenary.h"
#include <fstream>
#include <iostream>
#include <string>
#include <OVM_SDK.h>
#include <math.h>
#ifdef __APPLE__
	#include <mach-o/dyld.h>
#endif
#include <time.h>

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

void GetSystemID(MEM_POINTER mem){
#ifdef __APPLE__
	*mem.eax = 0;
#endif
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

void Reallocate(MEM_POINTER mem){
    int pageIndex = ((*mem._mem)[mem.ebp]) >> 32;
    std::vector<long>* page = &(*mem._heap)[pageIndex];
    int newSize = ((*mem._mem))[mem.ebp - 1];

    page->resize(newSize);
}

void GetAlLocatedSize(MEM_POINTER mem){
    int pageIndex =((*mem._mem)[mem.ebp]) >> 32;
    std::vector<long>* page = &(*mem._heap)[pageIndex];
    *mem.eax = page->size();
}

void Sin(MEM_POINTER mem){
    float x = *(float*)(getObjectReferenceByAddress(mem, mem.ebp));
    float result = sin(x);
    *mem.eax = *(long*)&(result);
}

void Cos(MEM_POINTER mem){
    float x = *(float*)(getObjectReferenceByAddress(mem, mem.ebp));
    float result = cos(x);
    *mem.eax = *(long*)&(result);
}

void GetClockTime(MEM_POINTER mem){
    clock_t time = clock();

    *mem.eax = time;
}

void GetRunTime(MEM_POINTER mem){
    clock_t time = clock();
    float timeF = (float)time / CLOCKS_PER_SEC;
    *mem.eax = *(int*)&timeF;
}

void GetRealPointer(MEM_POINTER mem){
    long* address = getObjectReferenceByAddress(mem, *getObjectReferenceByAddress(mem, mem.ebp));

    *mem.eax = (long)address;
}

void SetRealPointer(MEM_POINTER mem){
    long* pointer = (long*)(*getObjectReferenceByAddress(mem, mem.ebp));
    long sizeInBytes = (long)(*getObjectReferenceByAddress(mem, mem.ebp - 1));
    long data = (*getObjectReferenceByAddress(mem, mem.ebp - 2));

    switch (sizeInBytes) {
        case 1: {
            *(char *) pointer = (char) data;
            break;
        }
        case 2: {
            *(short *) pointer = (short) data;
            break;
        }
        case 4: {
            *(int *) pointer = (int) data;
            break;
        }
        case 8: {
            *(long *) pointer = (long) data;
            break;
        }
    }
}

void GetRealPointerContent(MEM_POINTER mem){
    long* pointer = (long*)(*getObjectReferenceByAddress(mem, mem.ebp));
    long sizeInBytes = (long)(*getObjectReferenceByAddress(mem, mem.ebp - 1));

    switch (sizeInBytes) {
        case 1: {
            *mem.eax = *(char*)pointer;
            break;
        }
        case 2: {
            *mem.eax = *(short *)pointer;
            break;
        }
        case 4: {
            *mem.eax = *(int *)pointer;
            break;
        }
        case 8: {
            *mem.eax = *(long * )pointer;
            break;
        }
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

#ifdef __APPLE__
    while(_NSGetExecutablePath(execPathBuf, &execPathBufSize) == -1){
        free(execPathBuf);
        execPathBufSize += 1024;
        execPathBuf = (char*)malloc(execPathBufSize);
    }
#endif

    std::string execPath(execPathBuf);
    free(execPathBuf);

    execPath = execPath.substr(0, execPath.rfind("/"));

    std::string loadFile = args[1];

    std::ifstream f(loadFile);

    O::Memory mem(20000);
    O::LogicUnit lu(&mem);

    lu.AddNewInterrupt("getHighFloat", GetHighFloat);
    lu.AddNewInterrupt("packSend", LoadPackage);
    lu.AddNewInterrupt("packExtract", ExtractPackage);
    lu.AddNewInterrupt("realloc", Reallocate);
    lu.AddNewInterrupt("cos", Cos);
    lu.AddNewInterrupt("sin", Sin);
    lu.AddNewInterrupt("mSize", GetAlLocatedSize);
    lu.AddNewInterrupt("clock", GetClockTime);
    lu.AddNewInterrupt("getTime", GetRunTime);
    lu.AddNewInterrupt("getRPointer", GetRealPointer);
    lu.AddNewInterrupt("setRPointerData", SetRealPointer);
    lu.AddNewInterrupt("getRPointerContent", GetRealPointerContent);
    lu.AddNewInterrupt("getSysId", GetSystemID);

    LoadDL(execPath + "/stdbin/libs.conf", &lu);

    int sectorCount;
    f.read((char*)&sectorCount, sizeof(int));

    for(int i = 0; i < sectorCount; i++){
        int nameSize;
        f.read((char*)&nameSize, sizeof(int));
        char* sectorName = (char*)malloc((nameSize + 1) * sizeof(char));

        for(int j = 0; j < nameSize; j++){
            char32_t newC;
            f.read((char*)&newC, sizeof(char32_t ) / sizeof(char ));
            sectorName[j] = (char)newC;
        }

        int size;
        f.read((char*)&size, sizeof(int));

        std::vector<int> data;
        data.resize(size);

        f.read((char*)data.data(), size * sizeof(char32_t));

        std::vector<long> lData(data.begin(), data.end());

        mem.LoadProgram(std::string(sectorName), lData);

        free(sectorName);
    }

    f.close();

    if(argc == 3){
        LoadDL(args[2], &lu);
    }

    SectorDescription mainSector = mem.getSectorDescription("main");

    lu.mov(O::Memory::Registers::eip, mainSector.start);

    while (*(mem.GetRegisterAccess(O::Memory::Registers::eip)) < mainSector.start + mainSector.size){
        auto scenary = O::Scenary::generateScript(mem.getMem(), *(mem.GetRegisterAccess(O::Memory::Registers::eip)));
        *(mem.GetRegisterAccess(O::Memory::Registers::eip)) += scenary.first;
        O::Scenary::EvaluateWord(scenary.second, &lu, &mem);
    }

    return 0;
}
