#include "SematicAnalyser.h"

#define MAIN_FLOW_NAME L"___MAIN___"
#define SET_VALUE_NAME L"SET_VALUE"

#define VAR_CREATION_NAME L"var"

#define FUNC_CREATION_NAME L"func"

#define RETURN_NAME L"return"

#define TEMPLATE_TOKEN L"template"

#define POINTER_GET_INSTRUCTION_TOKEN L"@"
#define POINTER_ACCESS_INSTRUCTION_TOKEN L"~"
#define COMMA_OPERATOR L","
#define FUNCTION_CALL L"()"
#define SQUARE_OPERATOR L"[]"
#define MATH_UNARY_MINUS L"-"
#define MATH_SET L"="
#define ENUM_NAME L"enum"

#define POINTER_GET_INSTRUCTION_NAME L"GET_POINTER"

#define FREE_INSTRUCTION_TOKEN L"free"
#define FREE_INSTRUCTION_NAME L"_FREE"
#define MALLOC_INSTRUCTION_TOKEN L"malloc"
#define MALLOC_INSTRUCTION_NAME L"_MALLOC"

#define POINTER_ACCESS_INSTRUCTION_NAME L"GET_POINTER_CONTENT"

#define EXTERN_DEFINITION_TOKEN L"extern"

#define STRUCTURE_DEFINITION_TOKEN L"structure"
#define UNIT_DEFINITION_TOKEN L"__init__"

#define WHILE_CYCLE_TOKEN L"while"
#define WHILE_CYCLE_NAME L"WHILE_CYCLE"

#define ARRAY_CREATION_TOKEN L"[]"
#define ARRAY_CREATION_NAME L"ARRAY_INIT"

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

