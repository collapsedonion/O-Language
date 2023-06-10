//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "Memory.h"

namespace O {
    int Memory::registersCount = 17;

    Memory::Memory(int stackSize) {
        SectorDescription sd;
        sd.start = _mem.size();
        sd.size = registersCount;
        _sectors.insert({"__REGISTERS__", sd});
        registerSectionDescriptor = sd;
        _mem.resize(registersCount);

        sd.size = stackSize;
        sd.start = _mem.size();
        _sectors.insert({"__STACK__",  sd});
        _mem.resize(stackSize + _mem.size());
        stackStart = sd.start;
        long long* esp = GetRegisterAccess(Registers::esp);
        *esp = sd.start + sd.size;
    }

    long long Memory::LoadProgram(std::string sectorName, std::vector<long long> content) {
        long long lastId = _mem.size();
        SectorDescription sd;
        sd.start = lastId;
        sd.size = content.size();

        _mem.insert(_mem.end(), content.begin(), content.end());

        _sectors.insert({sectorName,sd});

        return lastId;
    }

    long long *Memory::GetRegisterAccess(Memory::Registers reg) {

        long long index = registerSectionDescriptor.start + (int)reg;

        return &(_mem[index]);
    }

    void Memory::push(Memory::Registers reg) {
        long long * registerSource = GetRegisterAccess(reg);
        long long * esp = GetRegisterAccess(Registers::esp);
        (*esp)--;
        if((*esp) < 20){
            int a = 0;
        }
        
        if((*esp) < stackStart){
            throw std::exception();
        }
        _mem[*esp] = *registerSource;
    }

    void Memory::push(long long value) {
        long long * esp = GetRegisterAccess(Registers::esp);
        (*esp)--;
        if((*esp) < stackStart){
            throw std::exception();
        }
        _mem[*esp] = value;
    }

    void Memory::pop(Memory::Registers reg) {
        long long * esp = GetRegisterAccess(Registers::esp);
        long long * dest = GetRegisterAccess(reg);
        *dest = _mem[*esp];
        (*esp)++;
    }

    long long *Memory::GetAccessByMemoryDescriptor(MemoryAddressDescriptor mad) {
        SectorDescription sd = registerSectionDescriptor;
        if(mad.sectorName == ""){
            sd = registerSectionDescriptor;
        }else{
            if(_sectors.find(mad.sectorName) != _sectors.end()){
                sd = _sectors[mad.sectorName];
            }
        }

        long long anchor = 0;
        if((int)mad.anchor != -1){
            anchor = *GetRegisterAccess(mad.anchor);
        }

        long long index = sd.start + anchor + mad.offset;

        int pageIndex = index >> 32;

        if(pageIndex != 0){
            return &_heap[pageIndex][(int)index];
        }

        return &_mem[index];
    }

    void Memory::pop(long long value) {
        throw std::exception();
    }

    void Memory::pop(Memory::MemoryAddressDescriptor mad) {
        throw std::exception();
    }

    void Memory::push(Memory::MemoryAddressDescriptor mad) {
        throw std::exception();
    }

    long long *Memory::getMem() {
        return _mem.data();
    }

    long long Memory::GetIdByMAD(Memory::MemoryAddressDescriptor mad) {
        SectorDescription sd;
        if(mad.sectorName == ""){
            sd = registerSectionDescriptor;
        }else {
            if(_sectors.find(mad.sectorName) != _sectors.end()){
                sd = _sectors[mad.sectorName];
            }
        }
        long long anchor = 0;
        if((int)mad.anchor != -1){
            anchor = *GetRegisterAccess(mad.anchor);
        }
        return sd.start + anchor + mad.offset;
    }

    void Memory::pushs() {
        push(Registers::mc0);
        push(Registers::mc1);
        push(Registers::mc2);
        push(Registers::aa0);
        push(Registers::flag);
        push(Registers::esi);
        push(Registers::edi);
    }

    void Memory::pops() {
        pop(Registers::edi);
        pop(Registers::esi);
        pop(Registers::flag);
        pop(Registers::aa0);
        pop(Registers::mc2);
        pop(Registers::mc1);
        pop(Registers::mc0);
    }

    void Memory::malloc(Memory::Registers reg) {
        malloc(*GetRegisterAccess(reg));
    }

    void Memory::malloc(Memory::MemoryAddressDescriptor mad) {
        malloc(*GetAccessByMemoryDescriptor(mad));
    }

    void Memory::malloc(long long value) {
        auto start = getFreeHeap();
        _heap.insert({start, std::vector<long long>()});

        _heap[start].resize(value);

        SectorDescription sd;
        sd.size = value;
        sd.start = start;
        _sectors.insert({std::to_string(start) + "ALLOC", sd});

        long long result = start;
        result <<= 32;

        *GetRegisterAccess(Registers::eax) = result;
    }

    void Memory::free(long long value) {
        auto name = std::to_string(value >> 32) + "ALLOC";

        int toErase = int(value >> 32);

        _heap.erase(int(value >> 32));
        _sectors.erase(name);
        heapFree.push(toErase);
    }

    void Memory::free(Memory::Registers reg) {
        free(*GetRegisterAccess(reg));
    }

    void Memory::free(Memory::MemoryAddressDescriptor mad) {
        free(*GetAccessByMemoryDescriptor(mad));
    }

    MEM_POINTER Memory::getMemPointer() {
        MEM_POINTER mp;
        mp._mem = &_mem;
        mp.sectors = &_sectors;
        mp._heap = &_heap;
        mp.eax = GetRegisterAccess(Registers::eax);
        mp.esp = *GetRegisterAccess(Registers::esp);
        mp.ebp = *GetRegisterAccess(Registers::ebp);
        return mp;
    }

    long long Memory::getFreeHeap() {

        if(!heapFree.empty()){
            int value = heapFree.top();
            heapFree.pop();
            return value;
        }
        else{
            return _heap.size() + 1;
        }

    }

    SectorDescription Memory::getSectorDescription(std::string name) {
        return _sectors[name];
    }

    std::string toString(Memory::Registers reg){
        switch (reg) {
            case Memory::Registers::NULL_REG:
                return "0";
            case Memory::Registers::eax:
                return "eax";
            case Memory::Registers::ebx:
                return "ebx";
                break;
            case Memory::Registers::ecx:
                return "ecx";
                break;
            case Memory::Registers::edx:
                return "edx";
                break;
            case Memory::Registers::eip:
                return "eip";
                break;
            case Memory::Registers::esp:
                return "esp";
                break;
            case Memory::Registers::edp:
                return "edp";
                break;
            case Memory::Registers::esi:
                return "esi";
                break;
            case Memory::Registers::edi:
                return "edi";
                break;
            case Memory::Registers::mc0:
                return "mc0";
                break;
            case Memory::Registers::mc1:
                return "mc1";
                break;
            case Memory::Registers::mc2:
                return "mc2";
                break;
            case Memory::Registers::mc3:
                return "mc3";
                break;
            case Memory::Registers::aa0:
                return "aa0";
                break;
            case Memory::Registers::aa1:
                return "aa1";
                break;
            case Memory::Registers::ebp:
                return "ebp";
                break;
            case Memory::Registers::flag:
                return "flag";
                break;
        }
    }

    std::string toString(Memory::MemoryAddressDescriptor mad){
        std::string toRet = "[";
        toRet += (mad.sectorName.empty() ? "0" : mad.sectorName) + "+";
        toRet += toString(mad.anchor) + "+";
        toRet += "(" + std::to_string(mad.offset) + ")";
        toRet += "]";
        return toRet;
    }
} // O
