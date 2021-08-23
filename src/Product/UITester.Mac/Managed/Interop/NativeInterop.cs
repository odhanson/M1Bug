

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace UITester.Mac.Managed.Interop
{
    internal class NativeInterop : ILogger, IUIHost
    {
        private readonly IntPtr[] nativeMethods;
        private readonly InteropDelegates.LogLine logLine;
        private readonly InteropDelegates.ShowErrorMessage showErrorMessage;
        private readonly List<Delegate> callbacks;

        public NativeInterop(IntPtr[] nativeMethods)
        {
            this.nativeMethods = nativeMethods;

            this.logLine = GetDelegateForNativeMethod<InteropDelegates.LogLine>(
                nativeMethods, Managed2NativeIndexes.LogLine);
            this.showErrorMessage = GetDelegateForNativeMethod<InteropDelegates.ShowErrorMessage>(
                nativeMethods, Managed2NativeIndexes.ShowErrorMessage);
            this.callbacks = new List<Delegate>();
        }

        private enum Native2ManagedIndexes
        {
            Dispose = 0,
            ExecuteTest,

            // Do not remove this
            NumberOfMethods,
        }

        private enum Managed2NativeIndexes
        {
            LogLine = 0,
            ShowErrorMessage,

            // Do not remove this
            NumberOfMethods,
        }

        public IntPtr[] CreateCallbackThunks(IUITester uiTester, IDisposable disposable)
        {
            IntPtr[] thunks = new IntPtr[(int)Native2ManagedIndexes.NumberOfMethods];

            this.AddThunk<InteropDelegates.Dispose>(thunks, Native2ManagedIndexes.Dispose, disposable.Dispose);

            // Mac input auto-replaces double quotes into left-double quotes and right-double quotes, which causes JSON parsing to fail
            this.AddThunk<InteropDelegates.ExecuteTest>(
                thunks,
                Native2ManagedIndexes.ExecuteTest,
                uiTester.ExecuteTest);

            return thunks;
        }

        // ILogger
        public void WriteLine(string text)
        {
            this.logLine(text);
        }

        // IUIHost
        public void ShowError(string text, string caption)
        {
            this.showErrorMessage(text, caption);
        }

        private static TDelegate GetDelegateForNativeMethod<TDelegate>(
            IntPtr[] nativeMethods, Managed2NativeIndexes index)
        {
            return Marshal.GetDelegateForFunctionPointer<TDelegate>(nativeMethods[(int)index]);
        }

        private void AddThunk<TDelegate>(IntPtr[] thunks, Native2ManagedIndexes index, TDelegate callback)
            where TDelegate : Delegate
        {
            this.callbacks.Add(callback);
            thunks[(int)index] = Marshal.GetFunctionPointerForDelegate(callback);
        }
    }
}
