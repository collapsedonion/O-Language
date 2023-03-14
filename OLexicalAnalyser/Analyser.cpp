#include "Analyser.h"
#include "pch.h"

wchar_t O::Analyser::mathOperators[] = {
        '|',
        '&',
        '?',
        '>',
        '<',
        '-',
        '+',
        '*',
        '/',
        '%',
};

wchar_t O::Analyser::unarMathOperators[] = {
    '@',
    '~',
    '!',
};

std::wstring O::Analyser::defaultServiceNames[] = {
    L"int",
    L"bool",
    L"float",
    L"char",
    L"void",
    L"false",
    L"true",
    L"while",
    L"return"
};

int O::Analyser::charNotInBrackets(std::wstring str, wchar_t c)
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

int O::Analyser::charNotInQuets(std::wstring str, wchar_t c)
{
    bool inQuets = false;

    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '\"') {
            inQuets = !inQuets;
        }

        if (str[i] == c && !inQuets) {
            return i;
        }
    }

    return -1;
}

int O::Analyser::charNotInFunction(std::wstring str, wchar_t c)
{
    int level = -1;
    int levelInCube = -1;
    bool inQuets = false;

    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '\"') {
            inQuets = !inQuets;
        }

        if (str[i] == '[' && !inQuets) {
            levelInCube++;
        }

        if (str[i] == ']' && !inQuets) {
            levelInCube--;
        }

        if (str[i] == '(' && !inQuets) {
            level++;
            continue;
        }

        if (str[i] == ')' && !inQuets) {
            level--;
            continue;
        }

        if (str[i] == c && level == -1 && !inQuets && levelInCube == -1) {
            return i;
        }
    }
    return -1;
}

std::pair<bool, std::pair<std::wstring, std::wstring>> O::Analyser::doubleBracketOperator(std::wstring str, wchar_t left, wchar_t right)
{
    int level = -1;
    if ((*(str.end() - 1)) != right || str[0] == left) {
        return { false, {L"", L""} };
    }

    std::wstring operatorContent = L"";
    std::wstring leftPart;

    std::wstring operatorContentRight = L"";

    for (int i = str.size() - 1; i >= 0; i--) {
        if (str[i] == right) {
            level++;
        }
        else if (str[i] == left) {
            level--;
        }
        if (level == -1) {
            leftPart = str.substr(0, i);
            break;
        }
        operatorContent += str[i];
    }

    for (int i = operatorContent.size() - 1; i >= 0; i--) {
        operatorContentRight += operatorContent[i];
    }

    return { true, {leftPart, operatorContentRight.substr(0, operatorContentRight.size() - 1)} };
}

