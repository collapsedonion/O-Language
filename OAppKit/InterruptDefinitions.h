//
// Created by Роман  Тимофеев on 15.03.2023.
//

#ifndef OAPPKIT_INTERRUPTDEFINITIONS_H
#define OAPPKIT_INTERRUPTDEFINITIONS_H
#include <OVM_SDK.h>

void CreateApplication(MEM_POINTER memPointer);
void GetSharedApplication(MEM_POINTER memPointer);
void CreateWindow(MEM_POINTER memPointer);
void SetWindowFlags(MEM_POINTER memPointer);
void SetWindowRect(MEM_POINTER memPointer);
void MakeWindowFront(MEM_POINTER memPointer);
void GetLastWindowEvent(MEM_POINTER memPointer);
void AppDispatchEvent(MEM_POINTER memPointer);
void GetEventType(MEM_POINTER memPointer);
void SetWindowTitle(MEM_POINTER memPointer);
void GetEventCharacters(MEM_POINTER memPointer);
void GetMetalDevice(MEM_POINTER memPointer);
void InitView(MEM_POINTER memPointer);
void SetViewColorFormat(MEM_POINTER memPointer);
void SetViewClearColor(MEM_POINTER memPointer);
void SetWindowView(MEM_POINTER memPointer);
void CreateCommandQueue(MEM_POINTER memPointer);
void GetRenderPassDescriptor(MEM_POINTER memPointer);
void GetRenderCommandEncoder(MEM_POINTER memPointer);
void GetCommandBuffer(MEM_POINTER memPointer);
void EndEncoding(MEM_POINTER memPointer);
void GetCurrentDrawable(MEM_POINTER memPointer);
void PresentDrawable(MEM_POINTER memPointer);
void CommitCommandBuffer(MEM_POINTER memPointer);

#endif //OAPPKIT_INTERRUPTDEFINITIONS_H
