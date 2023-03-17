
#include <OVM_SDK.h>
#include "InterruptDefinitions.h"

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

    return {createApplication,
            getSharedApplication,
            createWindow,
            setWindowFlags,
            setWindowRect,
            makeWindowFront,
            getLastWindowEvent,
            dispatch,
            getEventType};
}