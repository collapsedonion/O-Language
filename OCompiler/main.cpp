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
#include <cdc.h>

const std::u32string PreInclude =
        U"#include std.olf\n";

const int error_line_count = 6;

void skipFileToLine(std::ifstream& file, int line){

    int current_line = 1;

    while (current_line < line && file.good())
    {
        if(file.get() == '\n'){
            current_line++;
        }
    }
}

int main(int argC, char* args[]) {

    std::string filepath;
    std::string outfilepath;

    char* sourcePath = cdc_get_executable_directory();
    std::string execPath(sourcePath);
    free(sourcePath);

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

        std::ifstream error_file(exception.file_name);

        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        std::string res = converter.to_bytes(exception.getText());
        std::cout << res << "\n";

        if(error_file.good()){
            skipFileToLine(error_file, exception.line - (int)(error_line_count / 2));
            int read_line = 0;
            while(read_line < error_line_count / 2 && error_file.good()){
                std::cout << (exception.line - (int)(error_line_count / 2) - read_line == exception.line ? "!(":"#(") << exception.line - (int)(error_line_count / 2) - read_line
                    << ")--->\t";
                char current_symbol;
                while((current_symbol = error_file.get()) != '\n' && error_file.good()){
                    std::cout << current_symbol;
                }
                std::cout << "\n";
                read_line--;
            }
            error_file.close();
        }
        return -1;
    }

    O::OtoOTranslator translator;
    translator.Build(sematiser.getFileRepresantation());
    translator.WriteResulToFile(realOutFilePath);
    translator.WriteDebugSymbols(realOutFilePath + U".ovm_dbg", pp.break_points);

    std::cout << "Succesfuly compiled\n";

    return 0;
}
