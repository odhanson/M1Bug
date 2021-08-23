

using System;

namespace UITester.Mac.Managed
{
    public static class ILoggerExtensions
    {
        public static void LogException(this ILogger logger, Exception e)
        {
            logger.WriteLine(e.ToString());
        }
    }
}
