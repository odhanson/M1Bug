//
//  AppDelegate.h
//  ProviderTestUI
//
//  Created by Oded Hanson on 2/4/18.
//  
//

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, WKNavigationDelegate>

- (IBAction)executeTest:(id)sender;

@property (weak) IBOutlet NSButton *executeTest;

@end

