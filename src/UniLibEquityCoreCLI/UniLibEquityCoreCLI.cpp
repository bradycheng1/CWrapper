#include "pch.h"
#include "UniLibEquityCoreCLI.h"

namespace UniLibEquityCLI
{
    CalculationContext^ CalculationContext::Create(
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
        [Out] System::String^% error)
    {
        error = System::String::Empty;

        // ---- Helper lambdas ----
        auto strMap = [](System::Collections::Generic::Dictionary<System::String^, System::String^>^ d) -> std::map<std::string, std::string> {
            std::map<std::string, std::string> m;
            if (d != nullptr)
                for each (auto kvp in d)
                    m[msclr::interop::marshal_as<std::string>(kvp.Key)] = msclr::interop::marshal_as<std::string>(kvp.Value);
            return m;
        };
        auto listToVecD = [](System::Collections::Generic::List<double>^ lst) -> std::vector<double> {
            std::vector<double> v;
            if (lst != nullptr) for each (double d in lst) v.push_back(d);
            return v;
        };
        auto listToVecL = [](System::Collections::Generic::List<long>^ lst) -> std::vector<long> {
            std::vector<long> v;
            if (lst != nullptr) for each (long d in lst) v.push_back(d);
            return v;
        };

        // 1. instrument_spots
        std::map<std::string, double> cpp_instrument_spots;
        if (instrumentSpots != nullptr)
            for each (auto kvp in instrumentSpots)
                cpp_instrument_spots[msclr::interop::marshal_as<std::string>(kvp.Key)] = kvp.Value;

        // 2. fx_spots
        std::map<std::string, std::tuple<std::string, std::string, double>> cpp_fx_spots;
        if (fxSpots != nullptr)
            for each (auto kvp in fxSpots)
                cpp_fx_spots[msclr::interop::marshal_as<std::string>(kvp.Key)] =
                std::make_tuple(
                    msclr::interop::marshal_as<std::string>(kvp.Value->Item1),
                    msclr::interop::marshal_as<std::string>(kvp.Value->Item2),
                    kvp.Value->Item3);

        // 3. yield_curve_data
        std::map<std::string, std::map<long, double>> cpp_yield_curve_data;
        if (yieldCurveData != nullptr)
            for each (auto kvp in yieldCurveData) {
                std::map<long, double> inner;
                if (kvp.Value != nullptr)
                    for each (auto ikvp in kvp.Value)
                        inner[ikvp.Key] = ikvp.Value;
                cpp_yield_curve_data[msclr::interop::marshal_as<std::string>(kvp.Key)] = inner;
            }

        // 4. repo_data
        std::map<std::string, std::map<long, double>> cpp_repo_data;
        if (repoData != nullptr)
            for each (auto kvp in repoData) {
                std::map<long, double> inner;
                if (kvp.Value != nullptr)
                    for each (auto ikvp in kvp.Value)
                        inner[ikvp.Key] = ikvp.Value;
                cpp_repo_data[msclr::interop::marshal_as<std::string>(kvp.Key)] = inner;
            }

        // 5. volatility_expiries
        std::map<std::string, std::vector<long>> cpp_vol_expiries;
        if (volatilityExpiries != nullptr)
            for each (auto kvp in volatilityExpiries)
                cpp_vol_expiries[msclr::interop::marshal_as<std::string>(kvp.Key)] = listToVecL(kvp.Value);

        // 6. volatility_strikes
        std::map<std::string, std::vector<double>> cpp_vol_strikes;
        if (volatilityStrikes != nullptr)
            for each (auto kvp in volatilityStrikes)
                cpp_vol_strikes[msclr::interop::marshal_as<std::string>(kvp.Key)] = listToVecD(kvp.Value);

        // 7. volatility_values
        std::map<std::string, std::vector<double>> cpp_vol_values;
        if (volatilityValues != nullptr)
            for each (auto kvp in volatilityValues)
                cpp_vol_values[msclr::interop::marshal_as<std::string>(kvp.Key)] = listToVecD(kvp.Value);

        // 8. currencies
        std::map<std::string, std::string> cpp_currencies = strMap(currencies);

        // 9. calendars
        std::map<std::string, std::string> cpp_calendars = strMap(calendars);

        // 10. market_offsets
        std::map<std::string, std::string> cpp_market_offsets = strMap(marketOffsets);

        // 11. yield_curves
        std::map<std::string, std::string> cpp_yield_curves = strMap(yieldCurves);

        // 12. dividend_exdates
        std::map<std::string, std::vector<long>> cpp_div_exdates;
        if (dividendExDates != nullptr)
            for each (auto kvp in dividendExDates)
                cpp_div_exdates[msclr::interop::marshal_as<std::string>(kvp.Key)] = listToVecL(kvp.Value);

        // 13. dividend_paydates
        std::map<std::string, std::vector<long>> cpp_div_paydates;
        if (dividendPayDates != nullptr)
            for each (auto kvp in dividendPayDates)
                cpp_div_paydates[msclr::interop::marshal_as<std::string>(kvp.Key)] = listToVecL(kvp.Value);

        // 14. dividend_currencies
        std::map<std::string, std::vector<std::string>> cpp_div_currencies;
        if (dividendCurrencies != nullptr)
            for each (auto kvp in dividendCurrencies) {
                std::vector<std::string> v;
                if (kvp.Value != nullptr)
                    for each (System::String^ s in kvp.Value)
                        v.push_back(msclr::interop::marshal_as<std::string>(s));
                cpp_div_currencies[msclr::interop::marshal_as<std::string>(kvp.Key)] = v;
            }

        // 15. dividend_gross
        std::map<std::string, std::vector<double>> cpp_div_gross;
        if (dividendGross != nullptr)
            for each (auto kvp in dividendGross)
                cpp_div_gross[msclr::interop::marshal_as<std::string>(kvp.Key)] = listToVecD(kvp.Value);

        // 16. dividend_tax_factor
        std::map<std::string, std::vector<double>> cpp_div_tax;
        if (dividendTaxFactor != nullptr)
            for each (auto kvp in dividendTaxFactor)
                cpp_div_tax[msclr::interop::marshal_as<std::string>(kvp.Key)] = listToVecD(kvp.Value);

        // 17. fx_yield_curves
        std::map<std::string, std::tuple<std::string, std::string>> cpp_fx_yc;
        if (fxYieldCurves != nullptr)
            for each (auto kvp in fxYieldCurves)
                cpp_fx_yc[msclr::interop::marshal_as<std::string>(kvp.Key)] =
                std::make_tuple(
                    msclr::interop::marshal_as<std::string>(kvp.Value->Item1),
                    msclr::interop::marshal_as<std::string>(kvp.Value->Item2));

        // 调用 C++ Create()
        std::string cpp_error;
        const UniLibEquity::Interface::ICalculationContext* p =
            UniLibEquity::Interface::ICalculationContext::Create(
                today,
                cpp_instrument_spots,
                cpp_fx_spots,
                cpp_yield_curve_data,
                cpp_repo_data,
                cpp_vol_expiries,
                cpp_vol_strikes,
                cpp_vol_values,
                cpp_currencies,
                cpp_calendars,
                cpp_market_offsets,
                cpp_yield_curves,
                cpp_div_exdates,
                cpp_div_paydates,
                cpp_div_currencies,
                cpp_div_gross,
                cpp_div_tax,
                cpp_fx_yc,
                cpp_error);

        if (p == nullptr) {
            error = gcnew System::String(cpp_error.c_str());
            return nullptr;
        }

        return gcnew CalculationContext(const_cast<UniLibEquity::Interface::ICalculationContext*>(p), true);
    }

