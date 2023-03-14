#include "SematicAnalyser.h"

#define MAIN_FLOW_NAME L"___MAIN___"
#define SET_VALUE_NAME L"SET_VALUE"

#define VAR_CREATION_NAME L"var"

#define FUNC_CREATION_NAME L"func"

#define RETURN_NAME L"return"

#define POINTER_GET_INSTRUCTION_TOKEN L"@"
#define POINTER_GET_INSTRUCTION_NAME L"GET_POINTER"

#define FREE_INSTRUCTION_TOKEN L"free"
#define FREE_INSTRUCTION_NAME L"_FREE"
#define MALLOC_INSTRUCTION_TOKEN L"malloc"
#define MALLOC_INSTRUCTION_NAME L"_MALLOC"

#define POINTER_ACCESS_INSTRUCTION_TOKEN L"~"
#define POINTER_ACCESS_INSTRUCTION_NAME L"GET_POINTER_CONTENT"

#define EXTERN_DEFINITION_TOKEN L"extern"

#define STRUCTURE_DEFINITION_TOKEN L"structure"
#define UNIT_DEFINITION_TOKEN L"__init__"

#define WHILE_CYCLE_TOKEN L"while"
#define WHILE_CYCLE_NAME L"WHILE_CYCLE"

#define ARRAY_CREATION_TOKEN L"a[]"
#define ARRAY_CREATION_NAME L"ARRAY_INIT"

#define ARRAY_ELEMENT_ACCESS_TOKEN L"[]a"
#define ARRAY_ELEMENT_ACCESS_NAME L"ARRAY_ACCESS_INTEGER"
#define STRUCTURE_ELEMENT_ACCESS_NAME L"STRUCTURE_ACCESS"

#define IF_NAME L"if"
#define ELSE_NAME L"else"
#define ELIF_NAME L"else if"

#define FALSE_LITERAL_DEFAULT_NAME L"false"
#define FALSE_NAME L"FALSE"
//#define FALSE_LITERAL_SECONDARY_ACTIVE 
//#define FALSE_LITERAL_SECONDARY_NAME L"False"

#define TRUE_LITERAL_DEFAULT_NAME L"true"
#define TRUE_NAME L"TRUE"
//#define TRUE_LITERAL_SECONDARY_ACTIVE 
//#define TRUE_LITERAL_SECONDARY_NAME L"True"

DataTypes O::SematicAnalyser::containsFunction(std::wstring name, std::vector<DataTypes> dt)
{
	for (auto elem : functions) {
        bool test = elem.getArgumentsDataTypes() == dt;
		if (elem.name == name && elem.getArgumentsDataTypes() == dt) {
			return elem.returnType;
		}
	}

	return DataTypes::Error;
}

bool O::SematicAnalyser::containsVariable(std::wstring name)
{
	for (auto elem : variables) {
		if (elem.name == name) {
			return true;
		}
	}

	return false;
}

bool O::SematicAnalyser::containsOperator(std::wstring op, DataTypes left, DataTypes right)
{
	for (auto elem : operators) {
		if (elem.op == op && elem.left == left && elem.right == right) {
			return true;
		}
	}

	return false;
}

DataTypes O::SematicAnalyser::getReturnDataTypeOfOperator(std::wstring op, DataTypes left, DataTypes right)
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

        auto check = token.token;

        Analyser::Token checkToken;
        Analyser::Token *lastCreated = &checkToken;

        while(*(check.end() - 1) == '~'){
            lastCreated->token = '~';
            lastCreated->childToken.emplace_back(Analyser::Token());
            lastCreated = &lastCreated->childToken[0];
            check = check.substr(0, check.size() - 1);
        }

        lastCreated->token = check;

        auto dt = getDataType(checkToken);

        if(dt != DataTypes::Error && retInst.Parameters.size() == 1){
            retInst.Parameters[0].type = dt;
            return retInst.Parameters[0];
        }

		std::wstring message = L"Unable to find function with name \"" + retInst.name + L"\"";

		throw(std::exception());
	}

	retInst.type = f;

	return retInst;
}

