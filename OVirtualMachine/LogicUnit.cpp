//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "LogicUnit.h"

#include <iostream>
#include <cdc.h>
#define GETREG(varname) *_mem->GetRegisterAccess(varname)
#define GETMAD(varname) *_mem->GetAccessByMemoryDescriptor(varname)

namespace O {

    LogicUnit::LogicUnit(Memory* memoryUnit) {
         this->_mem = memoryUnit;
    }

    void LogicUnit::ga(O::Memory::MemoryAddressDescriptor desc){
    	GETREG(Memory::Registers::mc3) = _mem->getSectorDescription(desc.sectorName).start;
    }

    void LogicUnit::ga(O::Memory::Registers desc){
	throw(std::exception());
    }

    void LogicUnit::ga(long desc){
    	throw(std::exception());
    }

    void LogicUnit::mov(O::Memory::Registers destReg, long value) {
        long* destination = _mem->GetRegisterAccess(destReg);
        *destination = value;
    }

    void LogicUnit::mov(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        long value = *(_mem->GetRegisterAccess(sourceReg));
        mov(destReg, value);
    }

    void LogicUnit::mov(O::Memory::MemoryAddressDescriptor dest, long value) {
        long* d = _mem->GetAccessByMemoryDescriptor(dest);
        *d = value;
    }

    void LogicUnit::mov(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        long* d = _mem->GetAccessByMemoryDescriptor(dest);
        long* s = _mem->GetAccessByMemoryDescriptor(source);
        *d = *s;
    }

    void LogicUnit::mov(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        long* d = _mem->GetRegisterAccess(dest);
        *d = *_mem->GetAccessByMemoryDescriptor(source);
    }

    void LogicUnit::mov(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        long* d = _mem->GetAccessByMemoryDescriptor(dest);
        *d = *_mem->GetRegisterAccess(source);
    }

    void LogicUnit::add(O::Memory::Registers dest, long value) {
        GETREG(dest) += value;
    }

    void LogicUnit::add(O::Memory::Registers dest, O::Memory::Registers source) {
        GETREG(dest) += GETREG(source);
    }

