//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "Memory.h"

namespace O {
    int Memory::registersCount = 17;

    Memory::Memory(int stackSize) {
        SectorDescription sd;
        sd.name = "__REGISTERS__";
        sd.start = _mem.size();
        sd.size = registersCount;
        _sectors.push_back(sd);
        registerSectionDescriptor = sd;
        for(int i = 0; i < registersCount; i++){
            _mem.push_back(0);
        }

        sd.name = "__STACK__";
        sd.size = stackSize;
        sd.start = _mem.size();
        _sectors.push_back(sd);
        for (int i = 0; i < stackSize; i++) {
            _mem.push_back(0);
        }
        stackStart = sd.start;
        int* esp = GetRegisterAccess(Registers::esp);
        *esp = sd.start + sd.size;
    }

    int Memory::LoadProgram(std::string sectorName, std::vector<int> content) {
        int lastId = _mem.size();
        SectorDescription sd;
        sd.name = sectorName;
        sd.start = lastId;
        sd.size = content.size();

        _mem.insert(_mem.end(), content.begin(), content.end());

        _sectors.push_back(sd);

        return lastId;
    }

    int *Memory::GetRegisterAccess(Memory::Registers reg) {

        int index = registerSectionDescriptor.start + (int)reg;

        return &(_mem[index]);
    }

    void Memory::push(Memory::Registers reg) {
        int* registerSource = GetRegisterAccess(reg);
        int* esp = GetRegisterAccess(Registers::esp);
        (*esp)--;
        if((*esp) < stackStart){
            throw std::exception();
        }
        _mem[*esp] = *registerSource;
    }

    void Memory::push(int value) {
        int* esp = GetRegisterAccess(Registers::esp);
        (*esp)--;
        if((*esp) < stackStart){
            throw std::exception();
        }
        _mem[*esp] = value;
    }

    void Memory::pop(Memory::Registers reg) {
        int* esp = GetRegisterAccess(Registers::esp);
        int* dest = GetRegisterAccess(reg);
        *dest = _mem[*esp];
        (*esp)++;
    }

    int *Memory::GetAccessByMemoryDescriptor(MemoryAddressDescriptor mad) {
        SectorDescription sd = registerSectionDescriptor;
        if(mad.sectorName == ""){
            sd = registerSectionDescriptor;
        }else{
            for(auto s : _sectors){
                if(s.name == mad.sectorName){
                    sd = s;
                    break;
                }
            }
        }

        int anchor = 0;
        if((int)mad.anchor != -1){
            anchor = *GetRegisterAccess(mad.anchor);
        }

        int index = sd.start + anchor + mad.offset;

        return &_mem[index];
    }

    void Memory::pop(int value) {
        throw std::exception();
    }

    void Memory::pop(Memory::MemoryAddressDescriptor mad) {
        throw std::exception();
    }

    void Memory::push(Memory::MemoryAddressDescriptor mad) {
        throw std::exception();
    }

    int *Memory::getMem() {
        return _mem.data();
    }

    int Memory::GetIdByMAD(Memory::MemoryAddressDescriptor mad) {
        SectorDescription sd;
        if(mad.sectorName == ""){
            sd = registerSectionDescriptor;
        }else {
            for (auto s: _sectors) {
                if (s.name == mad.sectorName) {
                    sd = s;
                    break;
                }
            }
        }
        int anchor = 0;
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

    void Memory::malloc(int value) {
        auto start = _mem.size();
        for(int i = 0; i < value; i++){
            _mem.push_back(0);
        }
        SectorDescription sd;
        sd.name = std::to_string(start) + "ALLOC";
        sd.size = value;
        sd.start = start;
        _sectors.push_back(sd);
        *GetRegisterAccess(Registers::eax) = start;
    }

    void Memory::free(int value) {
        auto name = std::to_string(value) + "ALLOC";
        int id = GetSectorIndex(name);
        if(id == _sectors.size() - 1){
            _mem.erase(_mem.begin() + _sectors[id].start, _mem.end());
        }
        _sectors.erase(_sectors.begin() + id);
    }

    void Memory::free(Memory::Registers reg) {
        free(*GetRegisterAccess(reg));
    }

    void Memory::free(Memory::MemoryAddressDescriptor mad) {
        free(*GetAccessByMemoryDescriptor(mad));
    }

    int Memory::GetSectorIndex(std::string name) {
        for(int i = 0; i < _sectors.size(); i++){
            if(_sectors[i].name == name){
                return i;
            }
        }

        return -1;
    }

    MEM_POINTER Memory::getMemPointer() {
        MEM_POINTER mp;
        mp._mem = &_mem;
        mp.sectors = &_sectors;
        mp.eax = GetRegisterAccess(Registers::eax);
        mp.esp = *GetRegisterAccess(Registers::esp);
        return mp;
    }


} // O