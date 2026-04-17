#include "pch.h"
#include "UniLibEquityCoreCLI.h"

namespace UniLibEquityCLI
{
    // ========================================================================
    // Functions - implementation delegates to extern "C" native calls
    // ========================================================================

    int Functions::IsWorkingDay(String^ calendar, int date, int useOracleDates)
    {
        std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
        return ::uxIsWorkingDay(cpp_cal.c_str(), date, useOracleDates);
    }

    int Functions::AddPeriod(String^ calendar, int startDate, String^ period, int dayAdjust, int useOracleDates)
    {
        std::string cpp_cal    = msclr::interop::marshal_as<std::string>(calendar);
        std::string cpp_period = msclr::interop::marshal_as<std::string>(period);
        return ::uxAddPeriod(cpp_cal.c_str(), startDate, cpp_period.c_str(), dayAdjust, useOracleDates);
    }

    int Functions::GetMatchingBusinessday(String^ calendar, int startDate, int useFollowing, int useOracleDates)
    {
        std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
        return ::uxGetMatchingBusinessday(cpp_cal.c_str(), startDate, useFollowing, useOracleDates);
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
        std::string cpp_cal = msclr::interop::marshal_as<std::string>(calendar);
        return ::uxOpenDayCount(cpp_cal.c_str(), start, end, useOracleDates);
    }

    // ========================================================================
    // CalculationContext - currently a stub; full implementation requires
    // a native C-wrapper DLL that exposes ICalculationContext via a C API.
    // The Functions class above is fully functional and ready to use.
    // ========================================================================

    // Helper: convert Dictionary<String^, String^>^ to std::map<std::string, std::string>
    static std::map<std::string, std::string> MarshalStringDict(Dictionary<String^, String^>^ d)
    {
        std::map<std::string, std::string> m;
        if (d != nullptr)
            for each (auto kvp in d)
                m[msclr::interop::marshal_as<std::string>(kvp.Key)] =
                   msclr::interop::marshal_as<std::string>(kvp.Value);
        return m;
    }

    // Helper: convert List<double>^ to std::vector<double>
    static std::vector<double> MarshalDoubleList(List<double>^ lst)
    {
        std::vector<double> v;
        if (lst != nullptr)
            for each (double d in lst)
                v.push_back(d);
        return v;
    }

    // Helper: convert List<long>^ to std::vector<long>
    static std::vector<long> MarshalLongList(List<long>^ lst)
    {
        std::vector<long> v;
        if (lst != nullptr)
            for each (long d in lst)
                v.push_back(d);
        return v;
    }

    // Helper: convert List<String^>^ to std::vector<std::string>
    static std::vector<std::string> MarshalStringList(List<String^>^ lst)
    {
        std::vector<std::string> v;
        if (lst != nullptr)
            for each (String^ s in lst)
                v.push_back(msclr::interop::marshal_as<std::string>(s));
        return v;
    }

    // NOTE: The Create() method below is a STUB.
    // It cannot directly call ICalculationContext::Create() because that function
    // returns a C++ object with STL containers, which managed code cannot hold.
    //
    // To fully implement CalculationContext, you need a native C-wrapper DLL that:
    //   1. Wraps ICalculationContext in opaque handle functions (create/destroy/method)
    //   2. Is called from this C++/CLI layer via extern "C" __declspec(dllexport)
    //
    // Once that C-wrapper DLL is available, replace the stub body with calls to
    // the C-wrapper functions (similar to how Functions does it above).
    CalculationContext^ CalculationContext::Create(
        long,
        Dictionary<String^, double>^,
        Dictionary<String^, Tuple<String^, String^, double>^>^,
        Dictionary<String^, Dictionary<long, double>^>^,
        Dictionary<String^, Dictionary<long, double>^>^,
        Dictionary<String^, List<long>^>^,
        Dictionary<String^, List<double>^>^,
        Dictionary<String^, List<double>^>^,
        Dictionary<String^, String^>^,
        Dictionary<String^, String^>^,
        Dictionary<String^, String^>^,
        Dictionary<String^, String^>^,
        Dictionary<String^, List<long>^>^,
        Dictionary<String^, List<long>^>^,
        Dictionary<String^, List<String^>^>^,
        Dictionary<String^, List<double>^>^,
        Dictionary<String^, List<double>^>^,
        Dictionary<String^, Tuple<String^, String^>^>^,
        [Out] String^% error)
    {
        error = "CalculationContext::Create is not yet implemented. "
                "The Functions class is fully functional. "
                "To implement CalculationContext, build a native C-wrapper DLL.";
        return nullptr;
    }

    double CalculationContext::GetSpot(String^ instrument, [Out] String^% error)
    {
        error = "CalculationContext not available - stub implementation.";
        return 0.0;
    }

    double CalculationContext::GetForwardCompoundFactor(
        String^ yieldCurve, long date1, long date2, double overrate, [Out] String^% error)
    {
        error = "CalculationContext not available - stub implementation.";
        return 0.0;
    }

    double CalculationContext::GetVolatility(
        String^ instrument, long startDate, long endDate, double strike,
        int volatType, bool isCall, [Out] String^% error)
    {
        error = "CalculationContext not available - stub implementation.";
        return 0.0;
    }

    double CalculationContext::GetRepoMargin(
        String^ instrument, long date1, long date2, [Out] String^% error)
    {
        error = "CalculationContext not available - stub implementation.";
        return 0.0;
    }

    double CalculationContext::GetForwardPrice(
        String^ instrument, long date, [Out] String^% error)
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
            // If a native handle existed, release it here.
            m_nativeHandle = nullptr;
            m_disposed = true;
        }
    }

    CalculationContext::!CalculationContext()
    {
        m_nativeHandle = nullptr;
    }
}
