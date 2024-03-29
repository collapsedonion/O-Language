//
// Created by Роман  Тимофеев on 28.02.2023.
//

#ifndef OPREPROCCESSOR_PREPROCCESOR_H
#define OPREPROCCESSOR_PREPROCCESOR_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace O {

    class Preproccesor {
    public:
        std::vector<std::pair<std::u32string, int>> break_points; 

    private:
        std::vector<std::u32string> included;
        std::u32string filePath;
	    std::u32string file_name;

    private:
        bool isAlredyIncluded(std::u32string name);

        std::vector<std::u32string> getParameters(std::u32string promt);

        std::u32string getInst(std::u32string promt, int line = 0, std::u32string file_name = 0);

        std::u32string Include(std::u32string str);

        void BreakPoint(int line, std::u32string file_name);

        std::u32string execPath;

    public:
        Preproccesor(std::u32string filePath, std::u32string execPath = U"");

        std::u32string proccess(std::u32string str);
    };

} // O

#endif //OPREPROCCESSOR_PREPROCCESOR_H
