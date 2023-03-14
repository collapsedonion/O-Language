//
// Created by Роман  Тимофеев on 01.03.2023.
//

#ifndef OTOOVMCOMPILER_OTOOTRANSLATOR_H
#define OTOOVMCOMPILER_OTOOTRANSLATOR_H

#include <BaseTypes.h>
#include <Geneerator.h>
#include <fstream>

namespace O {

    class OtoOTranslator {
    private:
        struct VariableStored{
            std::wstring name;
            std::wstring sector;
            int fromEbpOffset = 0;
        };

        struct FunctionStored{
            std::wstring name;
            std::wstring sector;
            std::vector<DataTypes> parameters;
            int stackSize = 0;
            int fromZeroOffset = 0;
        };

        enum StdLogicType {
            I,
            B,
            F
        };

        std::vector<VariableStored> variables;
        std::vector<VariableStored> additionalVariables;
        std::vector<Structure> structs;
        AdditionalDataType adtTable;
        int addOffset = 0;
        int localSize = 0;
        int popSystemCOunt = 0;
        int bodyStart = 0;

        std::vector<int>* Instructions;
        std::vector<int> mainFlow;
        std::vector<FunctionStored> storedFunctions;

    private:
        void LoadVariables(std::vector<Variable> variables, bool add = false);
        void LoadFunctions(std::vector<Function> functions);
        void ProccessInstruction(Instruction inst);
        void MovVariableToRegister(std::wstring name, Geneerator::Registers dest);
        void MovConstantToRegister(int constant, Geneerator::Registers dest);
        void SetInstruction(Instruction inst);
        void CallFunction(Instruction inst);
        void ReturnFunction(Instruction inst);
        void LoadInstToReg(Instruction inst, Geneerator::Registers reg);
        void FreeInstruction(Instruction inst);
        void IfFunction(Instruction inst);
        void ElseInstruction(Instruction inst);
        void ElifInstruction(Instruction inst);
        void WhileInstruction(Instruction inst);
        VariableStored getVar(std::wstring name);
        FunctionStored getFun(std::wstring name, std::vector<DataTypes> dt);
        int GetValueToInt(Instruction inst);
        bool isStdLogic(Instruction instOP1, Instruction instOP2);
        StdLogicType getLogicType(Instruction instOP1, Instruction instOP2);
        void ProccessStdLogic(Instruction instOP1, Instruction instOP2, std::wstring type);

        void MathematicalProccess(Instruction inst);

        int GetDataSize(DataTypes dt);

    public:
        void Build(File f);
        void WriteResulToFile(std::wstring filepath);
    };

} // O

#endif //OTOOVMCOMPILER_OTOOTRANSLATOR_H
