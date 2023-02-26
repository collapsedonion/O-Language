#include "Translator.h"
#include "pch.h"

#define POINTER_ACCESS_INSTRUCTION_NAME "GET_POINTER_CONTENT"
#define POINTER_GET_INSTRUCTION_NAME "GET_POINTER"
#define ARRAY_ELEMENT_ACCESS_NAME "ARRAY_ACCESS_INTEGER"
#define ARRAY_CREATION_NAME "ARRAY_INIT"
#define WHILE_CYCLE_NAME "WHILE_CYCLE"

std::string Translator::getPrefix()
{
    std::string toRet;
    for (int i = 0; i < level; i++) {
        toRet += "\t";
    }
    return toRet;
}

std::string Translator::getDefaultInitialisator(DataTypes t)
{
    switch (t)
    {
    case DataTypes::ServiceInstruction:
        return "";
    case DataTypes::MathematicalOperator:
        return "";
    case DataTypes::Void:
        return "[]";
    case DataTypes::Integer:
        return "[0]";
    case DataTypes::FloatingPoint:
        return "[0.0]";
    case DataTypes::Boolean:
        return "[False]";
    case DataTypes::Character:
        return "[\"A\"]";
    case DataTypes::Error:
        return "";
    default:
        return "[0]";
    }
}

std::string Translator::handleVarCreation(Variable v, AdditionalDataType adt)
{
    return "VV" + getType(dataTypeToString(v.type, adt)) + v.name + "=" + getDefaultInitialisator(v.type) + "\n";
}

std::string Translator::proccessInstruction(Instruction inst, AdditionalDataType adt, bool funcAdd)
{
    if (inst.type == DataTypes::ServiceInstruction) {
        if (inst.name == "SET_VALUE") {
            return proccessInstruction(inst.Parameters[0], adt) + "=" + proccessInstruction(inst.Parameters[1], adt) + "\n";
        }
    }
    else if (inst.IsVariable) {
        return "VV" + getType(dataTypeToString(inst.type, adt)) + inst.name + "[0]";
    }
    else {
        if (inst.IsFunction) {
            std::string toRet = generateFunctionCall(inst.name, inst.Parameters, adt);
            toRet = funcAdd ? toRet + "\n" : toRet;
            return toRet;
        }
        else if (inst.ArithmeticProccess) {
            std::vector<DataTypes> argumentsDataTypes;

            for(auto elem : inst.Parameters){
                argumentsDataTypes.push_back(elem.type);
            }

            auto getName = getOperatorFunction(inst.name, argumentsDataTypes);

            if(getName == "") {
                if (inst.name == "?") {
                    inst.name = "==";
                }

                return "(" + proccessInstruction(inst.Parameters[0], adt) + ")" + inst.name + "(" +
                       proccessInstruction(inst.Parameters[1], adt) + ")";
            }else{
                return generateFunctionCall(inst.name, inst.Parameters, adt);
            }
        }
        else if (inst.name == "if") {
            std::string toRet;
            toRet += "if " + proccessInstruction(inst.Parameters[0], adt) + ":\n";
            level += 1;
            if (inst.Parameters.size() == 1) {
                toRet += getPrefix() + "pass\n";
            }
            else {
                for (int i = 1; i < inst.Parameters.size(); i++) {
                    toRet += getPrefix() + proccessInstruction(inst.Parameters[i], adt);
                }
            }
            level -= 1;
            return toRet + "\n";
        }
        else if (inst.name == "else if") {
            std::string toRet;
            toRet += "elif " + proccessInstruction(inst.Parameters[0], adt) + ":\n";
            level += 1;
            if (inst.Parameters.size() == 1) {
                toRet += getPrefix() + "pass\n";
            }
            else {
                for (int i = 1; i < inst.Parameters.size(); i++) {
                    toRet += getPrefix() + proccessInstruction(inst.Parameters[i], adt);
                }
            }
            level -= 1;
            return toRet + "\n";
        }
        else if (inst.name == "else") {
            std::string toRet;
            toRet += "else:\n";
            level += 1;
            if (inst.Parameters.size() == 0) {
                toRet += getPrefix() + "pass\n";
            }
            else {
                for (int i = 0; i < inst.Parameters.size(); i++) {
                    toRet += getPrefix() + proccessInstruction(inst.Parameters[i], adt);
                }
            }
            level -= 1;
            return toRet + "\n";
        }
        else if (inst.name == "return") {
            return "return " + proccessInstruction(inst.Parameters[0], adt) + "\n";
        }
        else if (inst.name == "TRUE") {
            return "True";
        }
        else if (inst.name == "FALSE") {
            return "False";
        }
        else if (inst.name == POINTER_GET_INSTRUCTION_NAME) {
            std::string r = proccessInstruction(inst.Parameters[0], adt);
            if (inst.Parameters[0].IsVariable) {
                r = r.substr(0, r.size() - 3);
                return r;
            }
            else if (inst.Parameters[0].IsFunction) {
                return r;
            }
            else {
                return "[" + r + "]";
            }

        }
        else if (inst.name == POINTER_ACCESS_INSTRUCTION_NAME) {
            return proccessInstruction(inst.Parameters[0], adt) + "[0]";
        }
        else if (inst.name == ARRAY_CREATION_NAME) {
            std::string toRet = "[";
            for (int i = 1; i < inst.Parameters.size(); i++) {
                toRet += proccessInstruction(inst.Parameters[i], adt);

                if (i != inst.Parameters.size() - 1) {
                    toRet += ",";
                }
            }
            return toRet + "]";
        }
        else if (inst.name == ARRAY_ELEMENT_ACCESS_NAME) {
            return proccessInstruction(inst.Parameters[1], adt) + "[" + proccessInstruction(inst.Parameters[0], adt) + "]";
        }
        else if (inst.name == WHILE_CYCLE_NAME) {
            std::string toRet = "while " + proccessInstruction(inst.Parameters[0], adt) + ":\n";
            level += 1;
            if (inst.Parameters.size() == 1) {
                toRet += getPrefix() + "pass\n";
            }
            else {
                for (int i = 1; i < inst.Parameters.size(); i++) {
                    toRet += getPrefix() + proccessInstruction(inst.Parameters[i], adt);
                }
            }
            level -= 1;
            return toRet + "\n";
        }
        else {
            if (inst.type == DataTypes::Character) {
                return "\'" + inst.name + "\'";
            }
            return inst.name;
        }
    }
    return std::string();
}

