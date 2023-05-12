//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "LogicUnit.h"
#include "Scenary.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cdc.h>
#define GETREG(varname) *_mem->GetRegisterAccess(varname)
#define GETMAD(varname) *_mem->GetAccessByMemoryDescriptor(varname)

namespace O {

    LogicUnit::LogicUnit(Memory* memoryUnit) {
         this->_mem = memoryUnit;
    }

    void LogicUnit::RunAtSector(std::string sector_name){
        SectorDescription mainSector = _mem->getSectorDescription(sector_name);

        this->mov(O::Memory::Registers::eip, mainSector.start);

        while (true){
            for(int i = 0; i < stop_lines.size(); i++){
                if(stop_lines[i].stop_address+1 == *_mem->GetRegisterAccess(O::Memory::Registers::eip)){
                    DebugMode(stop_lines[i]);
                }
            }
            
            auto scenary = O::Scenary::generateScript(_mem->getMem(), *(_mem->GetRegisterAccess(O::Memory::Registers::eip)));
            *(_mem->GetRegisterAccess(O::Memory::Registers::eip)) += scenary.first;
            O::Scenary::EvaluateWord(scenary.second, this, _mem);
            if(*(_mem->GetRegisterAccess(O::Memory::Registers::eip)) == mainSector.start + mainSector.size){
                break;
            }
        }
    }

    void LogicUnit::DebugMode(StopLine sl){
        std::cout << "\nProccess stoped at eip = 0x" << std::hex << *_mem->GetRegisterAccess(O::Memory::Registers::eip) << "\n";
        std::string command;
        while(true){
            command = "";
            std::cout << ">>";
            std::cin >> command;
            if(command == "continue" || command == "c"){
                break;
            }else if(command == "variables" || command == "vs"){
                char flag;
                if(command == "variables"){       
                    std::cin >> flag;      
                    if(flag != 'l' && flag != 'g' && flag != 'a'){
                        std::cout << "Invalid selector flag\n";
                        continue;
                    }
                }else{
                    flag = 'a';
                }

                if(components.count(sl.sector) == 1){
                    for(auto a : components[sl.sector]){
                        O::Memory::MemoryAddressDescriptor mad;
                        if(flag == 'l'){
                            if(a.offset_registor != O::Memory::Registers::ebp){
                                continue;
                            }
                        }else if(flag == 'g'){
                            if(a.offset_registor == O::Memory::Registers::ebp){
                                continue;
                            }
                        }

                        mad.anchor = a.offset_registor;
                        mad.offset = a.offset;
                        mad.sectorName = a.sector;
                        long long* variable = _mem->GetAccessByMemoryDescriptor(mad);
                        std::cout << "(0x"<< std::hex <<_mem->GetIdByMAD(mad) << "): " 
                            << a.name << " = 0x" << *variable << ";\n";
                    }
                    std::cout << "\n";
                }
            }
            else if(command == "dump"){
                long long address = 0;
                std::string input;
                std::getline(std::cin, input);
                std::stringstream input_stream;
                input_stream << input;        
                std::string st_adress;
                input_stream >> st_adress;
                long long count = 1;
                input_stream >> std::dec >> count;
                char format = 'h';
                input_stream >> format;

                if(st_adress.size() >= 2 && st_adress[0] == '0' && st_adress[1] == 'x'){
                    std::stringstream ads(st_adress);
                    ads >> std::hex >> address;
                }else{
                    auto component = components[sl.sector];
                    bool found = false;
                    for(auto elem : component){
                        if(elem.name == st_adress){
                            O::Memory::MemoryAddressDescriptor mad;
                            mad.anchor = elem.offset_registor;
                            mad.offset = elem.offset;
                            mad.sectorName = elem.sector;
                            found = true;
                            address = *_mem->GetAccessByMemoryDescriptor(mad);
                            break;
                        }
                    }
                    if(!found){
                        std::cout << "Now such variable with name " << st_adress << "\n";
                        continue;
                    }
                }
                
                for(int i = 0; i < count; i++){
                    std::cout << "(0x" << std::hex << address << ")";
                    O::Memory::MemoryAddressDescriptor mad;
                    mad.anchor = (O::Memory::Registers)-1;
                    mad.offset = address;
                    long long a;
                    
                    bool b = false;
                    try{
                       a = *_mem->GetAccessByMemoryDescriptor(mad);
                    }catch(...){
                        b = true;
                    }
                    if(b){
                        break;
                    }
                    
                    std::cout << "value = ";
                    if(format == 'h'){
                        std::cout << "0x" << std::hex << a << '\n';
                    }else if(format == 'd'){
                        std::cout << std::dec << a << '\n';
                    }else if(format == 'c'){
                        char* as_c = (char*)&a;
                        for(int i = 0; i < 8; i++){
                            std::cout << '\'' << as_c << '\'';
                            as_c++;
                        }
                        std::cout << '\n';
                    }else{
                        std::cout << "Invalid format\n";
                        break;
                    }
                    address++;
                }
                std::cout << "\n";
            }else if(command == "disasm" || command == "da"){
                int c;
                long long adress = *_mem->GetRegisterAccess(O::Memory::Registers::eip);
                long long max_adress = _mem->getSectorDescription(sl.sector).start +  _mem->getSectorDescription(sl.sector).size;
                std::cin >> c;
                
                for(int i = 0; i < c && adress < max_adress; i++){
                    auto scenary = O::Scenary::generateScript(_mem->getMem(), adress);
                    adress+=scenary.first;
                    std::cout << "\t(0x" << std::hex << adress << ")" << scenary.second.toString() << "\n";
                }
            }
            else{
                std::cout << "Unrecognised command\n";
            }
        }
    }

