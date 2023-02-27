#pragma once
#include <BaseTypes.h>

class Translator
{
private:
	std::string file =
			"def voidputCharchar(x):\n"
			"\tprint(x[0], end=\'\')\n";
    struct RegisteredFunction{
        std::string Oname;
        std::string compiledName;
        std::vector<DataTypes> argumentsDT;
		RegisteredFunction();
		RegisteredFunction(std::string name, std::string OName, std::vector<DataTypes> argsDt);
    };

    std::vector<RegisteredFunction> registeredFunctions = {
			RegisteredFunction("voidputCharchar", "putChar", {
				DataTypes::Character
			})
	};
    std::vector<Operator> registeredOperators;
    std::vector<Structure> structures;
	int level = 0;

private:

    std::string getRegisteredFunctionName(std::string name, std::vector<DataTypes> argDt);

    int getIndexOfElementInStructure(DataTypes structDt, std::string name);

	std::string getType(std::string type);

    std::string generateFunctionCall(std::string name, std::vector<Instruction> argumnets, AdditionalDataType adt);

    std::string getOperatorFunction(std::string name, std::vector<DataTypes> argDt);

	std::string getPrefix();
	std::string getDefaultInitialisator(DataTypes t);
	std::string handleVarCreation(Variable v, AdditionalDataType adt);
	std::string proccessInstruction(Instruction inst, AdditionalDataType adt, bool funcAdd = false);
	std::string proccessFunction(Function f, AdditionalDataType adt);

public:
	void TranslateFile(File f);
	std::string getF();
};

