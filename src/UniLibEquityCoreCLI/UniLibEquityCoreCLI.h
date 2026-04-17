#pragma once

#include "ICalculationContext.h"
#include <msclr/marshal_cppstd.h>
#include <vcclr.h>

namespace UniLibEquityCLI
{
    /// <summary>
    /// C++/CLI wrapper for UniLibEquityCore64.dll.
    /// Exposes ICalculationContext and Functions to C#.
    /// 
    /// Usage:
    ///   using UniLibEquityCLI;
    ///   
    ///   // 1. Static functions (no instance needed)
    ///   bool isWorkday = Functions::IsWorkingDay("SSE", 20250417);
    ///   int newDate   = Functions::AddPeriod("SSE", 20250417, "1M", 0);
    ///   double yf     = Functions::YearFraction(0, 20250101, 20251231);
    ///   
    ///   // 2. CalculationContext (IDisposable)
    ///   var spots = new Dictionary&lt;string, double&gt; { ["600519.SH"] = 1850.0 };
    ///   string error;
    ///   using (var ctx = CalculationContext.Create(today, spots, ... , out error))
    ///   {
    ///       double spot = ctx.GetSpot("600519.SH", out error);
    ///   }
    /// </summary>
    public ref class Functions
    {
    public:
        /// <summary>Check if a date is a working day</summary>
        /// <param name="calendar">Calendar name, e.g. "SSE" (Shanghai), "SZSE" (Shenzhen)</param>
        /// <param name="date">Excel date integer, e.g. 20250417 = 2025-04-17</param>
        /// <param name="useOracleDates">false=Excel dates, true=Oracle dates (default false)</param>
        /// <returns>1=workday, 0=holiday, -1=error</returns>
        static int IsWorkingDay(String^ calendar, int date, bool useOracleDates = false)
        {
            std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
            return UniLibEquity::Interface::Functions::uxIsWorkingDay(cpp_cal, date, useOracleDates);
        }

        /// <summary>Add/subtract a period to/from a date</summary>
        /// <param name="calendar">Calendar name</param>
        /// <param name="startDate">Start date (Excel integer)</param>
        /// <param name="period">Period string, e.g. "1D", "1W", "1M", "3M", "1Y"</param>
        /// <param name="dayAdjust">Date adjustment: 0=none, 1=next biz day, -1=prev biz day</param>
        /// <param name="useOracleDates">false=Excel dates, true=Oracle dates (default false)</param>
        /// <returns>Resulting date integer, 0=error</returns>
        static int AddPeriod(String^ calendar, int startDate, String^ period, int dayAdjust, bool useOracleDates = false)
        {
            std::string cpp_cal    = msclr::interop::marshal_as<std::string>(calendar);
            std::string cpp_period = msclr::interop::marshal_as<std::string>(period);
            return UniLibEquity::Interface::Functions::uxAddPeriod(cpp_cal, startDate, cpp_period, dayAdjust, useOracleDates);
        }

        /// <summary>Get the nearest business day per adjustment rule</summary>
        /// <param name="calendar">Calendar name</param>
        /// <param name="startDate">Start date (Excel integer)</param>
        /// <param name="useFollowing">true=next biz day, false=prev biz day</param>
        /// <param name="useOracleDates">false=Excel dates, true=Oracle dates (default false)</param>
        /// <returns>Adjusted date integer, 0=error</returns>
        static int GetMatchingBusinessday(String^ calendar, int startDate, bool useFollowing, bool useOracleDates = false)
        {
            std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
            return UniLibEquity::Interface::Functions::uxGetMatchingBusinessday(cpp_cal, startDate, useFollowing, useOracleDates);
        }

        /// <summary>Calculate year fraction between two dates</summary>
        /// <param name="source">Day-count basis: 0=US, 1=EU, 2=30/360, 3=Act/360, 4=Act/365, 5=Act/Act</param>
        /// <param name="start">Start date (Excel integer)</param>
        /// <param name="end">End date (Excel integer)</param>
        /// <param name="useOracleDates">false=Excel dates, true=Oracle dates (default false)</param>
        /// <returns>Year fraction, 0=error</returns>
        static double YearFraction(int source, int start, int end, bool useOracleDates = false)
        {
            return UniLibEquity::Interface::Functions::uxYearFraction(source, start, end, useOracleDates);
        }

        /// <summary>Calculate day count between two dates</summary>
        /// <param name="source">Day-count basis (same as YearFraction)</param>
        /// <param name="start">Start date (Excel integer)</param>
        /// <param name="end">End date (Excel integer)</param>
        /// <param name="useOracleDates">false=Excel dates, true=Oracle dates (default false)</param>
        /// <returns>Day count, 0=error</returns>
        static int DayCount(int source, int start, int end, bool useOracleDates = false)
        {
            return UniLibEquity::Interface::Functions::uxDayCount(source, start, end, useOracleDates);
        }

        /// <summary>Count working days between two dates</summary>
        /// <param name="calendar">Calendar name</param>
        /// <param name="start">Start date (Excel integer)</param>
        /// <param name="end">End date (Excel integer)</param>
        /// <param name="useOracleDates">false=Excel dates, true=Oracle dates (default false)</param>
        /// <returns>Working day count, -1=error</returns>
        static int OpenDayCount(String^ calendar, int start, int end, bool useOracleDates = false)
        {
            std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
            return UniLibEquity::Interface::Functions::uxOpenDayCount(cpp_cal, start, end, useOracleDates);
        }
    };

