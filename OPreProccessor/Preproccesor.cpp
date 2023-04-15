//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "Preproccesor.h"

namespace O {


    std::wstring Preproccesor::proccess(std::wstring str) {
        std::wstring result;

        std::wstring buffer;

        int strId = 0;

        bool bufferLoad = false;

        for(auto i : str){

            if(i == '\n' && bufferLoad){
                bufferLoad = false;
                result += getInst(buffer);
                strId++;
                buffer = L"";
                continue;
            }

            if(i == '#'){
                bufferLoad = true;
            }

            if(bufferLoad){
                buffer += i;
            }

            if(i == '\n') {
                if(*(result.end() - 1) == ';'){
                    result += L"\n";
                    result += L"#LINE_ID";
                    result += std::to_wstring(strId + 1);
                    result += L";";
                }
                strId++;
            }

            if(!bufferLoad && i != '\t') {
                result += i;
            }
        }

        return result;
    }

    bool Preproccesor::isAlredyIncluded(std::wstring name) {

        for(auto e : included){
            if(e == name){
                return true;
            }
        }

        return false;
    }

    std::wstring Preproccesor::Include(std::wstring str) {
        auto params = getParameters(str);

        if(!isAlredyIncluded(params[1])){
            included.push_back(params[1]);
            std::wifstream f(filePath + params[1]);
            if(!f.good()){
                f = std::wifstream(execPath + L"/stdLib/" + params[1]);
            }
            std::wstringstream buf;
            buf << f.rdbuf();
            std::wstring content = buf.str();
            f.close();
            return proccess(content);
        }

        return L"";
    }

    std::vector<std::wstring> Preproccesor::getParameters(std::wstring promt) {

        std::vector<std::wstring> toRet;

        if(promt[0] != '#'){
            return {};
        }

        std::wstring last;

        for(auto c : promt){
            if(c == ' '){
                toRet.push_back(last);
                last = L"";
            }else if(c != '\t'){
                last += c;
            }
        }

        if(last != L""){
            toRet.push_back(last);
        }

        return toRet;
    }

    std::wstring Preproccesor::getInst(std::wstring promt) {
        auto p = getParameters(promt);

        if(p[0] == L"#include"){
            return Include(promt);
        }

        return L"";
    }

    Preproccesor::Preproccesor(std::wstring filePath, std::wstring execPath) {
        auto indexOfLast = filePath.rfind('/');
        if(indexOfLast != std::string::npos)
        {
           this->filePath = filePath.substr(0, indexOfLast) + L"/";
        }

        this->execPath = execPath;
    }
} // O