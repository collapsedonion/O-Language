#include "SematicAnalyser.h"
#include <locale>
#include <codecvt>

#define MAIN_FLOW_NAME U"___MAIN___"
#define SET_VALUE_NAME U"SET_VALUE"

#define VAR_CREATION_NAME U"var"

#define GLOBAL_CREATION_NAME U"global"

#define FUNC_CREATION_NAME U"func"

#define RETURN_NAME U"return"

#define TEMPLATE_TOKEN U"template"

#define POINTER_GET_INSTRUCTION_TOKEN U"@"
#define POINTER_ACCESS_INSTRUCTION_TOKEN U"~"
#define COMMA_OPERATOR U","
#define FUNCTION_CALL U"()"
#define SQUARE_OPERATOR U"[]"
#define MATH_UNARY_MINUS U"-"
#define MATH_SET U"="
#define ENUM_NAME U"enum"

#define POINTER_GET_INSTRUCTION_NAME U"GET_POINTER"

#define FREE_INSTRUCTION_TOKEN U"free"
#define FREE_INSTRUCTION_NAME U"_FREE"
#define MALLOC_INSTRUCTION_TOKEN U"malloc"
#define MALLOC_INSTRUCTION_NAME U"_MALLOC"

#define POINTER_ACCESS_INSTRUCTION_NAME U"GET_POINTER_CONTENT"

#define EXTERN_DEFINITION_TOKEN U"extern"

#define STRUCTURE_DEFINITION_TOKEN U"structure"
#define UNIT_DEFINITION_TOKEN U"__init__"

#define WHILE_CYCLE_TOKEN U"while"
#define WHILE_CYCLE_NAME U"WHILE_CYCLE"

#define ARRAY_CREATION_TOKEN U"[]"
#define ARRAY_CREATION_NAME U"ARRAY_INIT"

#define STRUCTURE_ELEMENT_ACCESS_NAME U"STRUCTURE_ACCESS"

#define IF_NAME U"if"
#define ELSE_NAME U"else"
#define ELIF_NAME U"else if"

#define FALSE_LITERAL_DEFAULT_NAME U"false"
#define FALSE_NAME U"FALSE"
//#define FALSE_LITERAL_SECONDARY_ACTIVE 
//#define FALSE_LITERAL_SECONDARY_NAME U"False"

#define TRUE_LITERAL_DEFAULT_NAME U"true"
#define TRUE_NAME U"TRUE"
//#define TRUE_LITERAL_SECONDARY_ACTIVE 
//#define TRUE_LITERAL_SECONDARY_NAME U"True"

DataTypes O::SematicAnalyser::containsFunction(std::u32string name, std::vector<DataTypes> dt)
{
	for (auto elem : functions) {
        bool test = elem.getArgumentsDataTypes() == dt;
		if (elem.name == name && elem.getArgumentsDataTypes() == dt) {
			return elem.returnType;
		}
	}

	return DataTypes::Error;
}

bool O::SematicAnalyser::containsVariable(std::u32string name)
{
	for (auto elem : variables) {
		if (elem.name == name) {
			return true;
		}
	}

	return false;
}

bool O::SematicAnalyser::containsOperator(std::u32string op, DataTypes left, DataTypes right)
{
	for (auto elem : operators) {
		if (elem.op == op && elem.left == left && elem.right == right) {
			return true;
		}
	}

	return false;
}

DataTypes O::SematicAnalyser::getReturnDataTypeOfOperator(std::u32string op, DataTypes left, DataTypes right)
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
            retInst.Parameters[0].line = token.line_id;
            retInst.Parameters[0].file_name = token.file_name;
            return retInst.Parameters[0];
        }

        std::u32string message = U"Unable to find function with name \"" + retInst.name + U"\"";

        throw CompilationException(token.line_id, token.file_name, message);
    }

    retInst.type = f;
    retInst.line = token.line_id;
    retInst.file_name = token.file_name;

    return retInst;
}

Instruction O::SematicAnalyser::proccessPointerGet(Analyser::Token token)
{
	if (token.childToken.size() != 1) {
		throw CompilationException(token.line_id, token.file_name, U"Pointer-get operator require at least one argument");
	}
	else {
        Instruction inst = proccessInstCall(token.childToken[0]);
        if(inst.name == POINTER_ACCESS_INSTRUCTION_NAME){
            inst.Parameters[0].line = token.line_id;
            inst.Parameters[0].file_name = token.file_name;
            return inst.Parameters[0];
        }
		Analyser::Token typeToken;
		typeToken.token = U"~";
		Analyser::Token instTypeToken;
		instTypeToken.token = dataTypeToString(inst.type, adt);
		typeToken.childToken.push_back(instTypeToken);
		Instruction toRet;
		toRet.name = POINTER_GET_INSTRUCTION_NAME;
		toRet.type = getDataType(typeToken);
		toRet.Parameters.push_back(inst);
        toRet.file_name = token.file_name;
        toRet.line = token.line_id;
		return toRet;
	}
}

