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
#define ARRAY_CREATION_NAME "ARRAY_INIT"
#define FREE_INSTRUCTION_NAME "_FREE"
#define ARRAY_ELEMENT_ACCESS_NAME "ARRAY_ACCESS_INTEGER"
#define STRUCTURE_ELEMENT_ACCESS_NAME "STRUCTURE_ACCESS"
#define WHILE_CYCLE_NAME "WHILE_CYCLE"
#define MALLOC_INSTRUCTION_NAME "_MALLOC"

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
        structs = std::vector<Structure>(f.structures);
        adtTable = f.adtTable;

        auto mov = Geneerator::mov(Geneerator::Registers::ebp, Geneerator::Registers::esp);

        Instructions = &mainFlow;

        LoadFunctions(f.functions);

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
        if(inst.name == FREE_INSTRUCTION_NAME){
            FreeInstruction(inst);
        }
        else if(inst.name == SET_VALUE_NAME){
            SetInstruction(inst);
        }else if(inst.name == IF_NAME){
            IfFunction(inst);
        }else if(inst.name == ELSE_NAME){
            ElseInstruction(inst);
        } else if(inst.name == ELIF_NAME){
            ElifInstruction(inst);
        }else if(inst.name == WHILE_CYCLE_NAME){
            WhileInstruction(inst);
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
        }else if(inst.Parameters[0].name == ARRAY_ELEMENT_ACCESS_NAME) {
            LoadInstToReg(inst.Parameters[0].Parameters[0], GR::mc3);
            LoadInstToReg(inst.Parameters[0].Parameters[1], GR::edi);
            auto add = G::add(GR::edi, GR::mc3);
            ADDVTV(Instructions, add);
            auto mov = G::mov("", 0, GR::edi, GR::aa0);
            ADDVTV(Instructions, mov);
        }else if(inst.Parameters[0].name == STRUCTURE_ELEMENT_ACCESS_NAME){
            LoadInstToReg(inst.Parameters[0].Parameters[1], GR::edi);
            auto elementName = inst.Parameters[0].Parameters[0].name;
            auto dt = inst.Parameters[0].Parameters[1].type;
            Structure structure;
            for(auto elem : structs){
                if(elem.myDt == dt){
                    structure = elem;
                    break;
                }
            }
            int offset = 0;
            for(int i = 0; i < structure.variables.size(); i++){
                if(structure.variables[i].name == elementName){
                    offset = i;
                    break;
                }
            }
            auto add = G::add(GR::edi, offset);
            ADDVTV(Instructions, add)
            auto mov = G::mov("", 0, GR::edi, GR::aa0);
            ADDVTV(Instructions, mov);
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


        if(func.sector != "") {
            auto subEsp = Geneerator::sub(Geneerator::Registers::esp, func.stackSize - inst.Parameters.size());
            ADDVTV(Instructions, subEsp);
        }else{
            auto subEsp = Geneerator::sub(Geneerator::Registers::esp, inst.Parameters.size() - 1);
            ADDVTV(Instructions, subEsp);
        }

        auto movEbp = Geneerator::mov(Geneerator::Registers::ebp, Geneerator::Registers::esp);
        ADDVTV(Instructions, movEbp)
        if(func.stackSize != 0) {
            auto setEbp = Geneerator::add(Geneerator::Registers::ebp, func.stackSize - 1);
            ADDVTV(Instructions, setEbp)
        }
        std::vector<int> c;
        if(func.sector != "") {
            c = Geneerator::call(func.sector, func.fromZeroOffset, Geneerator::Registers::NULLREG);
        }else{
            c = Geneerator::call(inst.name, 0, GR::NULLREG);
        }
        Instructions->insert(Instructions->end(), c.begin(), c.end());
        if(func.stackSize != 0) {
            auto addEsp = Geneerator::add(Geneerator::Registers::esp, func.stackSize);
            ADDVTV(Instructions, addEsp);
        }else{
            auto addEsp = Geneerator::add(Geneerator::Registers::esp, inst.Parameters.size());
            ADDVTV(Instructions, addEsp);
        }
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
        auto saveMC1 = G::push(GR::mc1);
        auto saveMC2 = G::push(GR::mc2);
        auto loadMC1 = G::pop(GR::mc1);
        auto loadMC2 = G::pop(GR::mc2);
        ADDVTV(Instructions, saveMC1)
        ADDVTV(Instructions, saveMC2)
        LoadInstToReg(inst.Parameters[0], GR::mc1);
        ADDVTV(Instructions, saveMC1)
        LoadInstToReg(inst.Parameters[1], GR::mc2);
        ADDVTV(Instructions, loadMC1)

        if(isStdLogic(inst.Parameters[0], inst.Parameters[1])) {
            ProccessStdLogic(inst.Parameters[0], inst.Parameters[1], inst.name);
        }

        auto mm0 = G::mov(GR::mc0, GR::mc1);
        ADDVTV(Instructions, mm0);
        ADDVTV(Instructions, loadMC2);
        ADDVTV(Instructions, loadMC1);
    }

    void OtoOTranslator::LoadInstToReg(Instruction inst, Geneerator::Registers reg) {
        if(!inst.IsVariable && !inst.IsFunction && !inst.ArithmeticProccess){
            if(inst.name == POINTER_ACCESS_INSTRUCTION_NAME){
                LoadInstToReg(inst.Parameters[0], GR::mc1);
                auto mm1 = Geneerator::mov(reg, "", 0, GR::mc1);
                ADDVTV(Instructions, mm1);
            }else if(inst.name == MALLOC_INSTRUCTION_NAME) {
                auto malloc = G::malloc(std::stoi(inst.Parameters[0].name));
                ADDVTV(Instructions, malloc);
                auto movRes = G::mov(reg, GR::eax);
                ADDVTV(Instructions, movRes)
            }else if(inst.name == POINTER_GET_INSTRUCTION_NAME) {
                auto v = getVar(inst.Parameters[0].name);
                auto moveEbp = G::mov(reg, GR::ebp);
                auto subTarget = G::add(reg, v.fromEbpOffset);
                ADDVTV(Instructions, moveEbp);
                ADDVTV(Instructions, subTarget)

            }else if(inst.name == ARRAY_CREATION_NAME){
                auto malloc = G::malloc((int)inst.Parameters[0].name[0]);
                ADDVTV(Instructions, malloc);
                auto movRes = G::mov(reg, GR::eax);
                ADDVTV(Instructions, movRes)
                for(int i = 1; i < inst.Parameters.size(); i++){
                    LoadInstToReg(inst.Parameters[i], GR::aa1);
                    auto mov = G::mov("", i - 1, reg, GR::aa1);
                    ADDVTV(Instructions, mov);
                }
            }else if(inst.name == ARRAY_ELEMENT_ACCESS_NAME){
                LoadInstToReg(inst.Parameters[0], GR::mc1);
                LoadInstToReg(inst.Parameters[1], GR::edi);
                auto add = G::add(GR::edi, GR::mc1);
                ADDVTV(Instructions, add);
                auto mov = G::mov(reg, "", 0, GR::edi);
                ADDVTV(Instructions, mov);
            }else if(inst.name == STRUCTURE_ELEMENT_ACCESS_NAME){
                LoadInstToReg(inst.Parameters[1], GR::edi);
                Structure structure;
                for(auto elem : structs){
                    if(elem.myDt == inst.Parameters[1].type){
                        structure = elem;
                        break;
                    }
                }
                int offset = 0;
                for(int i = 0; i < structure.variables.size(); i++){
                    if(structure.variables[i].name == inst.Parameters[0].name){
                        offset = i;
                        break;
                    }
                }
                auto add = G::add(GR::edi, offset);
                ADDVTV(Instructions, add)
                auto mov = G::mov(reg, "", 0, GR::edi);
                ADDVTV(Instructions, mov);
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
        return (ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Integer) || ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Character));
    }

    OtoOTranslator::StdLogicType OtoOTranslator::getLogicType(Instruction instOP1, Instruction instOP2) {
        if(ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Integer) || ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Character)){
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

    void OtoOTranslator::FreeInstruction(Instruction inst) {
        LoadInstToReg(inst.Parameters[0], GR::esi);
        auto f = G::free(GR::esi);
        ADDVTV(Instructions, f)
    }

    void OtoOTranslator::WhileInstruction(Instruction inst) {
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
        (*Instructions)[jumpOffsetId] = Instructions->size() - oldSize;
        LoadInstToReg(inst.Parameters[0], GR::mc3);
        auto cm = G::cmp(GR::mc3, 1);
        auto je = G::jme("main", jumpOffsetId + 3, GR::NULLREG);
        ADDVTV(Instructions, cm);
        ADDVTV(Instructions, je);
    }
} // O