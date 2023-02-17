#include "CompilerSources.h"
#include <Windows.h>

TranslateFunction getTranslator(std::wstring dllName)
{
    HINSTANCE hDll;
    TranslateFunction tf;

    hDll = LoadLibrary(dllName.c_str());

    if (hDll != NULL) {
        tf = (TranslateFunction)GetProcAddress(hDll, "Translate");
        if (tf != NULL) {
            return tf;
        }
        else {
            FreeLibrary(hDll);
            throw(std::exception("Can`t find Translate function"));
        }
    }
    else{
        throw(std::exception("Unknown translator name"));
    }

    return nullptr;
}