    double CalculationContext::GetSpot(String^ instrument, [Out] System::String^% error)
    {
        std::string cpp_inst = msclr::interop::marshal_as<std::string>(instrument);
        std::string cpp_err;
        double result = m_pContext->GetSpot(cpp_inst, cpp_err);
        error = cpp_err.empty() ? System::String::Empty : gcnew System::String(cpp_err.c_str());
        return result;
    }

    double CalculationContext::GetForwardCompoundFactor(
        String^ yieldCurve, long date1, long date2, double overrate, [Out] System::String^% error)
    {
        std::string cpp_yc = msclr::interop::marshal_as<std::string>(yieldCurve);
        std::string cpp_err;
        double result = m_pContext->GetForwardCompoundFactor(cpp_yc, date1, date2, overrate, cpp_err);
        error = cpp_err.empty() ? System::String::Empty : gcnew System::String(cpp_err.c_str());
        return result;
    }

    double CalculationContext::GetVolatility(
        String^ instrument, long startDate, long endDate, double strike,
        int volatType, bool isCall, [Out] System::String^% error)
    {
        std::string cpp_inst = msclr::interop::marshal_as<std::string>(instrument);
        std::string cpp_err;
        double result = m_pContext->GetVolatility(cpp_inst, startDate, endDate, strike, volatType, isCall, cpp_err);
        error = cpp_err.empty() ? System::String::Empty : gcnew System::String(cpp_err.c_str());
        return result;
    }

    double CalculationContext::GetRepoMargin(
        String^ instrument, long date1, long date2, [Out] System::String^% error)
    {
        std::string cpp_inst = msclr::interop::marshal_as<std::string>(instrument);
        std::string cpp_err;
        double result = m_pContext->GetRepoMargin(cpp_inst, date1, date2, cpp_err);
        error = cpp_err.empty() ? System::String::Empty : gcnew System::String(cpp_err.c_str());
        return result;
    }

    double CalculationContext::GetForwardPrice(
        String^ instrument, long date, [Out] System::String^% error)
    {
        std::string cpp_inst = msclr::interop::marshal_as<std::string>(instrument);
        std::string cpp_err;
        double result = m_pContext->GetForwardPrice(cpp_inst, date, cpp_err);
        error = cpp_err.empty() ? System::String::Empty : gcnew System::String(cpp_err.c_str());
        return result;
    }

    void CalculationContext::Dispose()
    {
        Dispose(true);
        GC::SuppressFinalize(this);
    }

    void CalculationContext::Dispose(bool disposing)
    {
        if (!m_disposed) {
            if (m_pContext != nullptr) {
                delete m_pContext;
                m_pContext = nullptr;
            }
            m_disposed = true;
        }
    }

    CalculationContext::!CalculationContext()
    {
        if (m_pContext != nullptr) {
            delete m_pContext;
            m_pContext = nullptr;
        }
    }
}
