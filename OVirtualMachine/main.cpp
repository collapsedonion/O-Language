#include "Memory.h"
#include "LogicUnit.h"
#include "Scenary.h"
#include <fstream>
#include <iostream>
#include <string>
#include <OVM_SDK.h>
#include <math.h>
#include <cdc.h>
#include <time.h>
#include <stdio.h>

void GetHighFloat(MEM_POINTER mem){
    long long* origin = &(*mem._mem)[mem.ebp];
    *mem.eax = (int)(*(float*)origin);
}

void LoadPackage(MEM_POINTER mem){
    long long* destPackage = &((*mem._mem)[(*mem._mem)[mem.ebp]]);
    long long sourceData = (*mem._mem)[mem.ebp - 1];
    unsigned char elementIndex = (*mem._mem)[mem.ebp - 2];
    unsigned char length = (*mem._mem)[mem.ebp - 3];

    switch (length) {
        case 1: {
            long long val = (char)sourceData & 0b1;
            long long clearMask = ~(1 << elementIndex);
            val <<= elementIndex;
            *destPackage &= clearMask;
            *destPackage |= val;
            break;
        }
        case 8: {
            long long val = (char)sourceData;
            long long shift = elementIndex * 8;

            long long clearMask = 0b11111111;
            clearMask <<= shift;
            clearMask = ~clearMask;

            val <<= shift;
            *destPackage &= clearMask;
            *destPackage |= val;
            break;
        }
        case 32: {
            long long value = (int)sourceData;
            long long shift = elementIndex * 32;

            long long clearMask = 0xFFFFFFFF;
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
#elif defined(_WIN32) || defined(__CYGWIN__)
    *mem.eax = 1;
#endif
}

void OpenDynamicLib(MEM_POINTER mem){
	char* lPP = (char*)*getObjectReferenceByAddress(mem, mem.ebp);
	cdc_dynamic_lib_handle dl = cdc_open_dynamic_lib(lPP);
	*mem.eax = (long long)dl;
}

void GetDlMember(MEM_POINTER mem){
	cdc_dynamic_lib_handle dl = (cdc_dynamic_lib_handle)*getObjectReferenceByAddress(mem, mem.ebp);
	char* member_name = (char*)*getObjectReferenceByAddress(mem, mem.ebp - 1);
	*mem.eax = (long long)cdc_get_dynamic_lib_member(dl, member_name);
}

void CallCFunction(MEM_POINTER mem){
	void* fPointer = (void*)*getObjectReferenceByAddress(mem, mem.ebp);
	long long pCount = *getObjectReferenceByAddress(mem, mem.ebp - 1);
	long long* arguments = getObjectReferenceByAddress(mem, *getObjectReferenceByAddress(mem, mem.ebp - 2));
	char* arg_types = (char*)*getObjectReferenceByAddress(mem, mem.ebp - 3);
	*mem.eax = cdc_invoke(fPointer, pCount, arguments, arg_types);
}

void ExtractPackage(MEM_POINTER mem){
    long long sourceData = (*mem._mem)[mem.ebp];
    unsigned char elementIndex = (*mem._mem)[mem.ebp - 1];
    unsigned char length = (*mem._mem)[mem.ebp - 2];

    switch (length) {
        case 1: {
            long long value = sourceData & (0b1 << elementIndex);
            value >>= elementIndex;
            *mem.eax = value;
            break;
        }
        case 8: {
            long long shift = elementIndex * 8;
            long long mask = 0xFF;
            mask <<= shift;

            long long value = sourceData & mask;
            value >>= elementIndex * 8;
            *mem.eax = value;
            break;
        }
        case 32: {
            long long shift = elementIndex * 32;
            long long mask = 0xFFFFFFFF;
            mask <<= shift;

            long long value = sourceData & mask;
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
    std::vector<long long>* page = &(*mem._heap)[pageIndex];
    int newSize = ((*mem._mem))[mem.ebp - 1];

    page->resize(newSize);
}

void GetAlLocatedSize(MEM_POINTER mem){
    int pageIndex =((*mem._mem)[mem.ebp]) >> 32;
    std::vector<long long>* page = &(*mem._heap)[pageIndex];
    *mem.eax = page->size();
}

void Sin(MEM_POINTER mem){
    float x = *(float*)(getObjectReferenceByAddress(mem, mem.ebp));
    float result = sin(x);
    *mem.eax = *(long long*)&(result);
}

void Cos(MEM_POINTER mem){
    float x = *(float*)(getObjectReferenceByAddress(mem, mem.ebp));
    float result = cos(x);
    *mem.eax = *(long long*)&(result);
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
    long long* address = getObjectReferenceByAddress(mem, *getObjectReferenceByAddress(mem, mem.ebp));

    *mem.eax = (long long)address;
}

void SetRealPointer(MEM_POINTER mem){
    long long* pointer = (long long*)(*getObjectReferenceByAddress(mem, mem.ebp));
    long long sizeInBytes = (long long)(*getObjectReferenceByAddress(mem, mem.ebp - 1));
    long long data = (*getObjectReferenceByAddress(mem, mem.ebp - 2));

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
            *(long long *) pointer = (long long) data;
            break;
        }
    }
}

void GetRealPointerContent(MEM_POINTER mem){
    long long* pointer = (long long*)(*getObjectReferenceByAddress(mem, mem.ebp));
    long long sizeInBytes = (long long)(*getObjectReferenceByAddress(mem, mem.ebp - 1));

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
            *mem.eax = *(long long * )pointer;
            break;
        }
    }
}

