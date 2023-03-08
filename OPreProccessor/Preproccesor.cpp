//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "Preproccesor.h"

namespace O {


    std::string Preproccesor::proccess(std::string str) {
        std::string result;

        std::string buffer;

        bool bufferLoad = false;

        for(auto i : str){

            if(i == '\n' && bufferLoad){
                bufferLoad = false;

                result += getInst(buffer);

                buffer = "";
                continue;
            }

            if(i == '#'){
                bufferLoad = true;
            }

            if(bufferLoad){
                buffer += i;
            }

            if(!bufferLoad && i != '\t') {
                result += i;
            }
        }

        return result;
    }

    bool Preproccesor::isAlredyIncluded(std::string name) {

        for(auto e : included){
            if(e == name){
                return true;
            }
        }

        return false;
    }

    std::string Preproccesor::Include(std::string str) {
        auto params = getParameters(str);

        if(!isAlredyIncluded(params[1])){
            included.push_back(params[1]);
            std::ifstream f(filePath + params[1]);
            if(!f.good()){
                f = std::ifstream(execPath + "/stdLib/" + params[1]);
            }
            std::stringstream buf;
            buf << f.rdbuf();
            std::string content = buf.str();
            f.close();
            return proccess(content);
        }

        return "";
    }

    std::vector<std::string> Preproccesor::getParameters(std::string promt) {

        std::vector<std::string> toRet;

        if(promt[0] != '#'){
            return {};
        }

        std::string last;

        for(auto c : promt){
            if(c == ' '){
                toRet.push_back(last);
                last = "";
            }else if(c != '\t'){
                last += c;
            }
        }

        if(last != ""){
            toRet.push_back(last);
        }

        return toRet;
    }

    std::string Preproccesor::getInst(std::string promt) {
        auto p = getParameters(promt);

        if(p[0] == "#include"){
            return Include(promt);
        }

        return "";
    }

    Preproccesor::Preproccesor(std::string filePath, std::string execPath) {
        auto indexOfLast = filePath.rfind('/');
        if(indexOfLast != std::string::npos)
        {
           this->filePath = filePath.substr(0, indexOfLast) + "/";
        }

        this->execPath = execPath;
    }
} // O