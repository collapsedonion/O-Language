#include <iostream>
#include <BaseTypes.h>
#include <Analyser.h>
#include <SematicAnalyser.h>
#include <string>
#include <fstream>
#include <sstream>
#include <OtoOTranslator.h>
#include <Preproccesor.h>
#include <codecvt>
#ifdef __APPLE__
	#include <mach-o/dyld.h>
#endif

const std::u32string PreInclude =
        U"#include std.olf\n";

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

    std::u32string realFilePath(filepath.begin(), filepath.end());
    std::u32string realOutFilePath(outfilepath.begin(), outfilepath.end());
    std::u32string realExecPath(execPath.begin(), execPath.end());

    std::u32string filesource;

    std::ifstream f(realFilePath);    
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utfConvertor;
    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    filesource = utfConvertor.from_bytes(s);
    f.close();
    
    filesource = PreInclude + filesource;

    O::Preproccesor pp(realFilePath, realExecPath);

    filesource = pp.proccess(filesource);

    auto AnalisedFile = O::Analyser::StructuriseFile(filesource);
    auto TokenisedFile = O::Analyser::TokeniseFile(AnalisedFile);
    O::SematicAnalyser sematiser;

    try {
        sematiser.ProccessTokenisedFile(TokenisedFile);
    }catch (CompilationException& exception){
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        std::string res = converter.to_bytes(exception.getText());
        std::cout << res;
        return -1;
    }

    O::OtoOTranslator translator;
    translator.Build(sematiser.getFileRepresantation());
    translator.WriteResulToFile(realOutFilePath);

    return 0;
}
