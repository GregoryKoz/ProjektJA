/* PROJEKT JÊZYKI ASEMBLEROWE
 TEMAT: MNO¯ENIE MACIERZY
 AUTOR: GRZEGORZ KOZ£OWSKI sem.5 INF
 DATA WYKONANIA PROJEKTU : semestr zimowy rok akademicki 2025 / 2026
 WERSJA FINALNA
*/

#include "Macierze.h"
#include "pch.h"

extern "C" __declspec(dllexport)
void MultiplyCppPartial(
    const double* A, const double* B, double* C,
    int rowsA, int colsA, int colsB,
    int startRow, int endRow)
{
    for (int i = startRow; i < endRow; ++i)
    {
        for (int j = 0; j < colsB; ++j)
        {
            double sum = 0.0;
            for (int k = 0; k < colsA; ++k)
                sum += A[i * colsA + k] * B[k * colsB + j];
            C[i * colsB + j] = sum;
        }
    }
}