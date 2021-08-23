

#import "WebDialogWindowScriptMessageHandler.h"

@implementation WebDialogWindowScriptMessageHandler
{
    OnScriptNotify onScriptNotify;
}

+ (NSString *)scriptMessageName { return @"notify"; }

- (instancetype)initWithMessageNotifyCallback:(OnScriptNotify)onScriptNotify
{
    self = [super init];
    if (self)
    {
        self->onScriptNotify = onScriptNotify;
    }
    
    return self;
}

- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message
{
    if ([message.name isEqualToString:WebDialogWindowScriptMessageHandler.scriptMessageName])
    {
        self->onScriptNotify((__bridge CFStringRef)message.body);
    }
}
@end
