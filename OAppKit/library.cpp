
#include <OVM_SDK.h>
#include "InterruptDefinitions.h"

#define NEW_INTERRUPT(NAME, HANDLER, ID) Interrupt NAME; NAME.name = #NAME; NAME.hInt = HANDLER; NAME.id = last_id + ID;

extern "C" std::vector<Interrupt> _Omain(int last_id){
    Interrupt createApplication;
    createApplication.name = "initApplication";
    createApplication.hInt = CreateApplication;
    createApplication.id = last_id;

    Interrupt getSharedApplication;
    getSharedApplication.id = last_id+1;
    getSharedApplication.name = "getSharedApplication";
    getSharedApplication.hInt = GetSharedApplication;

    Interrupt createWindow;
    createWindow.id = last_id + 2;
    createWindow.name = "initWindow";
    createWindow.hInt = CreateWindow;

    Interrupt setWindowFlags;
    setWindowFlags.hInt = SetWindowFlags;
    setWindowFlags.id = last_id + 3;
    setWindowFlags.name = "setWindowFlags";

    Interrupt setWindowRect;
    setWindowRect.id = last_id + 4;
    setWindowRect.name = "setWindowRect";
    setWindowRect.hInt = SetWindowRect;

    Interrupt makeWindowFront;
    makeWindowFront.id = last_id + 5;
    makeWindowFront.name = "setWindowFront";
    makeWindowFront.hInt = MakeWindowFront;

    Interrupt getLastWindowEvent;
    getLastWindowEvent.id = last_id + 6;
    getLastWindowEvent.name = "getWindowLastEvent";
    getLastWindowEvent.hInt = GetLastWindowEvent;

    Interrupt dispatch;
    dispatch.id = last_id + 7;
    dispatch.name = "dispatchEvent";
    dispatch.hInt = AppDispatchEvent;

    Interrupt getEventType;
    getEventType.id = last_id + 8;
    getEventType.name = "getEventType";
    getEventType.hInt = GetEventType;

    Interrupt setWindowTitle;
    setWindowTitle.id = last_id + 9;
    setWindowTitle.name = "setWindowTitle";
    setWindowTitle.hInt = SetWindowTitle;

    Interrupt getEventCharacters;
    getEventCharacters.id = last_id + 10;
    getEventCharacters.name = "getEventCharacters";
    getEventCharacters.hInt = GetEventCharacters;

    Interrupt getMetalDevice;
    getMetalDevice.id = last_id + 11;
    getMetalDevice.name = "getSystemMetalDevice";
    getMetalDevice.hInt = GetMetalDevice;

    Interrupt initView;
    initView.id = last_id + 12;
    initView.name = "initMTView";
    initView.hInt = InitView;

    Interrupt setColorFormat;
    setColorFormat.id = last_id + 13;
    setColorFormat.name = "setViewColorPixelFormat";
    setColorFormat.hInt = SetViewColorFormat;

    Interrupt setViewClearColor;
    setViewClearColor.id = last_id + 14;
    setViewClearColor.name = "setViewClearColor";
    setViewClearColor.hInt = SetViewClearColor;

    Interrupt setWindowView;
    setWindowView.id = last_id + 15;
    setWindowView.name = "setWindowView";
    setWindowView.hInt = SetWindowView;

    Interrupt createNewCommandQueue;
    createNewCommandQueue.id = last_id + 16;
    createNewCommandQueue.name = "createNewCommandQueue";
    createNewCommandQueue.hInt = CreateCommandQueue;

    Interrupt getCurrentRenderPassDescriptor;
    getCurrentRenderPassDescriptor.id = last_id + 17;
    getCurrentRenderPassDescriptor.name = "getCurrentRenderPassDescriptor";
    getCurrentRenderPassDescriptor.hInt = GetRenderPassDescriptor;

    Interrupt getRenderCommandEncoder;
    getRenderCommandEncoder.id = last_id + 18;
    getRenderCommandEncoder.name = "getRenderCommandEncoder";
    getRenderCommandEncoder.hInt = GetRenderCommandEncoder;

    Interrupt getCommandBuffer;
    getCommandBuffer.id = last_id + 19;
    getCommandBuffer.name = "getCommandBuffer";
    getCommandBuffer.hInt = GetCommandBuffer;

    Interrupt endEncoding;
    endEncoding.id = last_id + 20;
    endEncoding.name = "endEncoding";
    endEncoding.hInt = EndEncoding;

    Interrupt currentDrawable;
    currentDrawable.id = last_id + 21;
    currentDrawable.name = "getCurrentDrawable";
    currentDrawable.hInt = GetCurrentDrawable;

    Interrupt presentDrawable;
    presentDrawable.id = last_id + 22;
    presentDrawable.name = "presentDrawable";
    presentDrawable.hInt = PresentDrawable;

    Interrupt commitBuffer;
    commitBuffer.id = last_id + 23;
    commitBuffer.name = "commitCommandBuffer";
    commitBuffer.hInt = CommitCommandBuffer;

    Interrupt createNewLib;
    createNewLib.id = last_id + 24;
    createNewLib.name = "initMTLLibWithFile";
    createNewLib.hInt = CreateMTLLibWithFile;

    Interrupt getMTLFunction;
    getMTLFunction.id = last_id + 25;
    getMTLFunction.name = "getMTLFunction";
    getMTLFunction.hInt = GetMTLFunction;

    Interrupt initRenderPipelineDescriptor;
    initRenderPipelineDescriptor.id = last_id + 26;
    initRenderPipelineDescriptor.name = "initRenderPipelineDescriptor";
    initRenderPipelineDescriptor.hInt = InitRenderPipelineDescriptor;

    Interrupt setFragmentFunction;
    setFragmentFunction.id = last_id + 27;
    setFragmentFunction.name = "setPipelineFragmentFunction";
    setFragmentFunction.hInt = SetPipelineFragmentFunction;

    Interrupt setVertexFunction;
    setVertexFunction.id = last_id + 28;
    setVertexFunction.name = "setPipelineVertexFunction";
    setVertexFunction.hInt = SetPipelineVertexFunction;

    Interrupt setPipeLineLabel;
    setPipeLineLabel.id = last_id + 29;
    setPipeLineLabel.hInt = SetPipelineLabel;
    setPipeLineLabel.name = "setPipelineLabel";

    Interrupt setPipelinePixelFormat;
    setPipelinePixelFormat.id = last_id + 30;
    setPipelinePixelFormat.name = "setPipelinePixelFormat";
    setPipelinePixelFormat.hInt = SetPipelinePixelFormat;

    NEW_INTERRUPT(initRenderPipelineState, InitRenderPipeState, 31)

    NEW_INTERRUPT(setEncoderPipelineState, SetRenderPipeState, 32)

    return {createApplication,
            getSharedApplication,
            createWindow,
            setWindowFlags,
            setWindowRect,
            makeWindowFront,
            getLastWindowEvent,
            dispatch,
            getEventType,
            setWindowTitle,
            getEventCharacters,
            getMetalDevice,
            initView,
            setColorFormat,
            setViewClearColor,
            setWindowView,
            createNewCommandQueue,
            getCurrentRenderPassDescriptor,
            getRenderCommandEncoder,
            getCommandBuffer,
            endEncoding,
            currentDrawable,
            presentDrawable,
            commitBuffer,
            createNewLib,
            getMTLFunction,
            initRenderPipelineDescriptor,
            setFragmentFunction,
            setVertexFunction,
            setPipeLineLabel,
            setPipelinePixelFormat,
            initRenderPipelineState,
            setEncoderPipelineState,};
}