std::string Translator::proccessFunction(Function f, AdditionalDataType adt)
{
    std::string functionName;

    functionName += getType(dataTypeToString(f.returnType, adt));

    if(f.name.size() == 1){
        switch (f.name[0]) {
            case '*':
                functionName += "mul";
                break;
            case '+':
                functionName += "add";
                break;
            case '-':
                functionName += "minus";
                break;
            case '/':
                functionName += "div";
                break;
            case '&':
                functionName += "and";
                break;
            case '|':
                functionName += "or";
                break;
            case '<':
                functionName += "less";
                break;
            case '>':
                functionName += "greater";
            case '?':
                functionName += "equal";
            default:
                functionName += f.name;
        }
    }else {
        functionName += f.name;
    }

    for(auto elem : f.arguments){
        functionName += getType(dataTypeToString(elem.type, adt));
    }

    std::string ret = "def " + functionName + "(";

    for (auto v : f.arguments) {
        ret += v.name;
        ret += ",";
    }

    if (f.arguments.size() > 0) {
        ret = ret.substr(0, ret.size() - 1);
    }
    ret += "):\n";
    level += 1;

    for (auto var : f.variables) {
        ret += getPrefix() + handleVarCreation(var, adt);
    }

    for (auto inst : f.body) {
        ret += getPrefix() + proccessInstruction(inst, adt, true);
    }

    level -= 1;

    RegisteredFunction rf;
    rf.Oname = f.name;
    rf.compiledName = functionName;
    for(auto elem : f.arguments){
        rf.argumentsDT.push_back(elem.type);
    }

    registeredFunctions.push_back(rf);

    return ret;
}

void Translator::TranslateFile(File f)
{
    registeredOperators = std::vector<Operator>(f.operators);

    for (auto var : f.variables) {
        file += handleVarCreation(var, f.adtTable);
    }

    for (auto func : f.functions) {
        file += proccessFunction(func, f.adtTable);
    }

    for (auto inst : f.instructions) {
        file += proccessInstruction(inst,f.adtTable, true);
    }
}

std::string Translator::getF()
{
    return file;
}

std::string Translator::getRegisteredFunctionName(std::string name, std::vector<DataTypes> argDt) {

    for(auto elem : registeredFunctions){
        if(elem.Oname == name && elem.argumentsDT == argDt){
            return  elem.compiledName;
        }
    }

    return "";
}

std::string Translator::getOperatorFunction(std::string name, std::vector<DataTypes> argDt) {
    if(argDt.size() != 2){
        return "";
    }

    for(auto op : registeredOperators){
        if(op.op == name && op.left == argDt[0] && op.right == argDt[1]){
            return getRegisteredFunctionName(name, argDt);
        }
    }

    return "";
}

std::string Translator::generateFunctionCall(std::string name, std::vector<Instruction> argumnets, AdditionalDataType adt) {
    std::vector<DataTypes> argTypes;

    for(auto elem : argumnets){
        argTypes.push_back(elem.type);
    }

    std::string toRet = getRegisteredFunctionName(name, argTypes) + "(";

    for (auto v : argumnets) {
        toRet += "[" + proccessInstruction(v, adt) + "]";
        toRet += ",";
    }
    if (argumnets.size() != 0) {
        toRet = toRet.substr(0, toRet.size() - 1);
    }
    toRet += ")";
    return toRet;
}

std::string Translator::getType(std::string type) {
    std::string toRet;
    for(auto c : type){
        if(c == '~'){
            toRet += "PP";
        }else{
            toRet += c;
        }
    }

    return toRet;
}

Translator::RegisteredFunction::RegisteredFunction() {

}

Translator::RegisteredFunction::RegisteredFunction(std::string name, std::string OName, std::vector<DataTypes> argsDt) {
    this->Oname = OName;
    this->compiledName = name;
    this->argumentsDT = argsDt;
}
