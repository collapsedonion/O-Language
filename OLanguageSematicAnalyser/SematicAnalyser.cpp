#include "SematicAnalyser.h"

#define MAIN_FLOW_NAME "___MAIN___"
#define SET_VALUE_NAME "SET_VALUE"

#define VAR_CREATION_NAME "var"

#define FUNC_CREATION_NAME "func"

#define RETURN_NAME "return"

#define POINTER_GET_INSTRUCTION_TOKEN "@"
#define POINTER_GET_INSTRUCTION_NAME "GET_POINTER"

#define POINTER_ACCESS_INSTRUCTION_TOKEN "~"
#define POINTER_ACCESS_INSTRUCTION_NAME "GET_POINTER_CONTENT"

#define ARRAY_CREATION_TOKEN "a[]"
#define ARRAY_CREATION_NAME "ARRAY_INIT"

#define ARRAY_ELEMENT_ACCESS_TOKEN "[]a"
#define ARRAY_ELEMENT_ACCESS_NAME "ARRAY_ACCESS_INTEGER"

#define IF_NAME "if"
#define ELSE_NAME "else"
#define ELIF_NAME "else if"

#define FALSE_LITERAL_DEFAULT_NAME "false"
#define FALSE_NAME "FALSE"
//#define FALSE_LITERAL_SECONDARY_ACTIVE 
//#define FALSE_LITERAL_SECONDARY_NAME "False"

#define TRUE_LITERAL_DEFAULT_NAME "true"
#define TRUE_NAME "TRUE"
//#define TRUE_LITERAL_SECONDARY_ACTIVE 
//#define TRUE_LITERAL_SECONDARY_NAME "True"

DataTypes O::SematicAnalyser::containsFunction(std::string name, std::vector<DataTypes> dt)
{
	for (auto elem : functions) {
		if (elem.name == name && elem.getArgumentsDataTypes() == dt) {
			return elem.returnType;
		}
	}

	return DataTypes::Error;
}

bool O::SematicAnalyser::containsVariable(std::string name)
{
	for (auto elem : variables) {
		if (elem.name == name) {
			return true;
		}
	}

	return false;
}

bool O::SematicAnalyser::containsOperator(std::string op, DataTypes left, DataTypes right)
{
	for (auto elem : operators) {
		if (elem.op == op && elem.left == left && elem.right == right) {
			return true;
		}
	}

	return false;
}

DataTypes O::SematicAnalyser::getReturnDataTypeOfOperator(std::string op, DataTypes left, DataTypes right)
{
	for (auto elem : operators) {
		if (elem.op == op && elem.left == left && elem.right == right) {
			return elem.resultType;
		}
	}

	return DataTypes::Error;
}

Instruction O::SematicAnalyser::checkAndGetFunction(Analyser::Token token)
{
	std::vector<DataTypes> dataTypes;
	Instruction retInst;
	retInst.IsFunction = true;

	retInst.name = token.token;

	for (auto elem : token.childToken) {
		auto inst = proccessInstCall(elem);
		dataTypes.push_back(inst.type);
		retInst.Parameters.push_back(inst);
	}
	
	auto f = containsFunction(retInst.name, dataTypes);

	if (f == DataTypes::Error) {
		std::string message = "Unable to find function with name \"" + retInst.name + "\"";
		throw(std::exception(message.c_str()));
	}

	retInst.type = f;

	return retInst;
}

Instruction O::SematicAnalyser::proccessPointerGet(Analyser::Token token)
{
	if (token.childToken.size() != 1) {
		throw(std::exception("Invalid count of arguments at pointer get operator"));
	}
	else {
		Instruction inst = proccessInstCall(token.childToken[0]);
		Analyser::Token typeToken;
		typeToken.token = "~";
		Analyser::Token instTypeToken;
		instTypeToken.token = dataTypeToString(inst.type, adt);
		typeToken.childToken.push_back(instTypeToken);
		Instruction toRet;
		toRet.name = POINTER_GET_INSTRUCTION_NAME;
		toRet.type = getDataType(typeToken);
		toRet.Parameters.push_back(inst);
		return toRet;
	}
}

Instruction O::SematicAnalyser::getVariableAsInstruction(std::string name)
{
	Variable v;

	for (auto elem : variables) {
		if (elem.name == name) {
			v = elem;
		}
	}

	Instruction inst;
	inst.name = v.name;
	inst.type = v.type;
	inst.IsVariable = true;

	return inst;
}

