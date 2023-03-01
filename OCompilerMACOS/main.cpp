#include <iostream>
#include <BaseTypes.h>
#include <Analyser.h>
#include <SematicAnalyser.h>
#include <string>
#include <fstream>
#include <sstream>
#include <Call.h>
#include <OtoOTranslator.h>
#include <Preproccesor.h>

int main() {

    std::string filepath = "";
    std::string outfilepath = "";

    std::cout << "Enter in and out filepaths:\n";

    std::cin >> filepath;
    std::cin >> outfilepath;

    std::string filesource;

    std::ifstream f(filepath);
    std::stringstream inBuffer;
    inBuffer << f.rdbuf();
    f.close();
    filesource = inBuffer.str();
    inBuffer.clear();

    O::Preproccesor pp(filepath);

    filesource = pp.proccess(filesource);

    auto AnalisedFile = O::Analyser::StructuriseFile(filesource);
    auto TokenisedFile = O::Analyser::TokeniseFile(AnalisedFile);
    O::SematicAnalyser sematiser;
    sematiser.ProccessTokenisedFile(TokenisedFile);

    if(outfilepath[outfilepath.size() - 1] == 'y' && outfilepath[outfilepath.size() - 2] == 'p') {
        std::string result = Translate(sematiser.getFileRepresantation());
        std::ofstream of(outfilepath);
        of << result;
        of.close();
    }else{
        O::OtoOTranslator translator;
        translator.Build(sematiser.getFileRepresantation());
        translator.WriteResulToFile(outfilepath);
    }

    return 0;
}
