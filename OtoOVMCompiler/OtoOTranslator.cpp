//
// Created by Роман  Тимофеев on 01.03.2023.
//

#include "OtoOTranslator.h"

#define SET_VALUE_NAME "SET_VALUE"
#define FALSE_NAME "FALSE"
#define RETURN_NAME "return"
#define POINTER_ACCESS_INSTRUCTION_NAME "GET_POINTER_CONTENT"
#define POINTER_GET_INSTRUCTION_NAME "GET_POINTER"
#define IF_NAME "if"
#define ELSE_NAME "else"
#define ELIF_NAME "else if"

#define ADDVTV(vd, vs) vd->insert(vd->end(), vs.begin(), vs.end());

#define G Geneerator
#define GR Geneerator::Registers

#define FILLVAR(varName) varName.sector, varName.fromEbpOffset, GR::ebp
#define LOADFUNRESULT(reg) G::mov(reg, GR::eax)
#define LOADMATH(reg) G::mov(reg, GR::mc0)
#define PUSH(reg) G::push(GR::reg)
#define POP(reg) G::pop(GR::reg)
#define ISINSTTYPEEQU(inst1, inst2, TYPE) inst1.type == TYPE && inst2.type == TYPE

namespace O {
    void OtoOTranslator::Build(File f) {
        auto mov = Geneerator::mov(Geneerator::Registers::ebp, Geneerator::Registers::esp);

        LoadFunctions(f.functions);
        Instructions = &mainFlow;

        bodyStart = this->Instructions->size();

        Instructions->insert(Instructions->end(), mov.begin(), mov.end());
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
            Instructions->insert(Instructions->end(), sub.begin(), sub.end());
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
        }else if(inst.name == IF_NAME){
            IfFunction(inst);
        }else if(inst.name == ELSE_NAME){
            ElseInstruction(inst);
        } else if(inst.name == ELIF_NAME){
            ElifInstruction(inst);
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
        Instructions->insert(Instructions->end(), inst.begin(), inst.end());
    }

    void OtoOTranslator::MovConstantToRegister(int constant, Geneerator::Registers dest) {
        auto instr = Geneerator::mov(dest, constant);
        Instructions->insert(Instructions->end(), instr.begin(), instr.end());
    }

    void OtoOTranslator::SetInstruction(Instruction inst) {
        LoadInstToReg(inst.Parameters[1], GR::aa0);

        if(inst.Parameters[0].IsVariable){
            auto var = getVar(inst.Parameters[0].name);
            auto mov = G::mov(FILLVAR(var), GR::aa0);
            ADDVTV(Instructions, mov)
        }else if(inst.Parameters[0].name == POINTER_ACCESS_INSTRUCTION_NAME){
            LoadInstToReg(inst.Parameters[0].Parameters[0], GR::mc3);
            auto mov = G::mov("", 0, GR::mc3, GR::aa0);
            ADDVTV(Instructions, mov)
        }
    }

    void OtoOTranslator::WriteResulToFile(std::string filepath) {
        std::ofstream f(filepath);

        int size = mainFlow.size();

        f.write((char*)&size, 4);
        f.write((char*)&bodyStart, 4);

        for(int i = 0; i < mainFlow.size(); i++){
            f.write((char*)&((*Instructions)[i]), 4);
        }

        f.close();
    }

