#pragma once
#include <string>
#include <vector>
#include <exception>
#include <locale>
#include <codecvt>

enum class DataTypes {
	ServiceInstruction = 0,
	MathematicalOperator = 1,
	Void = 2,
	Integer = 3,
	FloatingPoint = 4,
	Boolean = 5,
	Character = 6,
	Error = 7
};

struct AdditionalDataType {
	int lastId = 7;
	std::vector<int> additionalNumber;
	std::vector<std::u32string> additionalName;
    std::vector<int> temp;
    std::vector<std::u32string> name_temp;
};

inline DataTypes stringToDataType(std::u32string str, AdditionalDataType adt) {

	if (str == U"int") {
		return DataTypes::Integer;
	}
	else if (str == U"bool") {
		return DataTypes::Boolean;
	}
	else if (str == U"void") {
		return DataTypes::Void;
	}
	else if (str == U"float")
	{
		return DataTypes::FloatingPoint;
	}
	else if (str == U"char") {
		return DataTypes::Character;
	}
	else {
		for (int i = adt.lastId - 8; i >= 0; i--) {
			if (adt.additionalName[i] == str) {
				return (DataTypes)adt.additionalNumber[i];
			}
		}
        
        for(int i = adt.temp.size() - 1; i>=0;i--){
            if(adt.name_temp[i] == str){
                return (DataTypes)adt.temp[i];
            }
        }
	}

	return DataTypes::Error;
}

template<typename T>
inline bool contains(std::vector<T> vector, T value) {
	for (auto elem : vector) {
		if (elem == value) {
			return true;
		}
	}

	return false;
}

inline bool containsChar(std::u32string str, wchar_t c) {
	for (auto elem : str) {
		if (elem == c) {
			return true;
		}
	}

	return false;
}

inline DataTypes getTypeOfNumber(std::u32string str) {
	if (containsChar(str, '.')) {
		return DataTypes::FloatingPoint;
	}

	return DataTypes::Integer;
}

inline std::u32string dataTypeToString(DataTypes dt, AdditionalDataType adt = AdditionalDataType(), bool code_formated = false, bool name_formated = false) {
	switch (dt)
	{
	case DataTypes::ServiceInstruction:
		return U"___SI___";
	case DataTypes::MathematicalOperator:
		return U"___MO___";
	case DataTypes::Void:
		return U"void";
	case DataTypes::Integer:
		return U"int";
	case DataTypes::FloatingPoint:
		return U"float";
	case DataTypes::Boolean:
		return U"bool";
	case DataTypes::Character:
		return U"char";
	case DataTypes::Error:
		return U"ERROR";
	default:
		for (int i = adt.lastId - 8; i >= 0; i--) {
			if (adt.additionalNumber[i] == (int)dt) {
                if(!code_formated && !name_formated){
                    return adt.additionalName[i];
                }
                
                auto to_ret = adt.additionalName[i];
                
                while(to_ret[0] == '~'){
                    to_ret += '~';
                    to_ret = to_ret.substr(1, to_ret.size() - 1);
                }
                
                if(name_formated){
                    for(int i = 0; i < to_ret.size(); i++){
                        
                        if(to_ret[i] == '~'){
                            to_ret[i] = 'p';
                            continue;
                        }
                        
                        if(to_ret[i] == '('){
                            to_ret = to_ret.replace(i, 1, U"\\CBS");
                            continue;
                        }
                        
                        if(to_ret[i] == ')'){
                            to_ret = to_ret.replace(i, 1, U"\\CBE");
                            continue;
                        }
                        
                        if(to_ret[i] == '['){
                            to_ret = to_ret.replace(i, 1, U"\\SBS");
                            continue;
                        }
                        
                        if(to_ret[i] == ']'){
                            to_ret = to_ret.replace(i, 1, U"\\SBE");
                            continue;
                        }
                    }
                }
                
                return(to_ret);
			}
		}
        for(int i = adt.temp.size() - 1; i>=0;i--){
            if(adt.additionalNumber[i] == (int)dt){
                if(!code_formated && !name_formated){
                    return adt.name_temp[i];
                }
                
                auto to_ret = adt.name_temp[i];
                
                while(to_ret[0] == '~'){
                    to_ret += '~';
                    to_ret = to_ret.substr(1, to_ret.size() - 2);
                }
                
                if(name_formated){
                    for(int i = 0; i < to_ret.size(); i++){
                        if(to_ret[i] == '~'){
                            to_ret[i] = 'p';
                            continue;
                        }
                        
                        if(to_ret[i] == '('){
                            to_ret = to_ret.replace(i, 1, U"\\CBS");
                            continue;
                        }
                        
                        if(to_ret[i] == ')'){
                            to_ret = to_ret.replace(i, 1, U"\\CBE");
                            continue;
                        }
                        
                        if(to_ret[i] == '['){
                            to_ret = to_ret.replace(i, 1, U"\\SBS");
                            continue;
                        }
                        
                        if(to_ret[i] == ']'){
                            to_ret = to_ret.replace(i, 1, U"\\SBE");
                            continue;
                        }
                    }
                }
                
                return(to_ret);
            }
        }
		return U"ERROR";
	}
}

