//
//  main.m
//  ProviderTestUI
//
//  Created by Oded Hanson on 2/4/18.
//  
//

#import <Cocoa/Cocoa.h>
#import <JavaScriptCore/JavaScriptCore.h>

int main(int argc, const char * argv[])
{
    // This is a workaround provided by Apple for the following bug:
    // https://dev.azure.com/office/APEX/_workitems/edit/4368465
    // The fix is limited to OSX 10.15.6 for which the bug applies
    @autoreleasepool
    {
        NSOperatingSystemVersion workaroundVersion = {.majorVersion = 10, .minorVersion = 15, .patchVersion = 6};
        NSOperatingSystemVersion currentVersion = [[NSProcessInfo processInfo] operatingSystemVersion];
        if ((currentVersion.majorVersion == workaroundVersion.majorVersion) &&
            (currentVersion.minorVersion == workaroundVersion.minorVersion) &&
            (currentVersion.patchVersion >= workaroundVersion.patchVersion)) {
            // Workaround for JSC setting mach exception ports before our code does.
            // These options prevent JSC from crashing after we replace their mach exception ports for EXC_BAD_ACCESS
            setenv("JSC_useWebAssembly", "0", 1);
            setenv("JSC_usePollingTraps", "1", 1);

            // Get JSC to initialize itself.
            JSStringRef jsStringRef = JSStringCreateWithUTF8CString("Init JSC");
            if (jsStringRef != nil)
            {
                JSStringRelease(jsStringRef);
            }
            // Now we are free to replace JSC's ports.
        }
    }

    return NSApplicationMain(argc, argv);
}
