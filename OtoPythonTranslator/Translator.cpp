#include "Translator.h"
#include "pch.h"

#define POINTER_ACCESS_INSTRUCTION_NAME "GET_POINTER_CONTENT"
#define POINTER_GET_INSTRUCTION_NAME "GET_POINTER"

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
        return "[false]";
    case DataTypes::Character:
        return "[\"A\"]";
    case DataTypes::Error:
        return "";
    default:
        return "[0]";
    }
}

std::string Translator::handleVarCreation(Variable v)
{
    return v.name + "=" + getDefaultInitialisator(v.type) + "\n";
}

std::string Translator::proccessInstruction(Instruction inst, bool funcAdd)
{
    if (inst.type == DataTypes::ServiceInstruction) {
        if (inst.name == "SET_VALUE") {
            return proccessInstruction(inst.Parameters[0]) + "=" + proccessInstruction(inst.Parameters[1]) + "\n";
        }
    }
    else if (inst.IsVariable) {
        return inst.name + "[0]";
    }
    else {
        if (inst.IsFunction) {
            std::string toRet = inst.name + "(";
            for (auto v : inst.Parameters) {
                toRet += "[" + proccessInstruction(v) + "]";
                toRet += ",";
            }
            if (inst.Parameters.size() != 0) {
                toRet = toRet.substr(0, toRet.size() - 1);
            }
            toRet += ")";
            toRet = funcAdd ? toRet + "\n" : toRet;
            return toRet;
        }
        else if (inst.ArithmeticProccess) {
            if (inst.name == "?") {
                inst.name = "==";
            }
            return "(" + proccessInstruction(inst.Parameters[0]) + ")" + inst.name + "(" + proccessInstruction(inst.Parameters[1]) + ")";
        }
        else if (inst.name == "if") {
            std::string toRet;
            toRet += "if " + proccessInstruction(inst.Parameters[0]) + ":\n";
            level += 1;
            if (inst.Parameters.size() == 1) {
                toRet += getPrefix() + "pass\n";
            }
            else {
                for (int i = 1; i < inst.Parameters.size(); i++) {
                    toRet += getPrefix() + proccessInstruction(inst.Parameters[i]);
                }
            }
            level -= 1;
            return toRet;
        }
        else if (inst.name == "else if") {
            std::string toRet;
            toRet += "elif " + proccessInstruction(inst.Parameters[0]) + ":\n";
            level += 1;
            if (inst.Parameters.size() == 1) {
                toRet += getPrefix() + "pass\n";
            }
            else {
                for (int i = 1; i < inst.Parameters.size(); i++) {
                    toRet += getPrefix() + proccessInstruction(inst.Parameters[i]);
                }
            }
            level -= 1;
            return toRet;
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
                    toRet += getPrefix() + proccessInstruction(inst.Parameters[i]);
                }
            }
            level -= 1;
            return toRet;
        }
        else if (inst.name == "return") {
            return "return " + proccessInstruction(inst.Parameters[0]) + "\n";
        }
        else if (inst.name == "TRUE") {
            return "True";
        }
        else if (inst.name == "FALSE") {
            return "False";
        }
        else if (inst.name == POINTER_GET_INSTRUCTION_NAME) {
            std::string r = proccessInstruction(inst.Parameters[0]);
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
            return proccessInstruction(inst.Parameters[0]) + "[0]";
        }
        else {
            return inst.name;
        }
    }
    return std::string();
}

std::string Translator::proccessFunction(Function f)
{
    std::string ret = "def " + f.name + "(";

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
        ret += getPrefix() + handleVarCreation(var);
    }

    for (auto inst : f.body) {
        ret += getPrefix() + proccessInstruction(inst, true);
    }

    level -= 1;

    return ret;
}

void Translator::TranslateFile(File f)
{
    for (auto var : f.variables) {
        file += handleVarCreation(var);
    }

    for (auto func : f.functions) {
        file += proccessFunction(func);
    }

    for (auto inst : f.instructions) {
        file += proccessInstruction(inst, true);
    }
}

std::string Translator::getF()
{
    return file;
}