DataTypes O::SematicAnalyser::getDataType(Analyser::Token token)
{
	if (token.token == "~") {
		auto mydt = getDataType(token.childToken[0]);
		std::string strRep = dataTypeToString(mydt, adt);
		bool check = adtContains("~" + strRep, adt);
		if (check) {
			return stringToDataType("~" + strRep, adt);
		}
		else {
			adt.lastId += 1;
			adt.additionalNumber.push_back(adt.lastId);
			adt.additionalName.push_back("~" + strRep);
			return (DataTypes)adt.lastId;
		}
	}
	else {
		return stringToDataType(token.token, adt);
	}
}

Instruction O::SematicAnalyser::proccessPointerAccess(Analyser::Token token)
{
	if (token.childToken.size() != 1) {
		throw(std::exception("Invalid count of arguments at pointer access operator"));
	}

	Instruction inst = proccessInstCall(token.childToken[0]);

	std::string dtStrReprasentation = dataTypeToString(inst.type, adt);
	if (dtStrReprasentation[0] != '~') {
		throw(std::exception("Non pointer type at pointer access operator"));
	}

	Instruction retInst;
	retInst.name = POINTER_ACCESS_INSTRUCTION_NAME;
	retInst.type = stringToDataType(dtStrReprasentation.substr(1, dtStrReprasentation.size()-1), adt);
	retInst.Parameters.push_back(inst);

	return retInst;
}

Instruction O::SematicAnalyser::proccessArrayAccessInstruction(Analyser::Token token)
{
	if (token.childToken.size() != 2) {
		throw(std::exception("Invalid data at array access token"));
	}

	Instruction index = proccessInstCall(token.childToken[1]);
	if (index.type != DataTypes::Integer) {
		throw(std::exception("Array access operator only accepts integer values"));
	}

	Instruction dataSource = proccessInstCall(token.childToken[0]);

	std::string reprasentation = dataTypeToString(dataSource.type, adt);
	if (reprasentation[0] != '~') {
		throw(std::exception("Array access operator can be only applied to pointer type values"));
	}

	reprasentation = reprasentation.substr(1, reprasentation.size() - 1);

	Instruction toRet;
	toRet.type = stringToDataType(reprasentation, adt);
	toRet.name = ARRAY_ELEMENT_ACCESS_NAME;
	toRet.Parameters.push_back(index);
	toRet.Parameters.push_back(dataSource);


	return toRet;
}

Instruction O::SematicAnalyser::proccessArrayCreationInstruction(Analyser::Token token)
{
	if (token.forward) {
		if (token.childToken.size() == 0) {
			throw(std::exception("Creating array with zero elemnts is prohibited"));
		}

		Instruction toRet;
		toRet.name = ARRAY_CREATION_NAME;
		Instruction instSize;
		instSize.name = token.childToken.size();
		instSize.type = DataTypes::Integer;
		toRet.Parameters.push_back(instSize);

		for (int i = 0; i < token.childToken.size(); i++) {
			toRet.Parameters.push_back(proccessInstCall(token.childToken[i]));
			if (i != 0) {
				if (toRet.Parameters[i - 1].type != toRet.Parameters[i].type) {
					throw(std::exception("Array can contain only elements of one type"));
				}
			}
		}
		Analyser::Token typeToken;
		typeToken.token = "~";
		Analyser::Token instTypeToken;
		instTypeToken.token = dataTypeToString(toRet.Parameters[0].type, adt);
		typeToken.childToken.push_back(instTypeToken);
		toRet.type = getDataType(typeToken);
		return toRet;
	}
	else {
		throw(std::exception("Invalid data at array initialisator"));
	}
}

Instruction O::SematicAnalyser::proccessReturnCall(Analyser::Token token)
{
	if (returnType != DataTypes::Void && token.childToken.size() != 1) {
		throw(std::exception("Return function works only with one argument"));
	}

	Instruction toRet = proccessInstCall(token.childToken[0]);
	if (toRet.type != returnType) {
		throw(std::exception("Return type mismatch"));
	}

	Instruction inst;
	inst.type = toRet.type;
	inst.Parameters.push_back(toRet);
	inst.name = token.token;

	return inst;
}

Instruction O::SematicAnalyser::proccessIfInstruction(Analyser::TokenisedFile token)
{
	if (token.name.childToken.size() != 1) {
		throw(std::exception("Excepted exactly one argument at if structure"));
	}

	Instruction inst = proccessInstCall(token.name.childToken[0]);

	if (inst.type != DataTypes::Boolean) {
		throw(std::exception("If structure can work only with boolean type arguments"));
	}

	Instruction toRet;
	toRet.name = token.name.token;
	toRet.type = DataTypes::Void;

	toRet.Parameters.push_back(inst);

	for (auto elem : token.subToken) {
		toRet.Parameters.push_back(ProcessToken(elem, false));
	}

	return toRet;
}