void GetRelativePointer(MEM_POINTER mem){
    long long real_pointer = *getObjectReferenceByAddress(mem, mem.ebp);
    if(real_pointer - (long long)(&(mem._mem[0])) < mem._mem->size()){
        *mem.eax = real_pointer - (long long)mem._mem->data();
    }else{
        auto b = mem._heap->begin();
        for(int i = 0; i < mem._heap->size(); i++){          
            if(real_pointer - (long long)(&(*b).second[0]) < (*b).second.size()){
                long long to_ret = (*b).first;
                to_ret <<= 32;
                to_ret += real_pointer - (long long)(&(*b).second[0]);
                *mem.eax = to_ret;
            }
            b++;
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
	    if(newF!=""){
            	newF = orgPath + "/" + newF;
            	lu->LoadNewInterrupts(newF);
	    }
	}
    }
}

int main(int argc, char* args[]) {
    if(argc == 1){
        return -1;
    }

    uint32_t execPathBufSize;
    char* execPathBuf = cdc_get_executable_directory();
    std::string execPath(execPathBuf);
    free(execPathBuf);
 
    std::string loadFile = args[1];

    std::ifstream f(loadFile, std::ios::binary);

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
    lu.AddNewInterrupt("openDL", OpenDynamicLib);
    lu.AddNewInterrupt("extractDLMember", GetDlMember);
    lu.AddNewInterrupt("nativeInvoke", CallCFunction);
    lu.AddNewInterrupt("getRelativePointer", GetRelativePointer);

    LoadDL(execPath + "/stdbin/libs.conf", &lu);

    int sectorCount;
    f.read((char*)&sectorCount, sizeof(int) / sizeof(char));

    for(int i = 0; i < sectorCount; i++){
        int nameSize;
        f.read((char*)&nameSize, sizeof(int) / sizeof(char));
        char* sectorName = (char*)malloc((nameSize + 1) * sizeof(char));

        for(int j = 0; j < nameSize; j++){
            char32_t newC;
            f.read((char*)&newC, sizeof(char32_t ) / sizeof(char ));
            sectorName[j] = (char)newC;
        }
        sectorName[nameSize] = '\0';

        int size;
        f.read((char*)&size, sizeof(int));

        std::vector<long long> lData;

        for (int j = 0; j < size; j++) {
            int new_elem;
            f.read((char*)&new_elem, sizeof(int));
            lData.push_back(new_elem);
        }

        mem.LoadProgram(std::string(sectorName), lData);

        free(sectorName);
    }

    f.close();

    if(argc == 3){
        LoadDL(args[2], &lu);
    }
    lu.LoadDebugSymbols(loadFile + ".ovm_dbg");
    lu.RunAtSector("main");

    return 0;
}
