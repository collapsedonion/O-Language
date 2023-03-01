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
        _mem->pop(O::Memory::Registers::esp);
    }

    void LogicUnit::call(int value) {
        throw std::exception();
    }

    void LogicUnit::call(O::Memory::Registers destReg) {
        throw std::exception();
    }

    void LogicUnit::call(O::Memory::MemoryAddressDescriptor mad) {
        _mem->push(O::Memory::Registers::esp);
        GETREG(O::Memory::Registers::esp) = GETMAD(mad);
    }
}