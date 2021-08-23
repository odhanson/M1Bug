

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#include "WebDialogWindowCallbacks.h"

NS_ASSUME_NONNULL_BEGIN

@interface WebDialogWindowScriptMessageHandler : NSObject<WKScriptMessageHandler>
@property (class, nonatomic, readonly) NSString* scriptMessageName;
- (instancetype)initWithMessageNotifyCallback:(OnScriptNotify)onScriptNotify;
@end

NS_ASSUME_NONNULL_END
