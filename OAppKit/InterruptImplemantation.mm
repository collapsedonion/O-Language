#include <OVM_SDK.h>
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include "InterruptDefinitions.h"
#include "AppDelegate.h"

void CreateApplication(MEM_POINTER memPointer){
    NSApplication* app = [NSApplication sharedApplication];
    AppDelegate* delegate = [AppDelegate new];
    [app setDelegate:delegate];
    [app run];
    *memPointer.eax = (long)app;
}

void GetSharedApplication(MEM_POINTER memPointer){
    *memPointer.eax = (long)[NSApplication sharedApplication];
}