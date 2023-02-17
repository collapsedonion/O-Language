#pragma once
#include <BaseTypes.h>

class Translator
{
private:
	std::string file;
	int level = 0;

private:

	std::string getPrefix();
	std::string getDefaultInitialisator(DataTypes t);
	std::string handleVarCreation(Variable v);
	std::string proccessInstruction(Instruction inst, bool funcAdd = false);
	std::string proccessFunction(Function f);

public:
	void TranslateFile(File f);
	std::string getF();
};

