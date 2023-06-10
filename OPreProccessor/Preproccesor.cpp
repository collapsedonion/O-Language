//
// Created by Роман  Тимофеев on 28.02.2023.
//

#include "Preproccesor.h"
#include <codecvt>

namespace O {


    std::u32string Preproccesor::proccess(std::u32string str) {
        std::u32string result;

        std::u32string buffer;

        int strId = 1;

        bool bufferLoad = false;

        bool string = false;
        
        result += U"\n";
        result += U"\n";
        result += U"#LINE_ID";
        result += U"1";
        result += U";";
        result += U"\n#FILE_NAME" + this->filePath + this->file_name + U";";

        for(auto i : str){
            
            if(i == '"'){
                string = !string;
            }

            if(i == '\n' && bufferLoad){
                bufferLoad = false;
                result += getInst(buffer, strId, this->filePath + this->file_name);
                buffer = U"";
                result += U"\n";
                result += U"\n";
                result += U"#LINE_ID";
		        std::string line_id_str = std::to_string(strId);
	            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
                result += converter.from_bytes(line_id_str);
                result += U";";
                result += U"\n#FILE_NAME" + this->filePath + this->file_name + U";";
                strId++;
                continue;
            }

            if(i == '#'){
                bufferLoad = true;
            }

            if(bufferLoad){
                buffer += i;
            }

            if(i == '\n') {
                if(!bufferLoad && ((result[result.size() - 1] == ';' && !string)|| result[result.size() - 1]  == '}' || result[result.size() - 1]  == '{')){
                    result += U"\n";
                    result += U"\n";
                    result += U"#LINE_ID";
                    std::string line_id_str = std::to_string(strId);
                    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
                    result += converter.from_bytes(line_id_str);
                    result += U";";
                    result += U"\n#FILE_NAME" + this->filePath + this->file_name + U";";
                }
                strId++;
		        continue;
            }

            if(!bufferLoad && ((i == ';' && !string) || i == '}' || i == '{')){
                result += i;
                result += U"\n";
                result += U"\n";
                result += U"#LINE_ID";
		        std::string line_id_str = std::to_string(strId);
	            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
                result += converter.from_bytes(line_id_str);
                result += U";";
                result += U"\n#FILE_NAME" + this->filePath + this->file_name + U";";
                continue;
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
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convertor;
            std::ifstream f(convertor.to_bytes(filePath + params[1]));
            if(!f.good()){
                f = std::ifstream(convertor.to_bytes(execPath + U"/stdLib/" + params[1]));
            }
            std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            std::u32string content = convertor.from_bytes(s);
            f.close();
	        std::u32string pred_name = this->file_name;
	        this->file_name = params[1];
	        std::u32string new_content = proccess(content);
	        this->file_name = pred_name;
            return new_content;
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

    std::u32string Preproccesor::getInst(std::u32string promt, int line, std::u32string file_name) {
        auto p = getParameters(promt);

        if(p[0] == U"#include"){
            return Include(promt);
        }
        else if(p[0] == U"#brkpnt"){
            BreakPoint(line, file_name);
        }

        return U"";
    }

    void Preproccesor::BreakPoint(int line, std::u32string file){
        break_points.push_back({file, line});
    }

    Preproccesor::Preproccesor(std::u32string filePath, std::u32string execPath) {
        
#if defined(__APPLE__)
        auto indexOfLast = filePath.rfind('/');
#elif defined(_WIN32)
        auto indexOfLast = filePath.rfind('\\');
#endif
        if(indexOfLast != std::string::npos)
        {
           this->filePath = filePath.substr(0, indexOfLast) + U"/";
	   this->file_name = filePath.substr(indexOfLast + 1, filePath.size() - (indexOfLast+1));
        }
	else{
	   this->filePath = filePath;
	   this->file_name = filePath;
	}

        this->execPath = execPath;
    }
} // O