Instruction O::SematicAnalyser::proccessElseIfInstruction(Analyser::TokenisedFile token)
{
	if (instructions.size() == 0 || ((*(instructions.end() - 1)).name != IF_NAME && (*(instructions.end() - 1)).name != ELIF_NAME)) {
		throw(std::exception("Excepted if or else if structure before else if structure"));
	}

	if (token.name.childToken.size() != 1) {
		throw(std::exception("Excepted exactly one argument at else if structure"));
	}

	Instruction inst = proccessInstCall(token.name.childToken[0]);

	if (inst.type != DataTypes::Boolean) {
		throw(std::exception("Else if structure can work only with boolean type arguments"));
	}

	Instruction toRet;
	toRet.name = token.name.token;
	toRet.type = DataTypes::Void;

	toRet.Parameters.push_back(inst);

	for (auto elem : token.subToken) {
		toRet.Parameters.push_back(ProcessToken(elem, false));
	}

	return toRet;
}

Instruction O::SematicAnalyser::proccessElseInstruction(Analyser::TokenisedFile token)
{
	if (instructions.size() == 0 || ((*(instructions.end() - 1)).name != IF_NAME && (*(instructions.end() - 1)).name != ELIF_NAME)) {
		throw(std::exception("Excepted if or else if structure before else structure"));
	}

	if (token.name.childToken.size() != 0) {
		throw(std::exception("Excepted exactly zero argument at else structure"));
	}

	Instruction toRet;
	toRet.name = token.name.token;
	toRet.type = DataTypes::Void;

	for (auto elem : token.subToken) {
		toRet.Parameters.push_back(ProcessToken(elem, false));
	}

	return toRet;
}

Instruction O::SematicAnalyser::proccessFuncInstrucion(Analyser::TokenisedFile token)
{
	
	Instruction toRet;

	if (token.name.childToken.size() != 2) {
		throw(std::exception("Not valid function descriptor"));
	}


	if (token.name.childToken[0].type == Analyser::Type::ServiceName) {
		auto type = getDataType(token.name.childToken[0]);

		Function func;

		if (type == DataTypes::Error) {
			std::string exceptionMessage = "Unexpected return type at function definition \"" + token.name.childToken[0].token + "\"";
			throw(std::exception(exceptionMessage.c_str()));
		}

		func.returnType = type;

		if (token.name.childToken[1].type == Analyser::Type::ServiceName && token.name.childToken[1].token == "name" && token.name.childToken[1].childToken.size() >= 1) {
			auto nameField = token.name.childToken[1];
			func.name = nameField.childToken[0].token;
			std::vector<DataTypes> dt;
			for (int i = 1; i < nameField.childToken.size(); i++) {
				auto funcArgName = nameField.childToken[i];
				if (funcArgName.childToken.size() != 1 || funcArgName.childToken[0].type != Analyser::Type::ServiceName) {
					throw(std::exception("Invalid function argument type"));
				}
				auto funcArgType = getDataType(funcArgName.childToken[0]);
				if (funcArgType == DataTypes::Error) {
					std::string message = "Unrecognised data type at function argument \"" + funcArgName.childToken[0].token + "\"";
					throw(std::exception(message.c_str()));
				}
				Variable v;
				v.name = funcArgName.token;
				v.type = funcArgType;
				
				if (contains(func.arguments, v)) {
					throw(std::exception("Creating function with more than one same variable is prohibited"));
				}
				dt.push_back(v.type);
				func.arguments.push_back(v);
			}
			if (token.subToken.size() != 0) {
				SematicAnalyser subSematicAnalyser;
				subSematicAnalyser.adt = adt;
				subSematicAnalyser.returnType = func.returnType;
				subSematicAnalyser.variables = std::vector<Variable>(variables);
				subSematicAnalyser.functions = std::vector<Function>(functions);
				for (auto elem : func.arguments) {
					subSematicAnalyser.variables.push_back(elem);
				}
				for (auto elem : token.subToken) {
					subSematicAnalyser.ProcessToken(elem);
				}

				if (func.returnType != DataTypes::Void && subSematicAnalyser.returnCalls == 0) {
					std::string message = "Reqire at leas one return call at \"" + func.name + "\" function";
				}

				func.variables = subSematicAnalyser.variablesCreatedAtThatField;
				func.body = subSematicAnalyser.instructions;
			}
			if (containsFunction(func.name, dt) != DataTypes::Error) {
				std::string message = "Function redefinition \"" + func.name + "\"";
				throw(std::exception(message.c_str()));
			}
			functions.push_back(func);
		}
		else {
			throw(std::exception("Invalid function name field"));
		}
		toRet.name = func.name;
		toRet.type = func.returnType;
	}
	else {
		throw(std::exception("Unexpected field at function definition return type"));
	}

	return toRet;
}

