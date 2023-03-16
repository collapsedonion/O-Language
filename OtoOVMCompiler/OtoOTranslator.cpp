//
// Created by Роман  Тимофеев on 01.03.2023.
//

#include "OtoOTranslator.h"

#define SET_VALUE_NAME L"SET_VALUE"
#define FALSE_NAME L"FALSE"
#define RETURN_NAME L"return"
#define POINTER_ACCESS_INSTRUCTION_NAME L"GET_POINTER_CONTENT"
#define POINTER_GET_INSTRUCTION_NAME L"GET_POINTER"
#define IF_NAME L"if"
#define ELSE_NAME L"else"
#define ELIF_NAME L"else if"
#define ARRAY_CREATION_NAME L"ARRAY_INIT"
#define FREE_INSTRUCTION_NAME L"_FREE"
#define ARRAY_ELEMENT_ACCESS_NAME L"ARRAY_ACCESS_INTEGER"
#define STRUCTURE_ELEMENT_ACCESS_NAME L"STRUCTURE_ACCESS"
#define WHILE_CYCLE_NAME L"WHILE_CYCLE"
#define MALLOC_INSTRUCTION_NAME L"_MALLOC"

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
            vs.sector = L"";
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
        else if(inst.name == SET_VALUE_NAME && inst.ArithmeticProccess){
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
        else if(inst.name == RETURN_NAME) {
            for(int i = 0; i < popSystemCOunt; i++) {
                auto popSystem = G::pop(GR::flag);
                ADDVTV(Instructions, popSystem);
            }
            ReturnFunction(inst);
        }
        else if(inst.IsFunction){
            CallFunction(inst);
        }else if(inst.ArithmeticProccess){
            MathematicalProccess(inst);
        }
    }

    OtoOTranslator::VariableStored OtoOTranslator::getVar(std::wstring name) {
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

    void OtoOTranslator::MovVariableToRegister(std::wstring name, Geneerator::Registers dest) {
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
            auto mov = G::mov(L"", 0, GR::mc3, GR::aa0);
            ADDVTV(Instructions, mov)
        }else if(inst.Parameters[0].name == ARRAY_ELEMENT_ACCESS_NAME) {
            LoadInstToReg(inst.Parameters[0].Parameters[0], GR::mc3);
            LoadInstToReg(inst.Parameters[0].Parameters[1], GR::edi);
            auto add = G::add(GR::edi, GR::mc3);
            ADDVTV(Instructions, add);
            auto mov = G::mov(L"", 0, GR::edi, GR::aa0);
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
            auto mov = G::mov(L"", 0, GR::edi, GR::aa0);
            ADDVTV(Instructions, mov);
        }
    }

    void OtoOTranslator::WriteResulToFile(std::wstring filepath) {
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
            newF.sector = L"main";
            newF.fromZeroOffset = Instructions->size();
            LoadVariables(fun.arguments, true);
            addOffset += 1;
            LoadVariables(fun.variables, true);

            int countOfLocalVariable = addOffset - fun.arguments.size() - 1;

            localSize = countOfLocalVariable;

            auto sub0 = G::sub(GR::esp, countOfLocalVariable);
            ADDVTV(Instructions, sub0);

            for(auto argument : fun.arguments){
                newF.parameters.push_back(argument.type);
            }

            for(auto inst : fun.body){
                ProccessInstruction(inst);
            }

            if(fun.returnType == DataTypes::Void){
                if(countOfLocalVariable != 0) {
                    auto add0 = G::add(GR::esp, countOfLocalVariable);
                    ADDVTV(Instructions, add0);
                }
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
        std::vector<DataTypes> argumentsDataTypes;

        for(auto argument : inst.Parameters){
            argumentsDataTypes.push_back(argument.type);
        }

        auto func = getFun(inst.name, argumentsDataTypes);

        auto saveService = Geneerator::pushs();
        ADDVTV(Instructions, saveService);

        auto saveEBP = Geneerator::push(Geneerator::Registers::ebp);
        ADDVTV(Instructions, saveEBP);

        for(auto p : inst.Parameters){
            LoadInstToReg(p, GR::esi);
            auto push = PUSH(esi);
            ADDVTV(Instructions, push)
        }

        if(inst.Parameters.size() != 0) {
            auto m0 = G::mov(GR::ebp, GR::esp);
            ADDVTV(Instructions, m0);
            auto a0 = G::add(GR::ebp, inst.Parameters.size() - 1);
            ADDVTV(Instructions, a0);
        }

        std::vector<int> c;
        if(func.sector != L"") {
            c = Geneerator::call(func.sector, func.fromZeroOffset, Geneerator::Registers::NULLREG);
        }else{
            c = Geneerator::call(inst.name, 0, GR::NULLREG);
        }
        Instructions->insert(Instructions->end(), c.begin(), c.end());

        if(inst.Parameters.size() != 0){
            auto addEsp = G::add(GR::esp, inst.Parameters.size());
            ADDVTV(Instructions, addEsp);
        }

        auto loadEBP = Geneerator::pop(Geneerator::Registers::ebp);
        ADDVTV(Instructions, loadEBP)

        auto loadService = Geneerator::pops();
        ADDVTV(Instructions, loadService)
    }

    OtoOTranslator::FunctionStored OtoOTranslator::getFun(std::wstring name, std::vector<DataTypes> argumentDataTypes) {

        for(auto func: storedFunctions){
            if(func.name == name && func.parameters == argumentDataTypes){
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
                if(inst.name[0] == '\\'){
                    switch (inst.name[1]) {
                        case 'n':
                            source = '\n';
                            break;
                        case '0':
                            source = '\0';
                            break;
                        case '\r':
                            source = '\r';
                            break;
                    }
                }
                else {
                    source = (int) inst.name[0];
                }
                break;
            case DataTypes::Boolean:
                if(inst.name == FALSE_NAME){
                    source = 0;
                }else{
                    source = 1;
                }
                break;
            case DataTypes::FloatingPoint:
                float nv = std::stof(inst.name);
                source = *((int*)&nv);
                break;
        }

        return source;
    }

    void OtoOTranslator::ReturnFunction(Instruction inst) {
        if(inst.Parameters.size() != 0) {
            LoadInstToReg(inst.Parameters[0], GR::eax);
        }
        if(localSize!= 0) {
            auto add0 = G::add(GR::esp, localSize);
            ADDVTV(Instructions, add0);
        }
        auto ret = Geneerator::ret();
        ADDVTV(Instructions, ret);
    }

    void OtoOTranslator::MathematicalProccess(Instruction inst) {
        if(isStdLogic(inst.Parameters[0], inst.Parameters[1])) {
            auto saveMC1 = G::push(GR::mc1);
            auto saveMC2 = G::push(GR::mc2);
            auto loadMC1 = G::pop(GR::mc1);
            auto loadMC2 = G::pop(GR::mc2);
            ProccessStdLogic(inst.Parameters[0], inst.Parameters[1], inst.name);
            ADDVTV(Instructions, saveMC1)
            ADDVTV(Instructions, saveMC2)
            LoadInstToReg(inst.Parameters[0], GR::mc1);
            ADDVTV(Instructions, saveMC1)
            LoadInstToReg(inst.Parameters[1], GR::mc2);
            ADDVTV(Instructions, loadMC1)
            ProccessStdLogic(inst.Parameters[0], inst.Parameters[1], inst.name);
            auto mm0 = G::mov(GR::mc0, GR::mc1);
            ADDVTV(Instructions, mm0);
            ADDVTV(Instructions, loadMC2);
            ADDVTV(Instructions, loadMC1);
        }
        else{
            CallFunction(inst);
            auto mm0 = G::mov(GR::mc0, GR::eax);
            ADDVTV(Instructions, mm0);
        }
    }

    void OtoOTranslator::LoadInstToReg(Instruction inst, Geneerator::Registers reg) {
        if(!inst.IsVariable && !inst.IsFunction && !inst.ArithmeticProccess){
            if(inst.name == POINTER_ACCESS_INSTRUCTION_NAME){
                LoadInstToReg(inst.Parameters[0], GR::mc1);
                auto mm1 = Geneerator::mov(reg, L"", 0, GR::mc1);
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
                    auto mov = G::mov(L"", i - 1, reg, GR::aa1);
                    ADDVTV(Instructions, mov);
                }
            }else if(inst.name == ARRAY_ELEMENT_ACCESS_NAME){
                LoadInstToReg(inst.Parameters[0], GR::mc1);
                LoadInstToReg(inst.Parameters[1], GR::edi);
                auto add = G::add(GR::edi, GR::mc1);
                ADDVTV(Instructions, add);
                auto mov = G::mov(reg, L"", 0, GR::edi);
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
                auto mov = G::mov(reg, L"", 0, GR::edi);
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
            auto mm1 = G::mov(reg, GR::eax);
            ADDVTV(Instructions, mm1);
        }else if(inst.ArithmeticProccess){
            MathematicalProccess(inst);
            auto mm1 = LOADMATH(reg);
            ADDVTV(Instructions, mm1);
        }
    }

    bool OtoOTranslator::isStdLogic(Instruction instOP1, Instruction instOP2) {
        return (ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Integer) || ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Character) ||
                ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Boolean) || ISINSTTYPEEQU(instOP1, instOP2, DataTypes::FloatingPoint));
    }

    OtoOTranslator::StdLogicType OtoOTranslator::getLogicType(Instruction instOP1, Instruction instOP2) {
        if(ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Integer) || ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Character)){
            return I;
        }else if(ISINSTTYPEEQU(instOP1, instOP2, DataTypes::Boolean)){
            return B;
        }else if(ISINSTTYPEEQU(instOP1, instOP2, DataTypes::FloatingPoint)){
            return F;
        }
    }

    void OtoOTranslator::ProccessStdLogic(Instruction instOP1, Instruction instOP2, std::wstring type) {
        std::vector<int> newInst;
        switch (getLogicType(instOP1, instOP2)) {
            case I: {
                if (type == L"+") {
                    newInst = G::add(GR::mc1, GR::mc2);
                } else if (type == L"-") {
                    newInst = G::sub(GR::mc1, GR::mc2);
                } else if (type == L"?") {
                    newInst = G::cmp(GR::mc1, GR::mc2);
                    auto preReg = G::mov(GR::mc1, 0);
                    auto r = G::move(GR::mc1, 1);
                    newInst.insert(newInst.end(), preReg.begin(), preReg.end());
                    newInst.insert(newInst.end(), r.begin(), r.end());
                } else if (type == L">") {
                    newInst = G::cmp(GR::mc1, GR::mc2);
                    auto preReg = G::mov(GR::mc1, 0);
                    auto r = G::movg(GR::mc1, 1);
                    newInst.insert(newInst.end(), preReg.begin(), preReg.end());
                    newInst.insert(newInst.end(), r.begin(), r.end());
                } else if (type == L"<") {
                    newInst = G::cmp(GR::mc1, GR::mc2);
                    auto preReg = G::mov(GR::mc1, 0);
                    auto r = G::movl(GR::mc1, 1);
                    newInst.insert(newInst.end(), preReg.begin(), preReg.end());
                    newInst.insert(newInst.end(), r.begin(), r.end());
                } else if (type == L"*") {
                    newInst = G::mul(GR::mc1, GR::mc2);
                } else if (type == L"/") {
                    newInst = G::div(GR::mc1, GR::mc2);
                } else if (type == L"%") {
                    newInst = G::mod(GR::mc1, GR::mc2);
                } else if (type == L"&") {
                    newInst = G::AND(GR::mc1, GR::mc2);
                } else if (type == L"|") {
                    newInst = G::OR(GR::mc1, GR::mc2);
                }
                break;
            }
            case B: {
                if (type == L"?") {
                    newInst = G::cmp(GR::mc1, GR::mc2);
                    auto preReg = G::mov(GR::mc1, 0);
                    auto r = G::move(GR::mc1, 1);
                    newInst.insert(newInst.end(), preReg.begin(), preReg.end());
                    newInst.insert(newInst.end(), r.begin(), r.end());
                }else if (type == L"&") {
                    newInst = G::AND(GR::mc1, GR::mc2);
                } else if (type == L"|") {
                    newInst = G::OR(GR::mc1, GR::mc2);
                }
            }
            case F: {
                if (type == L"+") {
                    newInst = G::addf(GR::mc1, GR::mc2);
                } else if (type == L"-") {
                    newInst = G::subf(GR::mc1, GR::mc2);
                } else if (type == L"?") {
                    newInst = G::cmpf(GR::mc1, GR::mc2);
                    auto preReg = G::mov(GR::mc1, 0);
                    auto r = G::move(GR::mc1, 1);
                    newInst.insert(newInst.end(), preReg.begin(), preReg.end());
                    newInst.insert(newInst.end(), r.begin(), r.end());
                } else if (type == L">") {
                    newInst = G::cmpf(GR::mc1, GR::mc2);
                    auto preReg = G::mov(GR::mc1, 0);
                    auto r = G::movg(GR::mc1, 1);
                    newInst.insert(newInst.end(), preReg.begin(), preReg.end());
                    newInst.insert(newInst.end(), r.begin(), r.end());
                } else if (type == L"<") {
                    newInst = G::cmpf(GR::mc1, GR::mc2);
                    auto preReg = G::mov(GR::mc1, 0);
                    auto r = G::movl(GR::mc1, 1);
                    newInst.insert(newInst.end(), preReg.begin(), preReg.end());
                    newInst.insert(newInst.end(), r.begin(), r.end());
                } else if (type == L"*") {
                    newInst = G::mulf(GR::mc1, GR::mc2);
                } else if (type == L"/") {
                    newInst = G::divf(GR::mc1, GR::mc2);
                }
            }
        }

        ADDVTV(Instructions, newInst);
    }

    void OtoOTranslator::IfFunction(Instruction inst) {
        auto jmpToEndBody = Geneerator::jmp(L"", 123456, GR::eip);
        ADDVTV(Instructions, jmpToEndBody)
        int jumpOffsetId = Instructions->size() - 1 - 2;
        int oldSize = Instructions->size();
        auto saveFlag = G::push(GR::flag);
        ADDVTV(Instructions, saveFlag)
        popSystemCOunt += 1;
        for(int i = 1; i<inst.Parameters.size(); i++){
           ProccessInstruction(inst.Parameters[i]);
        }
        popSystemCOunt -= 1;
        auto loadFlag = G::pop(GR::flag);
        ADDVTV(Instructions, loadFlag)
        jmpToEndBody = Geneerator::jmp(L"", 123456, GR::eip);
        ADDVTV(Instructions, jmpToEndBody)
        int offsetOfExit = Instructions->size() - 1 - 2;
        int exitSize = Instructions->size();
        (*Instructions)[jumpOffsetId] = Instructions->size() - oldSize;
        LoadInstToReg(inst.Parameters[0], GR::mc3);
        auto cm = G::cmp(GR::mc3, 1);
        auto je = G::jme(L"main", jumpOffsetId + 3, GR::NULLREG);
        ADDVTV(Instructions, cm);
        ADDVTV(Instructions, je);
        (*Instructions)[offsetOfExit] = Instructions->size() - exitSize;
    }

    void OtoOTranslator::ElseInstruction(Instruction inst) {
        auto jmpIfT = Geneerator::jme(L"", 123456, GR::eip);
        ADDVTV(Instructions, jmpIfT);
        int jumpOffset = Instructions->size() - 1 - 2;
        int oldSize = Instructions->size();
        auto saveFlag = G::push(GR::flag);
        ADDVTV(Instructions, saveFlag)
        popSystemCOunt++;
        for(int i = 0; i < inst.Parameters.size(); i++){
            ProccessInstruction(inst.Parameters[i]);
        }
        popSystemCOunt--;
        auto loadFlag = G::pop(GR::flag);
        ADDVTV(Instructions, loadFlag)
        (*Instructions)[jumpOffset] = Instructions->size() - oldSize;
    }

    void OtoOTranslator::ElifInstruction(Instruction inst) {
        auto jmpIfT = Geneerator::jme(L"", 123456, GR::eip);
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
        auto jmpToEndBody = Geneerator::jmp(L"", 123456, GR::eip);
        ADDVTV(Instructions, jmpToEndBody)
        int jumpOffsetId = Instructions->size() - 1 - 2;
        int oldSize = Instructions->size();
        auto saveFlag = G::push(GR::flag);
        ADDVTV(Instructions, saveFlag)
        popSystemCOunt++;
        for(int i = 1; i<inst.Parameters.size(); i++){
            ProccessInstruction(inst.Parameters[i]);
        }
        popSystemCOunt--;
        auto loadFlag = G::pop(GR::flag);
        ADDVTV(Instructions, loadFlag)
        (*Instructions)[jumpOffsetId] = Instructions->size() - oldSize;
        LoadInstToReg(inst.Parameters[0], GR::mc3);
        auto cm = G::cmp(GR::mc3, 1);
        auto je = G::jme(L"main", jumpOffsetId + 3, GR::NULLREG);
        ADDVTV(Instructions, cm);
        ADDVTV(Instructions, je);
    }
} // O