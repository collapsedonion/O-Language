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
	std::vector<std::string> additionalName;
};

inline DataTypes stringToDataType(std::string str, AdditionalDataType adt) {

	if (str == "int") {
		return DataTypes::Integer;
	}
	else if (str == "bool") {
		return DataTypes::Boolean;
	}
	else if (str == "void") {
		return DataTypes::Void;
	}
	else if (str == "float")
	{
		return DataTypes::FloatingPoint;
	}
	else if (str == "char") {
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

inline bool containsChar(std::string str, char c) {
	for (auto elem : str) {
		if (elem == c) {
			return true;
		}
	}

	return false;
}

inline DataTypes getTypeOfNumber(std::string str) {
	if (containsChar(str, '.')) {
		return DataTypes::FloatingPoint;
	}

	return DataTypes::Integer;
}

inline std::string dataTypeToString(DataTypes dt, AdditionalDataType adt = AdditionalDataType()) {
	switch (dt)
	{
	case DataTypes::ServiceInstruction:
		return "___SI___";
	case DataTypes::MathematicalOperator:
		return "___MO___";
	case DataTypes::Void:
		return "void";
	case DataTypes::Integer:
		return "int";
	case DataTypes::FloatingPoint:
		return "float";
	case DataTypes::Boolean:
		return "bool";
	case DataTypes::Character:
		return "char";
	case DataTypes::Error:
		return "ERROR";
	default:
		for (int i = 0; i < adt.lastId - 7; i++) {
			if (adt.additionalNumber[i] == (int)dt) {
				return adt.additionalName[i];
			}
		}
		return "ERROR";
	}
}

inline bool adtContains(std::string str, AdditionalDataType adt) {
	for (auto elem : adt.additionalName) {
		if (elem == str) {
			return true;
		}
	}

	return false;
}

struct Variable {
	DataTypes type;
	std::string name;
};

struct Operator {
	std::string op;
	DataTypes left;
	DataTypes right;
	DataTypes resultType;

	Operator() {
		op = "";
		left = DataTypes::Error;
		right = DataTypes::Error;
	}

	Operator(std::string op, DataTypes left, DataTypes right, DataTypes resultType) {
		this->op = op;
		this->left = left;
		this->right = right;
		this->resultType = resultType;
	}
};

struct Instruction {
	std::string name = "UNSIGNED_TOKEN";
	DataTypes type = DataTypes::Error;
	bool ArithmeticProccess = false;
	bool IsVariable = false;
	bool IsFunction = false;
	std::vector<Instruction> Parameters;
};

struct Function {
	std::string name;
	DataTypes returnType;
	std::vector<Variable> arguments;
	std::vector<Variable> variables;
	std::vector<Instruction> body;

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
};

typedef std::string(__stdcall *TranslateFunction)(File);

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