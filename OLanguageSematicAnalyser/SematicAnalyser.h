#pragma once
#include <Analyser.h>
#include <exception>
#include <BaseTypes.h>

namespace O {
	class SematicAnalyser
	{
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

	private:
		// returns ERROR if not found
		DataTypes containsFunction(std::wstring name, std::vector<DataTypes> dt);
		bool containsVariable(std::wstring name);
		bool containsOperator(std::wstring op, DataTypes left, DataTypes right);
        bool dataTypeIsStructure(DataTypes dt);
        bool isExternFunction(std::wstring name, std::vector<DataTypes> dt);
        std::pair<bool, Structure> containsStructureByDataType(DataTypes dt);
        std::vector<Analyser::Token> getComma(Analyser::Token token);
		DataTypes getReturnDataTypeOfOperator(std::wstring op, DataTypes left, DataTypes right);

		Instruction checkAndGetFunction(Analyser::Token token);

		Instruction proccessPointerGet(Analyser::Token token);

		Instruction getVariableAsInstruction(std::wstring name);

		Instruction proccessInstCall(Analyser::Token token);

		DataTypes getDataType(Analyser::Token token);
	
		Instruction proccessPointerAccess(Analyser::Token token);

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
		Instruction proccessVarInstruction(Analyser::Token token, bool isExtern = false);

		Instruction ProcessToken(Analyser::TokenisedFile token, bool add = true);

	public:
		void ProccessTokenisedFile(Analyser::TokenisedFile tf);
		File getFileRepresantation();
	};
}

