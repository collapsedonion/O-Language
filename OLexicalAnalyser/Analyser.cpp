#include "Analyser.h"
#include <codecvt>
#include <locale>

std::u32string O::Analyser::numericPostfix[] = {
        U"f",
        U"i",
        U"c",
};

O::Analyser::Operator O::Analyser::mathOperatorMaxPriority[] = {
        {U",", U"math_comma", OperatorType::Binary},
        {U"&=", U"math_assign_and", OperatorType::Binary},
        {U"|=", U"math_assign_or", OperatorType::Binary},
        {U"+=", U"math_assign_add", OperatorType::Binary},
        {U"-=", U"math_assign_sub", OperatorType::Binary},
        {U"*=", U"math_assign_multiply", OperatorType::Binary},
        {U"/=", U"math_assign_divide", OperatorType::Binary},
        {U"%=", U"math_assign_modulo", OperatorType::Binary},
        {U"=", U"math_assign", OperatorType::Binary}

};

O::Analyser::Operator O::Analyser::mathOperatorMiddlePriority[] = {
        {U"|", U"math_or", OperatorType::Binary},
        {U"&", U"math_and", OperatorType::Binary},
        {U"?", U"math_equ", OperatorType::Binary},
        {U"<", U"math_less", OperatorType::Binary},
        {U">", U"math_greater", OperatorType::Binary}
};

O::Analyser::Operator O::Analyser::mathOperatorLowPriority[] {
        {U"-", U"math_subtract", OperatorType::Binary},
        {U"+", U"math_add",OperatorType::Binary},
        {U"*", U"math_multiply", OperatorType::Binary},
        {U"/", U"math_divide", OperatorType::Binary},
        {U"%", U"math_modulo", OperatorType::Binary},
        {U"()", U"scope_default", OperatorType::Scope},
        {U"[]", U"scope_square", OperatorType::Scope},
};

O::Analyser::Operator O::Analyser::mathOperatorUnary[] = {
        {U"@", U"math_get_pointer", OperatorType::Unary},
        {U"~", U"math_get_content", OperatorType::Unary},
        {U"!", U"math_not", OperatorType::Unary},
        {U"-", U"math_unary_minus", OperatorType::Unary},
};

std::u32string O::Analyser::defaultServiceNames[] = {
    U"int",
    U"bool",
    U"float",
    U"char",
    U"void",
    U"false",
    U"true",
    U"while",
    U"malloc",
    U"free",
    U"return",
    U"sizeof"
};

int O::Analyser::charNotInBrackets(std::u32string str, char32_t c)
{
    int level = -1;

    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '(') {
            level++;
            continue;
        }

        if (str[i] == ')') {
            level--;
            continue;
        }

        if (str[i] == c && level == -1) {
            return i;
        }
    }

    return -1;
}

int O::Analyser::charNotInQuets(std::u32string str, char32_t c)
{
    bool inQuets = false;
    bool singleQuets = false;

    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '\"') {
            inQuets = !inQuets;
        }
        
        if(str[i] == '\'' && !inQuets){
            singleQuets = !singleQuets;
        }

        if (str[i] == c && !inQuets && !singleQuets) {
            return i;
        }
    }

    return -1;
}

int O::Analyser::charNotInFunction(std::u32string str, char32_t c)
{
    int level = -1;
    int levelInCube = -1;
    bool inQuets = false;
    bool singleQuets = false;

    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '\"') {
            inQuets = !inQuets;
        }
        
        if(str[i] == '\'' && !inQuets){
            if((singleQuets && i - 1 > 0 && str[i - 1] != '\\') || !singleQuets){
                singleQuets = !singleQuets;
            }
        }

        if (str[i] == '[' && !inQuets && !singleQuets) {
            levelInCube++;
        }

        if (str[i] == ']' && !inQuets && !singleQuets) {
            levelInCube--;
        }

        if (str[i] == '(' && !inQuets && !singleQuets) {
            level++;
            continue;
        }

        if (str[i] == ')' && !inQuets && !singleQuets) {
            level--;
            continue;
        }

        if (str[i] == c && level == -1 && !inQuets && !singleQuets && levelInCube == -1) {
            if(i - 1 >= 0 && i + 1 < str.size() && str[i - 1] == '\'' && str[i + 1] == '\''){
                continue;
            }
            return i;
        }
    }
    return -1;
}