    void LogicUnit::AddBreakPoint(std::u32string path, int line){
        for(auto sym : debug_symbols){
            if(sym.file == path && sym.line >= line){
                int path = _mem->getSectorDescription(sym.sector).start;
                path += sym.esp_min;
                StopLine sl;
                sl.sector = sym.sector;
                sl.stop_address = path;
                stop_lines.push_back(sl);
                break;
            }
        }
    }

    void LogicUnit::LoadDebugSymbols(std::string path){
        std::ifstream input_file(path, std::ios::binary);
        if(input_file.good()){
            int break_point_count = 0;
            input_file.read((char*)&break_point_count, sizeof(int) / sizeof(char));

            for(int i = 0; i < break_point_count; i++){
                int name_size = 0;
                input_file.read((char*)&name_size, sizeof(int) / sizeof(char));
                std::u32string str;
                for(int j = 0; j < name_size; j++){
                    char32_t new_c;
                    input_file.read((char*)&new_c, sizeof(char32_t) / sizeof(char));
                    str += new_c;
                }
                input_file.read((char*)&name_size, sizeof(int) / sizeof(char));
                break_points.push_back({str, name_size});
            }

            int component_count = 0;
            input_file.read((char*)&component_count, sizeof(int) / sizeof(char));

            for(int i = 0; i < component_count; i++){
                int str_size = 0;
                input_file.read((char*)&str_size, sizeof(int) / sizeof(char));
                std::string sector_name;
                for(int j = 0; j < str_size; j++){
                    char32_t new_c;
                    input_file.read((char*)&new_c, sizeof(char32_t) / sizeof(char));
                    sector_name += new_c;
                }
                std::vector<VariableSymbol> variables;
                int variable_count = 0;
                input_file.read((char*)&variable_count, sizeof(int) / sizeof(char));
                for(int j = 0; j < variable_count; j++){
                    VariableSymbol new_vs;
                    
                    int name_size = 0;
                    int sector_size = 0;

                    input_file.read((char*)&name_size, sizeof(char32_t) / sizeof(char));
                    for(int l = 0; l < name_size; l++){
                        char32_t new_c;
                        input_file.read((char*)&new_c, sizeof(char32_t) / sizeof(char));
                        new_vs.name += new_c;
                    }
                    input_file.read((char*)&sector_size, sizeof(char32_t) / sizeof(char));
                    for(int l = 0; l < sector_size; l++){
                        char32_t new_c;
                        input_file.read((char*)&new_c, sizeof(char32_t) / sizeof(char));
                        new_vs.sector += new_c;
                    }

                    input_file.read((char*)&new_vs.offset, sizeof(char32_t) / sizeof(char));
                    input_file.read((char*)&new_vs.offset_registor, sizeof(char32_t) / sizeof(char));
                    variables.push_back(new_vs);
                }
                components.insert({sector_name, variables});
            }

            int debug_symbols_count = 0;
            input_file.read((char*)&debug_symbols_count, sizeof(int) / sizeof(char));

            for(int i = 0; i < debug_symbols_count; i++){
                DebugSymbol new_ds;
                
                int name_size = 0;
                input_file.read((char*)&name_size, sizeof(int) / sizeof(char));
                for(int j = 0; j < name_size; j++){
                    char32_t new_c;
                    input_file.read((char*)&new_c, sizeof(char32_t) / sizeof(char));
                    new_ds.file += new_c;
                }

                input_file.read((char*)&name_size, sizeof(int) / sizeof(char));
                for(int j = 0; j < name_size; j++){
                    char32_t new_c;
                    input_file.read((char*)&new_c, sizeof(char32_t) / sizeof(char));
                    new_ds.sector += new_c;
                }
                input_file.read((char*)&new_ds.line, sizeof(int) / sizeof(char));
                input_file.read((char*)&new_ds.esp_min, sizeof(int) / sizeof(char));
                input_file.read((char*)&new_ds.esp_max, sizeof(int) / sizeof(char));

                debug_symbols.push_back(new_ds);
            }

            input_file.close();
        }
        for(auto br:break_points){
            AddBreakPoint(br.first, br.second);
        }
    }

