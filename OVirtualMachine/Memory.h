//
// Created by Роман  Тимофеев on 28.02.2023.
//

#ifndef OVIRTUALMACHINE_MEMORY_H
#define OVIRTUALMACHINE_MEMORY_H

#include <vector>
#include <OVM_SDK.h>
#include <string>

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
        std::vector<int> _mem;
        SectorDescription registerSectionDescriptor;
        std::vector<SectorDescription> _sectors;
        int stackStart = -1;

    public:
        Memory(int stackSize);
        int LoadProgram(std::string sectorName, std::vector<int> content);
        int* GetRegisterAccess(Registers reg);
        int* GetAccessByMemoryDescriptor(MemoryAddressDescriptor mad);
        int GetIdByMAD(MemoryAddressDescriptor mad);
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
        void free(int value);
        void free(Registers reg);
        void free(MemoryAddressDescriptor mad);
        void pushs();
        void pops();
        MEM_POINTER getMemPointer();
        int* getMem();
    };

} // O

#endif //OVIRTUALMACHINE_MEMORY_H