std::pair<bool, std::pair<std::u32string, std::u32string>> O::Analyser::doubleBracketOperator(std::u32string str, char32_t left, char32_t right)
{
    int level = -1;
    if (str.empty() || (*(str.end() - 1)) != right || str[0] == left) {
        return { false, {U"", U""} };
    }

    std::u32string operatorContent = U"";
    std::u32string leftPart;

    std::u32string operatorContentRight = U"";

    bool quets = false;
    bool sq = false;
    
    for (int i = (int)str.size() - 1; i >= 0; i--) {
        if(str[i] == '"') {
            quets = !quets;
        }
        
        if(str[i] == '\'' && !quets){
            sq = !sq;
        }
        
        if (str[i] == right && !quets && !sq) {
            level++;
        }
        else if (str[i] == left && !quets && !sq) {
            level--;
        }
        if (level == -1) {
            leftPart = str.substr(0, i);
            break;
        }
        operatorContent += str[i];
    }

    for (int i = (int)operatorContent.size() - 1; i >= 0; i--) {
        operatorContentRight += operatorContent[i];
    }

    return { true, {leftPart, operatorContentRight.substr(0, operatorContentRight.size() - 1)} };
}

int O::Analyser::stringNotInFunction(std::u32string str, std::u32string toFind)
{
    std::u32string found = str;
    while (true)
    {
        int id = charNotInFunction(found, toFind[0]);

        if (id == -1) {
            return -1;
        }
        else {
            auto sliced = sliceString(found, id - 1);
            auto sliced2 = sliceString(found, id);
            found = sliced2.second;
            if (sliced.second & toFind) {
                return id;
            }
        }
    }
}

bool O::Analyser::isDefaultServiceName(std::u32string str)
{
    for (auto elem : defaultServiceNames){
        if (elem == str) {
            return true;
        }
    }

    return false;
}

bool O::Analyser::isNumber(std::u32string str)
{
    if(str == U""){
        return false;
    }

    for(auto postfix: numericPostfix){
        if(isStringEndsWith(str, postfix)){
            if(isNumber(str.substr(0, str.size() - postfix.size()))) {
                return true;
            }
        }
    }

    for (auto elem : str) {
        if (!std::isdigit(elem) && elem != '.') {
            return false;
        }
    }

    return true;
}

bool O::Analyser::isString(std::u32string str)
{
    return str[0] == '\"' && ((* (str.end() - 1)) == '\"');
}

std::u32string O::Analyser::removeBrackes(std::u32string str)
{
    if (str.size() < 2) {
        return str;
    }
    if (str[0] == '(' && (*(str.end() - 1) == ')')) {
        int level = 0;
        for (int i = 1; i < str.size(); i++) {
            if (str[i] == '(') {
                level++;
            }
            if (str[i] == ')') {
                level--;
            }

            if (level == -1) {
                if (str.size() - 1 == i) {
                    return str.substr(1, str.size() - 2);
                }
                else {
                    return str;
                }
            }
        }
    }

    return str;
}

std::pair<std::u32string, std::u32string> O::Analyser::sliceString(std::u32string str, int slicePoint)
{
    return {str.substr(0, slicePoint), str.substr(slicePoint + 1, str.size() - slicePoint)};
}

