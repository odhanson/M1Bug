//
//  CoreClrHost.cpp
//  CoreClrHosting
//
//  Created by Oded Hanson on 3/6/19.
//  
//

#include "CoreClrHost.hpp"
#include <mach-o/dyld.h>
#include <dirent.h>
#include <dlfcn.h>
#include <libproc.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#define CORECLR_LIBRARY_NAME "libcoreclr.dylib"
#define SHARED_SUPPORT_FOLDER_NAME "SharedSupport"
#define Assert assert

namespace CoreClrHosting
{

    /*static*/ std::string Exception::GetErrorString(int hr, const char* arg)
    {
        std::stringstream stream;
        stream << arg << ". Error: 0x" << std::hex << std::setw(8) << hr;
        return stream.str();
    }

    /*static*/ std::shared_ptr<CoreClrHost> CoreClrHost::m_instance;

    CoreClrHost::CoreClrHost()
    {
    }

    CoreClrHost::~CoreClrHost()
    {
        // In case Shutdown host was never called
        try
        {
            ShutDownHost();
        }
        // We eat up the exception in case shutdown throws during destructor.
        catch (...)
        {
        }
    }

    /*static*/ std::shared_ptr<CoreClrHost> CoreClrHost::InitializeAndGetInstance(const char* szClrBinariesRelativePath, const std::vector<const char*>& tpaList)
    {
        return InitializeAndGetInstance(szClrBinariesRelativePath, tpaList, false /* useNativeDebugger */);
    }
    /*static*/ std::shared_ptr<CoreClrHost> CoreClrHost::InitializeAndGetInstance(const char* szClrBinariesRelativePath, const std::vector<const char*>& tpaList, bool useNativeDebugger)
    {
        if (!m_instance)
        {
            auto coreClrHost = std::make_shared<CoreClrHost>(ctor_cookie());

            coreClrHost->InitializeHost(szClrBinariesRelativePath, tpaList, useNativeDebugger);

            m_instance = coreClrHost;
        }

        return m_instance;
    }

    /* static */ std::string CoreClrHost::CurrentRunningApplicationPath()
    {
        std::string hostPath;
        uint32_t bufSize = 0;

        if (_NSGetExecutablePath(nullptr, &bufSize) == -1)
        {
            hostPath.resize(bufSize-1);
            if (_NSGetExecutablePath(&hostPath[0], &bufSize) == -1)
            {
                ThrowCoreClrHostExceptionEx(StatusCode::HostApiFailed, "_NSGetExecutablePath");
            }
        }
        else
        {
            ThrowCoreClrHostExceptionEx(StatusCode::HostApiFailed, "_NSGetExecutablePath");
        }

        return hostPath;
    }

    /* static */ std::string CoreClrHost::CurrentRunningApplicationBundleContentsPath()
    {
        std::string hostPath = CurrentRunningApplicationPath();
        ConvertRunningApplicationBundleContentsPath(hostPath);
        return hostPath;
    }

    /* static */ std::string CoreClrHost::CurrentRunningApplicationBundleContentsPath(const std::string& currentRunningApplicationPath)
    {
        std::string hostPath = currentRunningApplicationPath;
        ConvertRunningApplicationBundleContentsPath(hostPath);
        return hostPath;
    }

    /* static */ void CoreClrHost::ConvertRunningApplicationBundleContentsPath(std::string& hostPath)
    {
        // Application path should be <App Bundle Contents>/MacOS/<executablename>
        size_t pos = hostPath.find_last_of('/');
        if (pos == std::string::npos)
        {
            ThrowCoreClrHostException(StatusCode::HostApiFailed);
        }
        pos = hostPath.find_last_of('/', pos - 1);
        if (pos == std::string::npos)
        {
            ThrowCoreClrHostException(StatusCode::HostApiFailed);
        }
        hostPath.resize(pos);
    }

    // Add all trusted platform assembly paths from the specified list to a string, separated by ':'.
    std::string CoreClrHost::CreateTpaListString(const std::string& szAssemblyRootDirectory, const std::vector<const char*>& tpaList)
    {
        std::string tpaListString;

        for (auto assemblyName : tpaList)
        {
            tpaListString += szAssemblyRootDirectory + '/' + assemblyName + ':';
        }

        return tpaListString;
    }

    template <typename T>
    void CoreClrHost::SetFunctionPointer(T& functionPointer, const char *functionName)
    {
        functionPointer = (T)dlsym(m_coreclrLib, functionName);
        if (functionPointer == nullptr)
        {
            std::string errorString = std::string(functionName) + ". " + dlerror();
            ThrowCoreClrHostExceptionEx(StatusCode::CoreHostEntryPointFailure, errorString.c_str());
        }
    }