    /// <summary>
    /// Wrapper for ICalculationContext. Implements IDisposable.
    /// Always use inside a using block or call Dispose() manually.
    /// </summary>
    public ref class CalculationContext : public IDisposable
    {
    private:
        UniLibEquity::Interface::ICalculationContext* m_pContext;
        bool m_disposed;

    internal:
        CalculationContext(UniLibEquity::Interface::ICalculationContext* p, bool owner)
            : m_pContext(p), m_disposed(false) {}

    public:
        /// <summary>Factory: create a CalculationContext instance.</summary>
        /// <param name="today">Settlement date, Excel integer (e.g. 20250417)</param>
        /// <param name="instrumentSpots">Instrument prices: Key=code, Value=price</param>
        /// <param name="fxSpots">FX spots: Key=ccy pair, Value=Tuple(CCY1, CCY2, SpotRate)</param>
        /// <param name="yieldCurveData">Yield curves: Key=curve name, Value=Map(DateInt, Rate)</param>
        /// <param name="repoData">Repo rates: Key=instrument, Value=Map(DateInt, RepoRate)</param>
        /// <param name="volatilityExpiries">Vol expiry dates: Key=instrument, Value=DateInt[]</param>
        /// <param name="volatilityStrikes">Vol strikes: Key=instrument, Value=double[]</param>
        /// <param name="volatilityValues">Vol values: Key=instrument, Value=double[]</param>
        /// <param name="currencies">Instrument currencies: Key=instrument, Value=ccy code</param>
        /// <param name="calendars">Instrument calendars: Key=instrument, Value=calendar name</param>
        /// <param name="marketOffsets">Market offsets: Key=instrument, Value=offset string</param>
        /// <param name="yieldCurves">Linked yield curves: Key=instrument, Value=curve name</param>
        /// <param name="dividendExDates">Dividend ex-dates: Key=instrument, Value=DateInt[]</param>
        /// <param name="dividendPayDates">Dividend pay dates: Key=instrument, Value=DateInt[]</param>
        /// <param name="dividendCurrencies">Dividend currencies: Key=instrument, Value=Currency[]</param>
        /// <param name="dividendGross">Gross dividends: Key=instrument, Value=double[]</param>
        /// <param name="dividendTaxFactor">Dividend tax factors: Key=instrument, Value=double[]</param>
        /// <param name="fxYieldCurves">FX yield curves: Key=Currency, Value=Tuple(CCY1, CCY2)</param>
        /// <param name="error">Error message output (non-empty on failure)</param>
        static CalculationContext^ Create(
            long today,
            System::Collections::Generic::Dictionary<System::String^, double>^ instrumentSpots,
            System::Collections::Generic::Dictionary<System::String^, System::Tuple<System::String^, System::String^, double>^>^ fxSpots,
            System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::Dictionary<long, double>^>^ yieldCurveData,
            System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::Dictionary<long, double>^>^ repoData,
            System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<long>^>^ volatilityExpiries,
            System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<double>^>^ volatilityStrikes,
            System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<double>^>^ volatilityValues,
            System::Collections::Generic::Dictionary<System::String^, System::String^>^ currencies,
            System::Collections::Generic::Dictionary<System::String^, System::String^>^ calendars,
            System::Collections::Generic::Dictionary<System::String^, System::String^>^ marketOffsets,
            System::Collections::Generic::Dictionary<System::String^, System::String^>^ yieldCurves,
            System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<long>^>^ dividendExDates,
            System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<long>^>^ dividendPayDates,
            System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<System::String^>^>^ dividendCurrencies,
            System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<double>^>^ dividendGross,
            System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<double>^>^ dividendTaxFactor,
            System::Collections::Generic::Dictionary<System::String^, System::Tuple<System::String^, System::String^>^>^ fxYieldCurves,
            [Out] System::String^% error);

        double GetSpot(String^ instrument, [Out] System::String^% error);
        double GetForwardCompoundFactor(String^ yieldCurve, long date1, long date2, double overrate, [Out] System::String^% error);
        double GetVolatility(String^ instrument, long startDate, long endDate, double strike, int volatType, bool isCall, [Out] System::String^% error);
        double GetRepoMargin(String^ instrument, long date1, long date2, [Out] System::String^% error);
        double GetForwardPrice(String^ instrument, long date, [Out] System::String^% error);

        // IDisposable
    public:
        property bool IsDisposed { bool get() { return m_disposed; } }
        virtual void Dispose();
    protected:
        !CalculationContext();
        virtual void Dispose(bool disposing);
    private:
        ~CalculationContext() { Dispose(true); }
    };
}
