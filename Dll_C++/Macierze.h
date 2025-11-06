#pragma once

extern "C" __declspec(dllexport)
void __cdecl MultiplyCpp(
    const double* A, const double* B, double* C,
    int rowsA, int colsA, int colsB);
extern "C" __declspec(dllexport)
void MultiplyCppPartial(
    const double* A, const double* B, double* C,
    int rowsA, int colsA, int colsB,
    int startRow, int endRow);