// Projekt.cpp : Definiuje punkt wejścia dla aplikacji.
/*
* 18.11.2025
* na nastepny raz
* Coś wektorowo w asmie
* 
* Termin prezentacji 20 stycznia!!!
*/
#include "framework.h"
#include "Projekt.h"
#include <commdlg.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <thread>
#include <iomanip>
#include <codecvt>
#include <locale>
#include <chrono>
#include <functional>
#define MAX_LOADSTRING 100
double ZmierzCzas(std::function<void()> func)
{
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    func(); 

    QueryPerformanceCounter(&end);
    return static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart; 
}


void PokazMacierz(HWND hWndParent, const std::vector<double>& M, int rows, int cols, double czas, int nThreads)
{
    if (M.empty()) {
        MessageBoxW(hWndParent, L"Macierz jest pusta!", L"Błąd", MB_OK | MB_ICONERROR);
        return;
    }

    HWND hWndEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL |
        ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 700,
        hWndParent, nullptr, GetModuleHandleW(nullptr), nullptr
    );

    if (!hWndEdit) {
        MessageBoxW(hWndParent, L"Nie mogę utworzyć okna wyników!", L"Błąd", MB_OK | MB_ICONERROR);
        return;
    }

    std::wstringstream ss;
    ss << L"Wynik mnożenia macierzy (" << rows << L"x" << cols << L")\r\n";
    ss << L"Liczba wątków: " << nThreads << L"\r\n";
    ss << L"Czas obliczeń: " << std::fixed << std::setprecision(2) << czas << L" ms\r\n";
    ss << L"----------------------------------------------\r\n";

    // Wypisanie macierzy
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j)
            ss << std::setw(10) << std::setprecision(4) << M[i * cols + j] << L" ";
        ss << L"\r\n";
    }

    std::wstring text = ss.str();
    SetWindowTextW(hWndEdit, text.c_str());
}
void ZapiszMacierzDoCSV(const std::wstring& filename,
    const std::vector<double>& M, int rows, int cols,
    double czas, int nThreads, const std::wstring& tryb)
{
    std::wofstream file(filename);
    if (!file.is_open()) return;

    // Nagłówki
    file << L"Wynik mnozenia macierzy;" << rows << L"x" << cols << L"\n";
    file << L"Tryb;" << tryb << L"\n";
    file << L"Liczba watkow;" << nThreads << L"\n";
    file << L"Czas [ms];" << std::fixed << std::setprecision(2) << czas << L"\n";
    file << L"\n";  // pusty wiersz

    // Właściwa macierz
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            file << std::setprecision(4) << M[i * cols + j];
            if (j + 1 < cols) file << L";";  // separator CSV
        }
        file << L"\n";
    }
}
std::vector<double> WczytajMacierz(const std::wstring& filename, int& rows, int& cols)
{
    std::wifstream file(filename);
    std::wstring line;
    std::vector<double> data;
    rows = 0;
    cols = 0;

    while (std::getline(file, line))
    {
        std::wstringstream ss(line);
        double val;
        int colCount = 0;

        while (ss >> val)
        {
            data.push_back(val);
            ++colCount;
        }

        if (colCount > 0)
        {
            if (cols == 0)
                cols = colCount;
            ++rows;
        }
    }
    return data;
}
std::wstring WybierzPlik(HWND hWnd)
{
    OPENFILENAMEW ofn;
    wchar_t szFile[MAX_PATH] = L"";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"Pliki tekstowe (*.txt)\0*.txt\0Wszystkie pliki (*.*)\0*.*\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = L"txt";

    if (GetOpenFileNameW(&ofn))
        return std::wstring(szFile);
    else
        return L"";
}


// Zmienne globalne:
HINSTANCE hInst;                                // bieżące wystąpienie
WCHAR szTitle[MAX_LOADSTRING];                  // Tekst paska tytułu
WCHAR szWindowClass[MAX_LOADSTRING];            // nazwa klasy okna głównego

// Przekaż dalej deklaracje funkcji dołączone w tym module kodu:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: W tym miejscu umieść kod.
    
    // Inicjuj ciągi globalne
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJEKT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    
    // Wykonaj inicjowanie aplikacji:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJEKT));

    MSG msg;

    // Główna pętla komunikatów:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNKCJA: MyRegisterClass()
