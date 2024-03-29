#pragma once
#include <Analyser.h>
#include <exception>
#include <BaseTypes.h>
#include <map>

namespace O {
	class SematicAnalyser
	{
    private:
        struct Universal{
            std::u32string name;
            Analyser::TokenisedFile content;
            bool isStruct;
            std::vector<std::u32string> type_places;
        };
        
	private:
		int returnCalls = 0;
		DataTypes returnType = DataTypes::Void;

		AdditionalDataType adt;

		std::vector<Variable> variables;
		std::vector<Variable> variablesCreatedAtThatField;
		std::vector<Instruction> instructions;
		std::vector<Function> functions;
        std::vector<Function> functionsCreatedAtThatField;
        std::vector<Function> exportedFunctions;
        std::vector<Structure> definedStructures;
		std::vector<Operator> operators;
        std::map<std::u32string, std::vector<Analyser::TokenisedFile>> templates;
        std::vector<std::pair<std::u32string, std::vector<Analyser::TokenisedFile>>> definedLabels;
        std::map<std::u32string, Instruction> enumerations;
        std::map<DataTypes, std::vector<DataTypes>> auto_casts;
        std::map<std::u32string, Universal> universals;
        std::vector<std::u32string> universal;
        bool use_universal = false;
        
	private:
		// returns ERROR if not found
		DataTypes containsFunction(std::u32string name, std::vector<DataTypes> dt);
        std::pair<DataTypes, std::vector<DataTypes>> getFunctionPointerType(std::u32string str);
        int getSize(DataTypes dt);
        std::pair<DataTypes, std::vector<DataTypes>> containsAutoCastebleFunction(std::u32string name, std::vector<DataTypes> dt);
        std::pair<bool, std::vector<Analyser::TokenisedFile>> containsLabel(std::u32string label);
        void add_new_auto_cast(DataTypes from, DataTypes to);
		bool containsVariable(std::u32string name);
		bool containsOperator(std::u32string op, DataTypes left, DataTypes right);
        bool dataTypeIsStructure(DataTypes dt);
        bool isExternFunction(std::u32string name, std::vector<DataTypes> dt);
        bool can_be_auto_casted(DataTypes from, DataTypes to, std::vector<DataTypes> alredy_checked);
        std::pair<bool, Structure> containsStructureByDataType(DataTypes dt);
        std::vector<Analyser::Token> getComma(Analyser::Token token);
		DataTypes getReturnDataTypeOfOperator(std::u32string op, DataTypes left, DataTypes right);
        
        void addUniversal(Analyser::TokenisedFile universal_content, std::u32string universal_name, bool isStruct);

		Instruction checkAndGetFunction(Analyser::Token token);
        
        Instruction processEnumeration(Analyser::TokenisedFile tokenFile);
        
        Instruction processAddAutoCast(Analyser::Token token);

		Instruction proccessPointerGet(Analyser::Token token);

        Instruction processTemplate(Analyser::TokenisedFile tokenisedFile);

		Instruction getVariableAsInstruction(std::u32string name);

		Instruction proccessInstCall(Analyser::Token token);

		DataTypes getDataType(Analyser::Token token);
	
		Instruction proccessPointerAccess(Analyser::Token token);

        Instruction processElementCall(Analyser::Token token);

        Instruction processFetchTemplate(Analyser::TokenisedFile token);
        
        Instruction getSizeOf(Analyser::Token token);

		Instruction proccessString(Analyser::Token token);
        Instruction proccessStructureCreation(Analyser::TokenisedFile token);
		Instruction proccessWhileCycleInstruction(Analyser::TokenisedFile token);
		Instruction proccessArrayAccessInstruction(Analyser::Token token);
		Instruction proccessArrayCreationInstruction(Analyser::Token token);
		Instruction proccessReturnCall(Analyser::Token token);
		Instruction proccessIfInstruction(Analyser::TokenisedFile token);
		Instruction proccessElseIfInstruction(Analyser::TokenisedFile token);
		Instruction proccessElseInstruction(Analyser::TokenisedFile token);
		Instruction proccessFuncInstrucion(Analyser::TokenisedFile token, bool isExtern = false);
		Instruction proccessVarInstruction(Analyser::Token token, bool isExtern = false, bool isGlobal = false);

		Instruction ProcessToken(Analyser::TokenisedFile token, bool add = true);

        void merge(const SematicAnalyser* const sa);

	public:
        SematicAnalyser() = default;
        SematicAnalyser(const SematicAnalyser* const origin);

		void ProccessTokenisedFile(Analyser::TokenisedFile tf);
		File getFileRepresantation();
	};
}

