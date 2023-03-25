#pragma once
#include <string>
#include <vector>

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
	std::vector<std::wstring> additionalName;
};

inline DataTypes stringToDataType(std::wstring str, AdditionalDataType adt) {

	if (str == L"int") {
		return DataTypes::Integer;
	}
	else if (str == L"bool") {
		return DataTypes::Boolean;
	}
	else if (str == L"void") {
		return DataTypes::Void;
	}
	else if (str == L"float")
	{
		return DataTypes::FloatingPoint;
	}
	else if (str == L"char") {
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

inline bool containsChar(std::wstring str, wchar_t c) {
	for (auto elem : str) {
		if (elem == c) {
			return true;
		}
	}

	return false;
}

inline DataTypes getTypeOfNumber(std::wstring str) {
	if (containsChar(str, '.')) {
		return DataTypes::FloatingPoint;
	}

	return DataTypes::Integer;
}

inline std::wstring dataTypeToString(DataTypes dt, AdditionalDataType adt = AdditionalDataType()) {
	switch (dt)
	{
	case DataTypes::ServiceInstruction:
		return L"___SI___";
	case DataTypes::MathematicalOperator:
		return L"___MO___";
	case DataTypes::Void:
		return L"void";
	case DataTypes::Integer:
		return L"int";
	case DataTypes::FloatingPoint:
		return L"float";
	case DataTypes::Boolean:
		return L"bool";
	case DataTypes::Character:
		return L"char";
	case DataTypes::Error:
		return L"ERROR";
	default:
		for (int i = 0; i < adt.lastId - 7; i++) {
			if (adt.additionalNumber[i] == (int)dt) {
				return adt.additionalName[i];
			}
		}
		return L"ERROR";
	}
}

inline bool adtContains(std::wstring str, AdditionalDataType adt) {
	for (auto elem : adt.additionalName) {
		if (elem == str) {
			return true;
		}
	}

	return false;
}

struct Variable {
	DataTypes type;
	std::wstring name;
};

struct Operator {
	std::wstring op;
	DataTypes left;
	DataTypes right;
	DataTypes resultType;

	Operator() {
		op = L"";
		left = DataTypes::Error;
		right = DataTypes::Error;
	}

	Operator(std::wstring op, DataTypes left, DataTypes right, DataTypes resultType) {
		this->op = op;
		this->left = left;
		this->right = right;
		this->resultType = resultType;
	}
};

struct Structure{
    std::wstring name;
    DataTypes myDt;
    std::vector<Variable> variables;
};

struct Instruction {
	std::wstring name = L"UNSIGNED_TOKEN";
	DataTypes type = DataTypes::Error;
	bool ArithmeticProccess = false;
	bool IsVariable = false;
	bool IsFunction = false;
	std::vector<Instruction> Parameters;
};

struct Function {
	std::wstring name;
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

struct File {
	AdditionalDataType adtTable;
	std::vector<Variable> variables;
	std::vector<Function> functions;
	std::vector<Instruction> instructions;
    std::vector<Operator> operators;
    std::vector<Structure> structures;
};

typedef std::wstring(__stdcall *TranslateFunction)(File);

inline bool isStringEndsWith(std::wstring wStr, std::wstring ending){
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