#include <iostream>
#include <BaseTypes.h>
#include <Analyser.h>
#include <SematicAnalyser.h>
#include <string>
#include <fstream>
#include <sstream>
#include <OtoOTranslator.h>
#include <Preproccesor.h>
#ifdef __APPLE__
	#include <mach-o/dyld.h>
#endif

const std::wstring PreInclude =
        L"#include std.olf\n";

int main(int argC, char* args[]) {

    std::string filepath;
    std::string outfilepath;

    char* sourcePath = (char*)malloc(1024);
    uint32_t sourceSize = 1024;

#ifdef __APPLE__
    while(_NSGetExecutablePath(sourcePath, &sourceSize) == -1){
        free(sourcePath);
        sourceSize = sourceSize + 1024;
        sourcePath = (char*) malloc(sourceSize);
    }
#endif
    std::string execPath(sourcePath);
    free(sourcePath);

    execPath = execPath.substr(0, execPath.rfind('/'));

    if (argC == 1) {
        return -1;
    }
    else if (argC == 2) {
        filepath = args[1];

        outfilepath = "a.ovm";
    } else if (argC == 3) {
        filepath = args[1];
        outfilepath = args[2];
    }

    std::wstring realFilePath(filepath.begin(), filepath.end());
    std::wstring realOutFilePath(outfilepath.begin(), outfilepath.end());
    std::wstring realExecPath(execPath.begin(), execPath.end());

    std::wstring filesource;

    std::wifstream f(realFilePath);
    std::wstringstream inBuffer;
    inBuffer << f.rdbuf();
    f.close();
    filesource = inBuffer.str();
    inBuffer.clear();

    filesource = PreInclude + filesource;

    O::Preproccesor pp(realFilePath, realExecPath);

    filesource = pp.proccess(filesource);

    auto AnalisedFile = O::Analyser::StructuriseFile(filesource);
    auto TokenisedFile = O::Analyser::TokeniseFile(AnalisedFile);
    O::SematicAnalyser sematiser;

    try {
        sematiser.ProccessTokenisedFile(TokenisedFile);
    }catch (CompilationException& exception){
        std::wcout << exception.getText();
        return -1;
    }

    O::OtoOTranslator translator;
    translator.Build(sematiser.getFileRepresantation());
    translator.WriteResulToFile(realOutFilePath);

    return 0;
}