Instruction O::SematicAnalyser::getVariableAsInstruction(std::u32string name)
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
	if (token.token == U"~") {
		auto mydt = getDataType(token.childToken[0]);
		std::u32string strRep = dataTypeToString(mydt, adt);
		bool check = adtContains(U"~" + strRep, adt);
		if (check) {
			return stringToDataType(U"~" + strRep, adt);
		}
		else {
			adt.lastId += 1;
			adt.additionalNumber.push_back(adt.lastId);
			adt.additionalName.push_back(U"~" + strRep);
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
		throw CompilationException(token.line_id, token.file_name, U"Pointer-data-access operator require one argument");
	}

	Instruction inst = proccessInstCall(token.childToken[0]);

	std::u32string dtStrReprasentation = dataTypeToString(inst.type, adt);
	if (dtStrReprasentation[0] != '~') {
		throw CompilationException(token.line_id, token.file_name, U"Excepted pointer data-type but got " + dtStrReprasentation);
	}

	Instruction retInst;
	retInst.name = POINTER_ACCESS_INSTRUCTION_NAME;
	retInst.type = stringToDataType(dtStrReprasentation.substr(1, dtStrReprasentation.size()-1), adt);
	retInst.Parameters.push_back(inst);
    retInst.file_name = token.file_name;
    retInst.line = token.line_id;
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
			newC.token = U"\\";
            newC.token  += token.token[i + 1];
			i++;
		}
		else {
			newC.token = token.token[i];
		}
        newC.line_id = token.line_id;
        newC.file_name = token.file_name;
		t.childToken.push_back(newC);
	}
	Analyser::Token newC;
	newC.type = Analyser::Type::Char;
	newC.token= U"\\0";
	t.childToken.push_back(newC);
    t.file_name = token.file_name;
    t.line_id = token.line_id;
	return proccessArrayCreationInstruction(t);
}

Instruction O::SematicAnalyser::proccessWhileCycleInstruction(Analyser::TokenisedFile token)
{
	if (token.name.childToken[0].token == COMMA_OPERATOR) {
		throw CompilationException(token.name.childToken[0].line_id, token.name.childToken[0].file_name, U"While cycle requires exactly one argument");
	}

	Instruction inst = proccessInstCall(token.name.childToken[1]);

	if (inst.type != DataTypes::Boolean) {
		throw CompilationException(token.name.childToken[1].line_id, token.name.childToken[1].file_name, U"While cycle works only with Boolean data-type");
	}

	Instruction toRet;
	toRet.name = WHILE_CYCLE_NAME;
	toRet.type = DataTypes::Void;
	toRet.Parameters.push_back(inst);
    toRet.file_name = token.name.file_name;
    toRet.line = token.name.line_id;

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
		throw CompilationException(token.line_id, token.file_name, U"Unexpected data at array-element-access operator");
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
        toRet.file_name = token.file_name;
        toRet.line = token.line_id;
        return toRet;
    }
    DataTypes originalType = dataSource.type;

	Instruction index = proccessInstCall(token.childToken[1]);
    index.file_name = token.file_name;
    index.line = token.line_id;

	std::u32string reprasentation = dataTypeToString(dataSource.type, adt);
	if (reprasentation[0] != '~') {
		throw CompilationException(token.line_id, token.file_name, U"Array-element-access operator works only with pointer data-type but got " + reprasentation);
	}

    dataSource.type = DataTypes::Integer;

	reprasentation = reprasentation.substr(1, reprasentation.size() - 1);

    Instruction plus;
    plus.Parameters.push_back(dataSource);
    plus.Parameters.push_back(index);
    plus.type = originalType;
    plus.ArithmeticProccess = true;
    plus.name = U"+";
    plus.file_name = token.file_name;
    plus.line = token.line_id;

    Instruction toRet;
    toRet.type = stringToDataType(reprasentation, adt);
    toRet.name = POINTER_ACCESS_INSTRUCTION_NAME;
    toRet.Parameters.push_back(plus);
    toRet.file_name = token.file_name;
    toRet.line = token.line_id;

	return toRet;
}

