

using System.Runtime.InteropServices;

namespace UITester.Mac.Managed.Interop
{
    internal static class InteropDelegates
    {
        public delegate void LogLine([MarshalAs(UnmanagedType.LPStr)] string text);

        public delegate void ShowErrorMessage([MarshalAs(UnmanagedType.LPStr)] string text, [MarshalAs(UnmanagedType.LPStr)] string caption);

        public delegate void ExecuteTest();

        public delegate void Dispose();
    }
}