int O::Analyser::stringNotInFunction(std::wstring str, std::wstring toFind)
{

    std::wstring found = str;
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

bool O::Analyser::isDefaultServiceName(std::wstring str)
{
    for (auto elem : defaultServiceNames){
        if (elem == str) {
            return true;
        }
    }

    return false;
}

bool O::Analyser::isNumber(std::wstring str)
{
    for (auto elem : str) {
        if (!std::isdigit(elem) && elem != '.') {
            return false;
        }
    }

    return true;
}

bool O::Analyser::isString(std::wstring str)
{
    return str[0] == '\"' && ((* (str.end() - 1)) == '\"');
}

std::wstring O::Analyser::removeBrackes(std::wstring str)
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

std::pair<std::wstring, std::wstring> O::Analyser::sliceString(std::wstring str, int slicePoint)
{
    return {str.substr(0, slicePoint), str.substr(slicePoint + 1, str.size() - slicePoint)};
}

std::wstring O::Analyser::removeSpaceBars(std::wstring str)
{
    std::wstring res = L"";

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

    if (*(res.end() - 1) == ' ') {
        res.pop_back();
    }

    return res;
}

bool O::operator&(std::wstring str1, std::wstring str2)
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

O::Analyser::Token O::Analyser::getMathematicExpression(std::wstring str)
{
    Token t;

    for (int i = 0; i < sizeof(mathOperators) / sizeof(wchar_t); i++) {
        auto op = charNotInFunction(str, mathOperators[i]);
        if (op != -1) {
            auto splited = sliceString(str, op);
            t.type = Type::MathematicalOperator;
            t.token = mathOperators[i];
            t.twoSided = true;
            t.childToken = { StringToTree(splited.first), StringToTree(splited.second) };
            return t;
        }
    }

    auto isOp = doubleBracketOperator(str, '[', ']');
    if (isOp.first) {
        t.type = Type::MathematicalOperator;
        t.token = L"[]a";
        t.childToken = { StringToTree(isOp.second.first), StringToTree(isOp.second.second) };
        return t;
    }

    for (int i = 0; i < sizeof(unarMathOperators)/ sizeof(wchar_t); i++) {
        if (str[0] == unarMathOperators[i]) {
            t.type = Type::MathematicalOperator;
            t.token = unarMathOperators[i];
            t.childToken = { StringToTree(str.substr(1, str.size() - 1))};
            return t;
        }
    }

    return t;
}

O::Analyser::Token O::Analyser::ProccessNameOrCreation(std::wstring str)
{
    int idOfVar = stringNotInFunction(str, L"var");
    int idOfFunc = stringNotInFunction(str, L"func");

    Token res;

    if (*(str.end()-1) == '~') {
        res.token = '~';
        res.type = Type::ServiceName;
        res.childToken = { StringToTree(str.substr(0, str.size() - 1)) };

        return res;
    }
    if (str & L"extern:"){
        res.token = L"extern";
        res.type = Type::ServiceName;
        int idOfSpace = charNotInFunction(str, ':');
        auto splited = sliceString(str, idOfSpace);
        res.childToken.push_back(StringToTree(splited.second));
        return res;
    }

    if (str & L"struct:"){
        res.token = L"structure";
        res.type = Type::ServiceName;
        int idOfSeparator = charNotInFunction(str, ':');
        auto splited = sliceString(str, idOfSeparator);
        res.childToken.push_back(StringToTree(splited.second));
        return res;
    }
    
    if (idOfVar == 0) {
        int idOfDoubleDot = charNotInFunction(str, ':');
        if (idOfDoubleDot != -1) {
            auto splited = sliceString(str, idOfDoubleDot);
            Token r;
            r.type = Type::ServiceName;
            r.twoSided = true;
            r.token = L"var";
            splited = sliceString(splited.second, charNotInFunction(splited.second, ' '));
            r.childToken = { StringToTree(splited.first), StringToTree(splited.second) };
            return r;
        }
    }
    else if (idOfFunc == 0) {
        int idOfDoubleDot = charNotInFunction(str, ':');
        Token Func;
        Func.type = Type::ServiceName;
        Func.token = L"func";
        std::wstring leftFunctionPart = str;
        if (idOfDoubleDot != -1) {
            auto sliced = sliceString(str, idOfDoubleDot);
            leftFunctionPart = sliced.second;
            sliced = sliceString(leftFunctionPart, charNotInFunction(leftFunctionPart, ' '));
            Func.childToken.push_back(StringToTree(sliced.first));
            leftFunctionPart = sliced.second;
        }
        else {
            Func.childToken.push_back(StringToTree(L"void"));
            leftFunctionPart = sliceString(leftFunctionPart, charNotInFunction(leftFunctionPart, ' ')).second;
        }
        int idOfBracket = charNotInQuets(leftFunctionPart, '(');
        auto sliced = sliceString(leftFunctionPart, idOfBracket);
        Token nameAndAntherData;
        nameAndAntherData.token = L"name";
        nameAndAntherData.type = Type::ServiceName;
        sliced.first = removeSpaceBars(sliced.first);
        int idOfSp = charNotInQuets(sliced.first, ' ');
        if (idOfSp != -1) {
            auto nameAndSL = sliceString(sliced.first, idOfSp);
            nameAndAntherData.childToken.push_back(StringToTree(nameAndSL.first));
            
            if (nameAndAntherData.childToken[0].token == L"operator") {
                nameAndAntherData.childToken[0].type = Type::ServiceName;
            }

            Token nt;
            nt.type = Type::MathematicalOperator;
            nt.token = nameAndSL.second;
            nameAndAntherData.childToken.push_back(nt);
        }
        else {
            nameAndAntherData.childToken.push_back(StringToTree(sliced.first));
        }
        leftFunctionPart = sliced.second;
        if (leftFunctionPart.size() > 1) {
            leftFunctionPart = leftFunctionPart.substr(0, leftFunctionPart.size() - 1);
            while (true) {
                int idOfNext = charNotInFunction(leftFunctionPart, ',');
                if (idOfNext == -1) {
                    Token t;
                    int idOfSpace = charNotInFunction(leftFunctionPart, ' ');
                    auto slice = sliceString(leftFunctionPart, idOfSpace);
                    t.token = slice.second;
                    t.childToken.push_back(StringToTree(slice.first));
                    nameAndAntherData.childToken.push_back(t);
                    break;
                }
                else {
                    Token t;
                    auto slice = sliceString(leftFunctionPart, idOfNext);
                    int idOfSpace = charNotInFunction(slice.first, ' ');
                    leftFunctionPart = removeSpaceBars(slice.second);
                    slice = sliceString(slice.first, idOfSpace);
                    t.token = slice.second;
                    t.childToken.push_back(StringToTree(slice.first));
                    nameAndAntherData.childToken.push_back(t);
                }
            }
        }
        Func.childToken.push_back(nameAndAntherData);
        return Func;
    }
    else if (str[0] == '[' && (*(str.end() - 1)) == ']') {
        res.token = L"a[]";
        res.type = Type::ServiceName;
        res.forward = true;
        std::wstring leftPart = str.substr(1, str.size() - 2);
        if (leftPart != L" ") {
            leftPart = removeSpaceBars(leftPart);
            auto isHaveNewElem = charNotInFunction(leftPart, ',');
            while (isHaveNewElem != -1)
            {
                auto splited = sliceString(leftPart, isHaveNewElem);
                res.childToken.push_back(StringToTree(splited.first));
                leftPart = splited.second;
                isHaveNewElem = charNotInFunction(leftPart, ',');
            }

            if (leftPart.size() != 0) {
                res.childToken.push_back(StringToTree(leftPart));
            }
        }
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
            res.token = str.substr(1, str.size()-2);
            res.type = Type::Char;
        }
        else {
            int idOfBracket = charNotInQuets(str, '(');
            if (idOfBracket != -1) {
                auto sliced = sliceString(str, idOfBracket);
                res.token = sliced.first;
                res.forward = true;
                res.type = Type::Name;
                if (sliced.second.size() > 1) {
                    std::wstring last = sliced.second.substr(0, sliced.second.size() - 1);
                    while (true) {
                        int idOfNext = charNotInFunction(last, ',');
                        if (idOfNext == -1) {
                            res.childToken.push_back(StringToTree(last));
                            break;
                        }
                        else {
                            auto slice = sliceString(last, idOfNext);
                            res.childToken.push_back(StringToTree(slice.first));
                            last = slice.second;
                        }
                    }
                }
            }
            else {
                auto idOfSeparator = charNotInFunction(str, ' ');
                if(idOfSeparator != -1){
                    res.token = L"__init__";
                    auto splited = sliceString(str, idOfSeparator);
                    res.childToken = {StringToTree(splited.first), StringToTree(splited.second)};
                }else {
                    res.token = str;
                    res.type = Type::Name;
                }
            }
        }
        
    }

    return res;
}

