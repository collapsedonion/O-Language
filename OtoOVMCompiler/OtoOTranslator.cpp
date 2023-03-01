//
// Created by Роман  Тимофеев on 01.03.2023.
//

#include "OtoOTranslator.h"

#define SET_VALUE_NAME "SET_VALUE"
#define FALSE_NAME "FALSE"

#define ADDVTV(vd, vs) vd.insert(vd.end(), vs.begin(), vs.end());

namespace O {
    void OtoOTranslator::Build(File f) {
        auto mov = Geneerator::mov(Geneerator::Registers::ebp, Geneerator::Registers::esp);

        LoadFunctions(f.functions);

        bodyStart = this->Instructions.size();

        Instructions.insert(Instructions.end(), mov.begin(), mov.end());
        LoadVariables(f.variables);

        for(auto inst : f.instructions){
            ProccessInstruction(inst);
        }
    }

    void OtoOTranslator::LoadVariables(std::vector<Variable> variables, bool add) {
        int offset = addOffset;

        for(auto elem : variables){
            offset+= GetDataSize(elem.type);
            VariableStored vs;
            vs.sector = "__REGISTERS__";
            vs.fromEbpOffset = (offset * -1) + 1;
            vs.name = elem.name;
            if(!add) {
                this->variables.push_back(vs);
            }else{
                this->additionalVariables.push_back(vs);
            }
        }

        if(add){
            addOffset = offset;
        }
        if(!add) {
            auto sub = Geneerator::sub(Geneerator::Registers::esp, offset);
            Instructions.insert(Instructions.end(), sub.begin(), sub.end());
        }
    }

    int OtoOTranslator::GetDataSize(DataTypes dt) {
        switch (dt) {
            case DataTypes::Integer:case DataTypes::FloatingPoint:case DataTypes::Boolean:case DataTypes::Void: case DataTypes::Character:
                return 1;
            default:
                return 1;
        }
    }

    void OtoOTranslator::ProccessInstruction(Instruction inst) {
        if(inst.name == SET_VALUE_NAME){
            SetInstruction(inst);
        }else if(inst.IsFunction){
            CallFunction(inst);
        }
    }

    OtoOTranslator::VariableStored OtoOTranslator::getVar(std::string name) {
        for(auto v:variables){
            if(v.name == name){
                return v;
            }
        }
        for(auto v:additionalVariables){
            if(v.name == name){
                return v;
            }
        }
        return VariableStored();
    }

    void OtoOTranslator::MovVariableToRegister(std::string name, Geneerator::Registers dest) {
        auto var = getVar(name);
        auto inst = Geneerator::mov(dest, var.sector, var.fromEbpOffset, Geneerator::Registers::esp);
        Instructions.insert(Instructions.end(), inst.begin(), inst.end());
    }

    void OtoOTranslator::MovConstantToRegister(int constant, Geneerator::Registers dest) {
        auto instr = Geneerator::mov(dest, constant);
        Instructions.insert(Instructions.end(), Instructions.begin(), Instructions.end());
    }

    void OtoOTranslator::SetInstruction(Instruction inst) {
        if(inst.Parameters[0].IsVariable && !inst.Parameters[1].IsFunction && !inst.Parameters[1].IsVariable){
            auto varDest = getVar(inst.Parameters[0].name);
            int dest;
            switch (inst.Parameters[1].type) {
                case DataTypes::Integer:
                    dest = std::stoi(inst.Parameters[1].name);
                    break;
                case DataTypes::Character:
                    dest = (int)inst.Parameters[1].name[0];
                    break;
                case DataTypes::Boolean:
                    if(inst.Parameters[1].name == FALSE_NAME){
                        dest = 0;
                    }else{
                        dest = 1;
                    }
                    break;
                case DataTypes::FloatingPoint:
                    float nv = std::stof(inst.Parameters[1].name);
                    dest = *((int*)&nv);
                    break;
            }
            auto instr = Geneerator::mov(varDest.sector, varDest.fromEbpOffset, Geneerator::Registers::ebp, dest);
            Instructions.insert(Instructions.end(), instr.begin(), instr.end());
        }
        else if(inst.Parameters[0].IsVariable && inst.Parameters[1].IsVariable){
            auto varDest = getVar(inst.Parameters[0].name);
            auto varSource = getVar(inst.Parameters[1].name);

            auto instr = Geneerator::mov(varDest.sector, varDest.fromEbpOffset, Geneerator::Registers::ebp, varSource.sector, varSource.fromEbpOffset, Geneerator::Registers::ebp);
            Instructions.insert(Instructions.end(), instr.begin(), instr.end());
        }
    }

