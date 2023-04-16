//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "Preproccesor.h"
#include <codecvt>

namespace O {


    std::u32string Preproccesor::proccess(std::u32string str) {
        std::u32string result;

        std::u32string buffer;

        int strId = 0;

        bool bufferLoad = false;

        for(auto i : str){

            if(i == '\n' && bufferLoad){
                bufferLoad = false;
                result += getInst(buffer);
                strId++;
                buffer = U"";
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
                    result += U"\n";
                    result += U"#LINE_ID";
		            std::string line_id_str = std::to_string(strId);
		            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
                    result += converter.from_bytes(line_id_str);
                    result += U";";
                }
                strId++;
            }

            if(!bufferLoad && i != '\t') {
                result += i;
            }
        }

        return result;
    }

    bool Preproccesor::isAlredyIncluded(std::u32string name) {

        for(auto e : included){
            if(e == name){
                return true;
            }
        }

        return false;
    }

    std::u32string Preproccesor::Include(std::u32string str) {
        auto params = getParameters(str);

        if(!isAlredyIncluded(params[1])){
            included.push_back(params[1]);
            std::ifstream f(filePath + params[1]);
            if(!f.good()){
                f = std::ifstream(execPath + U"/stdLib/" + params[1]);
            }
	        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convertor;
            std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            std::u32string content = convertor.from_bytes(s);
            f.close();
            return proccess(content);
        }

        return U"";
    }

    std::vector<std::u32string> Preproccesor::getParameters(std::u32string promt) {

        std::vector<std::u32string> toRet;

        if(promt[0] != '#'){
            return {};
        }

        std::u32string last;

        for(auto c : promt){
            if(c == ' '){
                toRet.push_back(last);
                last = U"";
            }else if(c != '\t'){
                last += c;
            }
        }

        if(last != U""){
            toRet.push_back(last);
        }

        return toRet;
    }

    std::u32string Preproccesor::getInst(std::u32string promt) {
        auto p = getParameters(promt);

        if(p[0] == U"#include"){
            return Include(promt);
        }

        return U"";
    }

    Preproccesor::Preproccesor(std::u32string filePath, std::u32string execPath) {
        auto indexOfLast = filePath.rfind('/');
        if(indexOfLast != std::string::npos)
        {
           this->filePath = filePath.substr(0, indexOfLast) + U"/";
        }

        this->execPath = execPath;
    }
} // O