std::u32string O::Analyser::removeSpaceBars(std::u32string str)
{
    std::u32string res = U"";

    bool inQuets = false;

    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '\"') {
            inQuets = !inQuets;
        }

        if (inQuets) {
            res += str[i];
        }
        else {
            if (str[i] == ' ' && res.size() == 0) {
                continue;
            }

            if (str[i] != ' ' || ((res.size() != 0 && ((*(res.end() - 1))) != ' '))) {
                res += str[i];
            }
        }
    }

    if (res.size() > 0 && * (res.end() - 1) == ' ') {
        res.pop_back();
    }

    return res;
}

bool O::operator&(std::u32string str1, std::u32string str2)
{
    if (str1.size() < str2.size()) {
        return false;
    }

    for (int i = 0; i < str2.size(); i++) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }

    return true;
}

O::Analyser::Token O::Analyser::getMathematicExpression(std::u32string str, int line_id, std::u32string file_name)
{
    Token t;

    std::pair<bool, Token> processedOperator;

    for(const auto& highOperator : mathOperatorMaxPriority){
        if(highOperator.operatorType == OperatorType::Scope){
            for(const auto& unaryOperator : mathOperatorUnary){
                processedOperator = getOperator(str, unaryOperator, line_id, file_name);
                if(processedOperator.first){
                    return processedOperator.second;
                }
            }
        }
        processedOperator = getOperator(str, highOperator, line_id, file_name);
        if(processedOperator.first){
            return processedOperator.second;
        }
    }

    for(const auto& midOperator : mathOperatorMiddlePriority){
        if(midOperator.operatorType == OperatorType::Scope){
            for(const auto& unaryOperator : mathOperatorUnary){
                processedOperator = getOperator(str, unaryOperator, line_id, file_name);
                if(processedOperator.first){
                    return processedOperator.second;
                }
            }
        }
        processedOperator = getOperator(str, midOperator, line_id, file_name);
        if(processedOperator.first){
            return processedOperator.second;
        }
    }

    for(const auto& lowOperator : mathOperatorLowPriority){
        if(lowOperator.operatorType == OperatorType::Scope){
            for(const auto& unaryOperator : mathOperatorUnary){
                processedOperator = getOperator(str, unaryOperator, line_id, file_name);
                if(processedOperator.first){
                    return processedOperator.second;
                }
            }
        }
        processedOperator = getOperator(str, lowOperator, line_id, file_name);
        if(processedOperator.first){
            return processedOperator.second;
        }
    }

    for(const auto& unaryOperator : mathOperatorUnary){
        processedOperator = getOperator(str, unaryOperator, line_id, file_name);
        if(processedOperator.first){
            return processedOperator.second;
        }
    }

    return t;
}

