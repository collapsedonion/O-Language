//
// Created by Роман  Тимофеев on 15.03.2023.
//
#include "AppDelegate.h"


@implementation AppDelegate
- (void)applicationWillFinishLaunching:(NSNotification *)notification {
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
    [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    [[NSApplication sharedApplication] stop:self];
    [[NSApplication sharedApplication] abortModal];
}

@end