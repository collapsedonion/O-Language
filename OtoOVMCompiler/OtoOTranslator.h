//
// Created by Роман  Тимофеев on 01.03.2023.
//

#ifndef OTOOVMCOMPILER_OTOOTRANSLATOR_H
#define OTOOVMCOMPILER_OTOOTRANSLATOR_H

#include <BaseTypes.h>
#include <Geneerator.h>
#include <fstream>
#include <map>

namespace O {

    class OtoOTranslator {
    private:
        struct VariableStored{
            std::u32string name;
            std::u32string sector;
            int fromEbpOffset = 0;
	        bool useEbp;
        };

        struct FunctionStored{
            std::u32string name;
            std::u32string sector;
            std::vector<DataTypes> parameters;
            int stackSize = 0;
            int fromZeroOffset = 0;
        };

        struct OVMDebug{
            int line = 0;
            std::u32string file;
            int esp_min = 0;
            int esp_max = 0;
            std::u32string sector;
        };

        struct VariableSymbol{
            std::u32string name;
            Geneerator::Registers offset_registor;
            int offset;
            std::u32string sector;
        };

        struct ComponentSymbol{
            std::u32string sector;
            std::vector<VariableSymbol> vs;
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
        std::u32string sectorName = U"main";
        int addOffset = 0;
        int localSize = 0;
        int popSystemCOunt = 0;
        int bodyStart = 0;

        std::vector<int>* Instructions;
        std::vector<int> mainFlow;
        std::vector<FunctionStored> storedFunctions;
        std::vector<ComponentSymbol> components;
        std::map<std::u32string, std::vector<int>> addSectors;
        std::vector<OVMDebug> debug_info;

    private:
        void LoadVariables(std::vector<Variable> variables, bool globals, bool add = false);
        void LoadFunctions(std::vector<Function> functions);
        void ProccessInstruction(Instruction inst);
        void MovVariableToRegister(std::u32string name, Geneerator::Registers dest);
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
        VariableStored getVar(std::u32string name);
        FunctionStored getFun(std::u32string name, std::vector<DataTypes> dt);
        int GetValueToInt(Instruction inst);
        bool isStdLogic(Instruction instOP1, Instruction instOP2);
        StdLogicType getLogicType(Instruction instOP1, Instruction instOP2);
        void ProccessStdLogic(Instruction instOP1, Instruction instOP2, std::u32string type);

        void MathematicalProccess(Instruction inst);

        int GetDataSize(DataTypes dt);

    public:
        void Build(File f);
        void WriteDebugSymbols(std::u32string path, std::vector<std::pair<std::u32string, int>> breakPoints);
        void WriteResulToFile(std::u32string filepath);
    };

} // O

#endif //OTOOVMCOMPILER_OTOOTRANSLATOR_H