//
//  PRZEZNACZENIE: Rejestruje klasę okna.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJEKT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROJEKT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNKCJA: InitInstance(HINSTANCE, int)
//
//   PRZEZNACZENIE: Zapisuje dojście wystąpienia i tworzy okno główne
//
//   KOMENTARZE:
//
//        W tej funkcji dojście wystąpienia jest zapisywane w zmiennej globalnej i
//        jest tworzone i wyświetlane okno główne programu.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Przechowuj dojście wystąpienia w naszej zmiennej globalnej

   HWND hWnd = CreateWindowW(szWindowClass, L"Mnożenie macierzy - JA PROJEKT", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 600, 350, nullptr, nullptr, hInstance, nullptr); // te dwie liczby to od zmiany wielkosci okna przy uruchamianiu

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNKCJA: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PRZEZNACZENIE: Przetwarza komunikaty dla okna głównego.
//
//  WM_COMMAND  - przetwarzaj menu aplikacji
//  WM_PAINT    - Maluj okno główne
//  WM_DESTROY  - opublikuj komunikat o wyjściu i wróć
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateWindowW(L"BUTTON", L"Wczytaj macierz A", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            20, 20, 180, 30, hWnd, (HMENU)3001, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Wczytaj macierz B", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            220, 20, 180, 30, hWnd, (HMENU)3002, NULL, NULL);

        // --- ROZMIAR MACIERZY A ---
        CreateWindowW(L"STATIC", L"Rozmiar macierzy A (wiersze x kolumny):", WS_VISIBLE | WS_CHILD,
            20, 70, 350, 20, hWnd, NULL, NULL, NULL);

        CreateWindowW(L"STATIC", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
            320, 70, 40, 20, hWnd, (HMENU)1001, NULL, NULL);

        CreateWindowW(L"STATIC", L"x", WS_VISIBLE | WS_CHILD | SS_CENTER,
            365, 70, 10, 20, hWnd, NULL, NULL, NULL);

        CreateWindowW(L"STATIC", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
            380, 70, 40, 20, hWnd, (HMENU)1002, NULL, NULL);

        // --- ROZMIAR MACIERZY B ---
        CreateWindowW(L"STATIC", L"Rozmiar macierzy B (wiersze x kolumny):", WS_VISIBLE | WS_CHILD,
            20, 100, 350, 20, hWnd, NULL, NULL, NULL);

        CreateWindowW(L"STATIC", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
            320, 100, 40, 20, hWnd, (HMENU)1003, NULL, NULL);

        CreateWindowW(L"STATIC", L"x", WS_VISIBLE | WS_CHILD | SS_CENTER,
            365, 100, 10, 20, hWnd, NULL, NULL, NULL);

        CreateWindowW(L"STATIC", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
            380, 100, 40, 20, hWnd, (HMENU)1004, NULL, NULL);

        // --- WYBÓR TRYBU ---
        CreateWindowW(L"STATIC", L"Tryb:", WS_VISIBLE | WS_CHILD,
            20, 140, 50, 20, hWnd, NULL, NULL, NULL);

        CreateWindowW(L"BUTTON", L"C++", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
            80, 140, 80, 20, hWnd, (HMENU)2001, NULL, NULL);

        CreateWindowW(L"BUTTON", L"ASM", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            160, 140, 80, 20, hWnd, (HMENU)2002, NULL, NULL);

        // --- LICZBA WĄTKÓW ---
        CreateWindowW(L"STATIC", L"Liczba wątków:", WS_VISIBLE | WS_CHILD,
            20, 170, 120, 20, hWnd, NULL, NULL, NULL);

        int maxWatki = std::thread::hardware_concurrency();
        std::wstring maxWatkiString = std::to_wstring(maxWatki);
        CreateWindowW(L"EDIT", maxWatkiString.c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
            150, 170, 40, 20, hWnd, (HMENU)4001, NULL, NULL);

        // --- PRZYCISK OBLICZ ---
        CreateWindowW(L"BUTTON", L"Oblicz", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 210, 180, 35, hWnd, (HMENU)3003, NULL, NULL);

        SendMessageW(GetDlgItem(hWnd, 2001), BM_SETCHECK, BST_CHECKED, 0);
        

        return 0;
    }
    case WM_COMMAND:
        {
            static std::vector<double> macierzA, macierzB;
            static int rowsA = 0, colsA = 0, rowsB = 0, colsB = 0;
            static std::wstring plikA, plikB;
            static wchar_t watki[32];
            GetWindowTextW(GetDlgItem(hWnd, 4001), watki, 32);
            int PodanaLiczbaW = _wtoi(watki);
            int wmId = LOWORD(wParam);
            // Analizuj zaznaczenia menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case 3001: // Wczytaj macierz A
            {
                plikA = WybierzPlik(hWnd);
                if (!plikA.empty())
                {
                    macierzA = WczytajMacierz(plikA, rowsA, colsA);
                    wchar_t msg[256];
                    wsprintfW(msg, L"Wczytano macierz A: %dx%d", rowsA, colsA);
                    MessageBoxW(hWnd, msg, L"Macierz A", MB_OK);
                    wchar_t buf[16];
                    wsprintfW(buf, L"%d", rowsA);
                    SetWindowTextW(GetDlgItem(hWnd, 1001), buf);

                    wsprintfW(buf, L"%d", colsA);
                    SetWindowTextW(GetDlgItem(hWnd, 1002), buf);
                }
            }
            break;

            case 3002: // Wczytaj macierz B
            {
                plikB = WybierzPlik(hWnd);
                if (!plikB.empty())
                {
                    macierzB = WczytajMacierz(plikB, rowsB, colsB);
                    wchar_t msg[256];
                    wsprintfW(msg, L"Wczytano macierz B: %dx%d", rowsB, colsB);
                    MessageBoxW(hWnd, msg, L"Macierz B", MB_OK);
                    wchar_t buf[16];
                    wsprintfW(buf, L"%d", rowsB);
                    SetWindowTextW(GetDlgItem(hWnd, 1003), buf);

                    wsprintfW(buf, L"%d", colsB);
                    SetWindowTextW(GetDlgItem(hWnd, 1004), buf);
                }
            }
            break;
            case 3003: // Oblicz
            {
                if (macierzA.empty() || macierzB.empty()) {
                    MessageBoxW(hWnd, L"Najpierw wczytaj obie macierze!", L"Błąd", MB_OK | MB_ICONERROR);
                    break;
                }
                if (colsA != rowsB) {
                    MessageBoxW(hWnd, L"Nie można mnożyć: liczba kolumn A musi być równa liczbie wierszy B!", L"Błąd", MB_OK | MB_ICONERROR);
                    break;
                }
                if (macierzA.size() != (size_t)(rowsA * colsA) ||
                    macierzB.size() != (size_t)(rowsB * colsB)) {
                    MessageBoxW(hWnd, L"Plik z macierzą ma nierówne wiersze / zły format.", L"Błąd danych", MB_OK | MB_ICONERROR);
                    break;
                }
                if (PodanaLiczbaW > 64) {
                    MessageBoxW(hWnd, L"Podano za dużą liczbe wątków! Max = 64", L"Błąd", MB_OK | MB_ICONERROR);
                    break;
                }

                std::vector<double> macierzC(rowsA * colsB, 0.0);

                // ---- PADDING ----
                int colsB_pad = (colsB + 3) & ~3;
                std::vector<double> macierzB_pad(rowsB * colsB_pad, 0.0);
                for (int i = 0; i < rowsB; ++i)
                    memcpy(&macierzB_pad[i * colsB_pad],
                        &macierzB[i * colsB],
                        colsB * sizeof(double));

                std::vector<double> macierzC_pad(rowsA * colsB_pad, 0.0);
                // ------------------

                const BOOL cppChecked = (SendMessageW(GetDlgItem(hWnd, 2001), BM_GETCHECK, 0, 0) == BST_CHECKED);
                const BOOL asmChecked = (SendMessageW(GetDlgItem(hWnd, 2002), BM_GETCHECK, 0, 0) == BST_CHECKED);

                if (cppChecked) {
                    HINSTANCE dllCpp = LoadLibraryW(L"Dll_C++.dll");
                    if (!dllCpp) {
                        MessageBoxW(hWnd, L"Nie mogę załadować Dll_C++.dll!", L"Błąd", MB_OK | MB_ICONERROR);
                        break;
                    }

                    typedef void(__cdecl* MultiplyCppPartialFunc)(const double*, const double*, double*, int, int, int, int, int);
                    MultiplyCppPartialFunc multiplyCppPartial =
                        (MultiplyCppPartialFunc)GetProcAddress(dllCpp, "MultiplyCppPartial");

                    if (!multiplyCppPartial) {
                        MessageBoxW(hWnd, L"Nie mogę znaleźć funkcji MultiplyCppPartial w Dll_C++.dll!", L"Błąd", MB_OK | MB_ICONERROR);
                        FreeLibrary(dllCpp);
                        break;
                    }

                    wchar_t bufThreads[16];
                    GetWindowTextW(GetDlgItem(hWnd, 4001), bufThreads, 16);
                    int nThreads = _wtoi(bufThreads);
                    if (nThreads <= 0) {
                        nThreads = std::thread::hardware_concurrency();
                        if (nThreads == 0) nThreads = 1;
                    }

                    int chunk = rowsA / nThreads;
                    int remainder = rowsA % nThreads;

                    double czasCPP = ZmierzCzas([&]() {
                        std::vector<std::thread> threads;
                        int current = 0;

                        for (int i = 0; i < nThreads; ++i) {
                            int start = current;
                            int end = start + chunk + (i < remainder ? 1 : 0);

                            threads.emplace_back([&, start, end]() {
                                multiplyCppPartial(
                                    macierzA.data(),
                                    macierzB_pad.data(),
                                    macierzC_pad.data(),
                                    rowsA, colsA, colsB_pad,
                                    start, end);
                                });

                            current = end;
                        }

                        for (auto& t : threads)
                            t.join();
                        });

                    // kopiowanie bez paddingu
                    for (int i = 0; i < rowsA; ++i)
                        memcpy(&macierzC[i * colsB],
                            &macierzC_pad[i * colsB_pad],
                            colsB * sizeof(double));

                    PokazMacierz(hWnd, macierzC, rowsA, colsB, czasCPP, nThreads);
                    ZapiszMacierzDoCSV(L"wynikCPP.csv", macierzC, rowsA, colsB, czasCPP, nThreads, L"C++");
                    FreeLibrary(dllCpp);
                }
                else if (asmChecked) {

                    HINSTANCE dllAsm = LoadLibraryW(L"JAAsm.dll");
                    if (!dllAsm) {
                        MessageBoxW(hWnd, L"Nie mogę załadować JAAsm.dll!", L"Błąd", MB_OK | MB_ICONERROR);
                        break;
                    }

                    typedef void(__cdecl* MultiplyAsmPartialFunc)(const double*, const double*, double*, int, int, int, int, int);
                    MultiplyAsmPartialFunc multiplyAsmPartial =
                        (MultiplyAsmPartialFunc)GetProcAddress(dllAsm, "MultiplyAsmPartial");

                    if (!multiplyAsmPartial) {
                        MessageBoxW(hWnd, L"Nie mogę znaleźć funkcji MultiplyAsmPartial!", L"Błąd", MB_OK | MB_ICONERROR);
                        FreeLibrary(dllAsm);
                        break;
                    }

                    wchar_t bufThreads[16];
                    GetWindowTextW(GetDlgItem(hWnd, 4001), bufThreads, 16);
                    int nThreads = _wtoi(bufThreads);
                    if (nThreads <= 0) {
                        nThreads = std::thread::hardware_concurrency();
                        if (nThreads == 0) nThreads = 1;
                    }

                    int chunk = rowsA / nThreads;
                    int remainder = rowsA % nThreads;

                    double czasASM = ZmierzCzas([&]() {
                        std::vector<std::thread> threads;
                        int current = 0;

                        for (int i = 0; i < nThreads; ++i) {
                            int start = current;
                            int end = start + chunk + (i < remainder ? 1 : 0);

                            threads.emplace_back([&, start, end]() {
                                multiplyAsmPartial(
                                    macierzA.data(),
                                    macierzB_pad.data(),
                                    macierzC_pad.data(),
                                    rowsA, colsA, colsB_pad,
                                    start, end);
                                });

                            current = end;
                        }

                        for (auto& t : threads)
                            t.join();
                        });

                    for (int i = 0; i < rowsA; ++i)
                        memcpy(&macierzC[i * colsB],
                            &macierzC_pad[i * colsB_pad],
                            colsB * sizeof(double));

                    PokazMacierz(hWnd, macierzC, rowsA, colsB, czasASM, nThreads);
                    ZapiszMacierzDoCSV(L"wynikASM.csv", macierzC, rowsA, colsB, czasASM, nThreads, L"ASM");
                    FreeLibrary(dllAsm);
                }
                else {
                    MessageBoxW(hWnd, L"Zaznacz tryb: C++ lub ASM.", L"Brak wyboru trybu", MB_OK | MB_ICONWARNING);
                }
            }
            break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }

        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Tutaj dodaj kod rysujący używający elementu hdc...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Procedura obsługi komunikatów dla okna informacji o programie.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
