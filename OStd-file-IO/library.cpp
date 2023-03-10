
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
    std::fstream* f = new std::fstream(path);
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
    auto *file = (std::fstream*)addressOfFile;

    file->put(toWrite);
}

void ReadCharFromFile(MEM_POINTER mem){
    long addressOfFile = (*mem._mem)[mem.ebp];
    auto *file = (std::fstream*)addressOfFile;

    char res;

    file->read(&res, 1);

    *mem.eax = res;
}

void CloseFile(MEM_POINTER mem) {
    long adressOfFile = (*mem._mem)[mem.ebp];
    auto *file = (std::fstream *) adressOfFile;

    file->close();
    delete (file);
}

extern "C" std::vector<Interrupt> _Omain(int _lastId){
    Interrupt OpenFileInterrupt;
    OpenFileInterrupt.id = _lastId;
    OpenFileInterrupt.name = "openFile";
    OpenFileInterrupt.hInt = OpenFileInput;

    Interrupt ReadCharFromFileI;
    ReadCharFromFileI.id = _lastId + 1;
    ReadCharFromFileI.name = "fReadChar";
    ReadCharFromFileI.hInt = ReadCharFromFile;

    Interrupt closeFile;
    closeFile.id = _lastId + 2;
    closeFile.name = "closeFile";
    closeFile.hInt = CloseFile;

    Interrupt eof;
    eof.id = _lastId + 3;
    eof.name = "endOfFile";
    eof.hInt = IsEOF;

    Interrupt good;
    good.id = _lastId + 4;
    good.name = "fGood";
    good.hInt = IsGood;

    Interrupt writeChar;
    writeChar.id = _lastId + 5;
    writeChar.name = "fPutChar";
    writeChar.hInt = WriteCharToFile;

    return {OpenFileInterrupt, ReadCharFromFileI, closeFile, eof, good, writeChar};
}

