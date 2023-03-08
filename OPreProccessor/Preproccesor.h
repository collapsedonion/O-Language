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
    private:
        std::vector<std::string> included;
        std::string filePath;

    private:
        bool isAlredyIncluded(std::string name);

        std::vector<std::string> getParameters(std::string promt);

        std::string getInst(std::string promt);

        std::string Include(std::string str);

        std::string execPath;

    public:
        Preproccesor(std::string filePath, std::string execPath = "");

        std::string proccess(std::string str);
    };

} // O

#endif //OPREPROCCESSOR_PREPROCCESOR_H
