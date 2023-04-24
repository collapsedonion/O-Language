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
		for (int i = 0; i < adt.lastId - 7; i++) {
			if (adt.additionalName[i] == str) {
				return (DataTypes)adt.additionalNumber[i];
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

inline std::u32string dataTypeToString(DataTypes dt, AdditionalDataType adt = AdditionalDataType()) {
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
		for (int i = 0; i < adt.lastId - 7; i++) {
			if (adt.additionalNumber[i] == (int)dt) {
				return adt.additionalName[i];
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
};

struct Instruction {
	std::u32string name = U"UNSIGNED_TOKEN";
	DataTypes type = DataTypes::Error;
	bool ArithmeticProccess = false;
	bool IsVariable = false;
	bool IsFunction = false;
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
