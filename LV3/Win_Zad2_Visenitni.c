// Sistemsko programiranje
// Treca laboratorijska vjezba
// Zadatak 2. - VISENITNA VERZIJA

#undef UNICODE
#undef UNICODE_

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <time.h>

/* Broj niti koristenih za racunanje
 * Mijenjati prema izvjestaju (1-4) */
#define MAX_THREADS 4

 /* Definira velicinu kvadratne matrice
  * Velicina matrice je MATRIX_SIZE * MATRIX_SIZE
  * Za sve kolicine koristenih niti ova konstanta mora biti iste vrijednosti
  * tj. moze se konstanta promijeniti,
  *     ali ne da je za 1 nit vrijednost n,
  *     a za 3 niti vrijednost m
  * Vrijednost ove konstante mora biti jednaka za sekvencijalni mod i ovaj mod,
  *     inace se ne moze provjeriti tocnost preko zbroja */
#define MATRIX_SIZE 2000

DWORD WINAPI FillMatrixThread0(LPVOID startIndex);
DWORD WINAPI FillMatrix(LPVOID startIndex);
void FillRow(int j, int index, HANDLE matrixMutex);

volatile double** matrix;
volatile double prevColMean = 0, currColSum = 0;
SYNCHRONIZATION_BARRIER syncBarrier1, syncBarrier2;

int _tmain()
{
    clock_t time = clock();
    DWORD dwThreadIdArray[MAX_THREADS];
    HANDLE hThreadArray[MAX_THREADS];

    HANDLE matrixMutex = CreateMutex(NULL, FALSE, "matrixMutex");
    if (matrixMutex == NULL)
        ExitProcess(2);
    ReleaseMutex(matrixMutex);

    if (!InitializeSynchronizationBarrier(&syncBarrier1, MAX_THREADS, -1)
        || !InitializeSynchronizationBarrier(&syncBarrier2, MAX_THREADS, -1))
        ExitProcess(3);

    matrix = (volatile double**)HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        sizeof(volatile double*) * MATRIX_SIZE);

    if (matrix == NULL)
        ExitProcess(4);

    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        matrix[i] = (volatile double*)HeapAlloc(GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            sizeof(volatile double) * MATRIX_SIZE);
        if (matrix[i] == NULL)
            ExitProcess(5);
    }

    int** startIndex = (int**)HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        sizeof(int*) * MAX_THREADS);

    if (startIndex == NULL)
        ExitProcess(6);

    for (int i = 0; i < MAX_THREADS; i++)
    {
        startIndex[i] = (int*)HeapAlloc(GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            sizeof(int));
        if (startIndex[i] == NULL)
            ExitProcess(7);
        startIndex[i][0] = i;
    }

    hThreadArray[0] = CreateThread(NULL, 0, FillMatrixThread0,
        startIndex[0], 0,
        &dwThreadIdArray[0]);

    if (hThreadArray[0] == NULL)
        ExitProcess(8);

    for (int i = 1; i < MAX_THREADS; i++)
    {
        hThreadArray[i] = CreateThread(NULL, 0, FillMatrix,
            startIndex[i], 0,
            &dwThreadIdArray[i]);

        if (hThreadArray[i] == NULL)
            ExitProcess(9);
    }

    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    for (int i = 0; i < MAX_THREADS; i++)
    {
        CloseHandle(hThreadArray[i]);
        if (startIndex[i] != NULL)
        {
            HeapFree(GetProcessHeap(), 0, startIndex[i]);
            startIndex[i] = NULL;
        }
    }

    if (startIndex != NULL)
    {
        HeapFree(GetProcessHeap(), 0, startIndex);
        startIndex = NULL;
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
            HeapFree(GetProcessHeap(), 0, (LPVOID)matrix[i]);
            matrix[i] = NULL;
        }
    }

    if (matrix != NULL)
    {
        HeapFree(GetProcessHeap(), 0, matrix);
        matrix = NULL;
    }

    _tprintf(_T("Vrijeme (ms): %ld\n"), clock() - time);

    CloseHandle(matrixMutex);
    DeleteSynchronizationBarrier(&syncBarrier1);
    DeleteSynchronizationBarrier(&syncBarrier2);

    return 0;
}

DWORD WINAPI FillMatrixThread0(LPVOID startIndex)
{
    int index = *(int*)startIndex;

    HANDLE matrixMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "matrixMutex");

    for (int j = 0; j < MATRIX_SIZE; j++)
    {
        EnterSynchronizationBarrier(&syncBarrier1, 0);

    	WaitForSingleObject(matrixMutex, INFINITE);
        prevColMean = currColSum / MATRIX_SIZE;
        currColSum = 0;
        ReleaseMutex(matrixMutex);

        EnterSynchronizationBarrier(&syncBarrier2, 0);

        FillRow(j, index, matrixMutex);
    }

    CloseHandle(matrixMutex);
    return 0;
}

DWORD WINAPI FillMatrix(LPVOID startIndex)
{
    int index = *(int*)startIndex;

    HANDLE matrixMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "matrixMutex");

    for (int j = 0; j < MATRIX_SIZE; j++)
    {
        EnterSynchronizationBarrier(&syncBarrier1, 0);
        EnterSynchronizationBarrier(&syncBarrier2, 0);
        FillRow(j, index, matrixMutex);
    }

    CloseHandle(matrixMutex);
	return 0;
}

void FillRow(int j, int index, HANDLE matrixMutex)
{
    for (int i = index; i < MATRIX_SIZE; i += MAX_THREADS)
    {
        double temp = 0;
        for (int k = 0; k <= i; k++)
            temp += k * sin(j) - j * cos(k);

        WaitForSingleObject(matrixMutex, INFINITE);
        matrix[i][j] = prevColMean + temp;
        currColSum += matrix[i][j];
        ReleaseMutex(matrixMutex);
    }
}