#include <windows.h>
#include <string>
#include <comdef.h>
#include <WbemIdl.h>
#include <sstream>
#include <iomanip>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
HFONT hFont;
std::wstring reportText;
std::wstring GetBatteryInfo() {
 std::wstringstream ss;
 HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
 hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
 IWbemLocator *pLoc = NULL;
 CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
 IWbemServices *pSvcCIM = NULL, *pSvcWMI = NULL;
 if (pLoc) {
 pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvcCIM);
 pLoc->ConnectServer(_bstr_t(L"ROOT\\WMI"), NULL, NULL, 0, NULL, 0, 0, &pSvcWMI);
 }
 if (pSvcWMI) {
 IEnumWbemClassObject* pEnum = NULL;
 IWbemClassObject *pObj = NULL;
 ULONG uRet = 0;
 pSvcWMI->ExecQuery(bstr_t("WQL"), bstr_t("SELECT DeviceName FROM BatteryStaticData"), 0, NULL, &pEnum);
 if (pEnum) {
 hr = pEnum->Next(WBEM_INFINITE, 1, &pObj, &uRet);
 if (uRet == 0) {
 pEnum->Release();
 if (pSvcCIM) pSvcCIM->Release();
 pSvcWMI->Release();
 pLoc->Release();
 CoUninitialize();
 return L"NOT_FOUND";
 }
 pObj->Release();
 pEnum->Release();
 }
 unsigned int design = 0, full = 0, current = 0;
 pSvcWMI->ExecQuery(bstr_t("WQL"), bstr_t("SELECT DeviceName, DesignedCapacity FROM BatteryStaticData"), 0, NULL, &pEnum);
 if (pEnum && SUCCEEDED(pEnum->Next(WBEM_INFINITE, 1, &pObj, &uRet)) && uRet > 0) {
 VARIANT v1, v3;
 pObj->Get(L"DeviceName", 0, &v1, 0, 0);
 ss << L"Device Name: " << (v1.bstrVal ? v1.bstrVal : L"Unknown") << L"\n";
 pObj->Get(L"DesignedCapacity", 0, &v3, 0, 0);
 design = v3.uintVal;
 ss << L"Design Cap: " << design << L" mWh\n";
 pObj->Release();
 VariantClear(&v1);
 VariantClear(&v3);
 }
 if (pEnum) pEnum->Release();
 pSvcWMI->ExecQuery(bstr_t("WQL"), bstr_t("SELECT FullChargedCapacity FROM BatteryFullChargedCapacity"), 0, NULL, &pEnum);
 if (pEnum && SUCCEEDED(pEnum->Next(WBEM_INFINITE, 1, &pObj, &uRet)) && uRet > 0) {
 VARIANT v;
 pObj->Get(L"FullChargedCapacity", 0, &v, 0, 0);
 full = v.uintVal;
 ss << L"Full Charge: " << full << L" mWh\n";
 pObj->Release();
 VariantClear(&v);
 }
 if (pEnum) pEnum->Release();
 pSvcWMI->ExecQuery(bstr_t("WQL"), bstr_t("SELECT RemainingCapacity, Voltage, DischargeRate FROM BatteryStatus"), 0, NULL, &pEnum);
 if (pEnum && SUCCEEDED(pEnum->Next(WBEM_INFINITE, 1, &pObj, &uRet)) && uRet > 0) {
 VARIANT v1, v2, v3;
 pObj->Get(L"RemainingCapacity", 0, &v1, 0, 0);
 current = v1.uintVal;
 ss << L"Current Cap: " << current << L" mWh\n";
 pObj->Get(L"Voltage", 0, &v2, 0, 0);
 ss << L"Voltage: " << std::fixed << std::setprecision(3) << (v2.uintVal / 1000.0) << L" V\n";
 pObj->Get(L"DischargeRate", 0, &v3, 0, 0);
 ss << L"Discharge: " << v3.intVal << L" mW\n";
 pObj->Release();
 VariantClear(&v1);
 VariantClear(&v2);
 VariantClear(&v3);
 }
 if (pEnum) pEnum->Release();
 if (design > 0 && full > 0) {
 double h = (full / (double)design) * 100.0;
 int wear = (int)(100.0 - h + 0.5);
 ss << L"Health: 100% (" << wear << L"% wear) -> " << (int)(h + 0.5) << L"%\n";
 }
 pSvcWMI->ExecQuery(bstr_t("WQL"), bstr_t("SELECT CycleCount FROM BatteryCycleCount"), 0, NULL, &pEnum);
 if (pEnum && SUCCEEDED(pEnum->Next(WBEM_INFINITE, 1, &pObj, &uRet)) && uRet > 0) {
 VARIANT v;
 pObj->Get(L"CycleCount", 0, &v, 0, 0);
 ss << L"Cycle Count: " << v.uintVal << L"\n";
 pObj->Release();
 VariantClear(&v);
 }
 if (pEnum) pEnum->Release();
 }
 if (pSvcCIM) pSvcCIM->Release();
 if (pSvcWMI) pSvcWMI->Release();
 if (pLoc) pLoc->Release();
 CoUninitialize();
 return ss.str();
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
 switch (uMsg) {
 case WM_CREATE:
 hFont = CreateFont(19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, L"Consolas");
 break;
 case WM_PAINT: {
 PAINTSTRUCT ps;
 HDC hdc = BeginPaint(hwnd, &ps);
 RECT rect;
 GetClientRect(hwnd, &rect);
 FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
 SelectObject(hdc, hFont);
 SetBkMode(hdc, TRANSPARENT);
 std::wstring header = L"Battery Health Report";
 TextOut(hdc, 20, 20, header.c_str(), (int)header.length());
 int y = 60;
 std::wstringstream fss(reportText);
 std::wstring line;
 while (std::getline(fss, line)) {
 TextOut(hdc, 20, y, line.c_str(), (int)line.length());
 y += 25;
 }
 EndPaint(hwnd, &ps);
 break;
 }
 case WM_DESTROY:
 DeleteObject(hFont);
 PostQuitMessage(0);
 return 0;
 }
 return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
 reportText = GetBatteryInfo();
 if (reportText == L"NOT_FOUND" || reportText.empty()) {
 MessageBox(NULL, L"Battery not found.", L"Battery Error", MB_OK | MB_ICONERROR);
 return 0;
 }
 const wchar_t CLASS_NAME[] = L"BatteryHealthClass";
 WNDCLASS wc = {
}
;
 wc.lpfnWndProc = WindowProc;
 wc.hInstance = hInstance;
 wc.lpszClassName = CLASS_NAME;
 wc.hCursor = LoadCursor(NULL, IDC_ARROW);
 wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
 wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
 RegisterClass(&wc);
 HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Battery Health", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
 CW_USEDEFAULT, CW_USEDEFAULT, 420, 350, NULL, NULL, hInstance, NULL);
 if (!hwnd) return 0;
 SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)wc.hIcon);
 SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)wc.hIcon);
 ShowWindow(hwnd, nCmdShow);
 MSG msg = {
}
;
 while (GetMessage(&msg, NULL, 0, 0)) {
 TranslateMessage(&msg);
 DispatchMessage(&msg);
 }
 return 0;
}
