

using System;
using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using UITester.Mac.Managed.Interop;

namespace UITester.Mac.Managed
{
    public class NativeBootstrap : IDisposable, IUITester
    {
        private static NativeBootstrap? instance;
        private readonly NativeInterop nativeInterop;

        private NativeBootstrap(NativeInterop nativeInterop)
        {
            this.nativeInterop = nativeInterop;
        }

        // Static factory method will be called by .Net core host during initialization to
        // setup native/managed callbacks in one call.
        public static int CreateInstance(
            [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] IntPtr[] nativeMethods,
#pragma warning disable CA1801 // This parameter is unused but is part of public contract
            [In] int nativeMethodCount,
#pragma warning restore CA1801 // This parameter is unused but is part of public contract
            [In, Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] IntPtr[] callbacks,
            [In] int callbackCount)
        {
            NativeInterop nativeInterop = new NativeInterop(nativeMethods);

            PrintStartup(nativeInterop);

            if (instance != null)
            {
                nativeInterop.WriteLine("NativeBootstrap already created");

                // Singleton, can't re-initialize.
                return -1;
            }

            try
            {
                instance = new NativeBootstrap(nativeInterop);
                nativeInterop.WriteLine("NativeBootstrap Created");
                IntPtr[] thunks = instance.GetCallbackThunks();
                callbackCount = Math.Min(callbackCount, thunks.Length);
                Array.Copy(thunks, callbacks, callbackCount);

                nativeInterop.WriteLine("NativeBootstrap Started Successfully");

                return 0;
            }
            catch (Exception e)
            {
                nativeInterop.LogException(e);
            }

            return -1;
        }

        public void ShowError(string text, string caption)
        {
            this.nativeInterop.ShowError(text, caption);
        }

        public void Trace(string text)
        {
            this.nativeInterop.WriteLine(text);
        }

        // IDisposable
        public void Dispose()
        {
            if (instance != null)
            {
                instance = null;
            }
        }

        // IUITester
        public void ExecuteTest()
        {
            this.nativeInterop.WriteLine($"{System.Threading.Thread.CurrentThread.ManagedThreadId}: Going to GC.Collect");
            GC.Collect();

        }

        private static void PrintStartup(ILogger logger)
        {
            logger.WriteLine($"UITester for Mac Started at {DateTime.Now}");
            logger.WriteLine($"DotNet Version: {Environment.Version}, Framework: {RuntimeInformation.FrameworkDescription} OS Version: {RuntimeInformation.OSDescription}, OS Architecture: {RuntimeInformation.OSArchitecture}");
            FileVersionInfo coreAssemblyInfo = FileVersionInfo.GetVersionInfo(typeof(object).Assembly.Location);
            logger.WriteLine($"CoreClr Version: {coreAssemblyInfo.ProductVersion}");
        }

        private IntPtr[] GetCallbackThunks()
        {
            return this.nativeInterop.CreateCallbackThunks(this, this);
        }
    }
}