Instruction O::SematicAnalyser::proccessArrayCreationInstruction(Analyser::Token token)
{
	if (token.forward) {
		if (token.childToken.size() == 0) {
			throw CompilationException(token.line_id, token.file_name, U"Array-creation construction requires at least one argument");
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
					throw CompilationException(token.line_id, token.file_name, U"All elements of array must have one data-type");
				}
			}
		}

		Analyser::Token typeToken;
		typeToken.token = U"~";
		Analyser::Token instTypeToken;
		instTypeToken.token = dataTypeToString(toRet.Parameters[1].type, adt);
		typeToken.childToken.push_back(instTypeToken);
		toRet.type = getDataType(typeToken);

        toRet.file_name = token.file_name;
        toRet.line = token.line_id;
		return toRet;
	}
	else {
		throw CompilationException(token.line_id, token.file_name, U"Not an array-creation construction");
	}
}

Instruction O::SematicAnalyser::proccessReturnCall(Analyser::Token token)
{
    auto arguments = getComma(token.childToken[1]);

	if (returnType != DataTypes::Void && arguments.size() != 1) {
		throw CompilationException(token.line_id, token.file_name, U"Function with non void return data-type requires one argument");
	}

    if(arguments.size() != 0) {
        Instruction toRet = proccessInstCall(arguments[0]);
        if (toRet.type != returnType) {
            throw CompilationException(token.line_id, token.file_name, U"Expected " + dataTypeToString(returnType, adt) + U" but got " +
                    dataTypeToString(toRet.type, adt));
        }

        Instruction inst;
        inst.type = toRet.type;
        inst.Parameters.push_back(toRet);
        inst.name = RETURN_NAME;

        this->returnCalls += 1;
        inst.file_name = token.file_name;
        inst.line = token.line_id;

        return inst;
    }else{
        Instruction inst;
        inst.type = DataTypes::Void;
        inst.name = RETURN_NAME;
        inst.line = token.line_id;
        inst.file_name = token.file_name;
        return inst;
    }

}

