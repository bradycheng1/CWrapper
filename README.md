# CWrapper - UniLibEquityCore64 C++/CLI Wrapper

## Project Structure

```
CWrapper/
├── UniLibEquityCore64.dll              # Native C++ DLL (financial calculation engine)
├── UniLibEquityCore64.lib              # Import library
├── ICalculationContext.h              # C++ header
├── UniLibEquityCoreCLI/                # C++/CLI wrapper
│   ├── UniLibEquityCoreCLI.vcxproj    # VS project file (double-click to open)
│   ├── UniLibEquityCoreCLI.h          # Declarations
│   ├── UniLibEquityCoreCLI.cpp        # Implementation
│   ├── pch.h                          # Precompiled header
│   └── pch.cpp
└── README.md
```

---

## Setup on Windows (VS 2019 + C# 7.3)

### Step 1: Pull the latest code

```bash
git pull origin main
```

Make sure `UniLibEquityCore64.dll` and `ICalculationContext.h` are present.

### Step 2: Build the C++/CLI wrapper

1. Open `UniLibEquityCoreCLI/UniLibEquityCoreCLI.vcxproj` in **Visual Studio 2019**
2. Switch to **x64** + **Release** configuration
3. Copy `UniLibEquityCore64.dll` to the output directory (`bin/x64/Release/`)
4. **Build** (Ctrl+B)
5. Output: `bin/x64/Release/UniLibEquityCoreCLI.dll`

> **Note**: C++/CLI requires the "Desktop development with C++" workload in VS Installer.

### Step 3: Reference from your C# project

1. In your C# Solution: **Right-click → Add → Reference**
2. Browse to `UniLibEquityCoreCLI.dll` in `bin/x64/Release/`

Or add directly in `.csproj`:

```xml
<ItemGroup>
  <Reference Include="UniLibEquityCoreCLI">
    <HintPath>..\UniLibEquityCoreCLI\bin\x64\Release\UniLibEquityCoreCLI.dll</HintPath>
  </Reference>
</ItemGroup>
```

3. Make sure `UniLibEquityCore64.dll` is in your C# project's **bin output directory**.

---

## C# Usage Examples

```csharp
using System;
using System.Collections.Generic;
using UniLibEquityCLI;

class Program
{
    static void Main(string[] args)
    {
        // =========================================================
        // Example 1: Static functions (no instance needed)
        // =========================================================

        // Is 2025-04-17 a working day on SSE?
        int result1 = Functions.IsWorkingDay("SSE", 20250417);
        Console.WriteLine($"20250417 is workday: {result1}");  // 1=yes, 0=no

        // 2025-04-17 + 1 month
        int result2 = Functions.AddPeriod("SSE", 20250417, "1M", 0);
        Console.WriteLine($"20250417 + 1M = {result2}");

        // Next business day
        int result3 = Functions.GetMatchingBusinessday("SSE", 20250417, useFollowing: true);
        Console.WriteLine($"Next biz day = {result3}");

        // Year fraction (Actual/365)
        double yf = Functions.YearFraction(source: 4, start: 20250101, end: 20251231);
        Console.WriteLine($"Year fraction = {yf:F6}");

        // Day count
        int dc = Functions.DayCount(source: 4, start: 20250101, end: 20251231);
        Console.WriteLine($"Day count = {dc}");

        // Working days in year
        int odc = Functions.OpenDayCount("SSE", 20250101, 20251231);
        Console.WriteLine($"SSE working days 2025 = {odc}");


        // =========================================================
        // Example 2: CalculationContext (requires Init + Dispose)
        // =========================================================

        long today = 20250417;
        string error;

        var instrumentSpots = new Dictionary<string, double>
        {
            ["600519.SH"] = 1850.0,
            ["000001.SZ"] = 12.5
        };

        var calendars = new Dictionary<string, string>
        {
            ["600519.SH"] = "SSE",
            ["000001.SZ"] = "SZSE"
        };

        var currencies = new Dictionary<string, string>
        {
            ["600519.SH"] = "CNY",
            ["000001.SZ"] = "CNY"
        };

        // Create Context (using block handles Dispose)
        using (var ctx = CalculationContext.Create(
            today: today,
            instrumentSpots: instrumentSpots,
            fxSpots: null,
            yieldCurveData: null,
            repoData: null,
            volatilityExpiries: null,
            volatilityStrikes: null,
            volatilityValues: null,
            currencies: currencies,
            calendars: calendars,
            marketOffsets: null,
            yieldCurves: null,
            dividendExDates: null,
            dividendPayDates: null,
            dividendCurrencies: null,
            dividendGross: null,
            dividendTaxFactor: null,
            fxYieldCurves: null,
            error: out error))
        {
            if (ctx == null)
            {
                Console.WriteLine($"Context creation failed: {error}");
                return;
            }

            // Get spot price
            double spot = ctx.GetSpot("600519.SH", out error);
            if (!string.IsNullOrEmpty(error))
                Console.WriteLine($"GetSpot error: {error}");
            else
                Console.WriteLine($"600519.SH spot = {spot}");

            // Get forward compound factor
            double fwdFactor = ctx.GetForwardCompoundFactor(
                yieldCurve: "CNY_YIELD",
                date1: 20250417,
                date2: 20260417,
                overrate: 0.0,
                error: out error);
            if (!string.IsNullOrEmpty(error))
                Console.WriteLine($"GetForwardCompoundFactor error: {error}");
            else
                Console.WriteLine($"Forward compound factor = {fwdFactor}");
        }
    }
}
```

