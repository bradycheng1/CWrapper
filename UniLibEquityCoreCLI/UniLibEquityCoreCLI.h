#pragma once

#include "ICalculationContext.h"
#include <msclr/marshal_cppstd.h>
#include <vcclr.h>

namespace UniLibEquityCLI
{
    /// <summary>
    /// C++/CLI 包装类：将 UniLibEquityCore64.dll 中的 ICalculationContext 接口
    /// 和 Functions 命名空间下的纯函数包装为 .NET 友好的 API，供 C# 直接调用。
    /// 
    /// 使用方式：
    ///   using UniLibEquityCLI;
    ///   
    ///   // 1. 纯函数调用（静态方法，无需实例化）
    ///   bool isWorkday = Functions::IsWorkingDay("SSE", 20250417);
    ///   int newDate   = Functions::AddPeriod("SSE", 20250417, "1M", 0);
    ///   double yf     = Functions::YearFraction(0, 20250101, 20251231);
    ///   
    ///   // 2. ICalculationContext 实例（实现 IDisposable）
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
        /// <summary>判断指定日期是否为工作日</summary>
        /// <param name="calendar">日历名称，如 "SSE"（上交所）、"SZSE"（深交所）</param>
        /// <param name="date">Excel 日期整数，如 20250417 表示 2025-04-17</param>
        /// <param name="useOracleDates">false=用 Excel 日期，true=用 Oracle 日期（默认 false）</param>
        /// <returns>1=工作日，0=非工作日，-1=失败</returns>
        static int IsWorkingDay(String^ calendar, int date, bool useOracleDates = false)
        {
            std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
            return UniLibEquity::Interface::Functions::uxIsWorkingDay(cpp_cal, date, useOracleDates);
        }

        /// <summary>对指定日期加减一段时间</summary>
        /// <param name="calendar">日历名称</param>
        /// <param name="startDate">起始日期（Excel 整数）</param>
        /// <param name="period">期间字符串，如 "1D"、"1W"、"1M"、"3M"、"1Y"</param>
        /// <param name="dayAdjust">日期调整方向：0=不调整，1=下个有效日，-1=上个有效日</param>
        /// <param name="useOracleDates">false=用 Excel 日期，true=用 Oracle 日期（默认 false）</param>
        /// <returns>计算后的日期整数，0=失败</returns>
        static int AddPeriod(String^ calendar, int startDate, String^ period, int dayAdjust, bool useOracleDates = false)
        {
            std::string cpp_cal    = msclr::interop::marshal_as<std::string>(calendar);
            std::string cpp_period = msclr::interop::marshal_as<std::string>(period);
            return UniLibEquity::Interface::Functions::uxAddPeriod(cpp_cal, startDate, cpp_period, dayAdjust, useOracleDates);
        }

        /// <summary>获取指定规则对应的最近工作日</summary>
        /// <param name="calendar">日历名称</param>
        /// <param name="startDate">起始日期（Excel 整数）</param>
        /// <param name="useFollowing">true=下一个工作日，false=上一个工作日</param>
        /// <param name="useOracleDates">false=用 Excel 日期，true=用 Oracle 日期（默认 false）</param>
        /// <returns>调整后的日期整数，0=失败</returns>
        static int GetMatchingBusinessday(String^ calendar, int startDate, bool useFollowing, bool useOracleDates = false)
        {
            std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
            return UniLibEquity::Interface::Functions::uxGetMatchingBusinessday(cpp_cal, startDate, useFollowing, useOracleDates);
        }

        /// <summary>计算两个日期之间的年分数（按 DayCount 规则）</summary>
        /// <param name="source">日期计数基准：0=US, 1=EU, 2=30/360, 3=Actual/360, 4=Actual/365, 5=Actual/Actual</param>
        /// <param name="start">起始日期（Excel 整数）</param>
        /// <param name="end">结束日期（Excel 整数）</param>
        /// <param name="useOracleDates">false=用 Excel 日期，true=用 Oracle 日期（默认 false）</param>
        /// <returns>年分数，0=失败</returns>
        static double YearFraction(int source, int start, int end, bool useOracleDates = false)
        {
            return UniLibEquity::Interface::Functions::uxYearFraction(source, start, end, useOracleDates);
        }

        /// <summary>计算两个日期之间的天数（按 DayCount 规则）</summary>
        /// <param name="source">日期计数基准（同 YearFraction）</param>
        /// <param name="start">起始日期（Excel 整数）</param>
        /// <param name="end">结束日期（Excel 整数）</param>
        /// <param name="useOracleDates">false=用 Excel 日期，true=用 Oracle 日期（默认 false）</param>
        /// <returns>天数，0=失败</returns>
        static int DayCount(int source, int start, int end, bool useOracleDates = false)
        {
            return UniLibEquity::Interface::Functions::uxDayCount(source, start, end, useOracleDates);
        }

        /// <summary>计算两个日期之间有多少个工作日</summary>
        /// <param name="calendar">日历名称</param>
        /// <param name="start">起始日期（Excel 整数）</param>
        /// <param name="end">结束日期（Excel 整数）</param>
        /// <param name="useOracleDates">false=用 Excel 日期，true=用 Oracle 日期（默认 false）</param>
        /// <returns>工作日天数，-1=失败</returns>
        static int OpenDayCount(String^ calendar, int start, int end, bool useOracleDates = false)
        {
            std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
            return UniLibEquity::Interface::Functions::uxOpenDayCount(cpp_cal, start, end, useOracleDates);
        }
    };

    /// <summary>
    /// CalculationContext 工厂与实例包装类。
    /// 实现 IDisposable，请在 using 块中使用，或手动调用 Dispose()。
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
