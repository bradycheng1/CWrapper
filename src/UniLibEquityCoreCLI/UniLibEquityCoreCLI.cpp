#include "pch.h"
#include "UniLibEquityCoreCLI.h"
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>

namespace UniLibEquityCLI
{
    using namespace System;
    using namespace System::Runtime::InteropServices;

    // ========================================================================
    // Helpers
    // ========================================================================
    static std::string MarshalString(String^ s)
    {
        return msclr::interop::marshal_as<std::string>(s);
    }

    static std::map<std::string, std::string> MarshalStringDict(
        Collections::Generic::Dictionary<String^, String^>^ d)
    {
        std::map<std::string, std::string> m;
        if (d != nullptr)
        {
            for each (auto kvp in d)
                m[MarshalString(kvp.Key)] = MarshalString(kvp.Value);
        }
        return m;
    }

    static std::vector<double> MarshalDoubleList(Collections::Generic::List<double>^ lst)
    {
        std::vector<double> v;
        if (lst != nullptr)
            for each (double d in lst)
                v.push_back(d);
        return v;
    }

    static std::vector<long> MarshalLongList(Collections::Generic::List<long>^ lst)
    {
        std::vector<long> v;
        if (lst != nullptr)
            for each (long d in lst)
                v.push_back(d);
        return v;
    }

    static std::vector<std::string> MarshalStringList(Collections::Generic::List<String^>^ lst)
    {
        std::vector<std::string> v;
        if (lst != nullptr)
            for each (String^ s in lst)
                v.push_back(MarshalString(s));
        return v;
    }

    // ========================================================================
    // Functions - implementation delegates to extern "C" native calls
    // ========================================================================

    int Functions::IsWorkingDay(String^ calendar, int date, int useOracleDates)
    {
        std::string cpp_cal = MarshalString(calendar);
        return ::uxIsWorkingDay(cpp_cal.c_str(), date, useOracleDates);
    }

    int Functions::AddPeriod(String^ calendar, int startDate, String^ period,
                             int dayAdjust, int useOracleDates)
    {
        std::string cpp_cal    = MarshalString(calendar);
        std::string cpp_period = MarshalString(period);
        return ::uxAddPeriod(cpp_cal.c_str(), startDate,
                              cpp_period.c_str(), dayAdjust, useOracleDates);
    }

    int Functions::GetMatchingBusinessday(String^ calendar, int startDate,
                                          int useFollowing, int useOracleDates)
    {
        std::string cpp_cal = MarshalString(calendar);
        return ::uxGetMatchingBusinessday(cpp_cal.c_str(), startDate,
                                          useFollowing, useOracleDates);
    }

    double Functions::YearFraction(int source, int start, int end, int useOracleDates)
    {
        return ::uxYearFraction(source, start, end, useOracleDates);
    }

    int Functions::DayCount(int source, int start, int end, int useOracleDates)
    {
        return ::uxDayCount(source, start, end, useOracleDates);
    }

    int Functions::OpenDayCount(String^ calendar, int start, int end, int useOracleDates)
    {
        std::string cpp_cal = MarshalString(calendar);
        return ::uxOpenDayCount(cpp_cal.c_str(), start, end, useOracleDates);
    }

    // ========================================================================
    // CalculationContext - stub
    // ========================================================================

    CalculationContext::CalculationContext(void* handle)
        : m_nativeHandle(handle), m_disposed(false) {}

    CalculationContext^ CalculationContext::Create(
        long,
        Collections::Generic::Dictionary<String^, double>^,
        Collections::Generic::Dictionary<String^, Tuple<String^, String^, double>^>^,
        Collections::Generic::Dictionary<String^, Collections::Generic::Dictionary<long, double>^>^,
        Collections::Generic::Dictionary<String^, Collections::Generic::Dictionary<long, double>^>^,
        Collections::Generic::Dictionary<String^, Collections::Generic::List<long>^>^,
        Collections::Generic::Dictionary<String^, Collections::Generic::List<double>^>^,
        Collections::Generic::Dictionary<String^, Collections::Generic::List<double>^>^,
        Collections::Generic::Dictionary<String^, String^>^,
        Collections::Generic::Dictionary<String^, String^>^,
        Collections::Generic::Dictionary<String^, String^>^,
        Collections::Generic::Dictionary<String^, String^>^,
        Collections::Generic::Dictionary<String^, Collections::Generic::List<long>^>^,
        Collections::Generic::Dictionary<String^, Collections::Generic::List<long>^>^,
        Collections::Generic::Dictionary<String^, Collections::Generic::List<String^>^>^,
        Collections::Generic::Dictionary<String^, Collections::Generic::List<double>^>^,
        Collections::Generic::Dictionary<String^, Collections::Generic::List<double>^>^,
        Collections::Generic::Dictionary<String^, Tuple<String^, String^>^>^,
        [Out] String^% error)
    {
        error = "CalculationContext::Create is not yet implemented. "
                "The Functions class is fully functional. "
                "To implement CalculationContext, build a native C-wrapper DLL.";
        return nullptr;
    }

    double CalculationContext::GetSpot(String^, [Out] String^% error)
    {
        error = "CalculationContext not available - stub implementation.";
        return 0.0;
    }

    double CalculationContext::GetForwardCompoundFactor(
        String^, long, long, double, [Out] String^% error)
    {
        error = "CalculationContext not available - stub implementation.";
        return 0.0;
    }

    double CalculationContext::GetVolatility(
        String^, long, long, double, int, bool, [Out] String^% error)
    {
        error = "CalculationContext not available - stub implementation.";
        return 0.0;
    }

    double CalculationContext::GetRepoMargin(String^, long, long, [Out] String^% error)
    {
        error = "CalculationContext not available - stub implementation.";
        return 0.0;
    }

    double CalculationContext::GetForwardPrice(String^, long, [Out] String^% error)
    {
        error = "CalculationContext not available - stub implementation.";
        return 0.0;
    }

    void CalculationContext::Dispose()
    {
        Dispose(true);
        GC::SuppressFinalize(this);
    }

    void CalculationContext::Dispose(bool)
    {
        if (!m_disposed) {
            m_nativeHandle = nullptr;
            m_disposed = true;
        }
    }

    CalculationContext::!CalculationContext()
    {
        m_nativeHandle = nullptr;
    }
}
