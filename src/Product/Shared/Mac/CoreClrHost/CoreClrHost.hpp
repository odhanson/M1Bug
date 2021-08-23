//
//  CoreClrHost.hpp
//  CoreClrHosting
//
//  Created by Oded Hanson on 3/6/19.
//  
//

#ifndef CoreClrHost_
#define CoreClrHost_

#include <sys/errno.h>

#include <vector>
#include <string>
#include <system_error>

/* The classes below are exported */
#pragma GCC visibility push(default)

#ifndef SUCCEEDED
#define SUCCEEDED(Status) ((Status) >= 0)
#endif // !SUCCEEDED

#ifndef FAILED
#define FAILED(Status) ((Status) < 0)
#endif // !FAILED

#define ThrowCoreClrHostException(hr) throw CoreClrHosting::Exception(__FILE__, __LINE__, hr, #hr)
#define ThrowCoreClrHostExceptionEx(hr, description) throw CoreClrHosting::Exception(__FILE__, __LINE__, hr, description)
#define ThrowOSException(description) throw CoreClrHosting::OSException(__FILE__, __LINE__, description)

#define IfFailThrow(description, FUNC) { \
  int _hr = FUNC; \
  if (FAILED(_hr)) throw CoreClrHosting::Exception(__FILE__, __LINE__, _hr, description); \
}

namespace CoreClrHosting
{
    enum StatusCode
    {
        Success                     = 0,
        InvalidArgFailure           = 0x80070057,
        OutOfMemoryFailure          = 0x8007000E,
        CoreHostNotInitialized      = 0x80000080,
        CoreHostLibLoadFailure      = 0x80008082,
        CoreHostLibMissingFailure   = 0x80008083,
        CoreHostEntryPointFailure   = 0x80008084,
        CoreHostCurExeFindFailure   = 0x80008085,
        CoreHostResolveModeFailure  = 0x80008086,
        CoreClrResolveFailure       = 0x80008087,
        CoreClrBindFailure          = 0x80008088,
        CoreClrInitFailure          = 0x80008089,
        CoreClrExeFailure           = 0x8000808a,
        HostApiFailed               = 0x80008097,
        HostApiBufferTooSmall       = 0x80008098,
        LibHostUnknownCommand       = 0x80008099,
        LibHostAppRootFindFailure   = 0x8000809a,
    };

    class ExceptionDebugInfo
    {
    public:
        ExceptionDebugInfo(const char *fileName, int linenumber)
        {
            this->fileName = fileName;
            this->linenumber = linenumber;
        }

        const char *GetFileName() const noexcept
        {
            return fileName;
        }

        int GetLineNumber() const noexcept
        {
            return linenumber;
        }
    private:
        const char *fileName;
        int linenumber;
    };

    class Exception : public std::runtime_error, public ExceptionDebugInfo
    {
    public:
        Exception(const char *fileName, int linenumber, int hr) :
          Exception(fileName, linenumber, hr, nullptr)
        {
        }

        Exception(const char *fileName, int linenumber, int hr, const char* arg) :
          std::runtime_error(GetErrorString(hr, arg)),
          ExceptionDebugInfo(fileName, linenumber)
        {
            this->hr = hr;
        }

        int GetHR() const noexcept
        {
            return hr;
        }
    private:
        static std::string GetErrorString(int hr, const char* arg);
        int hr;
    };

    class OSException : public std::system_error, public ExceptionDebugInfo
    {
    public:
        OSException(const char *fileName, int linenumber, const char* arg) :
          std::system_error(errno, std::system_category(), arg),
          ExceptionDebugInfo(fileName, linenumber)
        {
        }
    };

    class CoreClrHost
    {
        struct ctor_cookie {};
    public:
        CoreClrHost(const CoreClrHost&) = delete;
        explicit CoreClrHost(ctor_cookie) {};
        ~CoreClrHost();
        void ShutDownHost();
        void CreateDelegate(const char* entryPointAssemblyName, const char* entryPointTypeName, const char* entryPointMethodName, void** delegate);
        void ExecuteAssembly(int argc, const char** argv, const char* managedAssemblyPath, unsigned int* exitCode);

    public:
        // szClrBinariesRelativePath should be relative to current running app's bundle contents folder
        // tpaList a vector of dll paths relative to application bundles contents path
        static std::shared_ptr<CoreClrHost> GetInstance()
        {
            if (!m_instance)
            {
                ThrowCoreClrHostException(CoreHostNotInitialized);
            }
            return m_instance;
        }
        static std::shared_ptr<CoreClrHost> InitializeAndGetInstance(const char* szClrBinariesRelativePath, const std::vector<const char*>& tpaList);
        static std::shared_ptr<CoreClrHost> InitializeAndGetInstance(const char* szClrBinariesRelativePath, const std::vector<const char*>& tpaList, bool useNativeDebugger);
        static std::string CurrentRunningApplicationPath();
        static std::string CurrentRunningApplicationBundleContentsPath();
        static std::string CurrentRunningApplicationBundleContentsPath(const std::string& currentRunningApplicationPath);

    private:

        CoreClrHost();
        void InitializeHost(const char* szClrBinariesRelativePath, const std::vector<const char*>& tpaList, bool nativeDebugger);
        std::string CreateTpaListString(const std::string& szAssemblyRootDirectory, const std::vector<const char*>& tpaList);
        void OpenCoreLib(const std::string& coreClrDllPath);

        bool FHostInitialized() { return m_hostHandle != nullptr && m_domainId != 0; }

        template <typename T>
        void SetFunctionPointer(T& functionPointer, const char *functionName);

        static void ConvertRunningApplicationBundleContentsPath(std::string& hostPath);

    private:
        // initialization state
        void* m_hostHandle = nullptr;
        void* m_coreclrLib =  nullptr;

        unsigned int m_domainId = 0;

        // functions pointers
        int (*m_initializeFunc)(const char* exePath, const char* appDomainFriendlyName, int propertyCount, const char** propertyKeys, const char** propertyValues, void** hostHandle, unsigned int* domainId) = nullptr;
        int (*m_shutdownFunc)(void* hostHandle, unsigned int domainId) = nullptr;
        int (*m_executeAssemblyFunc)(void* hostHandle, unsigned int domainId, int argc, const char** argv, const char* managedAssemblyPath, unsigned int* exitCode) = nullptr;
        int (*m_createDelegateFunc)(void* hostHandle, unsigned int domainId, const char* entryPointAssemblyName, const char* entryPointTypeName, const char* entryPointMethodName, void** delegate) = nullptr;

        std::string m_applicationBundleContentsPath;
    private:
        static std::shared_ptr<CoreClrHost> m_instance;
    };
}
#pragma GCC visibility pop
#endif