std::pair<bool, std::pair<std::wstring, std::wstring>> O::Analyser::getEqulitySign(std::wstring str)
{
    auto equalId = charNotInFunction(str, '=');

    if (equalId == -1) {
        return { false, {L"", L""} };
    }

    auto splited = sliceString(str, equalId);

    return { true, {splited.first, splited.second} };
}

O::Analyser::Token O::Analyser::StringToTree(std::wstring str)
{
    std::wstring withOutSpaces = removeSpaceBars(str);
    withOutSpaces = removeBrackes(withOutSpaces);
    auto isEqual = getEqulitySign(withOutSpaces);
    Token result;

    if (isEqual.first) {
        result.token = L"=";
        result.twoSided = true;
        result.type = Type::ServiceName;
        result.childToken = { StringToTree(isEqual.second.first), StringToTree(isEqual.second.second) };
    }
    else {
        auto t = getMathematicExpression(withOutSpaces);
        if (t.token != L"") {
            return t;
        }
        return ProccessNameOrCreation(withOutSpaces);
    }

    return result;
}

O::Analyser::StructurisedFile O::Analyser::StructuriseFile(std::wstring str, std::wstring name)
{
    StructurisedFile main;
    main.name = name;

    StructurisedFile newL;
    std::wstring last;
    std::wstring newBody;

    int level = -1;
    int levelCurly = -1;
    bool inQouets = false;

    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '\"' && levelCurly == -1) {
            inQouets = !inQouets;
        }

        if (str[i] == '(' && !inQouets && levelCurly == -1) {
            level++;
        }

        if (str[i] == ')' && !inQouets && levelCurly == -1) {
            level--;
        }

        if (str[i] == '{' && level == -1 && !inQouets) {
            levelCurly++;
            if (levelCurly == 0) {
                continue;
            }
        }
        if (str[i] == '}' && level == -1 && !inQouets) {
            levelCurly--;
            if (levelCurly == -1) {
                main.subFile.push_back(StructuriseFile(newBody, last));
                last = L"";
                newBody = L"";
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

        if (inQouets || level != -1) {
            last += str[i];
        }
        else {
            if (str[i] == ';') {
                newL.name = last;
                main.subFile.push_back(newL);
                newL = StructurisedFile();
                last = L"";
            }
            else {
                last += str[i];
            }
        }
    }

    if (newBody != L"") {
        main.subFile.push_back(StructuriseFile(newBody, last));
        last = L"";
        newBody = L"";
    }

    if (last != L"") {
        newL.name = last;
        main.subFile.push_back(newL);
    }

    return main;
}

O::Analyser::TokenisedFile O::Analyser::TokeniseFile(StructurisedFile sf)
{
    TokenisedFile tf;
    if (sf.name == L"___MAIN___") {
        Token t;
        t.token = L"___MAIN___";
        t.type = Type::ServiceName;
        tf.name = t;
    }
    else {
        tf.name = StringToTree(sf.name);
    }

    for (auto elem : sf.subFile) {
        tf.subToken.push_back(TokeniseFile(elem));
    }

    return tf;
}