Instruction O::SematicAnalyser::proccessIfInstruction(Analyser::TokenisedFile token)
{
	if (token.name.childToken[0].token == COMMA_OPERATOR) {
		throw CompilationException(token.name.childToken[0].line_id, token.name.childToken[0].file_name, U"If construction works only with one argument");
	}

	Instruction inst = proccessInstCall(token.name.childToken[1]);

	if (inst.type != DataTypes::Boolean) {
		throw CompilationException(token.name.childToken[1].line_id, token.name.childToken[0].file_name, U"Boolean type expected but got " +
                dataTypeToString(inst.type, adt));
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

    toRet.line = token.name.line_id;
    toRet.file_name = token.name.file_name;

	return toRet;
}

Instruction O::SematicAnalyser::proccessElseIfInstruction(Analyser::TokenisedFile token)
{
	if (instructions.size() == 0 || ((*(instructions.end() - 1)).name != IF_NAME && (*(instructions.end() - 1)).name != ELIF_NAME)) {
		throw CompilationException(token.name.childToken[0].line_id, token.name.childToken[0].file_name, U"\"if\" or \"else if\" must be used before \"else if\" construction");
	}

	if (token.name.childToken[0].token == COMMA_OPERATOR) {
		throw CompilationException(token.name.childToken[0].line_id, token.name.childToken[0].file_name, U"\"else if\" construction works only with one argument");
	}

	Instruction inst = proccessInstCall(token.name.childToken[1]);

	if (inst.type != DataTypes::Boolean) {
		throw CompilationException(token.name.childToken[1].line_id, token.name.childToken[1].file_name, U"Expected Boolean data-type but got " +
                dataTypeToString(inst.type, adt));
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

    toRet.file_name = token.name.file_name;
    toRet.line = token.name.line_id;

	return toRet;
}

Instruction O::SematicAnalyser::proccessElseInstruction(Analyser::TokenisedFile token)
{
	if (instructions.size() == 0 || ((*(instructions.end() - 1)).name != IF_NAME && (*(instructions.end() - 1)).name != ELIF_NAME)) {
        throw CompilationException(token.name.line_id, token.name.file_name, U"\"if\" or \"else if\" must be used before \"else\" construction");
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

    toRet.file_name = token.name.file_name;
    toRet.line = token.name.line_id;

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
        throw CompilationException(token.name.childToken[0].line_id, token.name.childToken[0].file_name, U"Unexpected data at function definition field");
    }

    auto type = getDataType(funcFirstDescriptor.childToken[0]);

    Function func;

    if(isExtern){
        func.IsExtern = true;
    }

    if (type == DataTypes::Error) {
        std::u32string exceptionMessage =
                U"Unexpected return data-type at function definition \"" + funcFirstDescriptor.token + U"\"";
        throw CompilationException(funcFirstDescriptor.line_id, funcFirstDescriptor.file_name, exceptionMessage);
    }

    func.returnType = type;

    if (funcFirstDescriptor.type == Analyser::Type::ServiceName) {
        auto nameField = funcFirstDescriptor.childToken[1];
        bool isOperator = false;
        if (nameField.type == O::Analyser::Type::ServiceName
            && nameField.childToken.size() == 1
            && nameField.token== U"operator") {
            std::u32string resultToken = nameField.childToken[0].token.substr(1, nameField.childToken[0].token.size() - 2);
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
                std::u32string message = U"Initialisation definition expected";
                throw CompilationException(newArg.line_id, newArg.file_name, message);
            }

            auto funcArgType = getDataType(newArg.childToken[0]);
            if (funcArgType == DataTypes::Error) {
                std::u32string message =
                        U"Unrecognised data type at function argument \"" + newArg.childToken[0].token + U"\"";
                throw CompilationException(newArg.line_id, newArg.file_name, message);
            }
            Variable v;
            v.name = newArg.childToken[1].token;
            v.type = funcArgType;

            if (contains(func.arguments, v)) {
                throw CompilationException(newArg.line_id, newArg.file_name, U"Argument already defined");
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
            subSematicAnalyser.enumerations = std::map<std::u32string, Instruction>(enumerations);

            for (auto elem: func.arguments) {
                subSematicAnalyser.variables.push_back(elem);
            }
            for (auto elem: token.subToken) {
                subSematicAnalyser.ProcessToken(elem);
            }

            if (func.returnType != DataTypes::Void && subSematicAnalyser.returnCalls == 0) {
                std::u32string message = U"Require at least one return call at \"" + func.name + U"\" function";
                throw CompilationException(funcFirstDescriptor.line_id, funcFirstDescriptor.file_name, message);
            }

            func.variables = subSematicAnalyser.variablesCreatedAtThatField;
            func.body = subSematicAnalyser.instructions;
        }
        if (containsFunction(func.name, dt) != DataTypes::Error) {
            std::u32string message = U"Function redefinition \"" + func.name + U"\"";
            throw CompilationException(funcFirstDescriptor.line_id, funcFirstDescriptor.file_name, message);
        }
        if (!isExtern) {
            if (isOperator) {
                if (func.arguments.size() != 2) {
                    throw CompilationException(funcFirstDescriptor.line_id, funcFirstDescriptor.file_name, U"Operator definition works only with two arguments");
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
        throw CompilationException(funcFirstDescriptor.line_id, funcFirstDescriptor.file_name, U"Unexpected data at function definition field");
    }
    toRet.name = func.name;
    toRet.type = func.returnType;
    toRet.file_name = token.name.file_name;
    toRet.line = token.name.line_id;

    return toRet;
}

Instruction O::SematicAnalyser::proccessVarInstruction(Analyser::Token token, bool isExtern, bool isGlobal)
{
	Instruction toRet;
	toRet.IsVariable = true;
	
	if (token.childToken.size() == 1) {
        if(token.childToken[0].token != UNIT_DEFINITION_TOKEN){
            std::u32string noDefinitionUnit = U"No unit definition at variable definition";
            throw CompilationException(token.line_id, token.file_name, noDefinitionUnit);
        }
		toRet.name = token.childToken[0].childToken[1].token;
		if (containsVariable(toRet.name)) {
			std::u32string message = (U"Variable with name " + toRet.name + U" already exists");
			throw CompilationException(token.line_id, token.file_name, message);
		}
		std::u32string invalidDatatype = U"Invalid data type name " + token.childToken[0].childToken[0].token;

		auto getType = getDataType(token.childToken[0].childToken[0]);

		if (getType == DataTypes::Error) {
			throw CompilationException(token.line_id, token.file_name, invalidDatatype);
		}

		toRet.type = getType;

		Variable v;
		v.name = toRet.name;
		v.type = toRet.type;
		v.isGlobal = isGlobal;
		variables.push_back(v);
        if(!isExtern) {
            variablesCreatedAtThatField.push_back(v);
            if(dataTypeIsStructure(getType)){
                Structure structure = containsStructureByDataType(getType).second;
                if(structure.variables.size() != 0){
                    Analyser::Token size;
		    std::wstring wSize = std::to_wstring(structure.variables.size());
		    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
		    std::u32string uSize = converter.from_bytes((char*)wSize.c_str());
                    size.token = uSize;
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
                    malloc.token = U"()";
                    malloc.type = Analyser::Type::MathematicalOperator;
                    malloc.forward = true;
                    malloc.childToken = {mallocName, parameters};

                    Analyser::Token destination;
                    destination.token = v.name;
                    destination.type = Analyser::Type::Name;

                    Analyser::Token destination_pointer;
                    destination_pointer.token = U"@";
                    destination_pointer.type = Analyser::Type::MathematicalOperator;
                    destination_pointer.forward = true;
                    destination_pointer.childToken = {destination};

                    Analyser::Token structureMallocToken;
                    structureMallocToken.token = MATH_SET;
                    structureMallocToken.type = Analyser::Type::MathematicalOperator;
                    structureMallocToken.twoSided = true;
                    structureMallocToken.childToken = {destination_pointer, malloc};

                    instructions.push_back(proccessInstCall(structureMallocToken));
                }
            }
        }
	}
	else {
		throw CompilationException(token.line_id, token.file_name, U"Unexpected data at variable definition");
	}

    Instruction pointer;
    pointer.Parameters = {toRet};
    pointer.name = POINTER_GET_INSTRUCTION_NAME;
    Analyser::Token dt_token;
    dt_token.token = U"~";
    dt_token.childToken = {Analyser::Token()};
    dt_token.childToken[0].token = dataTypeToString(toRet.type, adt);
    pointer.type = getDataType(dt_token);
    pointer.file_name = token.file_name;
    pointer.line = token.line_id;

	return pointer;
}

Instruction O::SematicAnalyser::ProcessToken(Analyser::TokenisedFile token, bool add)
{
	Instruction inst;

    auto containsLabel = this->containsLabel(token.name.token);

    if(token.name.token == VAR_CREATION_NAME){
        inst = proccessVarInstruction(token.name);
    }else if(token.name.token == GLOBAL_CREATION_NAME){
   	inst = proccessVarInstruction(token.name, false, true);	
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
        sa.definedLabels = std::vector<std::pair<std::u32string, std::vector<Analyser::TokenisedFile>>>(this->definedLabels);
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

    inst.line = token.name.line_id;
    inst.file_name = token.name.file_name;

    if(add && !inst.IsVariable && inst.name != U"UNSIGNED_TOKEN") {
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

	throw CompilationException(-1, U"system_info", U"No main-flow field");
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
        throw CompilationException(token.name.childToken[0].line_id, token.name.childToken[0].file_name, U"Data-type already defined");
    }

    for(auto elem : token.subToken){
        if(elem.name.token == UNIT_DEFINITION_TOKEN){
            Variable v;
            if(elem.name.childToken.size() != 2){
                throw CompilationException(elem.name.line_id, elem.name.file_name, U"Unexpected data at structure element field");
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
            me.token = U"__init__";
            Analyser::Token dt;
            dt.type = Analyser::Type::Name;
            dt.token = newStruct.name;
            Analyser::Token meName;
            meName.token = U"me";
            meName.type = Analyser::Type::Name;
            me.childToken = {dt, meName};

            if(tf.name.childToken[1].token == U""){
                tf.name.childToken[1] = me;
            }else{
                Analyser::Token comma;
                comma.token = COMMA_OPERATOR;
                comma.type = Analyser::Type::MathematicalOperator;
                comma.forward = true;
                comma.childToken = {me, tf.name.childToken[1]};
                tf.name.childToken[1] = comma;
            }

            proccessFuncInstrucion(tf, tf.name.childToken[0].token == U"extern");
        }else{
            throw CompilationException(newMethod.name.line_id, newMethod.name.file_name, U"Expected function definition");
        }
    }

    Instruction toRet;

    toRet.name = newStruct.name;
    toRet.type = newStruct.myDt;
    toRet.file_name = token.name.file_name;
    toRet.line = token.name.line_id;

    return toRet;
}

Instruction O::SematicAnalyser::proccessInstCall(Analyser::Token token) {
    Instruction res;
    res.line = token.line_id;
    res.file_name = token.file_name;

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
                    std::u32string firstDt = dataTypeToString(t1.type, adt);
                    if (firstDt[0] == '~' && stringToDataType(firstDt.substr(1, firstDt.size() - 1), adt) == t2.type) {
                        res.name = U"SET_VALUE";
                        res.ArithmeticProccess = true;
                        res.type = DataTypes::ServiceInstruction;
                        res.Parameters = {t1, t2};
                        return res;
                    }else{
                        throw CompilationException(token.line_id, token.file_name, U"Excepted " + dataTypeToString(t1.type, adt) + U" data-type but got " +
                                dataTypeToString(t2.type, adt));
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
                ppToken.token = U"-" + ppToken.token;
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
        res.line = token.line_id;
        res.file_name = token.file_name;
        return res;
    }
    else if (token.type == Analyser::Type::Number) {
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        if(isStringEndsWith(token.token, U"f") || isStringEndsWith(token.token, U"F")){
            res.name = token.token.substr(0, token.token.size() - 1);
            res.type = DataTypes::FloatingPoint;
        }
        else if(isStringEndsWith(token.token, U"c") || isStringEndsWith(token.token, U"C")){
            if(token.token.find('.') != -1){
                throw CompilationException(token.line_id, token.file_name, U"Floating-Point do not allowed with \"C\" numeric postfix");
            }
	        std::string num = converter.to_bytes(token.token);
            res.name = (char)(std::stoi(num));
            res.type = DataTypes::Character;
        }
        else if(isStringEndsWith(token.token, U"i") || isStringEndsWith(token.token, U"I")){
            if(token.token.find('.') != -1){
                throw CompilationException(token.line_id, token.file_name, U"Floating-Point do not allowed with \"I\" numeric postfix");
            }
	        std::string num = converter.to_bytes(token.token);
            res.name = std::stoi(num);
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
            sa.definedLabels = std::vector<std::pair<std::u32string, std::vector<Analyser::TokenisedFile>>>(this->definedLabels);
            Analyser::TokenisedFile tf;
            tf.subToken = containsLabel.second;
            tf.name = {};
            tf.name.token = MAIN_FLOW_NAME;
            sa.ProccessTokenisedFile(tf);
            res = sa.instructions[0];
        }
        else {
            std::u32string message = U"Undefined name \"" + token.token + U"\"";
            throw CompilationException(token.line_id, token.file_name, message);
        }

    }
    res.file_name = token.file_name;
    res.line = token.line_id;
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
        if(token.token == U""){
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

bool O::SematicAnalyser::isExternFunction(std::u32string name, std::vector<DataTypes> dt) {
    for(auto function: functions){
        if(function.name == name && function.getArgumentsDataTypes() == dt){
            return function.IsExtern;
        }
    }
    return false;
}

Instruction O::SematicAnalyser::processEnumeration(O::Analyser::TokenisedFile tokenFile) {
    for(auto elem : tokenFile.subToken){
        auto replaceInst = proccessInstCall(elem.name.childToken[1]);
        auto name = elem.name.childToken[0].token;

        enumerations.insert({name, replaceInst});
    }

    return {};
}

Instruction O::SematicAnalyser::processElementCall(O::Analyser::Token token) {

    O::Analyser::Token converted;
    converted.token = U"()";
    converted.forward = true;

    if(token.childToken[0].childToken[0].token != U"__init__"){
        throw CompilationException(token.line_id, token.file_name, U"Unexpected data at function-call");
    }

    O::Analyser::Token me = token.childToken[0].childToken[0].childToken[0];
    O::Analyser::Token funName = token.childToken[0].childToken[0].childToken[1];

    if(token.childToken[0].childToken[1].token == U""){
        converted.childToken = {funName, me};
    }else{
        O::Analyser::Token comma;
        comma.token = U",";
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
    std::vector<std::pair<std::u32string, std::vector<Analyser::TokenisedFile>>> definedLabels =
            std::vector<std::pair<std::u32string, std::vector<Analyser::TokenisedFile>>>(this->definedLabels);

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
    this->enumerations = std::map<std::u32string, Instruction>(origin->enumerations);
    this->definedStructures = std::vector<Structure>(origin->definedStructures);
    this->templates = std::map<std::u32string, std::vector<Analyser::TokenisedFile>>(origin->templates);
    this->definedLabels = std::vector<std::pair<std::u32string, std::vector<Analyser::TokenisedFile>>>(origin->definedLabels);
}

std::pair<bool, std::vector<O::Analyser::TokenisedFile>> O::SematicAnalyser::containsLabel(std::u32string label) {
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
