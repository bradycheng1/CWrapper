# CWrapper - UniLibEquityCore64 C++/CLI 包装层

## 项目结构

```
CWrapper/
├── UniLibEquityCore64.dll              # 原生 C++ DLL（金融计算引擎）
├── UniLibEquityCore64.lib              # 导入库
├── ICalculationContext.h               # C++ 头文件
├── UniLibEquityCoreCLI/                # C++/CLI 包装层（核心）
│   ├── UniLibEquityCoreCLI.vcxproj     # VS 项目文件（双击打开）
│   ├── UniLibEquityCoreCLI.h           # 声明
│   ├── UniLibEquityCoreCLI.cpp         # 实现
│   ├── pch.h                           # 预编译头
│   └── pch.cpp
└── README.md
```

---

## 在 Windows 端（VS 2019 + C# 7.3）使用

### 第一步：拉取最新代码

```bash
git pull origin main
```

确保本地的 `UniLibEquityCore64.dll` 和 `ICalculationContext.h` 都已存在。

### 第二步：编译 C++/CLI 包装层

1. 用 **Visual Studio 2019** 打开 `UniLibEquityCoreCLI/UniLibEquityCoreCLI.vcxproj`
2. 配置 **x64 Release**（或 x64 Debug）
3. 把 `UniLibEquityCore64.dll` 复制到输出目录（`bin/x64/Release/`）
4. **编译**（Ctrl+B）
5. 编译成功后，输出文件为 `bin/x64/Release/UniLibEquityCoreCLI.dll`

> **注意**：C++/CLI 项目需要 **原生 C++ 开发工具**支持。首次编译时，VS 会提示缺少组件，请勾选"使用 C++ 的桌面开发"工作负载。

### 第三步：在 C# 项目中引用

1. 在你的 C# Solution 中，**右键 → Add → Reference**
2. 勾选 `UniLibEquityCoreCLI.dll`（在 `bin/x64/Release/` 下）
3. **或者**在 `.csproj` 中添加：

```xml
<ItemGroup>
  <Reference Include="UniLibEquityCoreCLI">
    <HintPath>..\UniLibEquityCoreCLI\bin\x64\Release\UniLibEquityCoreCLI.dll</HintPath>
  </Reference>
</ItemGroup>
```

4. 确保 `UniLibEquityCore64.dll` 在 **bin 输出目录**中（C# 程序运行时会加载它）

---

## C# 调用示例

```csharp
using System;
using System.Collections.Generic;
using UniLibEquityCLI;  // ← 引用后即可使用

class Program
{
    static void Main(string[] args)
    {
        // =========================================================
        // 示例一：纯函数调用（无需创建实例，直接用静态方法）
        // =========================================================

        // 判断 2025-04-17 是否为上交所（SSE）工作日
        int result1 = Functions.IsWorkingDay("SSE", 20250417);
        Console.WriteLine($"20250417 是否工作日: {result1}");  // 1=是, 0=否

        // 计算 2025-04-17 + 1个月 后的日期
        int result2 = Functions.AddPeriod("SSE", 20250417, "1M", 0);
        Console.WriteLine($"20250417 + 1个月 = {result2}");

        // 获取下一个工作日
        int result3 = Functions.GetMatchingBusinessday("SSE", 20250417, useFollowing: true);
        Console.WriteLine($"下一个工作日 = {result3}");

        // 计算年分数（Actual/365 基准）
        double yf = Functions.YearFraction(source: 4, start: 20250101, end: 20251231);
        Console.WriteLine($"年分数 = {yf:F6}");

        // 计算天数
        int dc = Functions.DayCount(source: 4, start: 20250101, end: 20251231);
        Console.WriteLine($"天数 = {dc}");

        // 计算工作日天数
        int odc = Functions.OpenDayCount("SSE", 20250101, 20251231);
        Console.WriteLine($"2025年工作日天数 = {odc}");


        // =========================================================
        // 示例二：ICalculationContext 实例（需要初始化 + Dispose）
        // =========================================================

        // 准备基础数据
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

        // 创建 Context（using 块自动 Dispose）
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
                Console.WriteLine($"Context 创建失败: {error}");
                return;
            }

            // 获取标的价格
            double spot = ctx.GetSpot("600519.SH", out error);
            if (!string.IsNullOrEmpty(error))
                Console.WriteLine($"GetSpot 错误: {error}");
            else
                Console.WriteLine($"600519.SH 现货价 = {spot}");

            // 获取远期复利因子
            double fwdFactor = ctx.GetForwardCompoundFactor(
                yieldCurve: "CNY_YIELD",
                date1: 20250417,
                date2: 20260417,
                overrate: 0.0,
                error: out error);
            if (!string.IsNullOrEmpty(error))
                Console.WriteLine($"GetForwardCompoundFactor 错误: {error}");
            else
                Console.WriteLine($"远期复利因子 = {fwdFactor}");
        }  // ← using 结束，Context 被正确释放
    }
}
```

---

## API 速查

### Functions 类（静态方法）

| C# 方法 | C++ 函数 | 说明 |
|---------|---------|------|
| `Functions.IsWorkingDay(calendar, date)` | `uxIsWorkingDay` | 是否工作日 |
| `Functions.AddPeriod(calendar, date, period, adjust)` | `uxAddPeriod` | 日期加减期间 |
| `Functions.GetMatchingBusinessday(calendar, date, following)` | `uxGetMatchingBusinessday` | 取最近工作日 |
| `Functions.YearFraction(source, start, end)` | `uxYearFraction` | 年分数 |
| `Functions.DayCount(source, start, end)` | `uxDayCount` | 天数 |
| `Functions.OpenDayCount(calendar, start, end)` | `uxOpenDayCount` | 工作日天数 |

### CalculationContext 类

| C# 方法 | 说明 |
|---------|------|
| `Create(...)` | 静态工厂方法，创建实例 |
| `GetSpot(instrument, out error)` | 获取标的价格 |
| `GetForwardCompoundFactor(yc, d1, d2, rate, out error)` | 远期复利因子 |
| `GetVolatility(inst, d1, d2, strike, type, isCall, out error)` | 波动率 |
| `GetRepoMargin(inst, d1, d2, out error)` | 回购保证金 |
| `GetForwardPrice(inst, date, out error)` | 远期价格 |
| `Dispose()` / `using` 块 | 释放 C++ 资源 |

---

## DayCount source 参数说明

| source | 规则名称 | 说明 |
|--------|---------|------|
| 0 | US | US (NASD) 30/360 |
| 1 | EU | EU 30/360 |
| 2 | 30/360 | 30/360 |
| 3 | Actual/360 | 实际天数/360 |
| 4 | Actual/365 | 实际天数/365（不闰年） |
| 5 | Actual/Actual | 实际天数/实际年数 |

## 常见问题

**Q: 编译报错 "unresolved external symbol"？**
> 确保 `UniLibEquityCore64.lib` 在链接器附加依赖项中，且 `Additional Library Directories` 指向正确路径。

**Q: 运行时报 "DLL not found"？**
> 把 `UniLibEquityCore64.dll` 复制到 C# 项目的 bin 输出目录。

**Q: C++/CLI 项目编译报错？**
> 确认 VS 安装时勾选了"使用 C++ 的桌面开发"工作负载，且平台工具集为 v142（VS 2019）。