Instruction O::SematicAnalyser::checkAndGetFunction(Analyser::Token token) {
    std::vector<DataTypes> dataTypes;
    Instruction retInst;

    retInst.IsFunction = true;
    retInst.name = token.childToken[0].token;

    auto arguments = getComma(token.childToken[1]);

    for (auto elem: arguments) {
        auto inst = proccessInstCall(elem);
        dataTypes.push_back(inst.type);
        retInst.Parameters.push_back(inst);
    }

    auto f = containsFunction(retInst.name, dataTypes);

    if (f == DataTypes::Error) {
        auto dt = getDataType(token.childToken[0]);

        if (dt != DataTypes::Error && retInst.Parameters.size() == 1) {
            retInst.Parameters[0].type = dt;
            return retInst.Parameters[0];
        }

        std::wstring message = L"Unable to find function with name \"" + retInst.name + L"\"";

        throw (std::exception());
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
	if (token.name.childToken[0].token == COMMA_OPERATOR) {
		throw(std::exception());
	}

	Instruction inst = proccessInstCall(token.name.childToken[1]);

	if (inst.type != DataTypes::Boolean) {
		throw(std::exception());
	}

	Instruction toRet;
	toRet.name = WHILE_CYCLE_NAME;
	toRet.type = DataTypes::Void;
	toRet.Parameters.push_back(inst);

	if(token.subToken.size() != 0) {
		SematicAnalyser sa(this);


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

        std::vector<Analyser::Token> tokens;

        if(token.childToken[0].token == COMMA_OPERATOR){
            tokens = getComma(token.childToken[0]);
        } else{
            tokens = token.childToken;
        }

		Instruction toRet;
		toRet.name = ARRAY_CREATION_NAME;
		Instruction instSize;
		instSize.name = tokens.size();
		instSize.type = DataTypes::Integer;
		toRet.Parameters.push_back(instSize);

		for (int i = 0; i < tokens.size(); i++) {
			toRet.Parameters.push_back(proccessInstCall(tokens[i]));
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
    auto arguments = getComma(token.childToken[1]);

	if (returnType != DataTypes::Void && arguments.size() != 1) {
		throw(std::exception());
	}

    if(arguments.size() != 0) {
        Instruction toRet = proccessInstCall(arguments[0]);
        if (toRet.type != returnType) {
            throw(std::exception());
        }

        Instruction inst;
        inst.type = toRet.type;
        inst.Parameters.push_back(toRet);
        inst.name = RETURN_NAME;

        this->returnCalls += 1;

        return inst;
    }else{
        Instruction inst;
        inst.type = DataTypes::Void;
        inst.name = RETURN_NAME;

        return inst;
    }

}

Instruction O::SematicAnalyser::proccessIfInstruction(Analyser::TokenisedFile token)
{
	if (token.name.childToken[0].token == COMMA_OPERATOR) {
		throw(std::exception());
	}

	Instruction inst = proccessInstCall(token.name.childToken[1]);

	if (inst.type != DataTypes::Boolean) {
		throw(std::exception());
	}

	Instruction toRet;
	toRet.name = IF_NAME;
	toRet.type = DataTypes::Void;

	toRet.Parameters.push_back(inst);

    if(token.subToken.size() != 0) {
        SematicAnalyser sa(this);

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

	if (token.name.childToken[0].token == COMMA_OPERATOR) {
		throw(std::exception());
	}

	Instruction inst = proccessInstCall(token.name.childToken[1]);

	if (inst.type != DataTypes::Boolean) {
		throw(std::exception());
	}

	Instruction toRet;
	toRet.name = ELIF_NAME;
	toRet.type = DataTypes::Void;

	toRet.Parameters.push_back(inst);

    if(token.subToken.size() != 0) {
        SematicAnalyser sa(this);

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

	Instruction toRet;
	toRet.name = ELSE_NAME;
	toRet.type = DataTypes::Void;

    if(token.subToken.size() != 0) {
        SematicAnalyser sa(this);

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

    Analyser::Token funcFirstDescriptor;

    if(isExtern){
        funcFirstDescriptor = token.name.childToken[0].childToken[0];
    } else{
        funcFirstDescriptor = token.name.childToken[0];
    }

    if (token.name.childToken.size() != 2) {
        throw (std::exception());
    }

    auto type = getDataType(funcFirstDescriptor.childToken[0]);

    Function func;

    if(isExtern){
        func.IsExtern = true;
    }

    if (type == DataTypes::Error) {
        std::wstring exceptionMessage =
                L"Unexpected return type at function definition \"" + funcFirstDescriptor.token + L"\"";
        throw (std::exception());
    }

    func.returnType = type;

    if (funcFirstDescriptor.type == Analyser::Type::ServiceName) {
        auto nameField = funcFirstDescriptor.childToken[1];
        bool isOperator = false;
        if (nameField.type == O::Analyser::Type::ServiceName
            && nameField.childToken.size() == 1
            && nameField.token== L"operator") {
            std::wstring resultToken = nameField.childToken[0].token.substr(1, nameField.childToken[0].token.size() - 2);
            func.name = resultToken;
            isOperator = true;
        } else {
            func.name = nameField.token;
        }
        std::vector<DataTypes> dt;

        std::vector<Analyser::Token> parameterToken = getComma(token.name.childToken[1]);

        for (int i = 0; i < parameterToken.size(); i++) {
            auto newArg = parameterToken[i];

            if(newArg.token != UNIT_DEFINITION_TOKEN){
                std::wstring message = L"Initialisation definition expected";
                throw std::exception();
            }

            auto funcArgType = getDataType(newArg.childToken[0]);
            if (funcArgType == DataTypes::Error) {
                std::wstring message =
                        L"Unrecognised data type at function argument \"" + newArg.childToken[0].token + L"\"";
                throw (std::exception());
            }
            Variable v;
            v.name = newArg.childToken[1].token;
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
            if (isOperator) {
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
            subSematicAnalyser.enumerations = std::map<std::wstring, Instruction>(enumerations);

            for (auto elem: func.arguments) {
                subSematicAnalyser.variables.push_back(elem);
            }
            for (auto elem: token.subToken) {
                subSematicAnalyser.ProcessToken(elem);
            }

            if (func.returnType != DataTypes::Void && subSematicAnalyser.returnCalls == 0) {
                std::wstring message = L"Require at least one return call at \"" + func.name + L"\" function";
                throw std::exception();
            }

            func.variables = subSematicAnalyser.variablesCreatedAtThatField;
            func.body = subSematicAnalyser.instructions;
        }
        if (containsFunction(func.name, dt) != DataTypes::Error) {
            std::wstring message = L"Function redefinition \"" + func.name + L"\"";
            throw (std::exception());
        }
        if (!isExtern) {
            if (isOperator) {
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
	
	if (token.childToken.size() == 1) {
        if(token.childToken[0].token != UNIT_DEFINITION_TOKEN){
            std::wstring noDefinitionUnit = L"No unit definition at var creation";
            throw std::exception();
        }
		toRet.name = token.childToken[0].childToken[1].token;
		if (containsVariable(toRet.name)) {
			std::wstring message = (L"Variable with name " + toRet.name + L" alredy exists");
			throw(std::exception());
		}
		std::wstring invalidDatatype = L"Invalid data type name " + token.childToken[0].childToken[0].token;
		auto exp = std::exception();

		auto getType = getDataType(token.childToken[0].childToken[0]);

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
            if(dataTypeIsStructure(getType)){
                Structure structure = containsStructureByDataType(getType).second;
                if(structure.variables.size() != 0){
                    Analyser::Token size;
                    size.token = std::to_wstring(structure.variables.size());
                    size.type = Analyser::Type::Number;

                    Analyser::Token dataType;
                    dataType.token = dataTypeToString(structure.myDt, adt);
                    dataType.type = Analyser::Type::Name;

                    Analyser::Token parameters;
                    parameters.token = COMMA_OPERATOR;
                    parameters.type = Analyser::Type::MathematicalOperator;
                    parameters.twoSided = true;
                    parameters.childToken = {dataType, size};

                    Analyser::Token mallocName;
                    mallocName.token = MALLOC_INSTRUCTION_TOKEN;
                    mallocName.type = Analyser::Type::ServiceName;

                    Analyser::Token malloc;
                    malloc.token = L"()";
                    malloc.type = Analyser::Type::MathematicalOperator;
                    malloc.forward = true;
                    malloc.childToken = {mallocName, parameters};

                    Analyser::Token destination;
                    destination.token = v.name;
                    destination.type = Analyser::Type::Name;

                    Analyser::Token structureMallocToken;
                    structureMallocToken.token = MATH_SET;
                    structureMallocToken.type = Analyser::Type::MathematicalOperator;
                    structureMallocToken.twoSided = true;
                    structureMallocToken.childToken = {destination, malloc};

                    instructions.push_back(proccessInstCall(structureMallocToken));
                }
            }
        }
	}
	else {
		throw(std::exception());
	}

	return toRet;
}

Instruction O::SematicAnalyser::ProcessToken(Analyser::TokenisedFile token, bool add)
{
	Instruction inst;

    auto containsLabel = this->containsLabel(token.name.token);

    if(token.name.token == VAR_CREATION_NAME){
        inst = proccessVarInstruction(token.name);
    }
    else if(token.name.token == STRUCTURE_DEFINITION_TOKEN){
        proccessStructureCreation(token);
    }else if(token.name.token == TEMPLATE_TOKEN){
        processTemplate(token);
    }
    else if(token.name.token == ENUM_NAME){
        processEnumeration(token);
    }
    else if(token.name.token == FUNCTION_CALL &&
    (token.name.childToken[0].token == FUNC_CREATION_NAME ||
    token.name.childToken[0].token == EXTERN_DEFINITION_TOKEN ||
    token.name.childToken[0].token == IF_NAME ||
    token.name.childToken[0].token == UNIT_DEFINITION_TOKEN ||
    token.name.childToken[0].token == WHILE_CYCLE_TOKEN)){
        if(token.name.childToken[0].token == FUNC_CREATION_NAME){
            proccessFuncInstrucion(token);
        }else if(token.name.childToken[0].token == IF_NAME){
            inst = proccessIfInstruction(token);
        }
        else if(token.name.childToken[0].token == UNIT_DEFINITION_TOKEN){
            if(token.name.childToken[0].childToken[0].token == ELSE_NAME && token.name.childToken[0].childToken[1].token == IF_NAME){
                inst = proccessElseIfInstruction(token);
            }
        }else if(token.name.childToken[0].token == WHILE_CYCLE_TOKEN){
            inst = proccessWhileCycleInstruction(token);
        }
        else if(token.name.childToken[0].token == EXTERN_DEFINITION_TOKEN){
            if(token.name.childToken[0].childToken[0].token == FUNC_CREATION_NAME) {
                proccessFuncInstrucion(token, true);
            }
        }
    }
    else if(token.name.token == ELSE_NAME){
        inst = proccessElseInstruction(token);
    }
    else if(token.name.token == SQUARE_OPERATOR && token.name.childToken.size() == 1 && token.name.childToken[0].token == FUNCTION_CALL){
        inst = processElementCall(token.name);
    }else if(templates.find(token.name.token) != templates.end()){
        processFetchTemplate(token);
    }else if(containsLabel.first){
        SematicAnalyser sa(this);
        sa.definedLabels = std::vector<std::pair<std::wstring, std::vector<Analyser::TokenisedFile>>>(this->definedLabels);
        Analyser::TokenisedFile tf;
        tf.subToken = containsLabel.second;
        tf.name = {};
        tf.name.token = MAIN_FLOW_NAME;
        sa.ProccessTokenisedFile(tf);
        merge(&sa);
    }
    else {
        inst = proccessInstCall(token.name);
    }

    if(add && !inst.IsVariable && inst.name != L"UNSIGNED_TOKEN") {
        instructions.push_back(inst);
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

    std::vector<Analyser::TokenisedFile> methods;

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
        }else{
            methods.push_back(elem);
        }
    }

    adt.lastId += 1;
    adt.additionalNumber.push_back(adt.lastId);
    adt.additionalName.push_back(newStruct.name);
    newStruct.myDt = stringToDataType(newStruct.name, adt);

    definedStructures.push_back(newStruct);

    for(auto newMethod : methods){
        if(newMethod.name.token == FUNCTION_CALL){
            Analyser::TokenisedFile tf = newMethod;

            Analyser::Token me;
            me.token = L"__init__";
            Analyser::Token dt;
            dt.type = Analyser::Type::Name;
            dt.token = newStruct.name;
            Analyser::Token meName;
            meName.token = L"me";
            meName.type = Analyser::Type::Name;
            me.childToken = {dt, meName};

            if(tf.name.childToken[1].token == L""){
                tf.name.childToken[1] = me;
            }else{
                Analyser::Token comma;
                comma.token = COMMA_OPERATOR;
                comma.type = Analyser::Type::MathematicalOperator;
                comma.forward = true;
                comma.childToken = {me, tf.name.childToken[1]};
                tf.name.childToken[1] = comma;
            }

            proccessFuncInstrucion(tf, tf.name.childToken[0].token == L"extern");
        }else{
            throw std::exception();
        }
    }

    Instruction toRet;

    toRet.name = newStruct.name;
    toRet.type = newStruct.myDt;

    return toRet;
}

Instruction O::SematicAnalyser::proccessInstCall(Analyser::Token token) {
    Instruction res;

    if (token.type == Analyser::Type::MathematicalOperator) {
        if (token.twoSided) {
            Analyser::TokenisedFile tokenFile = {token.childToken[0]};
            Instruction t1 = ProcessToken(tokenFile, false);
            tokenFile = {token.childToken[1]};
            Instruction t2 = ProcessToken(tokenFile, false);

            bool definedOperator = containsOperator(token.token, t1.type, t2.type);
            if (definedOperator) {
                if(isExternFunction(token.token, {t1.type, t2.type})){
                    res.ArithmeticProccess = true;
                }else{
                    res.IsFunction = true;
                }
                res.name = token.token;
                res.type = getReturnDataTypeOfOperator(token.token, t1.type, t2.type);
                res.Parameters = {t1, t2};
            }
            else {
                if (token.token == MATH_SET) {
                    if (t1.type == t2.type) {
                        res.name = L"SET_VALUE";
                        res.ArithmeticProccess = true;
                        res.type = DataTypes::ServiceInstruction;
                        res.Parameters = {t1, t2};
                        return res;
                    }
                }
            }
        }
        else if (token.forward) {
            if (token.token == FUNCTION_CALL) {
                if (token.childToken[0].type == Analyser::Type::ServiceName) {
                    if (token.childToken[0].token == RETURN_NAME) {
                        res = proccessReturnCall(token);
                    } else if (token.childToken[0].token == FREE_INSTRUCTION_TOKEN) {
                        res.name = FREE_INSTRUCTION_NAME;
                        auto arguments = getComma(token.childToken[1]);
                        res.Parameters.push_back(proccessInstCall(arguments[0]));
                        res.type = DataTypes::Void;
                    } else if (token.childToken[0].token == MALLOC_INSTRUCTION_TOKEN) {
                        auto arguments = getComma(token.childToken[1]);
                        res.type = getDataType(arguments[0]);
                        res.Parameters.push_back(proccessInstCall(arguments[1]));
                        res.name = MALLOC_INSTRUCTION_NAME;
                    }else {
                        res = checkAndGetFunction(token);
                    }
                } else {
                    res = checkAndGetFunction(token);
                }
            }
            else if (token.token == SQUARE_OPERATOR) {
                if(token.childToken.size() == 1){
                   res = proccessArrayCreationInstruction(token);
                }
                else{
                    Instruction dest = proccessInstCall(token.childToken[0]);
                    bool reCreateCheck = true;
                    Instruction secondOp;
                    try {
                        secondOp = proccessInstCall(token.childToken[1]);
                    }
                    catch (...) {
                        reCreateCheck = false;
                    }

                    if (reCreateCheck) {
                        bool exists = containsOperator(token.token, dest.type, secondOp.type);
                        if (exists) {
                            if(isExternFunction(token.token, {dest.type, secondOp.type})){
                                res.ArithmeticProccess = true;
                            }else{
                                res.IsFunction = true;
                            }
                            res.name = token.token;
                            res.type = getReturnDataTypeOfOperator(token.token, dest.type, secondOp.type);
                            res.Parameters = {dest, secondOp};
                        } else {
                            res = proccessArrayAccessInstruction(token);
                        }

                    } else {
                        res = proccessArrayAccessInstruction(token);
                    }
                }
            }
            else if (token.token == POINTER_GET_INSTRUCTION_TOKEN){
                res = proccessPointerGet(token);
            }
            else if (token.token == POINTER_ACCESS_INSTRUCTION_TOKEN){
                res = proccessPointerAccess(token);
            }
            else if(token.token == MATH_UNARY_MINUS){
                auto ppToken = token.childToken[0];
                ppToken.token = L"-" + ppToken.token;
                res = proccessInstCall(ppToken);
            }
        }
    }
    else if (token.type == Analyser::Type::StringLiteral) {
        return proccessString(token);
    }
    else if (token.type == Analyser::Type::ServiceName){
        if(token.token == FALSE_LITERAL_DEFAULT_NAME){
            res.type = DataTypes::Boolean;
            res.name = FALSE_NAME;
        }else if(token.token == TRUE_LITERAL_DEFAULT_NAME){
            res.type = DataTypes::Boolean;
            res.name = TRUE_NAME;
        }
    }
    else if (token.type == Analyser::Type::Char) {
        res.name = token.token;
        res.type = DataTypes::Character;
        return res;
    }
    else if (token.type == Analyser::Type::Number) {
        if(isStringEndsWith(token.token, L"f") || isStringEndsWith(token.token, L"F")){
            res.name = token.token.substr(0, token.token.size() - 1);
            res.type = DataTypes::FloatingPoint;
        }
        else if(isStringEndsWith(token.token, L"c") || isStringEndsWith(token.token, L"C")){
            if(token.token.find('.') != -1){
                throw std::exception();
            }

            res.name = (char)(std::stoi(token.token));
            res.type = DataTypes::Character;
        }
        else if(isStringEndsWith(token.token, L"i") || isStringEndsWith(token.token, L"I")){
            if(token.token.find('.') != -1){
                throw std::exception();
            }

            res.name = std::stoi(token.token);
            res.type = DataTypes::Integer;
        }
        else {
            res.name = token.token;
            res.type = getTypeOfNumber(token.token);
        }
    }
    else if (token.type == Analyser::Type::Name) {
        auto containsLabel = this->containsLabel(token.token);

        if (containsVariable(token.token)) {
            return getVariableAsInstruction(token.token);
        }else if(enumerations.find(token.token) != enumerations.end()) {
            return enumerations[token.token];
        }else if(containsLabel.first){
            SematicAnalyser sa(this);
            sa.definedLabels = std::vector<std::pair<std::wstring, std::vector<Analyser::TokenisedFile>>>(this->definedLabels);
            Analyser::TokenisedFile tf;
            tf.subToken = containsLabel.second;
            tf.name = {};
            tf.name.token = MAIN_FLOW_NAME;
            sa.ProccessTokenisedFile(tf);
            res = sa.instructions[0];
        }
        else {
            std::wstring message = L"Undefined variable with name \"" + token.token + L"\"";
            throw (std::exception());
        }

    }
    return res;
}

bool O::SematicAnalyser::dataTypeIsStructure(DataTypes dt) {
    for(auto structure : definedStructures){
        if(structure.myDt == dt){
            return true;
        }
    }
    return false;
}

std::vector<O::Analyser::Token> O::SematicAnalyser::getComma(O::Analyser::Token token) {
    std::vector<O::Analyser::Token> result;

    if(token.token != COMMA_OPERATOR){
        if(token.token == L""){
            return {};
        }
        return {token};
    }

    if(token.childToken[0].token == COMMA_OPERATOR){
        auto newToken = getComma(token.childToken[0]);
        result.erase(newToken.begin(), newToken.end());
    }else{
        result.push_back(token.childToken[0]);
    }

    if(token.childToken[1].token == COMMA_OPERATOR){
        auto newToken = getComma(token.childToken[1]);
        result.insert(result.end(),newToken.begin(), newToken.end());
    }else{
        result.push_back(token.childToken[1]);
    }

    return result;
}

bool O::SematicAnalyser::isExternFunction(std::wstring name, std::vector<DataTypes> dt) {
    for(auto function: functions){
        if(function.name == name && function.getArgumentsDataTypes() == dt){
            return function.IsExtern;
        }
    }
    return false;
}

Instruction O::SematicAnalyser::processEnumeration(O::Analyser::TokenisedFile tokenFile) {
    if(tokenFile.subToken.size() != 1){
        throw std::exception();
    }

    std::vector<Analyser::Token> tokens = getComma(tokenFile.subToken[0].name);

    for(auto elem : tokens){
        auto replaceInst = proccessInstCall(elem.childToken[1]);
        auto name = elem.childToken[0].token;

        enumerations.insert({name, replaceInst});
    }

    return {};
}

Instruction O::SematicAnalyser::processElementCall(O::Analyser::Token token) {

    O::Analyser::Token converted;
    converted.token = L"()";
    converted.forward = true;

    if(token.childToken[0].childToken[0].token != L"__init__"){
        throw std::exception();
    }

    O::Analyser::Token me = token.childToken[0].childToken[0].childToken[0];
    O::Analyser::Token funName = token.childToken[0].childToken[0].childToken[1];

    if(token.childToken[0].childToken[1].token == L""){
        converted.childToken = {funName, me};
    }else{
        O::Analyser::Token comma;
        comma.token = L",";
        comma.forward = true;
        comma.type = Analyser::Type::MathematicalOperator;
        comma.childToken = {me, token.childToken[0].childToken[1]};
        converted.childToken = {funName, comma};
    }


    return checkAndGetFunction(converted);
}

Instruction O::SematicAnalyser::processTemplate(O::Analyser::TokenisedFile tokenisedFile) {
    templates.insert({tokenisedFile.name.childToken[0].token, tokenisedFile.subToken});
    return {};
}

Instruction O::SematicAnalyser::processFetchTemplate(O::Analyser::TokenisedFile token) {
    std::vector<std::pair<std::wstring, std::vector<Analyser::TokenisedFile>>> definedLabels =
            std::vector<std::pair<std::wstring, std::vector<Analyser::TokenisedFile>>>(this->definedLabels);

    for(auto elem : token.subToken){
        definedLabels.push_back({elem.name.token, elem.subToken});
    }

    auto original = templates[token.name.token];

    SematicAnalyser sa(this);
    sa.definedLabels = definedLabels;

    Analyser::TokenisedFile tf;
    tf.subToken = original;
    tf.name = {};
    tf.name.token = MAIN_FLOW_NAME;

    sa.ProccessTokenisedFile(tf);

    merge(&sa);

    return {};
}

O::SematicAnalyser::SematicAnalyser(const O::SematicAnalyser* const origin) {
    this->adt = origin->adt;
    this->operators = std::vector<Operator>(origin->operators);
    this->functions = std::vector<Function>(origin->functions);
    this->variables = std::vector<Variable>(origin->variables);
    this->variables.insert(this->variables.end(), origin->variablesCreatedAtThatField.begin(), origin->variablesCreatedAtThatField.end());
    this->returnType = origin->returnType;
    this->enumerations = std::map<std::wstring, Instruction>(origin->enumerations);
    this->definedStructures = std::vector<Structure>(origin->definedStructures);
    this->templates = std::map<std::wstring, std::vector<Analyser::TokenisedFile>>(origin->templates);
    this->definedLabels = std::vector<std::pair<std::wstring, std::vector<Analyser::TokenisedFile>>>(origin->definedLabels);
}

std::pair<bool, std::vector<O::Analyser::TokenisedFile>> O::SematicAnalyser::containsLabel(std::wstring label) {
    for(int i = definedLabels.size() - 1; i >= 0; i--){
        if(definedLabels[i].first == label){
            return  {true,definedLabels[i].second};
        }
    }

    return {false, {}};
}

void O::SematicAnalyser::merge(const O::SematicAnalyser *const sa) {
    this->adt = sa->adt;
    this->variablesCreatedAtThatField.insert(this->variablesCreatedAtThatField.end(), sa->variablesCreatedAtThatField.begin(), sa->variablesCreatedAtThatField.end());
    this->variables.insert(this->variables.end(), sa->variablesCreatedAtThatField.begin(), sa->variablesCreatedAtThatField.end());
    this->definedStructures = sa->definedStructures;
    this->enumerations = sa->enumerations;
    this->functions = sa->functions;
    this->operators = sa->operators;
    this->instructions.insert(this->instructions.end(), sa->instructions.begin(), sa->instructions.end());
}
