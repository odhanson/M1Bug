

#ifndef WebDialogWindowCallbacks_h
#define WebDialogWindowCallbacks_h

typedef unsigned char (*TryGetResponseForUrl)(CFURLRef _Nonnull requestUri, CFStringRef _Nonnull * _Nonnull responseContentType, CFDataRef _Nonnull * _Nonnull responseData);
typedef void (*OnScriptNotify)(CFStringRef _Nonnull notification);
typedef void (*OnError)(CFErrorRef _Nonnull error);
typedef void (*Action)(void);
typedef void (*OnInvokeScriptComplete)(CFStringRef _Nonnull result, CFErrorRef _Nonnull error);
typedef unsigned char (*OnUriChanged)(CFURLRef _Nonnull uri);

#endif /* WebDialogWindowCallbacks_h */
