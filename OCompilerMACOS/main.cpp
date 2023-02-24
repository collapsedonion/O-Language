#include <iostream>
#include <BaseTypes.h>
#include <Analyser.h>
#include <SematicAnalyser.h>
#include <string>
#include <fstream>
#include <sstream>
#include <Call.h>

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

    auto AnalisedFile = O::Analyser::StructuriseFile(filesource);
    auto TokenisedFile = O::Analyser::TokeniseFile(AnalisedFile);
    O::SematicAnalyser sematiser;
    sematiser.ProccessTokenisedFile(TokenisedFile);

    std::string result = Translate(sematiser.getFileRepresantation());

    std::ofstream of(outfilepath);
    of << result;
    of.close();

    return 0;
}
