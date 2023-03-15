//
// Created by Роман  Тимофеев on 15.03.2023.
//

#ifndef OAPPKIT_APPDELEGATE_H
#define OAPPKIT_APPDELEGATE_H

#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>

@interface AppDelegate : NSObject<NSApplicationDelegate>{

};
- (void)applicationWillFinishLaunching:(NSNotification *)notification;
- (void)applicationDidFinishLaunching:(NSNotification *)notification;
@end

#endif //OAPPKIT_APPDELEGATE_H
