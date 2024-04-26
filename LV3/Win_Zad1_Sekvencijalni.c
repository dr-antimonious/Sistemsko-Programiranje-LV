// Sistemsko programiranje
// Treca laboratorijska vjezba
// Zadatak 1. - SEKVENCIJALNA VERZIJA

#undef UNICODE
#undef UNICODE_

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <time.h>

 /* Definira velicinu kvadratne matrice
  * Velicina matrice je MATRIX_SIZE * MATRIX_SIZE
  * Za sve kolicine koristenih niti ova konstanta mora biti iste vrijednosti
  * tj. moze se konstanta promijeniti,
  *     ali ne da je za 1 nit vrijednost n,
  *     a za 3 niti vrijednost m
  * Vrijednost ove konstante mora biti jednaka za sekvencijalni mod i ovaj mod,
  *     inace se ne moze provjeriti tocnost preko zbroja */
#define MATRIX_SIZE 2000

double** matrix;

int _tmain()
{
    clock_t time = clock();

    matrix = (double**)HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        sizeof(double*) * MATRIX_SIZE);

    if (matrix == NULL)
        ExitProcess(1);

    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        matrix[i] = (double*)HeapAlloc(GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            sizeof(double) * MATRIX_SIZE);
        if (matrix[i] == NULL)
            ExitProcess(2);
    }

    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            double temp = 0;
            for (int k = 0; k <= i; k++)
                temp += k * sin(j) - j * cos(k);
            matrix[i][j] = temp;
        }
    }

    double matrixSum = 0;
    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++)
            matrixSum += matrix[i][j];

    _tprintf(_T("Zbroj: %lf\n"), matrixSum);

    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        if (matrix[i] != NULL)
        {
            HeapFree(GetProcessHeap(), 0, matrix[i]);
            matrix[i] = NULL;
        }
    }

    if (matrix != NULL)
    {
        HeapFree(GetProcessHeap(), 0, matrix);
        matrix = NULL;
    }

    _tprintf(_T("Vrijeme (ms): %ld\n"), clock() - time);

    return 0;
}