    void LogicUnit::ga(O::Memory::MemoryAddressDescriptor desc){
    	GETREG(Memory::Registers::mc3) = _mem->getSectorDescription(desc.sectorName).start;
    }

    void LogicUnit::ga(O::Memory::Registers desc){
	throw(std::exception());
    }

    void LogicUnit::ga(long long desc){
    	throw(std::exception());
    }

    void LogicUnit::mov(O::Memory::Registers destReg, long long value) {
        long long* destination = _mem->GetRegisterAccess(destReg);
        *destination = value;
    }

    void LogicUnit::mov(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        long long value = *(_mem->GetRegisterAccess(sourceReg));
        mov(destReg, value);
    }

    void LogicUnit::mov(O::Memory::MemoryAddressDescriptor dest, long long value) {
        long long* d = _mem->GetAccessByMemoryDescriptor(dest);
        *d = value;
    }

    void LogicUnit::mov(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        long long* d = _mem->GetAccessByMemoryDescriptor(dest);
        long long* s = _mem->GetAccessByMemoryDescriptor(source);
        *d = *s;
    }

    void LogicUnit::mov(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        long long* d = _mem->GetRegisterAccess(dest);
        *d = *_mem->GetAccessByMemoryDescriptor(source);
    }

    void LogicUnit::mov(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        long long* d = _mem->GetAccessByMemoryDescriptor(dest);
        *d = *_mem->GetRegisterAccess(source);
    }

    void LogicUnit::add(O::Memory::Registers dest, long long value) {
        GETREG(dest) += value;
    }

    void LogicUnit::add(O::Memory::Registers dest, O::Memory::Registers source) {
        GETREG(dest) += GETREG(source);
    }