Instruction O::SematicAnalyser::proccessVarInstruction(Analyser::Token token)
{
	Instruction toRet;
	toRet.IsVariable = true;
	
	if (token.childToken.size() == 2) {
		toRet.name = token.childToken[1].token;
		if (containsVariable(toRet.name)) {
			std::string message = ("Variable with name " + toRet.name + " alredy exists");
			throw(std::exception(message.c_str()));
		}
		std::string invalidDatatype = "Invalid data type name " + token.childToken[0].token;
		auto exp = std::exception(invalidDatatype.c_str());

		if (token.childToken[0].type != Analyser::Type::ServiceName) {
			throw exp;
		}

		auto getType = getDataType(token.childToken[0]);

		if (getType == DataTypes::Error) {
			throw exp;
		}

		toRet.type = getType;

		Variable v;
		v.name = toRet.name;
		v.type = toRet.type;
		variables.push_back(v);
		variablesCreatedAtThatField.push_back(v);
	}
	else {
		throw(std::exception("Unexpected data at \"var\" instruction"));
	}

	return toRet;
}

Instruction O::SematicAnalyser::proccessSetInstruction(Analyser::Token token)
{
	Instruction toRet;

	toRet.type = DataTypes::ServiceInstruction;
	toRet.name = SET_VALUE_NAME;

	if (token.childToken.size() == 2) {
		auto destination = token.childToken[0];
		Instruction destInstr;
		if (destination.type == Analyser::Type::ServiceName && destination.token == VAR_CREATION_NAME) {
			destInstr = proccessVarInstruction(destination);
		}
		else if(destination.type == Analyser::Type::Name) {
			if (destination.forward) {
				destInstr = checkAndGetFunction(destination);
			}
			else {
				if (containsVariable(destination.token)) {
					destInstr = getVariableAsInstruction(destination.token);
				}
				else {
					throw(std::exception("Unexpected destination at \"=\" operator"));
				}
			}
		}
		else if (destination.token == POINTER_ACCESS_INSTRUCTION_TOKEN) {
			destInstr = proccessPointerAccess(destination);
		}
		else if (destination.token == ARRAY_ELEMENT_ACCESS_TOKEN) {
			destInstr = proccessArrayAccessInstruction(destination);
		}
		else {
			throw(std::exception("Unexpected destination at \"=\" operator"));
		}

		toRet.Parameters.push_back(destInstr);
		toRet.Parameters.push_back(proccessInstCall(token.childToken[1]));
	}
	else {
		throw(std::exception("Unexpected data at \"=\" operator"));
	}

	if (toRet.Parameters[0].type != toRet.Parameters[1].type) {
		throw(std::exception("Type mismatch at set operator"));
	}

	return toRet;
}

Instruction O::SematicAnalyser::ProcessToken(Analyser::TokenisedFile token, bool add)
{
	Instruction inst;

	if (token.name.type == Analyser::Type::ServiceName) {
		if (token.name.token == "=" && token.name.twoSided) {
			inst = proccessSetInstruction(token.name);
			if (add) {
				instructions.push_back(inst);
			}
			return inst;
		}
		else if (token.name.token == VAR_CREATION_NAME) {
			proccessVarInstruction(token.name);
		}
		else if (token.name.token == FUNC_CREATION_NAME) {
			proccessFuncInstrucion(token);
		}
	}
	else {
		if (token.name.token == IF_NAME) {
			
			inst = proccessIfInstruction(token);

			if (add) {
				instructions.push_back(inst);
			}
			return inst;
		} else if (token.name.token == ELIF_NAME) {
			inst = proccessElseIfInstruction(token);

			if (add) {
				instructions.push_back(inst);
			}
			return inst;
		}
		else if (token.name.token == ELSE_NAME) {
			inst = proccessElseInstruction(token);

			if (add) {
				instructions.push_back(inst);
			}
			return inst;
		}
		else if (token.name.token == RETURN_NAME)
		{
			inst = proccessReturnCall(token.name);
			if (add) {
				instructions.push_back(inst);
			}
			return inst;
		}
		else {
			inst = proccessInstCall(token.name);
			if (add) {
				instructions.push_back(inst);
			}
			return inst;
		}
	}

	return inst;
}

