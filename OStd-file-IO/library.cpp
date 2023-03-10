
#include <OVM_SDK.h>
#include <fstream>
#include <iostream>
#include <string>

std::string long_String_to_char8_String(long* str){
    std::string toRet;

    while (*str != 0){
        toRet += (char)*str;
        str += 1;
    }

    return toRet;
}

void OpenFileInput(MEM_POINTER mem){
    long addressOfFilePath = (*mem._mem)[mem.ebp];
    auto filePath = (long*)&((*mem._mem)[addressOfFilePath]);
    std::string path = long_String_to_char8_String(filePath);
    std::ifstream* f = new std::ifstream(path);
    *mem.eax = (long)f;
}

void OpenFileOutput(MEM_POINTER mem){
    long addressOfFilePath = (*mem._mem)[mem.ebp];
    auto filePath = (long*)&((*mem._mem)[addressOfFilePath]);
    std::string path = long_String_to_char8_String(filePath);
    std::ofstream* f = new std::ofstream(path);

    *mem.eax = (long)f;
}

void IsEOF(MEM_POINTER mem){
    auto file = (std::fstream*)((*mem._mem)[mem.ebp]);
    if(file->eof()){
        *mem.eax = 1;
    }else{
        *mem.eax = 0;
    }
}

void IsGood(MEM_POINTER mem){
    auto file = (std::fstream*)((*mem._mem)[mem.ebp]);
    if(file->good()){
        *mem.eax = 1;
    }else{
        *mem.eax = 0;
    }
}

void WriteCharToFile(MEM_POINTER mem){
    long addressOfFile = (*mem._mem)[mem.ebp];
    char toWrite = (char)(*mem._mem)[mem.ebp-1];
    auto *file = (std::ofstream*)addressOfFile;

    file->put(toWrite);
}

void ReadCharFromFile(MEM_POINTER mem){
    long addressOfFile = (*mem._mem)[mem.ebp];
    auto *file = (std::ifstream*)addressOfFile;

    char res;

    file->read(&res, 1);

    *mem.eax = res;
}

void CloseFile(MEM_POINTER mem){
    long adressOfFile = (*mem._mem)[mem.ebp];
    try {
        auto *file = (std::ifstream *) adressOfFile;

        file->close();
        delete (file);
    }catch (...){
        auto *file = (std::ofstream *) adressOfFile;

        file->close();
        delete (file);
    }
}

extern "C" std::vector<Interrupt> _Omain(int _lastId){
    Interrupt OpenFileInterrupt;
    OpenFileInterrupt.id = _lastId;
    OpenFileInterrupt.name = "openIFile";
    OpenFileInterrupt.hInt = OpenFileInput;

    Interrupt ReadCharFromFileI;
    ReadCharFromFileI.id = _lastId + 1;
    ReadCharFromFileI.name = "fReadChar";
    ReadCharFromFileI.hInt = ReadCharFromFile;

    Interrupt closeFile;
    closeFile.id = _lastId + 2;
    closeFile.name = "closeFile";
    closeFile.hInt = CloseFile;

    Interrupt openFileOutput;
    openFileOutput.id = _lastId + 3;
    openFileOutput.name = "openOFile";
    openFileOutput.hInt = OpenFileOutput;

    Interrupt eof;
    eof.id = _lastId + 4;
    eof.name = "endOfFile";
    eof.hInt = IsEOF;

    Interrupt good;
    good.id = _lastId + 5;
    good.name = "fGood";
    good.hInt = IsGood;

    Interrupt writeChar;
    writeChar.id = _lastId + 6;
    writeChar.name = "fPutChar";
    writeChar.hInt = WriteCharToFile;

    return {OpenFileInterrupt, ReadCharFromFileI, closeFile, openFileOutput, eof, good, writeChar};
}

