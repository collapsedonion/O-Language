#pragma once

#include <vector>
#include <string>
#include <BaseTypes.h>

namespace O {
	static bool operator & (std::wstring str1, std::wstring str2);

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
			std::wstring token;
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
            std::wstring name;
            std::wstring postAnalyseName;
            OperatorType operatorType;
        };

		struct StructurisedFile {
			std::wstring name;
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
        static std::wstring numericPostfix[];
		static std::wstring defaultServiceNames[];

	private:
		static int charNotInBrackets(std::wstring str, wchar_t c);
		static int charNotInQuets(std::wstring str, wchar_t c);
		static int charNotInFunction(std::wstring str, wchar_t c);
		static std::pair<bool, std::pair<std::wstring, std::wstring>> doubleBracketOperator(std::wstring str, wchar_t left, wchar_t right);
		static int stringNotInFunction(std::wstring str, std::wstring toFind);
		static bool isDefaultServiceName(std::wstring str);
		static bool isNumber(std::wstring str);
		static bool isString(std::wstring str);
        static bool isNumericPostfix(std::wstring str);
		static std::wstring removeBrackes(std::wstring str);
		static std::pair<std::wstring, std::wstring> sliceString(std::wstring str, int slicePoint);
        static std::pair<bool, Token> getOperator(const std::wstring& str, const Operator& anOperator, int line_id);
		static std::wstring removeSpaceBars(std::wstring str);

		static Token getMathematicExpression(std::wstring str, int line_id);

		static Token ProccessNameOrCreation(std::wstring str, int line_id);

	public:
		static Token StringToTree(std::wstring str, int line_id);
		static StructurisedFile StructuriseFile(std::wstring str, std::wstring name = L"___MAIN___", int line = 1);
		static TokenisedFile TokeniseFile(StructurisedFile sf);
	};
}