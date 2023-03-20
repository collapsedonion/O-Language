#include <OVM_SDK.h>
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include "InterruptDefinitions.h"
#include "AppDelegate.h"
#include <MetalKit/MetalKit.h>
#include <Metal/Metal.h>

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

void SetWindowTitle(MEM_POINTER memPointer){
    auto window = (NSWindow*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto titlePointer = (long)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));
    auto title = (char*)(getObjectReferenceByAddress(memPointer, titlePointer));

    NSString* nsString = [[NSString alloc] initWithCString:title];
    [window setTitle:nsString];
}

void GetEventCharacters(MEM_POINTER memPointer){
    auto event = (NSEvent*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto target = (*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));
    auto targetReference = (char*)(getObjectReferenceByAddress(memPointer, target));
    NSString* chatacters = event.characters;
    [chatacters cString];
    memcpy(targetReference, [chatacters cString], [chatacters cStringLength]);
}

void GetMetalDevice(MEM_POINTER memPointer){
    auto device = MTLCreateSystemDefaultDevice();
    *memPointer.eax = (long)device;
}

void InitView(MEM_POINTER memPointer){
    long* rectP = getObjectReferenceByAddress(memPointer, memPointer.ebp);
    long* rect = getObjectReferenceByAddress(memPointer, *rectP);
    long* origin = getObjectReferenceByAddress(memPointer, *rect);
    long* size = getObjectReferenceByAddress(memPointer, *(rect+1));
    auto device = (id<MTLDevice>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    CGRect cgRect = {{*(float*)origin, *(float*)(origin+1)}, {*(float*)size, *(float*)(size + 1)}};
    MTKView* view = [[MTKView alloc] initWithFrame:cgRect device:device];
    *memPointer.eax = (long)view;
}

void SetViewColorFormat(MEM_POINTER memPointer){
    auto view = (MTKView*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto format = (int)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    [view setColorPixelFormat:static_cast<MTLPixelFormat>(format)];
}

void SetViewClearColor(MEM_POINTER memPointer){
    auto view = (MTKView*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto r = (float)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));
    auto g = (float)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 2));
    auto b = (float)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 3));
    auto a = (float)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 4));
    view.clearColor = MTLClearColorMake(r,g,b,a);
}

void SetWindowView(MEM_POINTER memPointer){
    auto window = (NSWindow*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto view = (NSView*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));
    [window setContentView:view];
}

void CreateCommandQueue(MEM_POINTER memPointer){
    auto device = (id<MTLDevice>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    id<MTLCommandQueue> queue = [device newCommandQueue];
    *memPointer.eax = (long)queue;
}

void GetRenderPassDescriptor(MEM_POINTER memPointer){
    auto view = (MTKView*)(*getObjectReferenceByAddress(memPointer,memPointer.ebp));
    auto renderPassDescriptor = [view currentRenderPassDescriptor];
    *memPointer.eax = (long)renderPassDescriptor;
}

void GetRenderCommandEncoder(MEM_POINTER memPointer){
    auto commandBuffer = (id<MTLCommandBuffer>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto renderPassDescriptor = (MTLRenderPassDescriptor*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    id<MTLRenderCommandEncoder> commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    *memPointer.eax = (long)commandEncoder;
}

void GetCommandBuffer(MEM_POINTER memPointer){
    auto commandQueue = (id<MTLCommandQueue>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    id<MTLCommandBuffer> buffer = [commandQueue commandBuffer];
    *memPointer.eax = (long)buffer;
}

void EndEncoding(MEM_POINTER memPointer){
    auto commandEncoder = (id<MTLRenderCommandEncoder>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    [commandEncoder endEncoding];
}

void GetCurrentDrawable(MEM_POINTER memPointer){
    auto view = (MTKView*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    id<MTLDrawable> drawAble = [view currentDrawable];
    *memPointer.eax = (long)drawAble;
}

void PresentDrawable(MEM_POINTER memPointer){
    auto commandBuffer = (id<MTLCommandBuffer>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto drawable = (id<MTLDrawable>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    [commandBuffer presentDrawable:drawable];
}

void CommitCommandBuffer(MEM_POINTER memPointer){
    auto commandBuffer = (id<MTLCommandBuffer>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    [commandBuffer commit];
}

void CreateMTLLibWithFile(MEM_POINTER memPointer){
    auto device = (id<MTLDevice>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto path = (char*)(getObjectReferenceByAddress(memPointer ,*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1)));

    NSString* str = [[NSString alloc] initWithCString:path];
    id<MTLLibrary> library = [device newLibraryWithFile:str error:nil];
    *memPointer.eax = (long)library;
}

void GetMTLFunction(MEM_POINTER memPointer){
    auto lib = (id<MTLLibrary>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto name = (char*)(getObjectReferenceByAddress(memPointer ,*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1)));

    NSString* str = [[NSString alloc] initWithCString:name];

    id<MTLFunction> function = [lib newFunctionWithName:str];
    *memPointer.eax = (long)function;
}

void InitRenderPipelineDescriptor(MEM_POINTER memPointer){
    *memPointer.eax = (long)[[MTLRenderPipelineDescriptor alloc] init];
}

void SetPipelineVertexFunction(MEM_POINTER memPointer){
    auto pipeline = (MTLRenderPipelineDescriptor*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto function = (id<MTLFunction>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    [pipeline setVertexFunction:function];
}
void SetPipelineFragmentFunction(MEM_POINTER memPointer){
    auto pipeline = (MTLRenderPipelineDescriptor*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto function = (id<MTLFunction>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    [pipeline setFragmentFunction:function];
}

void SetPipelineLabel(MEM_POINTER memPointer){
    auto pipeline = (MTLRenderPipelineDescriptor*)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto label = (char*)(getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    NSString* nLabel = [[NSString alloc] initWithCString:label];
    [pipeline setLabel:nLabel];
}

void SetPipelinePixelFormat(MEM_POINTER memPointer){
    auto pipeline = (MTLRenderPipelineDescriptor*)(*getObjectReferenceByAddress(memPointer,memPointer.ebp));
    auto format = (int)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    pipeline.colorAttachments[0].pixelFormat = static_cast<MTLPixelFormat>(format);
}

void InitRenderPipeState(MEM_POINTER memPointer){
    auto device = (id<MTLDevice>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto pipeline = (MTLRenderPipelineDescriptor*)(*getObjectReferenceByAddress(memPointer,memPointer.ebp - 1));
    id<MTLRenderPipelineState> rps = [device newRenderPipelineStateWithDescriptor:pipeline error:nil];
    *memPointer.eax = (long)rps;
}

INTERRUPT_DEFINITION(SetRenderPipeState){
    auto encoder = (id<MTLRenderCommandEncoder>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp));
    auto state = (id<MTLRenderPipelineState>)(*getObjectReferenceByAddress(memPointer, memPointer.ebp - 1));

    [encoder setRenderPipelineState:state];
}