void O::SematicAnalyser::ProccessTokenisedFile(Analyser::TokenisedFile tf)
{
	if (tf.name.token == MAIN_FLOW_NAME) {
		for (auto elem : tf.subToken) {
			ProcessToken(elem);
		}
		return;
	}

	throw(std::exception("Unexpected token in root location"));
}

File O::SematicAnalyser::getFileRepresantation()
{
	File f;
	f.functions = std::vector<Function>(functions);
	f.instructions = std::vector<Instruction>(instructions);
	f.variables = std::vector<Variable>(variables);
	f.adtTable = adt;

	return f;
}

Instruction O::SematicAnalyser::proccessInstCall(Analyser::Token token)
{
	Instruction res;

	if (token.type == Analyser::Type::MathematicalOperator && token.twoSided == true) {
		res.name = token.token;
		res.ArithmeticProccess = true;
		if (token.childToken.size() != 2) {
			std::string message = "Unexpected count of operands at " + res.name + " operator";
			throw(std::exception(message.c_str()));
		}
		else {
			res.Parameters.push_back(proccessInstCall(token.childToken[0]));
			res.Parameters.push_back(proccessInstCall(token.childToken[1]));
			if (!containsOperator(res.name, res.Parameters[0].type, res.Parameters[1].type)) {
				std::string message = "Unexpected type of operands (" + dataTypeToString(res.Parameters[0].type) + ", " + dataTypeToString(res.Parameters[1].type) + ") at " + res.name + " operator";
				throw(std::exception(message.c_str()));
			}

			res.type = getReturnDataTypeOfOperator(res.name, res.Parameters[0].type, res.Parameters[1].type);
		}
	}
	else if (token.type == Analyser::Type::MathematicalOperator) {
		if (token.token == POINTER_GET_INSTRUCTION_TOKEN) {
			return proccessPointerGet(token);
		}
		else if(token.token == POINTER_ACCESS_INSTRUCTION_TOKEN) {
			return proccessPointerAccess(token);
		}
		else if (token.token == ARRAY_ELEMENT_ACCESS_TOKEN) {
			return proccessArrayAccessInstruction(token);
		}
	}
	else if (token.type == Analyser::Type::Number) {
		res.name = token.token;
		res.type = getTypeOfNumber(token.token);
	}
	else if (token.type == Analyser::Type::ServiceName) {
		// обработка логических имён
		if (true)
		{
			// обработка ключевых слов для лжи
#ifdef FALSE_LITERAL_SECONDARY_ACTIVE 
			if (token.token == FALSE_LITERAL_DEFAULT_NAME || token.token == FALSE_LITERAL_SECONDARY_NAME) {
#endif // FALSE_LITERAL_SECONDARY_ACTIVE 
#ifndef FALSE_LITERAL_SECONDARY_ACTIVE
				if (token.token == FALSE_LITERAL_DEFAULT_NAME) {
#endif // !FALSE_LITERAL_SECONDARY_ACTIVE
					res.name = FALSE_NAME;
					res.type = DataTypes::Boolean;
				}

				// обработка ключевых слов для истины
#ifdef TRUE_LITERAL_SECONDARY_ACTIVE 
				if (token.token == TRUE_LITERAL_DEFAULT_NAME || token.token == TRUE_LITERAL_SECONDARY_NAME) {
#endif // TRUE_LITERAL_SECONDARY_ACTIVE 
#ifndef TRUE_LITERAL_SECONDARY_ACTIVE
					if (token.token == TRUE_LITERAL_DEFAULT_NAME) {
#endif // !TRUE_LITERAL_SECONDARY_ACTIVE
						res.name = TRUE_NAME;
						res.type = DataTypes::Boolean;
					}
		}
		// обработка создания массива
		if (token.token == ARRAY_CREATION_TOKEN) {
			return proccessArrayCreationInstruction(token);
		}
	}
			else if (token.type == Analyser::Type::Name) {
				if (token.forward) {
					res = checkAndGetFunction(token);
				}
				else {
					if (containsVariable(token.token)) {
						return getVariableAsInstruction(token.token);
					}
					else {
						std::string message = "Undefined variable with name \"" + token.token + "\"";
						throw(std::exception(message.c_str()));
					}
				}
			}
			return res;
}
