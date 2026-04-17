#pragma once

#include <map>
#include <vector>
#include <string>

#ifdef CALCULATION_CONTEXT_EXPORTS
#	define CALCULATION_CONTEXT_INTERFACE __declspec(dllexport)
#else
#	define CALCULATION_CONTEXT_INTERFACE __declspec(dllimport)
#endif

namespace UniLibEquity
{
	namespace Interface
	{
		class CALCULATION_CONTEXT_INTERFACE ICalculationContext
		{
		public:
			static const ICalculationContext* Create(
				long today,
				const std::map<std::string, double>& instrument_spots,
				const std::map<std::string, std::tuple<std::string, std::string, double>>& fx_spots,
				const std::map<std::string, std::map<long, double>>& yield_curve_data,
				const std::map<std::string, std::map<long, double>>& repo_data,
				const std::map<std::string, std::vector<long>>& instrument_volatility_expiries,
				const std::map<std::string, std::vector<double>>& instrument_volatility_strikes,
				const std::map<std::string, std::vector<double>>& instrument_volatility_values,
				const std::map<std::string, std::string>& instrument_currencies,
				const std::map<std::string, std::string>& instrument_calendars,
				const std::map<std::string, std::string>& instrument_market_offsets,
				const std::map<std::string, std::string>& instrument_yield_curve,
				const std::map<std::string, std::vector<long>>& instrument_dividend_exdates,
				const std::map<std::string, std::vector<long>>& instrument_dividend_paydates, 
				const std::map<std::string, std::vector<std::string>>& instrument_dividend_currencies,
				const std::map<std::string, std::vector<double>>& instrument_dividend_gross,
				const std::map<std::string, std::vector<double>>& instrument_dividend_tax_factor,
				const std::map<std::string, std::tuple<std::string, std::string>>& fx_yield_curves, 
				std::string& error);

			virtual double GetSpot(
				const std::string& instrument,
				std::string& error) const = 0;

			virtual double GetForwardCompoundFactor(
				const std::string& yield_curve, 
				long date1, 
				long date2, 
				double overrate,
				std::string& error) const = 0;

			virtual double GetVolatility(
				const std::string& instrument,
				long startDate,
				long endDate,
				double strike,
				int	volatType,
				bool isCall,
				std::string& error) const = 0;

			virtual	double GetRepoMargin(
				const std::string& instrument,
				long date1, 
				long date2,
				std::string& error) const = 0;

			virtual double GetForwardPrice(
				const std::string& instrument,
				long date,
				std::string& error) const = 0;

			virtual ICalculationContext* GetShiftedCalculationContext(
				const std::string& shift,
				std::string& error) = 0; //NOT IMPLEMENTED

			virtual ~ICalculationContext() = 0;
		};

		namespace Functions
		{
			int CALCULATION_CONTEXT_INTERFACE uxIsWorkingDay(const std::string& calendars,
				int startDate,
				bool useOracleInsteadOfExcelDates = false);

			int CALCULATION_CONTEXT_INTERFACE uxAddPeriod(const std::string& calendars,
				int startDate,
				const std::string& period,
				int dayAdjust,
				bool useOracleInsteadOfExcelDates = false);

			int CALCULATION_CONTEXT_INTERFACE uxGetMatchingBusinessday(const std::string& calendars,
				int startDate,
				bool useFollowing,
				bool useOracleInsteadOfExcelDates = false);

			double CALCULATION_CONTEXT_INTERFACE  uxYearFraction(int source, 
				int start, 
				int end, 
				bool useOracleInsteadOfExcelDates = false);

			int CALCULATION_CONTEXT_INTERFACE  uxDayCount(int source,
				int start,
				int end,
				bool useOracleInsteadOfExcelDates = false);

			int CALCULATION_CONTEXT_INTERFACE  uxOpenDayCount(const std::string& calendar,
				int start,
				int end,
				bool useOracleInsteadOfExcelDates = false);
		}
	}
}