O::Analyser::Token O::Analyser::ProccessNameOrCreation(std::u32string str, int line_id, std::u32string file_name)
{
    int idOfVar = stringNotInFunction(str, U"var:");
    int idOfGlobal = stringNotInFunction(str, U"global:");
    int idOfFunc = stringNotInFunction(str, U"func");

    Token res;

    if (*(str.end()-1) == '~') {
        res.token = '~';
        res.type = Type::ServiceName;
        res.childToken = { StringToTree(str.substr(0, str.size() - 1), line_id, file_name)};
        res.line_id = line_id;
	    res.file_name = file_name;
        return res;
    }
    if (str & U"extern:"){
        res.token = U"extern";
        res.type = Type::ServiceName;
        int idOfSpace = charNotInFunction(str, ':');
        auto splited = sliceString(str, idOfSpace);
        res.childToken.push_back(StringToTree(splited.second, line_id, file_name));
        res.line_id = line_id;
        res.file_name = file_name;
        return res;
    }

    if (str & U"struct:"){
        res.token = U"structure";
        res.type = Type::ServiceName;
        int idOfSeparator = charNotInFunction(str, ':');
        auto splited = sliceString(str, idOfSeparator);
        res.childToken.push_back(StringToTree(splited.second, line_id, file_name));
        res.line_id = line_id;
        res.file_name = file_name;
        return res;
    }

    if(str & U"template:"){
        res.token = U"template";
        res.type = Type::ServiceName;
        int idOfSeparator = charNotInFunction(str, ':');
        auto splited = sliceString(str, idOfSeparator);
        res.childToken.push_back(StringToTree(splited.second, line_id, file_name));
        res.line_id = line_id;
        res.file_name = file_name;
        return res;
    }
    
    if(str & U"extends:"){
        res.token = U"extend";
        res.type = Type::ServiceName;
        int idOfSeparator = charNotInFunction(str, ':');
        auto splited = sliceString(str, idOfSeparator);
        res.childToken.push_back(StringToTree(splited.second, line_id, file_name));
        res.line_id = line_id;
        res.file_name = file_name;
        return res;
    }
    
    if (idOfVar == 0) {
        int idOfDoubleDot = charNotInFunction(str, ':');
        if (idOfDoubleDot != -1) {
            auto splited = sliceString(str, idOfDoubleDot);
            Token r;
            r.type = Type::ServiceName;
            r.twoSided = true;
            r.token = U"var";
            r.childToken = { StringToTree(splited.second, line_id, file_name)};
            r.line_id = line_id;
	    r.file_name = file_name;
            return r;
        }
    }else if(idOfGlobal == 0){
    	 int idOfDoubleDot = charNotInFunction(str, ':');
         if (idOfDoubleDot != -1) {
            auto splited = sliceString(str, idOfDoubleDot);
            Token r;
            r.type = Type::ServiceName;
            r.twoSided = true;
            r.token = U"global";
            r.childToken = { StringToTree(splited.second, line_id, file_name)};
            r.line_id = line_id;
	    r.file_name = file_name;
            return r;
	 }
    }
    else if (idOfFunc == 0) {
        int idOfDoubleDot = charNotInFunction(str, ':');
        Token Func;
        Func.type = Type::ServiceName;
        Func.token = U"func";
        std::u32string leftFunctionPart = str;
        if (idOfDoubleDot != -1) {
            auto sliced = sliceString(str, idOfDoubleDot);
            leftFunctionPart = sliced.second;
            sliced = sliceString(leftFunctionPart, charNotInFunction(leftFunctionPart, ' '));
            Func.childToken.push_back(StringToTree(sliced.first, line_id, file_name));
            leftFunctionPart = sliced.second;
        }
        else {
            Func.childToken.push_back(StringToTree(U"void", line_id, file_name));
            leftFunctionPart = sliceString(leftFunctionPart, charNotInFunction(leftFunctionPart, ' ')).second;
        }
        if(leftFunctionPart & U"operator"){
            Token name;
            name.token = U"operator";
            name.type = Type::ServiceName;
            int idOfSpace = charNotInFunction(leftFunctionPart, ' ');
            leftFunctionPart = sliceString(leftFunctionPart, idOfSpace).second;
            Token subName;
            subName.type = Type::Name;
            subName.token = leftFunctionPart;
            name.childToken.push_back(subName);
            Func.childToken.push_back(name);
        }else{
            Token name;
            name.token = leftFunctionPart;
            name.type = Type::Name;
            Func.childToken.push_back(name);
        }

        Func.line_id = line_id;
        Func.file_name = file_name;
        return Func;
    }
    else if (str[0] == '[' && (*(str.end() - 1)) == ']') {
        res.token = U"[]";
        res.type = Type::MathematicalOperator;
        res.forward = true;
        std::u32string leftPart = str.substr(1, str.size() - 2);
        res.childToken = {StringToTree(leftPart, line_id, file_name)};
    }
    else {
        if (isDefaultServiceName(str)) {
            res.token = str;
            res.type = Type::ServiceName;
        }
        else if (isNumber(str)) {
            res.token = str;
            res.type = Type::Number;
        }
        else if (isString(str)) {
            res.token = str;
            res.type = Type::StringLiteral;
        }
        else if ((str.size() == 3 || str.size() == 4) && str[0] == '\'' && ((str.size() == 3 && str[2] == '\'') || (str.size() == 4 && str[3] == '\''))){
            res.token = str;
            res.type = Type::Char;
        }
        else {
           /* int idOfBracket = charNotInQuets(str, '(');
            if (idOfBracket != -1) {
                auto sliced = sliceString(str, idOfBracket);
                res.token = sliced.first;
                res.forward = true;
                res.type = Type::Name;
                if (sliced.second.size() > 1) {
                    std::u32string last = sliced.second.substr(0, sliced.second.size() - 1);
                    while (true) {
                        int idOfNext = charNotInFunction(last, ',');
                        if (idOfNext == -1) {
                            res.childToken.push_back(StringToTree(last, line_id, file_name));
                            break;
                        }
                        else {
                            auto slice = sliceString(last, idOfNext);
                            res.childToken.push_back(StringToTree(slice.first, line_id, file_name));
                            last = slice.second;
                        }
                    }
                }
            }
            else {*/
                auto idOfSeparator = charNotInFunction(str, ' ');
                if(idOfSeparator != -1){
                    res.token = U"__init__";
                    auto splited = sliceString(str, idOfSeparator);
                    res.childToken = {StringToTree(splited.first, line_id, file_name), StringToTree(splited.second, line_id, file_name)};
                }else {
                    res.token = str;
                    res.type = Type::Name;
                }
            //}
        }
        
    }
    res.line_id = line_id;
    res.file_name = file_name;
    return res;
}