    void OtoOTranslator::LoadFunctions(std::vector<Function> functions) {

        for(auto fun : functions) {
            FunctionStored newF;
            newF.sector = "main";
            newF.fromZeroOffset = Instructions->size();
            LoadVariables(fun.arguments, true);
            LoadVariables(fun.variables, true);

            for(auto inst : fun.body){
                ProccessInstruction(inst);
            }

            if(fun.returnType == DataTypes::Void){
                auto retCommand = Geneerator::ret();
                Instructions->insert(Instructions->end(), retCommand.begin(), retCommand.end());
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
            LoadInstToReg(p, GR::esi);
            auto push = PUSH(esi);
            ADDVTV(Instructions, push)
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
        Instructions->insert(Instructions->end(), c.begin(), c.end());
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
        LoadInstToReg(inst.Parameters[0], GR::eax);
        auto ret = Geneerator::ret();
        ADDVTV(Instructions, ret);
    }

    void OtoOTranslator::MathematicalProccess(Instruction inst) {
        LoadInstToReg(inst.Parameters[0], GR::mc1);
        LoadInstToReg(inst.Parameters[1], GR::mc2);

        if(isStdLogic(inst.Parameters[0], inst.Parameters[1])) {
            ProccessStdLogic(inst.Parameters[0], inst.Parameters[1], inst.name);
        }

        auto mm0 = G::mov(GR::mc0, GR::mc1);
        ADDVTV(Instructions, mm0);
    }

    void OtoOTranslator::LoadInstToReg(Instruction inst, Geneerator::Registers reg) {
        if(!inst.IsVariable && !inst.IsFunction && !inst.ArithmeticProccess){
            if(inst.name == POINTER_ACCESS_INSTRUCTION_NAME){
                LoadInstToReg(inst.Parameters[0], GR::mc1);
                auto mm1 = Geneerator::mov(reg, "", 0, GR::mc1);
                ADDVTV(Instructions, mm1);
            }else if(inst.name == POINTER_GET_INSTRUCTION_NAME){
                auto v = getVar(inst.Parameters[0].name);
                auto moveEbp = G::mov(reg, GR::ebp);
                auto subTarget = G::add(reg, v.fromEbpOffset);
                ADDVTV(Instructions, moveEbp);
                ADDVTV(Instructions, subTarget)
            }
            else {
                auto op1 = GetValueToInt(inst);
                auto mm1 = Geneerator::mov(reg, op1);
                ADDVTV(Instructions, mm1);
            }
        }else if(inst.IsVariable){
            auto op1 = getVar(inst.name);
            auto mm1 = G::mov(reg,FILLVAR(op1));
            ADDVTV(Instructions, mm1);
        }else if(inst.IsFunction){
            CallFunction(inst);
            auto mm1 = LOADFUNRESULT(reg);
            ADDVTV(Instructions, mm1);
        }else if(inst.ArithmeticProccess){
            MathematicalProccess(inst);
            auto mm1 = LOADMATH(reg);
            ADDVTV(Instructions, mm1);
        }
    }

    bool OtoOTranslator::isStdLogic(Instruction instOP1, Instruction instOP2) {
        return (ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Integer));
    }

    OtoOTranslator::StdLogicType OtoOTranslator::getLogicType(Instruction instOP1, Instruction instOP2) {
        if(ISINSTTYPEEQU(instOP1, instOP2,DataTypes::Integer)){
            return I;
        }
    }

    void OtoOTranslator::ProccessStdLogic(Instruction instOP1, Instruction instOP2, std::string type) {
        std::vector<int> newInst;
        switch (getLogicType(instOP1, instOP2)) {
            case I:{
                if(type == "+") {
                    newInst = G::add(GR::mc1, GR::mc2);
                }else if(type == "-"){
                    newInst = G::sub(GR::mc1, GR::mc2);
                }else if(type == "?"){
                    newInst = G::cmp(GR::mc1, GR::mc2);
                    auto preReg = G::mov(GR::mc1, 0);
                    auto r = G::move(GR::mc1, 1);
                    newInst.insert(newInst.end(), preReg.begin(), preReg.end());
                    newInst.insert(newInst.end(), r.begin(), r.end());
                }else if(type == ">"){
                    newInst = G::cmp(GR::mc1, GR::mc2);
                    auto preReg = G::mov(GR::mc1, 0);
                    auto r = G::movg(GR::mc1, 1);
                    newInst.insert(newInst.end(), preReg.begin(), preReg.end());
                    newInst.insert(newInst.end(), r.begin(), r.end());
                }else if(type == "<"){
                    newInst = G::cmp(GR::mc1, GR::mc2);
                    auto preReg = G::mov(GR::mc1, 0);
                    auto r = G::movl(GR::mc1, 1);
                    newInst.insert(newInst.end(), preReg.begin(), preReg.end());
                    newInst.insert(newInst.end(), r.begin(), r.end());
                }
                break;
            }
        }

        ADDVTV(Instructions, newInst);
    }

    void OtoOTranslator::IfFunction(Instruction inst) {
        auto jmpToEndBody = Geneerator::jmp("", 123456, GR::eip);
        ADDVTV(Instructions, jmpToEndBody)
        int jumpOffsetId = Instructions->size() - 1 - 2;
        int oldSize = Instructions->size();
        auto saveFlag = G::push(GR::flag);
        ADDVTV(Instructions, saveFlag)
        for(int i = 1; i<inst.Parameters.size(); i++){
           ProccessInstruction(inst.Parameters[i]);
        }
        auto loadFlag = G::pop(GR::flag);
        ADDVTV(Instructions, loadFlag)
        jmpToEndBody = Geneerator::jmp("", 123456, GR::eip);
        ADDVTV(Instructions, jmpToEndBody)
        int offsetOfExit = Instructions->size() - 1 - 2;
        int exitSize = Instructions->size();
        (*Instructions)[jumpOffsetId] = Instructions->size() - oldSize;
        LoadInstToReg(inst.Parameters[0], GR::mc3);
        auto cm = G::cmp(GR::mc3, 1);
        auto je = G::jme("main", jumpOffsetId + 3, GR::NULLREG);
        ADDVTV(Instructions, cm);
        ADDVTV(Instructions, je);
        (*Instructions)[offsetOfExit] = Instructions->size() - exitSize;
    }

    void OtoOTranslator::ElseInstruction(Instruction inst) {
        auto jmpIfT = Geneerator::jme("", 123456, GR::eip);
        ADDVTV(Instructions, jmpIfT);
        int jumpOffset = Instructions->size() - 1 - 2;
        int oldSize = Instructions->size();
        auto saveFlag = G::push(GR::flag);
        ADDVTV(Instructions, saveFlag)
        for(int i = 0; i < inst.Parameters.size(); i++){
            ProccessInstruction(inst.Parameters[i]);
        }
        auto loadFlag = G::pop(GR::flag);
        ADDVTV(Instructions, loadFlag)
        (*Instructions)[jumpOffset] = Instructions->size() - oldSize;
    }

    void OtoOTranslator::ElifInstruction(Instruction inst) {
        auto jmpIfT = Geneerator::jme("", 123456, GR::eip);
        ADDVTV(Instructions, jmpIfT);
        int jumpOffset = Instructions->size() - 1 - 2;
        int oldSize = Instructions->size();
        IfFunction(inst);
        (*Instructions)[jumpOffset] = Instructions->size() - oldSize;
    }
} // O