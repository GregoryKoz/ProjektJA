#include "Macierze.h"
#include "pch.h"
extern "C" __declspec(dllexport)
void __cdecl MultiplyCpp(
    const double* A, const double* B, double* C,
    int rowsA, int colsA, int colsB)
{
    
    for (int i = 0; i < rowsA * colsB; ++i)
        C[i] = 0.0;

    // Mno¿enie macierzy
    for (int i = 0; i < rowsA; ++i)
    {
        for (int k = 0; k < colsA; ++k)
        {
            double a = A[i * colsA + k];
            const double* rowB = B + k * colsB;
            double* rowC = C + i * colsB;
            for (int j = 0; j < colsB; ++j)
            {
                rowC[j] += a * rowB[j];
            }
        }
    }
}
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