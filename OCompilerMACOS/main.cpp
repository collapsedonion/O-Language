#include <iostream>
#include <BaseTypes.h>
#include <Analyser.h>
#include <SematicAnalyser.h>
#include <string>
#include <fstream>
#include <sstream>
#include <OtoOTranslator.h>
#include <Preproccesor.h>

const std::string PreInclude =
        "extern:func:bool operator \"?\" (char a, char b);\n"
        "extern:func:bool operator \"|\" (bool a, bool b);\n"
        "extern:func:bool operator \"&\" (bool a, bool b);\n"
        "extern:func:int operator \"%\" (int a, int b);\n"
        "extern:func:char operator \"-\" (char a, char b);\n"
        "extern:func:char operator \"+\" (char a, char b);\n"
        "extern:func:float operator \"+\" (float a, float b);\n"
        "extern:func:float operator \"-\" (float a, float b);\n"
        "extern:func:float operator \"*\" (float a, float b);\n"
        "extern:func:float operator \"/\" (float a, float b);\n"
        "extern:func:bool operator \"<\" (float a, float b);\n"
        "extern:func:bool operator \">\" (float a, float b);\n"
        "extern:func:bool operator \"?\" (float a, float b);\n"
        "";

int main(int argC, char* args[]) {

    std::string filepath = "";
    std::string outfilepath = "";

    std::cout << args[0] << "\n";

    if (argC == 1) {
        return -1;
    } else if (argC == 2) {
        filepath = args[1];

        outfilepath = "a.ovm";
    } else if (argC == 3) {
        filepath = args[1];
        outfilepath = args[2];
    }

    std::string filesource;

    std::ifstream f(filepath);
    std::stringstream inBuffer;
    inBuffer << f.rdbuf();
    f.close();
    filesource = inBuffer.str();
    inBuffer.clear();

    filesource = PreInclude + filesource;

    O::Preproccesor pp(filepath);

    filesource = pp.proccess(filesource);

    auto AnalisedFile = O::Analyser::StructuriseFile(filesource);
    auto TokenisedFile = O::Analyser::TokeniseFile(AnalisedFile);
    O::SematicAnalyser sematiser;
    sematiser.ProccessTokenisedFile(TokenisedFile);

    O::OtoOTranslator translator;
    translator.Build(sematiser.getFileRepresantation());
    translator.WriteResulToFile(outfilepath);


    return 0;
}
