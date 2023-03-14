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
        std::vector<std::wstring> included;
        std::wstring filePath;

    private:
        bool isAlredyIncluded(std::wstring name);

        std::vector<std::wstring> getParameters(std::wstring promt);

        std::wstring getInst(std::wstring promt);

        std::wstring Include(std::wstring str);

        std::wstring execPath;

    public:
        Preproccesor(std::wstring filePath, std::wstring execPath = L"");

        std::wstring proccess(std::wstring str);
    };

} // O

#endif //OPREPROCCESSOR_PREPROCCESOR_H
