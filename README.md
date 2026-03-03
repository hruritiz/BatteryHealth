# Battery Health<br>

A lightweight C++/Win32 diagnostic tool designed for Windows PE environments to retrieve low-level battery telemetry via WMI.<br>

## Data Points Collected<br>
| **Device Name** | The hardware model string from the battery controller. |<br>
| **Design Cap** | The factory-original capacity in mWh. |<br>
| **Full Charge** | The current maximum capacity the battery can hold. |<br>
| **Current Cap** | Real-time remaining energy level. |<br>
| **Health** | Calculated health percentage and wear level (e.g., 29% wear -> 71% health). |<br>
| **Voltage** | Current battery voltage (formatted to 3 decimal places). |<br>
| **Discharge** | Current power draw or charge rate in mW. |<br>
| **Cycle Count** | Total lifetime charge cycles. |<br>

## Build
1. Compile resources: `rc.exe app.rc`
2. Build binary: `cl.exe /EHsc /MT /O2 /DUNICODE /D_UNICODE battery.cpp app.res /link wbemuuid.lib ole32.lib oleaut32.lib user32.lib gdi32.lib /SUBSYSTEM:WINDOWS`<br>



