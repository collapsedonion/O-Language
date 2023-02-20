#pragma once

#ifdef OLEXICALANALYSER_EXPORTS
#define OLEXICALANALYSER_EXPORTS __declspec(dllexport)
#else
#define OLEXICALANALYSER_EXPORTS __declspec(dllimport)
#endif // def OLEXICALANALYSER_EXPORTS


#include <vector>
#include <string>

namespace O {
	static bool operator & (std::string str1, std::string str2);

	class Analyser
	{
	public:
		enum class Type {
			ServiceName,
			MathematicalOperator,
			Number,
			Name,
			StringLiteral
		};

		struct Token {
		public:
			Type type;
			std::string token;
			bool twoSided = false;
			bool forward = false;
			std::vector<Token> childToken;
		};

		struct StructurisedFile {
			std::string name;
			std::vector<StructurisedFile> subFile;
		};

		struct TokenisedFile
		{
			Token name;
			std::vector<TokenisedFile> subToken;
		};

	private:
		static char mathOperators[];
		static char unarMathOperators[];
		static std::string defaultServiceNames[];

	private:
		static int charNotInBrackets(std::string str, char c);
		static int charNotInQuets(std::string str, char c);
		static int charNotInFunction(std::string str, char c);
		static std::pair<bool, std::pair<std::string, std::string>> doubleBracketOperator(std::string str, char left, char right);
		static int stringNotInFunction(std::string str, std::string toFind);
		static bool isDefaultServiceName(std::string str);
		static bool isNumber(std::string str);
		static bool isString(std::string str);
		static std::string removeBrackes(std::string str);
		static std::pair<std::string, std::string> sliceString(std::string str, int slicePoint);
		static std::string removeSpaceBars(std::string str);

		static Token getMathematicExpression(std::string str);

		static Token ProccessNameOrCreation(std::string str);

		static std::pair<bool, std::pair<std::string, std::string>> getEqulitySign(std::string str);

	public:
		static Token StringToTree(std::string str);
		static StructurisedFile StructuriseFile(std::string str, std::string name = "___MAIN___");
		static TokenisedFile TokeniseFile(StructurisedFile sf);
	};
}