    void LogicUnit::add(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) += GETMAD(source);
    }

    void LogicUnit::sub(O::Memory::Registers dest, long long value) {
        GETREG(dest) -= value;
    }

    void LogicUnit::sub(O::Memory::Registers dest, O::Memory::Registers source) {
        GETREG(dest) -= GETREG(source);
    }

    void LogicUnit::sub(O::Memory::Registers dest, O::Memory::MemoryAddressDescriptor source) {
        GETREG(dest) -= GETMAD(source);
    }

    void LogicUnit::add(O::Memory::MemoryAddressDescriptor dest, long long value) {
        GETMAD(dest) += value;
    }

    void LogicUnit::add(O::Memory::MemoryAddressDescriptor dest, O::Memory::Registers source) {
        GETMAD(dest) += GETREG(source);
    }

    void LogicUnit::add(O::Memory::MemoryAddressDescriptor dest, O::Memory::MemoryAddressDescriptor source) {
        GETMAD(dest) += GETMAD(source);
    }

    void LogicUnit::sub(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::call(long long value) {
        throw std::exception();
    }

    void LogicUnit::call(O::Memory::Registers destReg) {
        _mem->push(O::Memory::Registers::eip);
        GETREG(O::Memory::Registers::eip) = GETREG(destReg);
    }

    void LogicUnit::call(O::Memory::MemoryAddressDescriptor mad) {
        _mem->push(O::Memory::Registers::eip);
        GETREG(O::Memory::Registers::eip) = _mem->GetIdByMAD(mad);
    }

    void LogicUnit::cmp(O::Memory::Registers destReg, long long value) {
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

    void LogicUnit::cmp(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::move(O::Memory::Registers destReg, long long value) {
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

    void LogicUnit::move(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::movg(O::Memory::Registers destReg, long long value) {
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

    void LogicUnit::movg(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::movl(O::Memory::Registers destReg, long long value) {
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

    void LogicUnit::movl(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::jmp(long long value) {
        throw std::exception();
    }

    void LogicUnit::jmp(O::Memory::Registers destReg) {
        throw std::exception();
    }

    void LogicUnit::jmp(O::Memory::MemoryAddressDescriptor mad) {
        GETREG(O::Memory::Registers::eip) = _mem->GetIdByMAD(mad);
    }

    void LogicUnit::jme(long long value) {
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

    void LogicUnit::interrupt(long long value) {
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
            std::vector<long long> b = {17, 0, i.id, 0, 0,5,0,0,0,0};
            _mem->LoadProgram(i.name, b);
        }

        registeredInterrupts.insert(registeredInterrupts.end(), newInterrupts.begin(), newInterrupts.end());
    }

    void LogicUnit::mul(O::Memory::Registers destReg, long long value) {

        GETREG(destReg) = GETREG(destReg) * value;
    }

    void LogicUnit::mul(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        GETREG(destReg) = GETREG(destReg) * GETREG(sourceReg);
    }

    void LogicUnit::mul(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::div(O::Memory::Registers destReg, long long value) {

        GETREG(destReg) = GETREG(destReg) / value;
    }

    void LogicUnit::div(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        GETREG(destReg) = GETREG(destReg) / GETREG(sourceReg);
    }

    void LogicUnit::div(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::mod(O::Memory::Registers destReg, long long value) {

        GETREG(destReg) = GETREG(destReg) % value;
    }

    void LogicUnit::mod(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        GETREG(destReg) = GETREG(destReg) % GETREG(sourceReg);
    }

    void LogicUnit::mod(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::AND(O::Memory::Registers destReg,long long value) {

        GETREG(destReg) = GETREG(destReg) & value;
    }

    void LogicUnit::AND(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        GETREG(destReg) = GETREG(destReg) & GETREG(sourceReg);
    }

    void LogicUnit::AND(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::OR(O::Memory::Registers destReg, long long value) {

        GETREG(destReg) = GETREG(destReg) | value;
    }

    void LogicUnit::OR(O::Memory::Registers destReg, O::Memory::Registers sourceReg) {
        GETREG(destReg) = GETREG(destReg) | GETREG(sourceReg);
    }

    void LogicUnit::OR(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::addf(O::Memory::Registers destReg, long long value) {

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

    void LogicUnit::addf(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::subf(O::Memory::Registers destReg, long long value) {

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

    void LogicUnit::subf(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::mulf(O::Memory::Registers destReg, long long value) {

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

    void LogicUnit::mulf(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::divf(O::Memory::Registers destReg, long long value) {

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

    void LogicUnit::divf(O::Memory::MemoryAddressDescriptor dest, long long value) {
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

    void LogicUnit::cmpf(O::Memory::Registers destReg, long long value) {
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

    void LogicUnit::cmpf(O::Memory::MemoryAddressDescriptor dest, long long value) {
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
        std::vector<long long> b = {17, 0, (long long)registeredInterrupts.size(), 0, 0,5,0,0,0,0};
        _mem->LoadProgram(name, b);

        Interrupt inter;
        inter.id = registeredInterrupts.size();
        inter.name = name;
        inter.hInt = interrupt;
        registeredInterrupts.push_back(inter);
    }
}