---

## API Quick Reference

### Functions (Static Methods)

| C# Method | C++ Function | Description |
|-----------|--------------|-------------|
| `Functions.IsWorkingDay(calendar, date)` | `uxIsWorkingDay` | Check if workday |
| `Functions.AddPeriod(calendar, date, period, adjust)` | `uxAddPeriod` | Add/subtract period |
| `Functions.GetMatchingBusinessday(calendar, date, following)` | `uxGetMatchingBusinessday` | Nearest biz day |
| `Functions.YearFraction(source, start, end)` | `uxYearFraction` | Year fraction |
| `Functions.DayCount(source, start, end)` | `uxDayCount` | Day count |
| `Functions.OpenDayCount(calendar, start, end)` | `uxOpenDayCount` | Working day count |

### CalculationContext

| C# Method | Description |
|-----------|-------------|
| `Create(...)` | Static factory |
| `GetSpot(instrument, out error)` | Get spot price |
| `GetForwardCompoundFactor(yc, d1, d2, rate, out error)` | Forward compound factor |
| `GetVolatility(inst, d1, d2, strike, type, isCall, out error)` | Volatility |
| `GetRepoMargin(inst, d1, d2, out error)` | Repo margin |
| `GetForwardPrice(inst, date, out error)` | Forward price |
| `Dispose()` / `using` block | Release C++ resources |

---

## DayCount source Parameter

| source | Basis | Description |
|--------|-------|-------------|
| 0 | US | US (NASD) 30/360 |
| 1 | EU | EU 30/360 |
| 2 | 30/360 | 30/360 |
| 3 | Actual/360 | Actual days / 360 |
| 4 | Actual/365 | Actual days / 365 (no leap) |
| 5 | Actual/Actual | Actual days / Actual years |

## FAQ

**Q: "unresolved external symbol" at link time?**
> Make sure `UniLibEquityCore64.lib` is in Linker → Additional Dependencies and `Additional Library Directories` points to the right path.

**Q: "DLL not found" at runtime?**
> Copy `UniLibEquityCore64.dll` to your C# project's bin output directory.

**Q: C++/CLI build errors?**
> Confirm VS Installer has "Desktop development with C++" workload, and Platform Toolset is v142 (VS 2019).
