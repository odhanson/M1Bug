//
//  ManagedFacade.hpp
//  ProviderTestUI
//
//  Created by Oded Hanson on 2/15/18.
//  
//

#ifndef ManagedFacade_hpp
#define ManagedFacade_hpp

#include "CoreClrHost.hpp"
#include <CoreFoundation/CoreFoundation.h>

typedef void (*LogLineCallback)(const char* text);
typedef void (*ShowErrorMessage)(const char* text, const char* caption);
typedef char* (*PromptForFilename)();
typedef void (*ObservableStringUpdated)(const char* script);
typedef void (*ObservableBoolUpdated)(unsigned char isEnabled);

class ManagedFacade
{
public:
    ManagedFacade(std::shared_ptr<CoreClrHosting::CoreClrHost> pcoreClrHost,
                  LogLineCallback logLine,
                  ShowErrorMessage showErrorMessage);
    ~ManagedFacade()
    {
        this->dispose();
    }
    void ExecuteTest() const
    {
        this->executeTest();
    }
private:
    enum class Native2ManagedIndexes
    {
        Dispose = 0,
        ExecuteTest,

        // Do not remove this
        NumberOfMethods
    };

    enum class Managed2NativeIndexes
    {
        LogLine = 0,
        ShowErrorMessage,

        // Do not remove this
        NumberOfMethods
    };
    template <typename T>
    void SetManagedCallback(void** managedCallbacks, Native2ManagedIndexes callbackIndex, T& callback)
    {
        callback = (T)managedCallbacks[(int)callbackIndex];
    }

    int (*createInstance)(void **nativeMethods, int nativeMethodCount, void **callbacks, int callbackCount);
    void (*executeTest)();
    void (*dispose)();
};


#endif /* ManagedFacade_hpp */