Instruction O::SematicAnalyser::proccessPointerGet(Analyser::Token token)
{
	if (token.childToken.size() != 1) {
		throw(std::exception());
	}
	else {
		Instruction inst = proccessInstCall(token.childToken[0]);
		Analyser::Token typeToken;
		typeToken.token = L"~";
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

Instruction O::SematicAnalyser::getVariableAsInstruction(std::wstring name)
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
	if (token.token == L"~") {
		auto mydt = getDataType(token.childToken[0]);
		std::wstring strRep = dataTypeToString(mydt, adt);
		bool check = adtContains(L"~" + strRep, adt);
		if (check) {
			return stringToDataType(L"~" + strRep, adt);
		}
		else {
			adt.lastId += 1;
			adt.additionalNumber.push_back(adt.lastId);
			adt.additionalName.push_back(L"~" + strRep);
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
		throw(std::exception());
	}

	Instruction inst = proccessInstCall(token.childToken[0]);

	std::wstring dtStrReprasentation = dataTypeToString(inst.type, adt);
	if (dtStrReprasentation[0] != '~') {
		throw(std::exception());
	}

	Instruction retInst;
	retInst.name = POINTER_ACCESS_INSTRUCTION_NAME;
	retInst.type = stringToDataType(dtStrReprasentation.substr(1, dtStrReprasentation.size()-1), adt);
	retInst.Parameters.push_back(inst);

	return retInst;
}

Instruction O::SematicAnalyser::proccessString(Analyser::Token token)
{
	Analyser::Token t;
	t.token = ARRAY_CREATION_TOKEN;
	t.forward = true;
	for (int i = 1; i < token.token.size() - 1; i++) {
		Analyser::Token newC;
		newC.type = Analyser::Type::Char;
		if (token.token[i] == '\\'){
			newC.token = L"\\";
            newC.token  += token.token[i + 1];
			i++;
		}
		else {
			newC.token = token.token[i];
		}
		t.childToken.push_back(newC);
	}
	Analyser::Token newC;
	newC.type = Analyser::Type::Char;
	newC.token= L"\\0";
	t.childToken.push_back(newC);
	return proccessArrayCreationInstruction(t);
}

Instruction O::SematicAnalyser::proccessWhileCycleInstruction(Analyser::TokenisedFile token)
{
	if (token.name.childToken.size() != 1) {
		throw(std::exception());
	}

	Instruction inst = proccessInstCall(token.name.childToken[0]);

	if (inst.type != DataTypes::Boolean) {
		throw(std::exception());
	}

	Instruction toRet;
	toRet.name = WHILE_CYCLE_NAME;
	toRet.type = DataTypes::Void;
	toRet.Parameters.push_back(inst);

	if(token.subToken.size() != 0) {
		SematicAnalyser sa;
		sa.adt = adt;
		sa.operators = std::vector<Operator>(operators);
		sa.functions = std::vector<Function>(functions);
		sa.variables = std::vector<Variable>(variables);
        sa.returnType = returnType;
        sa.definedStructures = std::vector<Structure>(definedStructures);

		for(auto elem : token.subToken){
			sa.ProcessToken(elem);
		}

        variablesCreatedAtThatField.insert(variablesCreatedAtThatField.end(), sa.variablesCreatedAtThatField.begin(), sa.variablesCreatedAtThatField.end());

		for (int i = 0; i < sa.instructions.size(); i++) {
			toRet.Parameters.push_back(sa.instructions[i]);
		}
	}

	return toRet;
}

Instruction O::SematicAnalyser::proccessArrayAccessInstruction(Analyser::Token token)
{
	if (token.childToken.size() != 2) {
		throw(std::exception());
	}

    Instruction dataSource = proccessInstCall(token.childToken[0]);

    auto structureTest = containsStructureByDataType(dataSource.type);

    if(structureTest.first){
        Variable destInfo;
        for(auto v : structureTest.second.variables){
            if(v.name == token.childToken[1].token){
                destInfo = v;
                break;
            }
        }
        Instruction toRet;
        toRet.type = destInfo.type;
        toRet.name = STRUCTURE_ELEMENT_ACCESS_NAME;
        Instruction ind;
        ind.name = destInfo.name;
        ind.type = DataTypes::Void;
        toRet.Parameters.push_back(ind);
        toRet.Parameters.push_back(dataSource);
        return toRet;
    }

	Instruction index = proccessInstCall(token.childToken[1]);

	std::wstring reprasentation = dataTypeToString(dataSource.type, adt);
	if (reprasentation[0] != '~') {
		throw(std::exception());
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
			throw(std::exception());
		}

		Instruction toRet;
		toRet.name = ARRAY_CREATION_NAME;
		Instruction instSize;
		instSize.name = token.childToken.size();
		instSize.type = DataTypes::Integer;
		toRet.Parameters.push_back(instSize);

		for (int i = 0; i < token.childToken.size(); i++) {
			toRet.Parameters.push_back(proccessInstCall(token.childToken[i]));
			if (i != 0 && i != 1) {
				if (toRet.Parameters[i - 1].type != toRet.Parameters[i].type) {
					throw(std::exception());
				}
			}
		}
		Analyser::Token typeToken;
		typeToken.token = L"~";
		Analyser::Token instTypeToken;
		instTypeToken.token = dataTypeToString(toRet.Parameters[1].type, adt);
		typeToken.childToken.push_back(instTypeToken);
		toRet.type = getDataType(typeToken);
		return toRet;
	}
	else {
		throw(std::exception());
	}
}

Instruction O::SematicAnalyser::proccessReturnCall(Analyser::Token token)
{
	if (returnType != DataTypes::Void && token.childToken.size() != 1) {
		throw(std::exception());
	}

    if(token.childToken.size() != 0) {
        Instruction toRet = proccessInstCall(token.childToken[0]);
        if (toRet.type != returnType) {
            throw(std::exception());
        }

        Instruction inst;
        inst.type = toRet.type;
        inst.Parameters.push_back(toRet);
        inst.name = token.token;

        return inst;
    }else{
        Instruction inst;
        inst.type = DataTypes::Void;
        inst.name = token.token;

        return inst;
    }

}

Instruction O::SematicAnalyser::proccessIfInstruction(Analyser::TokenisedFile token)
{
	if (token.name.childToken.size() != 1) {
		throw(std::exception());
	}

	Instruction inst = proccessInstCall(token.name.childToken[0]);

	if (inst.type != DataTypes::Boolean) {
		throw(std::exception());
	}

	Instruction toRet;
	toRet.name = token.name.token;
	toRet.type = DataTypes::Void;

	toRet.Parameters.push_back(inst);

    if(token.subToken.size() != 0) {
        SematicAnalyser sa;
        sa.adt = adt;
        sa.operators = std::vector<Operator>(operators);
        sa.functions = std::vector<Function>(functions);
        sa.variables = std::vector<Variable>(variables);
        sa.returnType = returnType;
        sa.definedStructures = std::vector<Structure>(definedStructures);

        for(auto elem : token.subToken){
            sa.ProcessToken(elem);
        }

        variablesCreatedAtThatField.insert(variablesCreatedAtThatField.end(), sa.variablesCreatedAtThatField.begin(), sa.variablesCreatedAtThatField.end());

        for (int i = 0; i < sa.instructions.size(); i++) {
            toRet.Parameters.push_back(sa.instructions[i]);
        }
    }

	return toRet;
}

Instruction O::SematicAnalyser::proccessElseIfInstruction(Analyser::TokenisedFile token)
{
	if (instructions.size() == 0 || ((*(instructions.end() - 1)).name != IF_NAME && (*(instructions.end() - 1)).name != ELIF_NAME)) {
		throw(std::exception());
	}

	if (token.name.childToken.size() != 1) {
		throw(std::exception());
	}

	Instruction inst = proccessInstCall(token.name.childToken[0]);

	if (inst.type != DataTypes::Boolean) {
		throw(std::exception());
	}

	Instruction toRet;
	toRet.name = token.name.token;
	toRet.type = DataTypes::Void;

	toRet.Parameters.push_back(inst);

    if(token.subToken.size() != 0) {
        SematicAnalyser sa;
        sa.adt = adt;
        sa.operators = std::vector<Operator>(operators);
        sa.functions = std::vector<Function>(functions);
        sa.variables = std::vector<Variable>(variables);
        sa.returnType = returnType;
        sa.definedStructures = std::vector<Structure>(definedStructures);

        for(auto elem : token.subToken){
            sa.ProcessToken(elem);
        }

        variablesCreatedAtThatField.insert(variablesCreatedAtThatField.end(), sa.variablesCreatedAtThatField.begin(), sa.variablesCreatedAtThatField.end());

        for (int i = 0; i < sa.instructions.size(); i++) {
            toRet.Parameters.push_back(sa.instructions[i]);
        }
    }

	return toRet;
}

Instruction O::SematicAnalyser::proccessElseInstruction(Analyser::TokenisedFile token)
{
	if (instructions.size() == 0 || ((*(instructions.end() - 1)).name != IF_NAME && (*(instructions.end() - 1)).name != ELIF_NAME)) {
		throw(std::exception());
	}

	if (token.name.childToken.size() != 0) {
		throw(std::exception());
	}

	Instruction toRet;
	toRet.name = token.name.token;
	toRet.type = DataTypes::Void;

    if(token.subToken.size() != 0) {
        SematicAnalyser sa;
        sa.adt = adt;
        sa.operators = std::vector<Operator>(operators);
        sa.functions = std::vector<Function>(functions);
        sa.variables = std::vector<Variable>(variables);
        sa.returnType = returnType;
        sa.definedStructures = std::vector<Structure>(definedStructures);

        for(auto elem : token.subToken){
            sa.ProcessToken(elem);
        }

        variablesCreatedAtThatField.insert(variablesCreatedAtThatField.end(), sa.variablesCreatedAtThatField.begin(), sa.variablesCreatedAtThatField.end());

        for (int i = 0; i < sa.instructions.size(); i++) {
            toRet.Parameters.push_back(sa.instructions[i]);
        }
    }

	return toRet;
}

Instruction O::SematicAnalyser::proccessFuncInstrucion(Analyser::TokenisedFile token, bool isExtern) {

    Instruction toRet;

    if (token.name.childToken.size() != 2) {
        throw (std::exception());
    }


    auto type = getDataType(token.name.childToken[0]);

    Function func;

    if (type == DataTypes::Error) {
        std::wstring exceptionMessage =
                L"Unexpected return type at function definition \"" + token.name.childToken[0].token + L"\"";
        throw (std::exception());
    }

    func.returnType = type;

    if (token.name.childToken[1].type == Analyser::Type::ServiceName && token.name.childToken[1].token == L"name" &&
        token.name.childToken[1].childToken.size() >= 1) {
        auto nameField = token.name.childToken[1];
        int argOffset = 1;
        if (nameField.type == O::Analyser::Type::ServiceName
            && nameField.childToken.size() >= 2
            && nameField.childToken[0].token == L"operator") {
            std::wstring resultToken = nameField.childToken[1].token.substr(1, 1);
            func.name = resultToken;
            argOffset = 2;
        } else {
            func.name = nameField.childToken[0].token;
        }
        std::vector<DataTypes> dt;

        for (int i = argOffset; i < nameField.childToken.size(); i++) {
            auto funcArgName = nameField.childToken[i];
            if (funcArgName.childToken.size() != 1) {
                throw (std::exception());
            }
            auto funcArgType = getDataType(funcArgName.childToken[0]);
            if (funcArgType == DataTypes::Error) {
                std::wstring message =
                        L"Unrecognised data type at function argument \"" + funcArgName.childToken[0].token + L"\"";
                throw (std::exception());
            }
            Variable v;
            v.name = funcArgName.token;
            v.type = funcArgType;

            if (contains(func.arguments, v)) {
                throw (std::exception());
            }
            dt.push_back(v.type);
            func.arguments.push_back(v);
        }

        if (isExtern) {
            toRet.name = func.name;
            toRet.type = func.returnType;
            functions.push_back(func);
            if (argOffset == 2) {
                if (func.arguments.size() != 2) {
                    throw (std::exception());
                }
                auto newOp = Operator(func.name, func.arguments[0].type, func.arguments[1].type, func.returnType);
                operators.push_back(newOp);
            }

            if (token.subToken.size() == 0) {
                return toRet;
            }
        }

        if (token.subToken.size() != 0) {
            SematicAnalyser subSematicAnalyser;
            subSematicAnalyser.adt = adt;
            subSematicAnalyser.returnType = func.returnType;
            subSematicAnalyser.variables = std::vector<Variable>(variables);
            subSematicAnalyser.functions = std::vector<Function>(functions);
            subSematicAnalyser.operators = std::vector<Operator>(operators);
            subSematicAnalyser.definedStructures = std::vector<Structure>(definedStructures);
            for (auto elem: func.arguments) {
                subSematicAnalyser.variables.push_back(elem);
            }
            for (auto elem: token.subToken) {
                subSematicAnalyser.ProcessToken(elem);
            }

            if (func.returnType != DataTypes::Void && subSematicAnalyser.returnCalls == 0) {
                std::wstring message = L"Require at leas one return call at \"" + func.name + L"\" function";
            }

            func.variables = subSematicAnalyser.variablesCreatedAtThatField;
            func.body = subSematicAnalyser.instructions;
        }
        if (containsFunction(func.name, dt) != DataTypes::Error) {
            std::wstring message = L"Function redefinition \"" + func.name + L"\"";
            throw (std::exception());
        }
        if (!isExtern) {
            if (argOffset == 2) {
                if (func.arguments.size() != 2) {
                    throw (std::exception());
                }
                auto newOp = Operator(func.name, func.arguments[0].type, func.arguments[1].type, func.returnType);
                operators.push_back(newOp);
            }

            functions.push_back(func);
        } else {
            exportedFunctions.push_back(func);
        }
        functionsCreatedAtThatField.push_back(func);
    } else {
        throw (std::exception());
    }
    toRet.name = func.name;
    toRet.type = func.returnType;


    return toRet;
}

Instruction O::SematicAnalyser::proccessVarInstruction(Analyser::Token token, bool isExtern)
{
	Instruction toRet;
	toRet.IsVariable = true;
	
	if (token.childToken.size() == 2) {
		toRet.name = token.childToken[1].token;
		if (containsVariable(toRet.name)) {
			std::wstring message = (L"Variable with name " + toRet.name + L" alredy exists");
			throw(std::exception());
		}
		std::wstring invalidDatatype = L"Invalid data type name " + token.childToken[0].token;
		auto exp = std::exception();

		auto getType = getDataType(token.childToken[0]);

		if (getType == DataTypes::Error) {
			throw exp;
		}

		toRet.type = getType;

		Variable v;
		v.name = toRet.name;
		v.type = toRet.type;
		variables.push_back(v);
        if(!isExtern) {
            variablesCreatedAtThatField.push_back(v);
        }
	}
	else {
		throw(std::exception());
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
					throw(std::exception());
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
			throw(std::exception());
		}

		toRet.Parameters.push_back(destInstr);
		toRet.Parameters.push_back(proccessInstCall(token.childToken[1]));
	}
	else {
		throw(std::exception());
	}

	if (toRet.Parameters[0].type != toRet.Parameters[1].type) {
		throw(std::exception());
	}

	return toRet;
}

