#include "CompilerSources.h"
#include "Analyser.h"
#include "SematicAnalyser.h"
#include <iostream>
#include <fstream>
#include <sstream>

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

	auto translator = getTranslator(L"OtoPythonTranslator.dll");

	std::string outFile = translator(sematiser.getFileRepresantation());

	std::ofstream of(outfilepath);
	of << outFile;
	of.close();

	return 0;
}