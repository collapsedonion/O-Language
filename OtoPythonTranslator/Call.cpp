#include <string>
#include <BaseTypes.h>
#include "Translator.h"
#include "pch.h"

extern "C" std::string __stdcall Translate(File f) {
	Translator t;
	t.TranslateFile(f);
	return t.getF();
}
