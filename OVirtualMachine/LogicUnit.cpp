//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "LogicUnit.h"

#define GETREG(varname) *_mem->GetRegisterAccess(varname)
#define GETMAD(varname) *_mem->GetAccessByMemoryDescriptor(varname)

namespace O {

    LogicUnit::LogicUnit(Memory* memoryUnit) {
         this->_mem = memoryUnit;
    }

    void LogicUnit::mov(O::Memory::Registers destReg, int value) {
        int* destination = _mem->GetRegisterAccess(destReg);
        *destination = value;
    }

    void LogicUnit::mov(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        int value = *(_mem->GetRegisterAccess(sourceReg));
        mov(destReg, value);
    }

    void LogicUnit::mov(O::Memory::MemoryAddressDescriptor dest, int value) {
        int* d = _mem->GetAccessByMemoryDescriptor(dest);
        *d = value;
    }

    void LogicUnit::mov(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        int* d = _mem->GetAccessByMemoryDescriptor(dest);
        int* s = _mem->GetAccessByMemoryDescriptor(source);
        *d = *s;
    }

    void LogicUnit::mov(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        int* d = _mem->GetRegisterAccess(dest);
        *d = *_mem->GetAccessByMemoryDescriptor(source);
    }

    void LogicUnit::mov(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        int* d = _mem->GetAccessByMemoryDescriptor(dest);
        *d = *_mem->GetRegisterAccess(source);
    }

    void LogicUnit::add(O::Memory::Registers dest, int value) {
        GETREG(dest) += value;
    }

    void LogicUnit::add(O::Memory::Registers dest, O::Memory::Registers source) {
        GETREG(dest) += GETREG(source);
    }

