//
// Created by Роман  Тимофеев on 28.02.2023.
//

#ifndef OVIRTUALMACHINE_MEMORY_H
#define OVIRTUALMACHINE_MEMORY_H

#include <vector>
#include <OVM_SDK.h>
#include <string>
#include <map>
#include <stack>

namespace O {

    class Memory {
    public:
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

        static int registersCount;

        struct MemoryAddressDescriptor{
            std::string sectorName;
            Registers anchor = (Registers)-1;
            int offset = 0;
        };

    private:
        std::vector<long> _mem;
        std::map<int, std::vector<long>> _heap;
        std::stack<int> heapFree;
        SectorDescription registerSectionDescriptor;
        std::vector<SectorDescription> _sectors;
        int stackStart = -1;

    public:
        Memory(int stackSize);
        int getFreeHeap();
        int LoadProgram(std::string sectorName, std::vector<long> content);
        long* GetRegisterAccess(Registers reg);
        long* GetAccessByMemoryDescriptor(MemoryAddressDescriptor mad);
        long GetIdByMAD(MemoryAddressDescriptor mad);
        int GetSectorIndex(std::string name);
        void push(Registers reg);
        void push(MemoryAddressDescriptor mad);
        void push(int value);
        void pop(Registers reg);
        void pop(MemoryAddressDescriptor mad);
        void pop(int value);
        void malloc(Registers reg);
        void malloc(MemoryAddressDescriptor mad);
        void malloc(int value);
        void free(long value);
        void free(Registers reg);
        void free(MemoryAddressDescriptor mad);
        void pushs();
        void pops();
        MEM_POINTER getMemPointer();
        long* getMem();
    };

} // O

#endif //OVIRTUALMACHINE_MEMORY_H
