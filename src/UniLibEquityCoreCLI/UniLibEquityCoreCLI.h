#pragma once

#include <windows.h>

// ---------------------------------------------------------------------------
// extern "C" declarations for the native DLL.
// ---------------------------------------------------------------------------
extern "C"
{
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

    // ========================================================================
    // Functions - static helpers wrapping the native DLL calls
    // ========================================================================
    public ref class Functions
    {
    public:
        static int IsWorkingDay(String^ calendar, int date, int useOracleDates);
        static int AddPeriod(String^ calendar, int startDate, String^ period, int dayAdjust, int useOracleDates);
        static int GetMatchingBusinessday(String^ calendar, int startDate, int useFollowing, int useOracleDates);
        static double YearFraction(int source, int start, int end, int useOracleDates);
        static int DayCount(int source, int start, int end, int useOracleDates);
        static int OpenDayCount(String^ calendar, int start, int end, int useOracleDates);
    };

    // ========================================================================
    // CalculationContext - IDisposable wrapper (stub only)
    // ========================================================================
    public ref class CalculationContext : public IDisposable
    {
    private:
        void* m_nativeHandle;
        bool  m_disposed;

    public:
        static CalculationContext^ Create(
            long today,
            Collections::Generic::Dictionary<String^, double>^ instrumentSpots,
            Collections::Generic::Dictionary<String^, Tuple<String^, String^, double>^>^ fxSpots,
            Collections::Generic::Dictionary<String^, Collections::Generic::Dictionary<long, double>^>^ yieldCurveData,
            Collections::Generic::Dictionary<String^, Collections::Generic::Dictionary<long, double>^>^ repoData,
            Collections::Generic::Dictionary<String^, Collections::Generic::List<long>^>^ volatilityExpiries,
            Collections::Generic::Dictionary<String^, Collections::Generic::List<double>^>^ volatilityStrikes,
            Collections::Generic::Dictionary<String^, Collections::Generic::List<double>^>^ volatilityValues,
            Collections::Generic::Dictionary<String^, String^>^ currencies,
            Collections::Generic::Dictionary<String^, String^>^ calendars,
            Collections::Generic::Dictionary<String^, String^>^ marketOffsets,
            Collections::Generic::Dictionary<String^, String^>^ yieldCurves,
            Collections::Generic::Dictionary<String^, Collections::Generic::List<long>^>^ dividendExDates,
            Collections::Generic::Dictionary<String^, Collections::Generic::List<long>^>^ dividendPayDates,
            Collections::Generic::Dictionary<String^, Collections::Generic::List<String^>^>^ dividendCurrencies,
            Collections::Generic::Dictionary<String^, Collections::Generic::List<double>^>^ dividendGross,
            Collections::Generic::Dictionary<String^, Collections::Generic::List<double>^>^ dividendTaxFactor,
            Collections::Generic::Dictionary<String^, Tuple<String^, String^>^>^ fxYieldCurves,
            [Out] String^% error);

        double GetSpot(String^ instrument, [Out] String^% error);
        double GetForwardCompoundFactor(String^ yieldCurve, long date1, long date2, double overrate, [Out] String^% error);
        double GetVolatility(String^ instrument, long startDate, long endDate, double strike, int volatType, bool isCall, [Out] String^% error);
        double GetRepoMargin(String^ instrument, long date1, long date2, [Out] String^% error);
        double GetForwardPrice(String^ instrument, long date, [Out] String^% error);

    public:
        property bool IsDisposed { bool get() { return m_disposed; } }

    protected:
        void Dispose(bool disposing);

    private:
        ~CalculationContext();

    internal:
        CalculationContext(void* handle);

    public:
        virtual void Dispose() = IDisposable::Dispose;
    };
}
