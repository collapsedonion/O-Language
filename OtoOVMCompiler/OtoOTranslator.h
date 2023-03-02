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
            std::string name;
            std::string sector;
            int fromEbpOffset = 0;
        };

        struct FunctionStored{
            std::string name;
            std::string sector;
            std::vector<DataTypes> parameters;
            int stackSize = 0;
            int fromZeroOffset = 0;
        };

        std::vector<VariableStored> variables;
        std::vector<VariableStored> additionalVariables;
        int addOffset = 0;
        int bodyStart = 0;

        std::vector<int> Instructions;
        std::vector<FunctionStored> storedFunctions;

    private:
        void LoadVariables(std::vector<Variable> variables, bool add = false);
        void LoadFunctions(std::vector<Function> functions);
        void ProccessInstruction(Instruction inst);
        void MovVariableToRegister(std::string name, Geneerator::Registers dest);
        void MovConstantToRegister(int constant, Geneerator::Registers dest);
        void SetInstruction(Instruction inst);
        void CallFunction(Instruction inst);
        void ReturnFunction(Instruction inst);
        VariableStored getVar(std::string name);
        FunctionStored getFun(std::string name);
        int GetValueToInt(Instruction inst);

        int GetDataSize(DataTypes dt);

    public:
        void Build(File f);
        void WriteResulToFile(std::string filepath);
    };

} // O

#endif //OTOOVMCOMPILER_OTOOTRANSLATOR_H