O::Analyser::Token O::Analyser::StringToTree(std::u32string str, int line_id, std::u32string file_name) {
    std::u32string withOutSpaces = removeSpaceBars(str);
    withOutSpaces = removeBrackes(withOutSpaces);

    if (withOutSpaces.empty()) {
        return {};
    }

    Token result;
    auto t = getMathematicExpression(withOutSpaces, line_id, file_name);
    if (t.token != U"") {
        return t;
    }
    return ProccessNameOrCreation(withOutSpaces, line_id, file_name);

    return result;
}

O::Analyser::StructurisedFile O::Analyser::StructuriseFile(std::u32string str, std::u32string name, int prevLine, std::u32string file_name)
{
    StructurisedFile main;
    main.name = name;
    main.line_id = prevLine;
    main.file_name = file_name;

    StructurisedFile newL;
    std::u32string last;
    std::u32string newBody;

    int level = -1;
    int levelCurly = -1;
    int line = prevLine;
    std::u32string current_file_name = file_name;
    bool inQouets = false;
    bool singleQouets = false;

    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '\"' && levelCurly == -1) {
            inQouets = !inQouets;
        }
        
        if (str[i] == '\'' && levelCurly == -1 && !inQouets) {
            if((singleQouets && i - 1 > 0 && str[i - 1] != '\\') || !singleQouets){
                singleQouets = !singleQouets;
            }
        }

        if (str[i] == '(' && !inQouets && !singleQouets && levelCurly == -1) {
            level++;
        }

        if (str[i] == ')' && !inQouets && !singleQouets && levelCurly == -1) {
            level--;
        }

        if (str[i] == '{' && level == -1 && !inQouets && !singleQouets) {

            levelCurly++;
            if (levelCurly == 0) {
                continue;
            }
        }
        if (str[i] == '}' && level == -1 && !inQouets && !singleQouets) {
            levelCurly--;
            if (levelCurly == -1) {
                main.subFile.push_back(StructuriseFile(newBody, last, line, current_file_name));
                last = U"";
                newBody = U"";
                continue;
            }
        }

        if (levelCurly != -1) {
            newBody += str[i];
            continue;
        }

        if (str[i] == '\n' || str[i] == '\t') {
            continue;
        }

        if (singleQouets || inQouets || level != -1) {
            last += str[i];
        }
        else {
            if (str[i] == ';') {
                if(removeSpaceBars(last) & U"#LINE_ID")
		        {       
		            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
		            std::string num = convert.to_bytes(removeSpaceBars(last).substr(8, last.size()-8));
                    line  = std::stoi(num);
                    last = U"";
		        }else if(removeSpaceBars(last) & U"#FILE_NAME"){
	                current_file_name = removeSpaceBars(last).substr(10, last.size() - 10);
		            last = U"";
		        }else {
                    newL.name = last;
                    newL.line_id = line;
		            newL.file_name = current_file_name;
                    main.subFile.push_back(newL);
                    newL = StructurisedFile();
                    last = U"";
                }
            }
            else {
                last += str[i];
            }
        }
    }

    if (newBody != U"" && removeSpaceBars(last)[0] != '#') {
        main.subFile.push_back(StructuriseFile(newBody, last, line, current_file_name));
        last = U"";
        newBody = U"";
    }

    if (last != U"" && removeSpaceBars(last)[0] != '#') {
        newL.name = last;
        newL.line_id = line;
        newL.file_name = current_file_name;
        main.subFile.push_back(newL);
    }

    return main;
}

