#pragma once
#include <string>
#include <BaseTypes.h>
#include "Translator.h"

extern "C" __declspec(dllexport) std::string __stdcall Translate(File f);