    void CoreClrHost::OpenCoreLib(const std::string& coreClrDllPath)
    {
        // Verify the FX resolver library exists
        struct stat buffer;
        if (stat(coreClrDllPath.c_str(), &buffer) != 0)
        {
            ThrowOSException(coreClrDllPath.c_str());
        }

        // load the library
        m_coreclrLib = dlopen(coreClrDllPath.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (m_coreclrLib == nullptr)
        {
            ThrowCoreClrHostExceptionEx(StatusCode::CoreHostLibLoadFailure, dlerror());
        }
    }

    void CoreClrHost::InitializeHost(const char* szClrBinariesRelativePath, const std::vector<const char*>& tpaList, bool useNativeDebugger)
    {
        auto applicationFilePath = CurrentRunningApplicationPath();
        m_applicationBundleContentsPath = CurrentRunningApplicationBundleContentsPath(applicationFilePath);
        auto coreClrDllDirectoryPath = m_applicationBundleContentsPath + '/' + szClrBinariesRelativePath;
        auto coreClrDllFilePath =  coreClrDllDirectoryPath + '/' + CORECLR_LIBRARY_NAME;
        auto nativeLibrarySearchPath = coreClrDllDirectoryPath + ":/usr/local/lib:/usr/lib";
        auto szAssemblyRootDirectory = m_applicationBundleContentsPath + '/' + SHARED_SUPPORT_FOLDER_NAME;
        auto tpaListString = CreateTpaListString(szAssemblyRootDirectory, tpaList);

        printf("%s\n",nativeLibrarySearchPath.c_str());
        
        const int OverrideEnvironment =
#ifdef DEBUG
        0;
#else
        1;

        setenv("COMPlus_EnableDiagnostics", "0", OverrideEnvironment);
#endif

        setenv("DOTNET_SANDBOX_APPLICATION_GROUP_ID", "UBF8T346G9.ms", OverrideEnvironment);
        if (useNativeDebugger)
        {
          setenv("PAL_MachExceptionMode", "2", OverrideEnvironment);
        }

        OpenCoreLib(coreClrDllFilePath);

        // Allowed property names:
        // APPBASE
        // - The base path of the application from which the exe and other assemblies will be loaded
        //
        // TRUSTED_PLATFORM_ASSEMBLIES
        // - The list of complete paths to each of the fully trusted assemblies
        //
        // APP_PATHS
        // - The list of paths which will be probed by the assembly loader
        //
        // APP_NI_PATHS
        // - The list of additional paths that the assembly loader will probe for ngen images
        //
        // NATIVE_DLL_SEARCH_DIRECTORIES
        // - The list of paths that will be probed for native DLLs called by PInvoke
        const char *propertyKeys[] = {
            "TRUSTED_PLATFORM_ASSEMBLIES",
            "APP_PATHS",
            "APP_NI_PATHS",
            "NATIVE_DLL_SEARCH_DIRECTORIES",
            "AppDomainCompatSwitch"
        };
        const char *propertyValues[] = {
            // TRUSTED_PLATFORM_ASSEMBLIES
            tpaListString.c_str(),
            // APP_PATHS
            coreClrDllDirectoryPath.c_str(),
            // APP_NI_PATHS
            coreClrDllDirectoryPath.c_str(),
            // NATIVE_DLL_SEARCH_DIRECTORIES
            nativeLibrarySearchPath.c_str(),
            // AppDomainCompatSwitch
            "UseLatestBehaviorWhenTFMNotSpecified"
        };

        SetFunctionPointer(m_initializeFunc, "coreclr_initialize");

        // Initialize the CoreClr
        IfFailThrow("call m_initializeFunc",
                    m_initializeFunc(applicationFilePath.c_str(),
                    "corehostrun",
                    sizeof(propertyKeys) / sizeof(propertyKeys[0]),
                    propertyKeys,
                    propertyValues,
                    &m_hostHandle,
                    &m_domainId));

        // Load the symbols
        SetFunctionPointer(m_executeAssemblyFunc, "coreclr_execute_assembly");
        SetFunctionPointer(m_shutdownFunc, "coreclr_shutdown");
        SetFunctionPointer(m_createDelegateFunc, "coreclr_create_delegate");
    }

    void CoreClrHost::ShutDownHost()
    {
        // Please Make sure that multiple calls to ShutDownHost don't fail

        // Release the singleton instance
        m_instance = nullptr;

        int hr = StatusCode::Success;
        const char* errorString = nullptr;
        if (m_shutdownFunc != nullptr)
        {
            hr = m_shutdownFunc(m_hostHandle, m_domainId);
            if (FAILED(hr))
            {
                errorString = "coreclr_shutdown failed";
            }
        }

        m_domainId = -1;
        m_hostHandle = nullptr;
        m_shutdownFunc = nullptr;
        m_createDelegateFunc = nullptr;
        m_executeAssemblyFunc = nullptr;

        if (m_coreclrLib != nullptr)
        {
            if (dlclose(m_coreclrLib) != 0)
            {
                errorString = dlerror();
                hr = StatusCode::HostApiFailed;
            }
            m_coreclrLib = nullptr;
        }

        IfFailThrow(errorString, hr);
    }

    void CoreClrHost::CreateDelegate(const char *entryPointAssemblyName, const char *entryPointTypeName, const char *entryPointMethodName, void **delegate)
    {
        if (m_createDelegateFunc == nullptr)
        {
            ThrowCoreClrHostException(CoreHostNotInitialized);
        }

        IfFailThrow("coreclr_create_delegate failed",
            m_createDelegateFunc(m_hostHandle,
                                 m_domainId,
                                 entryPointAssemblyName,
                                 entryPointTypeName,
                                 entryPointMethodName,
                                 delegate));
    }

    void CoreClrHost::ExecuteAssembly(int argc, const char** argv, const char* managedAssemblyPath, unsigned int* exitCode)
    {
        if (m_createDelegateFunc == nullptr)
        {
            ThrowCoreClrHostException(CoreHostNotInitialized);
        }

        auto assemblyFilePath = m_applicationBundleContentsPath + '/' + managedAssemblyPath;
        IfFailThrow("m_executeAssemblyFunc failed",
            m_executeAssemblyFunc(m_hostHandle,
                                  m_domainId,
                                  argc,
                                  argv,
                                  assemblyFilePath.c_str(),
                                  exitCode));
    }
}
