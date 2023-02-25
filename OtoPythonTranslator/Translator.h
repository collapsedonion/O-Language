#pragma once
#include <BaseTypes.h>

class Translator
{
private:
	std::string file;
    struct RegisteredFunction{
        std::string Oname;
        std::string compiledName;
        std::vector<DataTypes> argumentsDT;
    };

    std::vector<RegisteredFunction> registeredFunctions;
    std::vector<Operator> registeredOperators;
	int level = 0;

private:

    std::string getRegisteredFunctionName(std::string name, std::vector<DataTypes> argDt);

    std::string generateFunctionCall(std::string name, std::vector<Instruction> argumnets);

    std::string getOperatorFunction(std::string name, std::vector<DataTypes> argDt);

	std::string getPrefix();
	std::string getDefaultInitialisator(DataTypes t);
	std::string handleVarCreation(Variable v);
	std::string proccessInstruction(Instruction inst, bool funcAdd = false);
	std::string proccessFunction(Function f, AdditionalDataType adt);

public:
	void TranslateFile(File f);
	std::string getF();
};