    void OtoOTranslator::WriteResulToFile(std::string filepath) {
        std::ofstream f(filepath);

        int size = Instructions.size();

        f.write((char*)&size, 4);
        f.write((char*)&bodyStart, 4);

        for(int i = 0; i < Instructions.size(); i++){
            f.write((char*)&Instructions[i], 4);
        }

        f.close();
    }

    void OtoOTranslator::LoadFunctions(std::vector<Function> functions) {

        for(auto fun : functions) {
            FunctionStored newF;
            newF.sector = "main";
            newF.fromZeroOffset = Instructions.size();
            auto mov = Geneerator::mov(Geneerator::Registers::ebp, Geneerator::Registers::esp);
            Instructions.insert(Instructions.end(), mov.begin(), mov.end());
            LoadVariables(fun.arguments, true);
            LoadVariables(fun.variables, true);

            auto addd = Geneerator::add(Geneerator::Registers::ebp, addOffset + 1);
            ADDVTV(Instructions, addd)

            for(auto inst : fun.body){
                ProccessInstruction(inst);
            }

            if(fun.returnType == DataTypes::Void){
                auto retCommand = Geneerator::ret();
                Instructions.insert(Instructions.end(), retCommand.begin(), retCommand.end());
            }
            newF.name = fun.name;
            newF.stackSize = addOffset;
            storedFunctions.push_back(newF);

            addOffset = 0;
            additionalVariables.clear();
        }
    }

    void OtoOTranslator::CallFunction(Instruction inst) {
        auto func = getFun(inst.name);

        auto saveEBP = Geneerator::push(Geneerator::Registers::ebp);
        ADDVTV(Instructions, saveEBP);
        auto util = Geneerator::push(Geneerator::Registers::ebp);
        ADDVTV(Instructions, util);

        int currentOffset = 0;

        for(auto p : inst.Parameters){
            if(!p.IsVariable && !p.IsFunction){
                int source;
                switch (p.type) {
                    case DataTypes::Integer:
                        source = std::stoi(p.name);
                        break;
                    case DataTypes::Character:
                        source = (int)p.name[0];
                        break;
                    case DataTypes::Boolean:
                        if(p.name == FALSE_NAME){
                            source = 0;
                        }else{
                            source = 1;
                        }
                        break;
                    case DataTypes::FloatingPoint:
                        float nv = std::stof(inst.Parameters[1].name);
                        source = *((int*)&nv);
                        break;
                }
                auto m = Geneerator::mov("__REGISTERS__", currentOffset, Geneerator::Registers::esp, source);
                Instructions.insert(Instructions.end(), m.begin(), m.end());
                currentOffset -= GetDataSize(p.type);
            }
        }

        auto su = Geneerator::sub(Geneerator::Registers::esp, func.stackSize);
        ADDVTV(Instructions, su)
        auto c = Geneerator::call(func.sector, func.fromZeroOffset, Geneerator::Registers::NULLREG);
        Instructions.insert(Instructions.end(), c.begin(), c.end());
        auto a = Geneerator::add(Geneerator::Registers::esp, func.stackSize);
        ADDVTV(Instructions, a);
        auto s = Geneerator::pop(Geneerator::Registers::ebp);
        ADDVTV(Instructions, s)
        auto load = Geneerator::pop(Geneerator::Registers::ebp);
        ADDVTV(Instructions, load)
    }

    OtoOTranslator::FunctionStored OtoOTranslator::getFun(std::string name) {

        for(auto func: storedFunctions){
            if(func.name == name){
                return func;
            }
        }

        return OtoOTranslator::FunctionStored();
    }
} // O