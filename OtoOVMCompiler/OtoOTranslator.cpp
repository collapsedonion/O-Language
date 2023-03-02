//
// Created by Роман  Тимофеев on 01.03.2023.
//

#include "OtoOTranslator.h"

#define SET_VALUE_NAME "SET_VALUE"
#define FALSE_NAME "FALSE"
#define RETURN_NAME "return"

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
            vs.sector = "";
            vs.fromEbpOffset = (offset * -1);
            if(add){
                vs.fromEbpOffset += 1;
            }
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
        }
        else if(inst.name == RETURN_NAME){
            ReturnFunction(inst);
        }
        else if(inst.IsFunction){
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
            auto instr = Geneerator::mov(varDest.sector, varDest.fromEbpOffset, Geneerator::Registers::ebp,GetValueToInt(inst.Parameters[1]));
            Instructions.insert(Instructions.end(), instr.begin(), instr.end());
        }
        else if(inst.Parameters[0].IsVariable && inst.Parameters[1].IsVariable){
            auto varDest = getVar(inst.Parameters[0].name);
            auto varSource = getVar(inst.Parameters[1].name);

            auto instr = Geneerator::mov(varDest.sector, varDest.fromEbpOffset, Geneerator::Registers::ebp, varSource.sector, varSource.fromEbpOffset, Geneerator::Registers::ebp);
            Instructions.insert(Instructions.end(), instr.begin(), instr.end());
        }else if(inst.Parameters[0].IsVariable && inst.Parameters[1].IsFunction){
            auto varDest = getVar(inst.Parameters[0].name);
            CallFunction(inst.Parameters[1]);

            auto instr = Geneerator::mov(varDest.sector, varDest.fromEbpOffset, Geneerator::Registers::ebp, Geneerator::Registers::eax);
            ADDVTV(Instructions, instr)
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
            LoadVariables(fun.arguments, true);
            LoadVariables(fun.variables, true);

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

        auto saveService = Geneerator::pushs();
        ADDVTV(Instructions, saveService);

        auto saveEBP = Geneerator::push(Geneerator::Registers::ebp);
        ADDVTV(Instructions, saveEBP);

        for(auto p : inst.Parameters){
            if(!p.IsVariable && !p.IsFunction){
                auto m = Geneerator::push(GetValueToInt(p));
                Instructions.insert(Instructions.end(), m.begin(), m.end());
            }else if(p.IsVariable){
                auto vd = getVar(p.name);
                auto movToR = Geneerator::mov(Geneerator::Registers::esi, vd.sector, vd.fromEbpOffset, Geneerator::Registers::ebp);
                ADDVTV(Instructions, movToR);
                auto p = Geneerator::push(Geneerator::Registers::esi);
                ADDVTV(Instructions, p);
            }else if(p.IsFunction){
                CallFunction(p);
                auto p = Geneerator::push(Geneerator::Registers::eax);
                ADDVTV(Instructions, p)
            }
        }

        auto subEsp = Geneerator::sub(Geneerator::Registers::esp, func.stackSize - inst.Parameters.size());
        ADDVTV(Instructions, subEsp);

        auto movEbp = Geneerator::mov(Geneerator::Registers::ebp, Geneerator::Registers::esp);
        ADDVTV(Instructions, movEbp)
        if(func.stackSize != 0) {
            auto setEbp = Geneerator::add(Geneerator::Registers::ebp, func.stackSize - 1);
            ADDVTV(Instructions, setEbp)
        }
        auto c = Geneerator::call(func.sector, func.fromZeroOffset, Geneerator::Registers::NULLREG);
        Instructions.insert(Instructions.end(), c.begin(), c.end());
        auto addEsp = Geneerator::add(Geneerator::Registers::esp, func.stackSize);
        ADDVTV(Instructions, addEsp);
        auto loadEBP = Geneerator::pop(Geneerator::Registers::ebp);
        ADDVTV(Instructions, loadEBP)

        auto loadService = Geneerator::pops();
        ADDVTV(Instructions, loadService)
    }

    OtoOTranslator::FunctionStored OtoOTranslator::getFun(std::string name) {

        for(auto func: storedFunctions){
            if(func.name == name){
                return func;
            }
        }

        return OtoOTranslator::FunctionStored();
    }

    int OtoOTranslator::GetValueToInt(Instruction inst) {
        int source;
        switch (inst.type) {
            case DataTypes::Integer:
                source = std::stoi(inst.name);
                break;
            case DataTypes::Character:
                source = (int)inst.name[0];
                break;
            case DataTypes::Boolean:
                if(inst.name == FALSE_NAME){
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

        return source;
    }

    void OtoOTranslator::ReturnFunction(Instruction inst) {
        if(!inst.Parameters[0].IsVariable && !inst.Parameters[0].IsFunction){
            int val = GetValueToInt(inst.Parameters[0]);
            auto movToEAX = Geneerator::mov(Geneerator::Registers::eax, val);
            ADDVTV(Instructions, movToEAX)
        }else if(inst.Parameters[0].IsVariable){
            auto v = getVar(inst.Parameters[0].name);
            auto movToEAX = Geneerator::mov(Geneerator::Registers::eax, v.sector, v.fromEbpOffset, Geneerator::Registers::ebp);
            ADDVTV(Instructions, movToEAX)
        }

        auto ret = Geneerator::ret();
        ADDVTV(Instructions, ret);
    }
} // O