inline bool adtContains(std::u32string str, AdditionalDataType adt) {
	for (auto elem : adt.additionalName) {
		if (elem == str) {
			return true;
		}
	}
    
    for(auto elem : adt.name_temp){
        if(elem == str){
            return true;
        }
    }

	return false;
}

struct Variable {
	DataTypes type = DataTypes::Error;
	bool isGlobal = false;
	std::u32string name = U"";
};

struct Operator {
	std::u32string op;
	DataTypes left;
	DataTypes right;
	DataTypes resultType;

	Operator() {
		op = U"";
		left = DataTypes::Error;
		right = DataTypes::Error;
	}

	Operator(std::u32string op, DataTypes left, DataTypes right, DataTypes resultType) {
		this->op = op;
		this->left = left;
		this->right = right;
		this->resultType = resultType;
	}
};

struct Structure{
    std::u32string name;
    DataTypes myDt;
    std::vector<Variable> variables;
    std::vector<Variable> method_table;
};

struct Instruction {
	std::u32string name = U"UNSIGNED_TOKEN";
	DataTypes type = DataTypes::Error;
	bool ArithmeticProccess = false;
	bool IsVariable = false;
	bool IsFunction = false;
	int line = 0;
	std::u32string file_name;
	std::vector<Instruction> Parameters;
};

struct Function {
	std::u32string name;
	DataTypes returnType;
	std::vector<Variable> arguments;
	std::vector<Variable> variables;
	std::vector<Instruction> body;
    bool IsExtern = false;

	inline std::vector<DataTypes> getArgumentsDataTypes() {
		std::vector<DataTypes> dt;
		for (auto elem : arguments) {
			dt.push_back(elem.type);
		}
		return dt;
	}
};

class CompilationException : std::exception{
public:
    int line;
    std::u32string description;
    std::u32string file_name;

public:
    CompilationException(int line, std::u32string file_name, std::u32string description){
        this->line = line;
        this->description = std::move(description);
	this->file_name = file_name;
    }

public:
    std::u32string getText(){
		std::string lineId = std::to_string(line);
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
		std::u32string line32 = converter.from_bytes((char*)lineId.c_str());
        return U"Critical error at file: " + file_name + U":" + line32 + U"\n\t--" + description + U"\n";
    }
};

struct File {
	AdditionalDataType adtTable;
	std::vector<Variable> variables;
	std::vector<Function> functions;
	std::vector<Instruction> instructions;
    std::vector<Operator> operators;
    std::vector<Structure> structures;
};

inline bool isStringEndsWith(std::u32string wStr, std::u32string ending){
    if(wStr.size() < ending.size()){
        return false;
    }

    for(int i = ending.size() - 1; i >= 0; i--){
        if(wStr[i+wStr.size() - ending.size()] != ending[i]){
            return false;
        }
    }

    return true;
}

template <typename T>
inline bool operator == (std::vector<T> v1, std::vector<T> v2) {

	if (v1.size() != v2.size()) {
		return false;
	}

	for (int i = 0; i < v2.size(); i++) {
		if (!(v1[i] == v2[i])) {
			return false;
		}
	}

	return true;
}

inline bool operator == (Variable v1, Variable v2) {
	return v1.name == v2.name && v1.type == v2.type;
}

inline bool isNumber(std::u32string str){
    
    for(auto elem : str){
        if(!isdigit(elem))
        {
            return false;
        }
    }
    
    return true;
}

inline DataTypes getLiteralType(std::u32string type){
    if(type[0] == '\'' && type[type.size() - 1] == '\''){
        return DataTypes::Character;
    }else if(type == U"TRUE" || type == U"FALSE"){
        return DataTypes::Boolean;
    }else if(type.find('.') == 1){
        return DataTypes::FloatingPoint;
    }else if(isNumber(type)){
        return DataTypes::Integer;
    }
    
    return DataTypes::Character;
}

namespace std{
    inline std::u32string to_ustring(int val){
        static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convertor;
        return convertor.from_bytes(std::to_string(val));
    }
}