Instruction O::SematicAnalyser::ProcessToken(Analyser::TokenisedFile token, bool add)
{
	Instruction inst;

	if (token.name.type == Analyser::Type::ServiceName) {
		if (token.name.token == L"=" && token.name.twoSided) {
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
        else if (token.name.token == EXTERN_DEFINITION_TOKEN){
            if(token.name.childToken.size() != 1){
                throw std::exception();
            }
            if(token.name.childToken[0].token == VAR_CREATION_NAME){
                proccessVarInstruction(token.name.childToken[0], true);
            }
            else if(token.name.childToken[0].token == FUNC_CREATION_NAME){
                Analyser::TokenisedFile f;
                f.name = token.name.childToken[0];
                f.subToken = token.subToken;
                proccessFuncInstrucion(f, true);
            }
        }
        else if (token.name.token == STRUCTURE_DEFINITION_TOKEN){
            inst = proccessStructureCreation(token);
        }
	}
	else {
		if (token.name.token == IF_NAME) {
			
			inst = proccessIfInstruction(token);

			if (add) {
				instructions.push_back(inst);
			}
			return inst;
		}
        else if (token.name.token == ELIF_NAME) {
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
		else if (token.name.token == WHILE_CYCLE_TOKEN) {
			inst = proccessWhileCycleInstruction(token);

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

	throw(std::exception());
}


std::pair<bool, Structure> O::SematicAnalyser::containsStructureByDataType(DataTypes dt) {
    for(auto s : definedStructures){
        if(s.myDt == dt){
            return {true, s};
        }
    }

    return {false, Structure()};
}

File O::SematicAnalyser::getFileRepresantation()
{
	File f;
	f.functions = std::vector<Function>(functionsCreatedAtThatField);
	f.instructions = std::vector<Instruction>(instructions);
	f.variables = std::vector<Variable>(variablesCreatedAtThatField);
    f.operators = std::vector<Operator>(operators);
	f.adtTable = adt;
    f.structures = std::vector<Structure>(definedStructures);

	return f;
}

Instruction O::SematicAnalyser::proccessStructureCreation(O::Analyser::TokenisedFile token) {
    Structure newStruct;

    newStruct.name = token.name.childToken[0].token;

    if(stringToDataType(newStruct.name, adt) != DataTypes::Error){
        throw std::exception();
    }

    for(auto elem : token.subToken){
        if(elem.name.token == UNIT_DEFINITION_TOKEN){
            Variable v;
            if(elem.name.childToken.size() != 2){
                throw std::exception();
            }
            v.type = getDataType(elem.name.childToken[0]);
            v.name = elem.name.childToken[1].token;
            newStruct.variables.push_back(v);
        }
    }

    adt.lastId += 1;
    adt.additionalNumber.push_back(adt.lastId);
    adt.additionalName.push_back(newStruct.name);
    newStruct.myDt = stringToDataType(newStruct.name, adt);

    definedStructures.push_back(newStruct);

    Instruction toRet;

    toRet.name = newStruct.name;
    toRet.type = newStruct.myDt;

    return toRet;
}

Instruction O::SematicAnalyser::proccessInstCall(Analyser::Token token)
{
	Instruction res;

	if (token.type == Analyser::Type::MathematicalOperator && token.twoSided == true) {
		res.name = token.token;
		res.ArithmeticProccess = true;
		if (token.childToken.size() != 2) {
			std::wstring message = L"Unexpected count of operands at " + res.name + L" operator";
			throw(std::exception());
		}
		else {
			res.Parameters.push_back(proccessInstCall(token.childToken[0]));
			res.Parameters.push_back(proccessInstCall(token.childToken[1]));
			if (!containsOperator(res.name, res.Parameters[0].type, res.Parameters[1].type)) {
				std::wstring message = L"Unexpected type of operands (" + dataTypeToString(res.Parameters[0].type) + L", " + dataTypeToString(res.Parameters[1].type) + L") at " + res.name + L" operator";
				throw(std::exception());
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
	else if (token.type == Analyser::Type::StringLiteral) {
		return proccessString(token);
	}
	else if (token.type == Analyser::Type::Char) {
		res.name = token.token;
		res.type = DataTypes::Character;
		return res;
	}
	else if (token.type == Analyser::Type::Number) {
		res.name = token.token;
		res.type = getTypeOfNumber(token.token);
	}
	else if (token.type == Analyser::Type::ServiceName) {
		if (true)
		{
#ifdef FALSE_LITERAL_SECONDARY_ACTIVE 
			if (token.token == FALSE_LITERAL_DEFAULT_NAME || token.token == FALSE_LITERAL_SECONDARY_NAME) {
#endif // FALSE_LITERAL_SECONDARY_ACTIVE 
#ifndef FALSE_LITERAL_SECONDARY_ACTIVE
				if (token.token == FALSE_LITERAL_DEFAULT_NAME) {
#endif // !FALSE_LITERAL_SECONDARY_ACTIVE
					res.name = FALSE_NAME;
					res.type = DataTypes::Boolean;
				}

				// ��������� �������� ���� ��� ������
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
		if (token.token == ARRAY_CREATION_TOKEN) {
			return proccessArrayCreationInstruction(token);
		}
	}
			else if (token.type == Analyser::Type::Name) {
                if(token.token == FREE_INSTRUCTION_TOKEN){
                    res.name = FREE_INSTRUCTION_NAME;
                    res.Parameters.push_back(proccessInstCall(token.childToken[0]));
                    res.type = DataTypes::Void;
                }else if(token.token == MALLOC_INSTRUCTION_TOKEN){
                    res.type = getDataType(token.childToken[0]);
                    res.Parameters.push_back(proccessInstCall(token.childToken[1]));
                    res.name = MALLOC_INSTRUCTION_NAME;
                }
				else if (token.forward) {
					res = checkAndGetFunction(token);
				}
				else {
					if (containsVariable(token.token)) {
						return getVariableAsInstruction(token.token);
					}
					else {
						std::wstring message = L"Undefined variable with name \"" + token.token + L"\"";
						throw(std::exception());
					}
				}
			}
			return res;
}
