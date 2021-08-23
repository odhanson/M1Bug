//
//  AppDelegate.m
//  ProviderTestUI
//
//  Created by Oded Hanson on 2/4/18.
//  
//

#import "AppDelegate.h"
#import "CoreClrHost.hpp"
#import "ManagedFacade.hpp"

static AppDelegate* gAppDelegate;

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;

@end

void WriteLine(const char *text)
{
    NSString* nsText = [NSString stringWithUTF8String:text];
    NSLog(@"%@", nsText);
}

void OnShowErrorMessage(const char* text, const char* caption)
{
    @autoreleasepool
    {
        WriteLine("error:");
        WriteLine(text);
        
        NSAlert* alert = [[NSAlert alloc] init];
        [alert setMessageText:[NSString stringWithUTF8String:caption]];
        [alert setInformativeText:[NSString stringWithUTF8String:caption]];
        
        [alert runModal];
    }
}

@implementation AppDelegate

const std::vector<const char*> tpaList
{
#include "UITesterAssemblies.h"
};

using namespace CoreClrHosting;

ManagedFacade *pManagedFacade = nullptr;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    gAppDelegate = self;

    bool useNativeDebugger = true;
    auto coreClrHost = CoreClrHost::InitializeAndGetInstance(
        "SharedSupport",
        tpaList,
        useNativeDebugger);

    pManagedFacade = new ManagedFacade(
        coreClrHost,
        WriteLine,
        OnShowErrorMessage);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    delete pManagedFacade;
    CoreClrHost::GetInstance()->ShutDownHost();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

- (IBAction)executeTest:(id)sender {
    
    for (int i =0 ;i<50;i++)
    {
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            pManagedFacade->ExecuteTest();
        });
    }
}

@end

