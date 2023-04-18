#pragma once

#include <vector>
#include <string>
#include <BaseTypes.h>

namespace O {
	static bool operator & (std::u32string str1, std::u32string str2);

	class Analyser
	{
	public:
		enum class Type {
			ServiceName,
			MathematicalOperator,
			Number,
			Name,
			StringLiteral,
			Char,
		};

		struct Token {
		public:
			Type type;
			std::u32string token;
			std::u32string file_name;
            int line_id;
			bool twoSided = false;
			bool forward = false;
			std::vector<Token> childToken;
		};

        enum class OperatorType{
            Unary,
            Binary,
            Scope,
        };

        struct Operator{
        public:
            std::u32string name;
            std::u32string postAnalyseName;
            OperatorType operatorType;
        };

		struct StructurisedFile {
			std::u32string name;
			std::u32string file_name;
            int line_id;
			std::vector<StructurisedFile> subFile;
		};

		struct TokenisedFile
		{
			Token name;
			std::vector<TokenisedFile> subToken;
		};

	private:
        static Operator mathOperatorMaxPriority[];
        static Operator mathOperatorMiddlePriority[];
        static Operator mathOperatorLowPriority[];
        static Operator mathOperatorUnary[];
        static std::u32string numericPostfix[];
		static std::u32string defaultServiceNames[];

	private:
		static int charNotInBrackets(std::u32string str, char32_t c);
		static int charNotInQuets(std::u32string str, char32_t c);
		static int charNotInFunction(std::u32string str, char32_t c);
		static std::pair<bool, std::pair<std::u32string, std::u32string>> doubleBracketOperator(std::u32string str, char32_t left, char32_t right);
		static int stringNotInFunction(std::u32string str, std::u32string toFind);
		static bool isDefaultServiceName(std::u32string str);
		static bool isNumber(std::u32string str);
		static bool isString(std::u32string str);
        static bool isNumericPostfix(std::u32string str);
		static std::u32string removeBrackes(std::u32string str);
		static std::pair<std::u32string, std::u32string> sliceString(std::u32string str, int slicePoint);
        static std::pair<bool, Token> getOperator(const std::u32string& str, const Operator& anOperator, int line_id, std::u32string file_name);
		static std::u32string removeSpaceBars(std::u32string str);

		static Token getMathematicExpression(std::u32string str, int line_id, std::u32string file_name);

		static Token ProccessNameOrCreation(std::u32string str, int line_id, std::u32string file_name);

	public:
		static Token StringToTree(std::u32string str, int line_id, std::u32string file_name);
		static StructurisedFile StructuriseFile(std::u32string str, std::u32string name = U"___MAIN___", int line = 1, std::u32string file_name = U"NONE_FILE");
		static TokenisedFile TokeniseFile(StructurisedFile sf);
	};
}
