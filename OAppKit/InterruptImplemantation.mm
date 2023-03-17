#include <OVM_SDK.h>
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include "InterruptDefinitions.h"
#include "AppDelegate.h"

void CreateApplication(MEM_POINTER memPointer){
    NSApplication* app = [NSApplication sharedApplication];
    AppDelegate* delegate = [AppDelegate new];
    [app setDelegate:delegate];
    [app finishLaunching];

    *memPointer.eax = (long)app;
}

void GetSharedApplication(MEM_POINTER memPointer){
    *memPointer.eax = (long)[NSApplication sharedApplication];
}

void CreateWindow(MEM_POINTER memPointer){
    CGRect rect = {{0,0}, {200,200}};

    NSWindow* window = [[NSWindow alloc] initWithContentRect:rect styleMask:NSClosableWindowMask|NSTitledWindowMask backing:NSBackingStoreBuffered defer:NO];
    *memPointer.eax = (long)window;
}

void SetWindowFlags(MEM_POINTER memPointer){
    auto window = (NSWindow*)((*memPointer._mem)[memPointer.ebp]);
    long mask = (*memPointer._mem)[memPointer.ebp - 1];
    [window setStyleMask:static_cast<NSWindowStyleMask>(mask)];
}

void SetWindowRect(MEM_POINTER memPointer){
    auto window = (NSWindow*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    long* rectP = getObjectReferenceByAddress(memPointer, memPointer.ebp - 1);
    long* rect = getObjectReferenceByAddress(memPointer, *rectP);
    long* origin = getObjectReferenceByAddress(memPointer, *rect);
    long* size = getObjectReferenceByAddress(memPointer, *(rect+1));

    CGRect frame = {{*(float*)origin, *(float*)(origin + 1)}, {*(float*)size, *(float*)(size + 1)}};
    [window setFrame:frame display:NO];
}

void MakeWindowFront(MEM_POINTER memPointer){
    auto window = (NSWindow*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));

    [window makeKeyAndOrderFront:nil];
}

void GetLastWindowEvent(MEM_POINTER memPointer){
    auto window = (NSWindow*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto dequeue = (bool)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    *memPointer.eax = (long)[window nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:dequeue];
}

void AppDispatchEvent(MEM_POINTER memPointer){
    auto app = (NSApplication*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto event = (NSEvent*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    [app sendEvent:event];
}

void GetEventType(MEM_POINTER memPointer){
    auto event = (NSEvent*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    *memPointer.eax = event.type;
}