    void LogicUnit::add(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) += GETMAD(source);
    }

    void LogicUnit::sub(O::Memory::Registers dest, long value) {
        GETREG(dest) -= value;
    }

    void LogicUnit::sub(O::Memory::Registers dest, O::Memory::Registers source) {
        GETREG(dest) -= GETREG(source);
    }

    void LogicUnit::sub(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) -= GETMAD(source);
    }

    void LogicUnit::add(O::Memory::MemoryAddressDescriptor dest, long value) {
        GETMAD(dest) += value;
    }

    void LogicUnit::add(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        GETMAD(dest) += GETREG(source);
    }

    void LogicUnit::add(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        GETMAD(dest) += GETMAD(source);
    }

    void LogicUnit::sub(O::Memory::MemoryAddressDescriptor dest, long value) {
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

    void LogicUnit::call(long value) {
        throw std::exception();
    }

    void LogicUnit::call(O::Memory::Registers destReg) {
        throw std::exception();
    }

    void LogicUnit::call(O::Memory::MemoryAddressDescriptor mad) {
        _mem->push(O::Memory::Registers::eip);
        GETREG(O::Memory::Registers::eip) = _mem->GetIdByMAD(mad);
    }

    void LogicUnit::cmp(O::Memory::Registers destReg, long value) {
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

    void LogicUnit::cmp(O::Memory::MemoryAddressDescriptor dest, long value) {
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

    void LogicUnit::move(O::Memory::Registers destReg, long value) {
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

    void LogicUnit::move(O::Memory::MemoryAddressDescriptor dest, long value) {
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

    void LogicUnit::movg(O::Memory::Registers destReg, long value) {
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

    void LogicUnit::movg(O::Memory::MemoryAddressDescriptor dest, long value) {
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

    void LogicUnit::movl(O::Memory::Registers destReg, long value) {
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

    void LogicUnit::movl(O::Memory::MemoryAddressDescriptor dest, long value) {
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

    void LogicUnit::jmp(long value) {
        throw std::exception();
    }

    void LogicUnit::jmp(O::Memory::Registers destReg) {
        throw std::exception();
    }

    void LogicUnit::jmp(O::Memory::MemoryAddressDescriptor mad) {
        GETREG(O::Memory::Registers::eip) = _mem->GetIdByMAD(mad);
    }

    void LogicUnit::jme(long value) {
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

    void LogicUnit::interrupt(long value) {
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
        OVM_MODULE_MAIN main;

        cdc_dynamic_lib_handle hDl = cdc_open_dynamic_lib((char*)path.c_str());
	std::string main_name = "_Omain";	
	main = (OVM_MODULE_MAIN)cdc_get_dynamic_lib_member(hDl, (char*)main_name.c_str());

        if(main == nullptr){
	    std::cout << path << "\n";
            throw std::exception();
        }

        std::vector<Interrupt> newInterrupts = main(registeredInterrupts.size());

        for(auto i : newInterrupts){
            std::vector<long> b = {17, 0, i.id, 0, 0,5,0,0,0,0};
            _mem->LoadProgram(i.name, b);
        }

        registeredInterrupts.insert(registeredInterrupts.end(), newInterrupts.begin(), newInterrupts.end());
    }

    void LogicUnit::mul(O::Memory::Registers destReg, long value) {

        GETREG(destReg) = GETREG(destReg) * value;
    }

    void LogicUnit::mul(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        GETREG(destReg) = GETREG(destReg) * GETREG(sourceReg);
    }

    void LogicUnit::mul(O::Memory::MemoryAddressDescriptor dest, long value) {
        GETMAD(dest) *= value;
    }

    void LogicUnit::mul(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        GETMAD(dest) *= GETMAD(source);
    }

    void LogicUnit::mul(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) *= GETMAD(source);
    }

    void LogicUnit::mul(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        GETMAD(dest) *= GETREG(source);
    }

    void LogicUnit::div(O::Memory::Registers destReg, long value) {

        GETREG(destReg) = GETREG(destReg) / value;
    }

    void LogicUnit::div(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        GETREG(destReg) = GETREG(destReg) / GETREG(sourceReg);
    }

    void LogicUnit::div(O::Memory::MemoryAddressDescriptor dest, long value) {
        GETMAD(dest) /= value;
    }

    void LogicUnit::div(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        GETMAD(dest) /= GETMAD(source);
    }

    void LogicUnit::div(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) /= GETMAD(source);
    }

    void LogicUnit::div(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        GETMAD(dest) /= GETREG(source);
    }

    void LogicUnit::mod(O::Memory::Registers destReg, long value) {

        GETREG(destReg) = GETREG(destReg) % value;
    }

    void LogicUnit::mod(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        GETREG(destReg) = GETREG(destReg) % GETREG(sourceReg);
    }

    void LogicUnit::mod(O::Memory::MemoryAddressDescriptor dest, long value) {
        GETMAD(dest) %= value;
    }

    void LogicUnit::mod(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        GETMAD(dest) %= GETMAD(source);
    }

    void LogicUnit::mod(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) %= GETMAD(source);
    }

    void LogicUnit::mod(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        GETMAD(dest) %= GETREG(source);
    }

    void LogicUnit::AND(O::Memory::Registers destReg,long value) {

        GETREG(destReg) = GETREG(destReg) & value;
    }

    void LogicUnit::AND(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        GETREG(destReg) = GETREG(destReg) & GETREG(sourceReg);
    }

    void LogicUnit::AND(O::Memory::MemoryAddressDescriptor dest, long value) {
        GETMAD(dest) &= value;
    }

    void LogicUnit::AND(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        GETMAD(dest) &= GETMAD(source);
    }

    void LogicUnit::AND(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) &= GETMAD(source);
    }

    void LogicUnit::AND(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        GETMAD(dest) &= GETREG(source);
    }

    void LogicUnit::OR(O::Memory::Registers destReg, long value) {

        GETREG(destReg) = GETREG(destReg) | value;
    }

    void LogicUnit::OR(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        GETREG(destReg) = GETREG(destReg) | GETREG(sourceReg);
    }

    void LogicUnit::OR(O::Memory::MemoryAddressDescriptor dest, long value) {
        GETMAD(dest) |= value;
    }

    void LogicUnit::OR(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        GETMAD(dest) |= GETMAD(source);
    }

    void LogicUnit::OR(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) |= GETMAD(source);
    }

    void LogicUnit::OR(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        GETMAD(dest) |= GETREG(source);
    }

    void LogicUnit::addf(O::Memory::Registers destReg, long value) {

        float op1 = *((float*)_mem->GetRegisterAccess(destReg));
        float op2 = *((float*)&value);

        op1 += op2;

        GETREG(destReg) = *((int*)&op1);
    }

    void LogicUnit::addf(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        float op1 = *((float*)_mem->GetRegisterAccess(destReg));
        float op2 = *((float*)_mem->GetRegisterAccess(sourceReg));

        op1 += op2;

        GETREG(destReg) = *((int*)&op1);
    }

    void LogicUnit::addf(O::Memory::MemoryAddressDescriptor dest, long value) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)&value);

        op1 += op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::addf(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)_mem->GetAccessByMemoryDescriptor(source));

        op1 += op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::addf(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        float op1 = *((float*)_mem->GetRegisterAccess(dest));
        float op2 = *((float*)_mem->GetAccessByMemoryDescriptor(source));

        op1 += op2;

        GETREG(dest) = *((int*)&op1);
    }

    void LogicUnit::addf(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)_mem->GetRegisterAccess(source));

        op1 += op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::subf(O::Memory::Registers destReg, long value) {

        float op1 = *((float*)_mem->GetRegisterAccess(destReg));
        float op2 = *((float*)&value);

        op1 -= op2;

        GETREG(destReg) = *((int*)&op1);
    }

    void LogicUnit::subf(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        float op1 = *((float*)_mem->GetRegisterAccess(destReg));
        float op2 = *((float*)_mem->GetRegisterAccess(sourceReg));

        op1 -= op2;

        GETREG(destReg) = *((int*)&op1);
    }

    void LogicUnit::subf(O::Memory::MemoryAddressDescriptor dest, long value) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)&value);

        op1 -= op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::subf(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)_mem->GetAccessByMemoryDescriptor(source));

        op1 -= op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::subf(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        float op1 = *((float*)_mem->GetRegisterAccess(dest));
        float op2 = *((float*)_mem->GetAccessByMemoryDescriptor(source));

        op1 -= op2;

        GETREG(dest) = *((int*)&op1);
    }

    void LogicUnit::subf(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)_mem->GetRegisterAccess(source));

        op1 -= op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::mulf(O::Memory::Registers destReg, long value) {

        float op1 = *((float*)_mem->GetRegisterAccess(destReg));
        float op2 = *((float*)&value);

        op1 *= op2;

        GETREG(destReg) = *((int*)&op1);
    }

    void LogicUnit::mulf(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        float op1 = *((float*)_mem->GetRegisterAccess(destReg));
        float op2 = *((float*)_mem->GetRegisterAccess(sourceReg));

        op1 *= op2;

        GETREG(destReg) = *((int*)&op1);
    }

    void LogicUnit::mulf(O::Memory::MemoryAddressDescriptor dest, long value) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)&value);

        op1 *= op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::mulf(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)_mem->GetAccessByMemoryDescriptor(source));

        op1 *= op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::mulf(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        float op1 = *((float*)_mem->GetRegisterAccess(dest));
        float op2 = *((float*)_mem->GetAccessByMemoryDescriptor(source));

        op1 *= op2;

        GETREG(dest) = *((int*)&op1);
    }

    void LogicUnit::mulf(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)_mem->GetRegisterAccess(source));

        op1 *= op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::divf(O::Memory::Registers destReg, long value) {

        float op1 = *((float*)_mem->GetRegisterAccess(destReg));
        float op2 = *((float*)&value);

        op1 /= op2;

        GETREG(destReg) = *((int*)&op1);
    }

    void LogicUnit::divf(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        float op1 = *((float*)_mem->GetRegisterAccess(destReg));
        float op2 = *((float*)_mem->GetRegisterAccess(sourceReg));

        op1 /= op2;

        GETREG(destReg) = *((int*)&op1);
    }

    void LogicUnit::divf(O::Memory::MemoryAddressDescriptor dest, long value) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)&value);

        op1 /= op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::divf(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)_mem->GetAccessByMemoryDescriptor(source));

        op1 /= op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::divf(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        float op1 = *((float*)_mem->GetRegisterAccess(dest));
        float op2 = *((float*)_mem->GetAccessByMemoryDescriptor(source));

        op1 /= op2;

        GETREG(dest) = *((int*)&op1);
    }

    void LogicUnit::divf(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        float op1 = *((float*)_mem->GetAccessByMemoryDescriptor(dest));
        float op2 = *((float*)_mem->GetRegisterAccess(source));

        op1 /= op2;

        GETMAD(dest) = *((int*)&op1);
    }

    void LogicUnit::cmpf(O::Memory::Registers destReg, long value) {
        float p1 = *((float*)_mem->GetRegisterAccess(destReg));
        float p2 = *((float*)&value);
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
    void LogicUnit::cmpf(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        auto p2 = GETREG(sourceReg);
        cmp(destReg, p2);
    }

    void LogicUnit::cmpf(O::Memory::MemoryAddressDescriptor dest, long value) {
        throw std::exception();
    }

    void LogicUnit::cmpf(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        throw std::exception();
    }

    void LogicUnit::cmpf(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        throw std::exception();
    }

    void LogicUnit::cmpf(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        throw std::exception();
    }

    void LogicUnit::AddNewInterrupt(std::string name, InterruptHandler interrupt) {
        std::vector<long> b = {17, 0, (long)registeredInterrupts.size(), 0, 0,5,0,0,0,0};
        _mem->LoadProgram(name, b);

        Interrupt inter;
        inter.id = registeredInterrupts.size();
        inter.name = name;
        inter.hInt = interrupt;
        registeredInterrupts.push_back(inter);
    }
}
