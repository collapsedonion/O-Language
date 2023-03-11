//
// Created by Роман  Тимофеев on 05.03.2023.
//

#ifndef OVM_SDK_OVM_SDK_H
#define OVM_SDK_OVM_SDK_H

#include <string>
#include <vector>
#include <map>

enum class Registers{
    eax = 0,
    ebx = 1,
    ecx = 2,
    edx = 3,
    eip = 4,
    esp = 5,
    edp = 6,
    esi = 7,
    edi = 8,
    mc0 = 9,
    mc1 = 10,
    mc2 = 11,
    mc3 = 12,
    aa0 = 13,
    aa1 = 14,
    ebp = 15,
    flag = 16
};

struct SectorDescription{
    std::string name;
    int start;
    int size;
};

inline std::pair<bool, std::pair<int, int>> decodeAddress(long address){
    int page = address >> 32;
    if(page != 0){
        return {true, {page, (int)address}};
    }else{
        return {false, {0, address}};
    }
}

struct MEM_POINTER{
    std::vector<long>* _mem;
    std::map<int, std::vector<long>>* _heap;
    std::vector<SectorDescription>* sectors;
    long esp;
    long ebp;
    long* eax;
};

inline long* getObjectReferenceByAddress(MEM_POINTER mem, long address){
    auto decodedAddress = decodeAddress(address);

    if(!decodedAddress.first){
        return &((*(mem._mem))[decodedAddress.second.second]);
    }else{
        return &((*(mem._heap))[decodedAddress.second.first][decodedAddress.second.second]);
    }
}

typedef void (*InterruptHandler)(MEM_POINTER);
struct Interrupt{
    int id;
    std::string name;
    InterruptHandler hInt;
};
typedef std::vector<Interrupt> (*OVM_MODULE_MAIN)(int last_id);

inline std::vector<int> CreateInterruptCall(int index){
    return {17, 0, index, 0, 0};
}

#endif //OVM_SDK_OVM_SDK_H
