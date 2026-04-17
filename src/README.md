# CWrapper - UniLibEquityCore64 C++/CLI Wrapper

## Project Structure

```
CWrapper/
├── .workbuddy/               # Local config (NOT synced to git)
└── src/                      # Git-tracked source code
    ├── UniLibEquityCore64.dll              # Native C++ DLL
    ├── UniLibEquityCore64.lib             # Import library
    ├── ICalculationContext.h              # C++ header
    ├── UniLibEquityCoreCLI/               # C++/CLI wrapper
    │   ├── UniLibEquityCoreCLI.vcxproj   # VS project file
    │   ├── UniLibEquityCoreCLI.h         # Declarations
    │   ├── UniLibEquityCoreCLI.cpp       # Implementation
    │   ├── pch.h / pch.cpp
    ├── Test/                             # Local test project (NOT synced)
    ├── README.md
    └── .gitignore
```

---

## Implementation Status

| Class | Status | Notes |
|-------|--------|-------|
| `Functions` | **Fully working** | All 6 static methods callable from C# |
| `CalculationContext` | **Stub** | Returns error message; full impl needs a native C-wrapper DLL |

> **Why is CalculationContext a stub?** The native `ICalculationContext::Create()` returns a C++ object with STL containers (`std::map`, `std::vector`). C++/CLI managed types cannot hold native C++ objects with STL members. A full implementation requires building a **native C-wrapper DLL** that exposes `ICalculationContext` via opaque handles (create/destroy/method C functions).

---

## Setup on Windows (VS 2019 + C# 7.3)

### Step 1: Pull the latest code

```bash
git pull origin main
```

### Step 2: Build the C++/CLI wrapper

1. Open `src/UniLibEquityCoreCLI/UniLibEquityCoreCLI.vcxproj` in **Visual Studio 2019**
2. Switch to **x64** + **Release** configuration
3. The DLL will output to `src/bin/x64/Release/UniLibEquityCoreCLI.dll`
4. **Build** (Ctrl+B)

> **Note**: C++/CLI requires the "Desktop development with C++" workload in VS Installer.

### Step 3: Reference from your C# project

Add in your C# project's `.csproj`:

```xml
<ItemGroup>
  <Reference Include="UniLibEquityCoreCLI">
    <HintPath>..\src\UniLibEquityCoreCLI\bin\x64\Release\UniLibEquityCoreCLI.dll</HintPath>
  </Reference>
</ItemGroup>
```

Ensure `UniLibEquityCore64.dll` is in your C# project's **bin output directory**.

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
        // Functions - all 6 static methods are fully implemented
        // =========================================================

        // IsWorkingDay(calendar, date, useOracleDates)
        //   useOracleDates: 0 = Excel dates, 1 = Oracle dates
        int r1 = Functions.IsWorkingDay("SSE", 20250417, 0);
        Console.WriteLine($"SSE 20250417 (Thu): {r1}");  // 1=workday, 0=holiday

        int r2 = Functions.IsWorkingDay("SSE", 20250419, 0);
        Console.WriteLine($"SSE 20250419 (Sat): {r2}");  // 0=holiday

        // AddPeriod(calendar, startDate, period, dayAdjust, useOracleDates)
        //   period: "1D", "1W", "1M", "3M", "1Y"
        //   dayAdjust: 0=none, 1=next biz day, -1=prev biz day
        int r3 = Functions.AddPeriod("SSE", 20250417, "1M", 0, 0);
        Console.WriteLine($"20250417 + 1M = {r3}");  // 20250517

        int r4 = Functions.AddPeriod("SSE", 20250417, "3M", 0, 0);
        Console.WriteLine($"20250417 + 3M = {r4}");  // 20250717

        // GetMatchingBusinessday(calendar, startDate, useFollowing, useOracleDates)
        //   useFollowing: 1=next biz day, 0=prev biz day
        int r5 = Functions.GetMatchingBusinessday("SSE", 20250419, 1, 0);
        Console.WriteLine($"SSE 20250419 -> next biz day = {r5}");  // 20250421

        // YearFraction(source, start, end, useOracleDates)
        double yf = Functions.YearFraction(4, 20250101, 20251231, 0);
        Console.WriteLine($"Act/365 20250101->20251231: {yf:F6}");  // ~364/365

        double yf2 = Functions.YearFraction(0, 20250101, 20251231, 0);
        Console.WriteLine($"US 30/360 20250101->20251231: {yf2:F6}");

        // DayCount(source, start, end, useOracleDates)
        int dc = Functions.DayCount(4, 20250101, 20251231, 0);
        Console.WriteLine($"Act/365 day count = {dc}");  // 364 or 365

        // OpenDayCount(calendar, start, end, useOracleDates)
        int odc = Functions.OpenDayCount("SSE", 20250101, 20251231, 0);
        Console.WriteLine($"SSE working days 2025 = {odc}");


        // =========================================================
        // CalculationContext - STUB (returns error message)
        // =========================================================
        string error;
        var ctx = CalculationContext.Create(
            today: 20250417,
            instrumentSpots: null, fxSpots: null, yieldCurveData: null,
            repoData: null, volatilityExpiries: null, volatilityStrikes: null,
            volatilityValues: null, currencies: null, calendars: null,
            marketOffsets: null, yieldCurves: null,
            dividendExDates: null, dividendPayDates: null,
            dividendCurrencies: null, dividendGross: null,
            dividendTaxFactor: null, fxYieldCurves: null,
            error: out error);

        if (ctx == null)
            Console.WriteLine($"CalculationContext not available: {error}");
    }
}
```

---

## API Reference

### Functions (Static Methods) - Fully Working

All methods require an explicit `useOracleDates` parameter (0=Excel dates, 1=Oracle dates).

| C# Method | C++ Function | Description |
|-----------|--------------|-------------|
| `Functions.IsWorkingDay(calendar, date, useOracle)` | `uxIsWorkingDay` | Check if workday |
| `Functions.AddPeriod(calendar, date, period, adjust, useOracle)` | `uxAddPeriod` | Add/subtract period |
| `Functions.GetMatchingBusinessday(calendar, date, following, useOracle)` | `uxGetMatchingBusinessday` | Nearest biz day |
| `Functions.YearFraction(source, start, end, useOracle)` | `uxYearFraction` | Year fraction |
| `Functions.DayCount(source, start, end, useOracle)` | `uxDayCount` | Day count |
| `Functions.OpenDayCount(calendar, start, end, useOracle)` | `uxOpenDayCount` | Working day count |

### CalculationContext - Stub

| Method | Status |
|--------|--------|
| `Create(...)` | Returns nullptr with explanatory error |
| All instance methods | Return 0.0 / 0 with error |

---

## DayCount Source Parameter

| source | Basis |
|--------|-------|
| 0 | US (NASD) 30/360 |
| 1 | EU 30/360 |
| 2 | 30/360 |
| 3 | Actual/360 |
| 4 | Actual/365 (no leap) |
| 5 | Actual/Actual |

---

## FAQ

**Q: "unresolved external symbol"?**
> Make sure `UniLibEquityCore64.lib` is linked and `Additional Library Directories` points to `src/bin/x64/Release/`.

**Q: "DLL not found" at runtime?**
> Copy `UniLibEquityCore64.dll` to your C# project's bin output directory.

**Q: CalculationContext not working?**
> Expected - it's a stub. See implementation status above. Build a native C-wrapper DLL to enable it.
