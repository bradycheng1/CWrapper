#include "pch.h"
#include "UniLibEquityCoreCLI.h"
#include <msclr/marshal.h>
#include <string>
#include <map>
#include <vector>

namespace UniLibEquityCLI
{
    using namespace System;
    using namespace System::Runtime::InteropServices;

    // ========================================================================
    // Helper: marshal System::String^ -> std::string
    // ========================================================================
    static std::string ToStdString(String^ s)
    {
        return msclr::interop::marshal_as<std::string>(s);
    }

    // ========================================================================
    // Functions - implementation
    // ========================================================================

    int Functions::IsWorkingDay(String^ calendar, int date, int useOracleDates)
    {
        return ::uxIsWorkingDay(ToStdString(calendar).c_str(), date, useOracleDates);
    }

    int Functions::AddPeriod(String^ calendar, int startDate, String^ period,
                             int dayAdjust, int useOracleDates)
    {
        return ::uxAddPeriod(ToStdString(calendar).c_str(), startDate,
                              ToStdString(period).c_str(), dayAdjust, useOracleDates);
    }

    int Functions::GetMatchingBusinessday(String^ calendar, int startDate,
                                          int useFollowing, int useOracleDates)
    {
        return ::uxGetMatchingBusinessday(ToStdString(calendar).c_str(),
                                          startDate, useFollowing, useOracleDates);
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
        return ::uxOpenDayCount(ToStdString(calendar).c_str(), start, end, useOracleDates);
    }

    // ========================================================================
    // CalculationContext - stub implementation
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
                "The Functions class is fully functional.";
        return nullptr;
    }

    double CalculationContext::GetSpot(String^, [Out] String^% error)
    {
        error = "CalculationContext not available - stub.";
        return 0.0;
    }

    double CalculationContext::GetForwardCompoundFactor(
        String^, long, long, double, [Out] String^% error)
    {
        error = "CalculationContext not available - stub.";
        return 0.0;
    }

    double CalculationContext::GetVolatility(
        String^, long, long, double, int, bool, [Out] String^% error)
    {
        error = "CalculationContext not available - stub.";
        return 0.0;
    }

    double CalculationContext::GetRepoMargin(String^, long, long, [Out] String^% error)
    {
        error = "CalculationContext not available - stub.";
        return 0.0;
    }

    double CalculationContext::GetForwardPrice(String^, long, [Out] String^% error)
    {
        error = "CalculationContext not available - stub.";
        return 0.0;
    }

    // C++/CLI destructor (maps to C# finalizer via ~)
    CalculationContext::~CalculationContext()
    {
        Dispose(true);
    }

    void CalculationContext::Dispose(bool)
    {
        if (!m_disposed) {
            m_nativeHandle = nullptr;
            m_disposed = true;
        }
    }

    void CalculationContext::Dispose()
    {
        Dispose(true);
        GC::SuppressFinalize(this);
    }

    // C++/CLI finalizer (maps to .NET finalizer via !)
    CalculationContext::!CalculationContext()
    {
        m_nativeHandle = nullptr;
    }
}