    void LogicUnit::add(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) += GETMAD(source);
    }

    void LogicUnit::sub(O::Memory::Registers dest, int value) {
        GETREG(dest) -= value;
    }

    void LogicUnit::sub(O::Memory::Registers dest, O::Memory::Registers source) {
        GETREG(dest) -= GETREG(source);
    }

    void LogicUnit::sub(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) -= GETMAD(source);
    }

    void LogicUnit::add(O::Memory::MemoryAddressDescriptor dest, int value) {
        GETMAD(dest) += value;
    }

    void LogicUnit::add(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        GETMAD(dest) += GETREG(source);
    }

    void LogicUnit::add(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        GETMAD(dest) += GETMAD(source);
    }

    void LogicUnit::sub(O::Memory::MemoryAddressDescriptor dest, int value) {
        GETMAD(dest) -= value;
    }

    void LogicUnit::sub(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        GETMAD(dest) -= GETREG(source);
    }

    void LogicUnit::sub(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        GETMAD(dest) -= GETMAD(source);
    }

    void LogicUnit::ret(){
        _mem->pop(O::Memory::Registers::eip);
    }

    void LogicUnit::call(int value) {
        throw std::exception();
    }

    void LogicUnit::call(O::Memory::Registers destReg) {
        throw std::exception();
    }

    void LogicUnit::call(O::Memory::MemoryAddressDescriptor mad) {
        _mem->push(O::Memory::Registers::eip);
        GETREG(O::Memory::Registers::eip) = _mem->GetIdByMAD(mad);
    }

    void LogicUnit::cmp(O::Memory::Registers destReg, int value) {
        auto p1 = GETREG(destReg);
        auto p2 = value;
        auto res = 0;

        if(p1 == p2){
            res = 0b1;
        }else if(p1 > p2){
            res = 0b10;
        }else if(p1 < p2){
            res = 0b100;
        }

        auto newRegV = GETREG(Memory::Registers::flag);

        auto mask = ~0b111;

        newRegV = newRegV & mask;
        newRegV += res;

        GETREG(Memory::Registers::flag) = newRegV;
    }

    // 1 = e ; 10 = g ; 100 = l
    void LogicUnit::cmp(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        auto p2 = GETREG(sourceReg);
        cmp(destReg, p2);
    }

    void LogicUnit::cmp(O::Memory::MemoryAddressDescriptor dest, int value) {
        throw std::exception();
    }

    void LogicUnit::cmp(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        throw std::exception();
    }

    void LogicUnit::cmp(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        throw std::exception();
    }

    void LogicUnit::cmp(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        throw std::exception();
    }

    void LogicUnit::move(O::Memory::Registers destReg, int value) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b1) == 0b1){
            mov(destReg, value);
        }
    }

    void LogicUnit::move(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b1) == 0b1){
            mov(destReg, sourceReg);
        }
    }

    void LogicUnit::move(O::Memory::MemoryAddressDescriptor dest, int value) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b1) == 0b1){
            mov(dest, value);
        }
    }

    void LogicUnit::move(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b1) == 0b1){
            mov(dest, source);
        }
    }

    void LogicUnit::move(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b1) == 0b1){
            mov(dest, source);
        }
    }

    void LogicUnit::move(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b1) == 0b1){
            mov(dest, source);
        }
    }

    void LogicUnit::movg(O::Memory::Registers destReg, int value) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b10) == 0b10){
            mov(destReg, value);
        }
    }

    void LogicUnit::movg(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b10) == 0b10){
            mov(destReg, sourceReg);
        }
    }

    void LogicUnit::movg(O::Memory::MemoryAddressDescriptor dest, int value) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b10) == 0b10){
            mov(dest, value);
        }
    }

    void LogicUnit::movg(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b10) == 0b10){
            mov(dest, source);
        }
    }

    void LogicUnit::movg(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b10) == 0b10){
            mov(dest, source);
        }
    }

    void LogicUnit::movg(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b10) == 0b10){
            mov(dest, source);
        }
    }

    void LogicUnit::movl(O::Memory::Registers destReg, int value) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b100) == 0b100){
            mov(destReg, value);
        }
    }

    void LogicUnit::movl(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b100) == 0b100){
            mov(destReg, sourceReg);
        }
    }

    void LogicUnit::movl(O::Memory::MemoryAddressDescriptor dest, int value) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b100) == 0b100){
            mov(dest, value);
        }
    }

    void LogicUnit::movl(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b100) == 0b100){
            mov(dest, source);
        }
    }

    void LogicUnit::movl(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b100) == 0b100){
            mov(dest, source);
        }
    }

    void LogicUnit::movl(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b100) == 0b100){
            mov(dest, source);
        }
    }

    void LogicUnit::jmp(int value) {
        throw std::exception();
    }

    void LogicUnit::jmp(O::Memory::Registers destReg) {
        throw std::exception();
    }

    void LogicUnit::jmp(O::Memory::MemoryAddressDescriptor mad) {
        GETREG(O::Memory::Registers::eip) = _mem->GetIdByMAD(mad);
    }

    void LogicUnit::jme(int value) {
        throw std::exception();
    }

    void LogicUnit::jme(O::Memory::Registers destReg) {
        throw std::exception();
    }

    void LogicUnit::jme(O::Memory::MemoryAddressDescriptor mad) {
        auto flags = GETREG(Memory::Registers::flag);
        if((flags & 0b1) == 0b1){
            jmp(mad);
        }
    }

    void LogicUnit::interrupt(int value) {
        Interrupt inter = registeredInterrupts[value];
        inter.hInt(_mem->getMemPointer());
    }

    void LogicUnit::interrupt(O::Memory::Registers destReg) {
        throw std::exception();
    }

    void LogicUnit::interrupt(O::Memory::MemoryAddressDescriptor mad) {
        throw std::exception();
    }

    void LogicUnit::LoadNewInterrupts(std::string path) {
        void* hDl = dlopen(path.c_str(), RTLD_NOW);

        if(hDl == nullptr){
            throw std::exception();
        }

        OVM_MODULE_MAIN main = (OVM_MODULE_MAIN)dlsym(hDl, "_Omain");
        if(main == nullptr){
            throw std::exception();
        }

        std::vector<Interrupt> newInterrupts = main(registeredInterrupts.size());

        for(auto i : newInterrupts){
            std::vector<int> b = {17, 0, i.id, 0, 0,5,0,0,0,0};
            _mem->LoadProgram(i.name, b);
        }

        registeredInterrupts.insert(registeredInterrupts.end(), newInterrupts.begin(), newInterrupts.end());
    }

}