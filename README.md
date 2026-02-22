# Battery Health

A lightweight C++/Win32 diagnostic tool designed for Windows PE environments to retrieve low-level battery telemetry via WMI.

## Data Points Collected
| **Device Name** | The hardware model string from the battery controller. |
| **Design Cap** | The factory-original capacity in mWh. |
| **Full Charge** | The current maximum capacity the battery can hold. |
| **Current Cap** | Real-time remaining energy level. |
| **Health** | Calculated health percentage and wear level (e.g., 29% wear -> 71% health). |
| **Voltage** | Current battery voltage (formatted to 3 decimal places). |
| **Discharge** | Current power draw or charge rate in mW. |
| **Cycle Count** | Total lifetime charge cycles. |


## Build

1. Compile resources: `rc.exe app.rc`
2. Build binary: `cl.exe /EHsc /MT /O2 /DUNICODE /D_UNICODE main.cpp app.res /link wbemuuid.lib ole32.lib oleaut32.lib user32.lib gdi32.lib /SUBSYSTEM:WINDOWS`