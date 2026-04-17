#pragma once

#include <windows.h>
#include <string>
#include <msclr\marshal.h>

// ---------------------------------------------------------------------------
// Wrapper for the native UniLibEquityCore64.dll.
// Uses extern "C" declarations to avoid __declspec(dllimport) calling-convention
// warnings. The DLL must be in the same directory as the output DLL at runtime.
// ---------------------------------------------------------------------------
extern "C"
{
    // Static functions from Functions namespace
    int __cdecl uxIsWorkingDay(const char* calendars, int date, int useOracleInsteadOfExcelDates);
    int __cdecl uxAddPeriod(const char* calendars, int startDate, const char* period, int dayAdjust, int useOracleInsteadOfExcelDates);
    int __cdecl uxGetMatchingBusinessday(const char* calendars, int startDate, int useFollowing, int useOracleInsteadOfExcelDates);
    double __cdecl uxYearFraction(int source, int start, int end, int useOracleInsteadOfExcelDates);
    int __cdecl uxDayCount(int source, int start, int end, int useOracleInsteadOfExcelDates);
    int __cdecl uxOpenDayCount(const char* calendar, int start, int end, int useOracleInsteadOfExcelDates);
}

namespace UniLibEquityCLI
{
    using namespace System;
    using namespace System::Collections::Generic;

    // ========================================================================
    // Functions - static helpers wrapping the native DLL calls
    // ========================================================================
    public ref class Functions
    {
    public:
        /// <summary>Check if a date is a working day.</summary>
        /// <param name="calendar">Calendar name, e.g. "SSE".</param>
        /// <param name="date">Excel date integer (e.g. 20250417 = 2025-04-17).</param>
        /// <param name="useOracleDates">0=Excel dates, 1=Oracle dates.</param>
        /// <returns>1=workday, 0=holiday, -1=error.</returns>
        static int IsWorkingDay(String^ calendar, int date, int useOracleDates)
        {
            std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
            return ::uxIsWorkingDay(cpp_cal.c_str(), date, useOracleDates);
        }

        /// <summary>Add/subtract a period to a date.</summary>
        /// <param name="period">Period string: "1D", "1W", "1M", "3M", "1Y".</param>
        /// <param name="dayAdjust">0=none, 1=next biz day, -1=prev biz day.</param>
        /// <returns>Resulting date integer, 0=error.</returns>
        static int AddPeriod(String^ calendar, int startDate, String^ period, int dayAdjust, int useOracleDates)
        {
            std::string cpp_cal    = msclr::interop::marshal_as<std::string>(calendar);
            std::string cpp_period = msclr::interop::marshal_as<std::string>(period);
            return ::uxAddPeriod(cpp_cal.c_str(), startDate, cpp_period.c_str(), dayAdjust, useOracleDates);
        }

        /// <summary>Get nearest business day.</summary>
        /// <param name="useFollowing">1=next biz day, 0=prev biz day.</param>
        /// <returns>Adjusted date integer, 0=error.</returns>
        static int GetMatchingBusinessday(String^ calendar, int startDate, int useFollowing, int useOracleDates)
        {
            std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
            return ::uxGetMatchingBusinessday(cpp_cal.c_str(), startDate, useFollowing, useOracleDates);
        }

        /// <summary>Year fraction between two dates.</summary>
        /// <param name="source">0=US, 1=EU, 2=30/360, 3=Act/360, 4=Act/365, 5=Act/Act.</param>
        static double YearFraction(int source, int start, int end, int useOracleDates)
        {
            return ::uxYearFraction(source, start, end, useOracleDates);
        }

        /// <summary>Day count between two dates.</summary>
        static int DayCount(int source, int start, int end, int useOracleDates)
        {
            return ::uxDayCount(source, start, end, useOracleDates);
        }

        /// <summary>Working day count between two dates.</summary>
        static int OpenDayCount(String^ calendar, int start, int end, int useOracleDates)
        {
            std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
            return ::uxOpenDayCount(cpp_cal.c_str(), start, end, useOracleDates);
        }
    };

    // ========================================================================
    // CalculationContext - IDisposable wrapper for ICalculationContext
    // ========================================================================
    public ref class CalculationContext : public IDisposable
    {
    private:
        // Forward declare the native handle type
        void* m_nativeHandle;
        bool  m_disposed;

    public:
        /// <summary>Factory: create a CalculationContext. All parameters nullable.</summary>
        /// <param name="error">Error message on failure (non-empty = error).</param>
        static CalculationContext^ Create(
            long today,
            Dictionary<String^, double>^ instrumentSpots,
            Dictionary<String^, Tuple<String^, String^, double>^>^ fxSpots,
            Dictionary<String^, Dictionary<long, double>^>^ yieldCurveData,
            Dictionary<String^, Dictionary<long, double>^>^ repoData,
            Dictionary<String^, List<long>^>^ volatilityExpiries,
            Dictionary<String^, List<double>^>^ volatilityStrikes,
            Dictionary<String^, List<double>^>^ volatilityValues,
            Dictionary<String^, String^>^ currencies,
            Dictionary<String^, String^>^ calendars,
            Dictionary<String^, String^>^ marketOffsets,
            Dictionary<String^, String^>^ yieldCurves,
            Dictionary<String^, List<long>^>^ dividendExDates,
            Dictionary<String^, List<long>^>^ dividendPayDates,
            Dictionary<String^, List<String^>^>^ dividendCurrencies,
            Dictionary<String^, List<double>^>^ dividendGross,
            Dictionary<String^, List<double>^>^ dividendTaxFactor,
            Dictionary<String^, Tuple<String^, String^>^>^ fxYieldCurves,
            [Out] String^% error);

        /// <summary>Get spot price for an instrument.</summary>
        double GetSpot(String^ instrument, [Out] String^% error);

        /// <summary>Get forward compound factor.</summary>
        double GetForwardCompoundFactor(String^ yieldCurve, long date1, long date2, double overrate, [Out] String^% error);

        /// <summary>Get implied volatility.</summary>
        double GetVolatility(String^ instrument, long startDate, long endDate, double strike, int volatType, bool isCall, [Out] String^% error);

        /// <summary>Get repo margin.</summary>
        double GetRepoMargin(String^ instrument, long date1, long date2, [Out] String^% error);

        /// <summary>Get forward price.</summary>
        double GetForwardPrice(String^ instrument, long date, [Out] String^% error);

        // IDisposable
    public:
        property bool IsDisposed { bool get() { return m_disposed; } }
        virtual void Dispose() sealed = IDisposable::Dispose;

    protected:
        !CalculationContext();
        void Dispose(bool disposing);

    private:
        ~CalculationContext() { Dispose(true); }

    internal:
        CalculationContext(void* handle)
            : m_nativeHandle(handle), m_disposed(false) {}
    };
}
