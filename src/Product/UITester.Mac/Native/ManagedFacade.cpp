//
//  ManagedFacade.cpp
//  ProviderTestUI
//
//  Created by Oded Hanson on 2/15/18.
//  
//

#include "ManagedFacade.hpp"

ManagedFacade::ManagedFacade(std::shared_ptr<CoreClrHosting::CoreClrHost> pcoreClrHost,
                             LogLineCallback logLine,
                             ShowErrorMessage showErrorMessage)
{
    void* nativeMethods[(int)Managed2NativeIndexes::NumberOfMethods] =
    {
        (void*)logLine,
        (void*)showErrorMessage,
    };

    void* managedCallbacks[(int)Native2ManagedIndexes::NumberOfMethods];

    pcoreClrHost->CreateDelegate("UITester.Mac, Version=1.0.0.0, Culture=neutral", "UITester.Mac.Managed.NativeBootstrap", "CreateInstance", (void**)&this->createInstance);

    int hr = this->createInstance(nativeMethods, sizeof(nativeMethods)/sizeof(void*), managedCallbacks, sizeof(managedCallbacks)/sizeof(void*));

    if (hr != 0)
    {
        throw hr;
    }

    SetManagedCallback(managedCallbacks, Native2ManagedIndexes::Dispose, this->dispose);
    SetManagedCallback(managedCallbacks, Native2ManagedIndexes::ExecuteTest, this->executeTest);
}