O::Analyser::TokenisedFile O::Analyser::TokeniseFile(StructurisedFile sf)
{
    TokenisedFile tf;
    if (sf.name == U"___MAIN___") {
        Token t;
        t.token = U"___MAIN___";
        t.type = Type::ServiceName;
        tf.name = t;
    }
    else {
        auto stt = StringToTree(sf.name, sf.line_id, sf.file_name);
        if(stt.token != U"") {
            tf.name = stt;
        }
    }

    for (auto elem : sf.subFile) {
        auto newF = TokeniseFile(elem);
        if(newF.name.token != U"") {
            tf.subToken.push_back(newF);
        }
    }

    return tf;
}

std::pair<bool, O::Analyser::Token> O::Analyser::getOperator(const std::u32string& str, const Operator& anOperator, int line_id, std::u32string file_name) {

    switch (anOperator.operatorType) {
        case OperatorType::Binary: {
            auto position = stringNotInFunction(str, anOperator.name);
            if (position != -1) {
                auto splitOperator = sliceString(str, position);
                auto left = splitOperator.first;
                auto right = anOperator.name.size() != 1 ? (splitOperator.second.substr(anOperator.name.size(), splitOperator.second.size() - anOperator.name.size() + 1)) : splitOperator.second;
                if (left != U"") {
                    return {true, {Type::MathematicalOperator,
                                   anOperator.name,
                        file_name,
                                   line_id,
                                   true,
                                   false,
                                   {StringToTree(left, line_id, file_name), StringToTree(right, line_id, file_name)}
                    }};
                }
            }
            break;
        }
        case OperatorType::Unary: {
            if (str & anOperator.name) {
                return {true, {
                        Type::MathematicalOperator,
                        anOperator.name,
			file_name,
                        line_id,
                        false,
                        true,
                        {StringToTree(str.substr(anOperator.name.size(), str.size() - anOperator.name.size()), line_id, file_name)}
                }};
            }
            break;
        }
        case OperatorType::Scope: {
            auto bracketOperator = doubleBracketOperator(str, anOperator.name[0], anOperator.name[1]);

            if (bracketOperator.first) {
                return {true, {
                        Type::MathematicalOperator,
                        anOperator.name,
			            file_name,
                        line_id,
                        false,
                        true,
                        {StringToTree(bracketOperator.second.first, line_id, file_name), StringToTree(bracketOperator.second.second, line_id, file_name)}
                }};
            }
            break;
        }
    }

    return {false, {}};
}

bool O::Analyser::isNumericPostfix(std::u32string str) {
    for (auto elem : numericPostfix){
        if(elem == str){
            return true;
        }
    }

    return false;
}

O::Analyser::TokenisedFile O::Analyser::quickProcess(std::u32string content) { 
    return TokeniseFile(StructuriseFile(content));
}

