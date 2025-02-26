#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#define MAX_THREADS 64  // Limit the maximum number of threads

typedef struct {
    int row; // Row number in the result matrix
    int n; // Number of columns in matrix B
    int m; // Number of columns in matrix A and rows in matrix B
    int** A; // Matrix A
    int** B; // Matrix B
    int** C; // Result matrix C
} ThreadData;

typedef struct {
    int row;        // Row number in the result matrix
    int n;          // Number of columns in matrix B
    int m;          // Number of columns in matrix A and rows in matrix B
    int(*A)[20];   // Pointer to static matrix A (array of 20 ints)
    int(*B)[20];   // Pointer to static matrix B
    int(*C)[20];   // Pointer to static matrix C
} ThreadData1;


DWORD WINAPI multiply_row(LPVOID param) {
    ThreadData* data = (ThreadData*)param;
    int row = data->row;

    for (int j = 0; j < data->n; j++) {
        data->C[row][j] = 0;
        for (int k = 0; k < data->m; k++) {
            data->C[row][j] += data->A[row][k] * data->B[k][j];
        }
    }

    return 0;
}

DWORD WINAPI multiply_row_static(LPVOID param) {
    ThreadData1* data = (ThreadData1*)param; // Use the ThreadData1 struct
    int row = data->row;

    for (int j = 0; j < data->n; j++) {
        data->C[row][j] = 0;  // Initialize the result cell
        for (int k = 0; k < data->m; k++) {
            data->C[row][j] += data->A[row][k] * data->B[k][j];
        }
    }

    return 0;
}


int** create_matrix(int rows, int cols) {
    int** matrix = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = rand() % 10;  // Random values between 0 and 9
        }
    }
    return matrix;
}

void free_matrix(int** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void print_matrix(int** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}


void read_matrix(FILE* file, int** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(file, "%d", &matrix[i][j]);
        }
    }
}


void set_thread_affinity(HANDLE thread, DWORD cpuCore);
DWORD WINAPI thread_function(LPVOID lpParam);

LARGE_INTEGER end_th_mig, start_th_mig, f;


int A_static[20][20];
int B_static[20][20];
int C_static[20][20];

int main() {
    const char* filePath = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\MatrixMul_input.txt";
    FILE* file = fopen(filePath, "r");
    if (!file) {
        printf("Error: Unable to open file %s\n", filePath);
        return 1;
    }

    int m, p, n;

    fscanf(file, "Matrix A (m x p): %d x %d\n", &m, &p);

    int** A = create_matrix(m, p);
    read_matrix(file, A, m, p);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            A_static[i][j] = A[i][j];
        }
    }

    fscanf(file, "\nMatrix B (p x n): %d x %d\n", &p, &n);

    int** B = create_matrix(p, n);
    read_matrix(file, B, p, n);
    for (int i = 0; i < p; i++) {
        for (int j = 0; j < n; j++) {
            B_static[i][j] = B[i][j];
        }
    }

    int** C = create_matrix(m, n);

    /*printf("Matrix A:\n");
    print_matrix(A, m, p);
    printf("\nMatrix B:\n");
    print_matrix(B, p, n);*/


    HANDLE threads[MAX_THREADS];
    ThreadData threadData[MAX_THREADS];

    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);

    double thread_creation_times[MAX_THREADS];

    LARGE_INTEGER freq, start_time, end_time;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start_time);

    for (int i = 0; i < m; i++) {
        threadData[i].row = i;
        threadData[i].n = n;
        threadData[i].m = p;
        threadData[i].A = A;
        threadData[i].B = B;
        threadData[i].C = C;

        QueryPerformanceCounter(&start);
        threads[i] = CreateThread(NULL, 0, multiply_row, &threadData[i], 0, NULL);
        QueryPerformanceCounter(&end);
        
        if (threads[i] == NULL) {
            printf("Error: Unable to create thread %d\n", i);
            return 1;
        }

        thread_creation_times[i] = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    }

    QueryPerformanceCounter(&start);
    WaitForMultipleObjects(m, threads, TRUE, INFINITE);
    QueryPerformanceCounter(&end);

    double wait_time = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;

    for (int i = 0; i < m; i++) {
        CloseHandle(threads[i]);
    }

    QueryPerformanceCounter(&end_time);

    HANDLE threads1[MAX_THREADS];
    ThreadData1 threadData1[MAX_THREADS];

    LARGE_INTEGER frequency1, start_time1, end_time1;
    QueryPerformanceFrequency(&frequency1);
    QueryPerformanceCounter(&start_time1);

    for (int i = 0; i < m; i++) {
        threadData1[i].row = i;
        threadData1[i].n = n;
        threadData1[i].m = p;
        threadData1[i].A = A_static;
        threadData1[i].B = B_static;
        threadData1[i].C = C_static;

        threads1[i] = CreateThread(NULL, 0, multiply_row_static, &threadData1[i], 0, NULL);
        if (threads1[i] == NULL) {
            printf("Error: Unable to create thread %d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < m; i++) {
        CloseHandle(threads1[i]);
    }

    QueryPerformanceCounter(&end_time1);

   double execution_time_dynamic = (double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart;
   double execution_time_static = (double)(end_time1.QuadPart - start_time1.QuadPart) / frequency1.QuadPart;

   double thread_creation_time = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;

   // ------------------------------------------------------------------------------------------------- Thread migration
   HANDLE thread = CreateThread(NULL, 0, thread_function, NULL, 0, NULL);

   if (thread == NULL) {
       printf("Error creating thread\n");
       return 1;
   }

   WaitForSingleObject(thread, INFINITE);
   CloseHandle(thread);

   double th_mig_time = (double)(end_th_mig.QuadPart - start_th_mig.QuadPart) / frequency.QuadPart;

   // ---------------------------------------------------------------------------------------------------

   double total = 0.0;
   for (int i = 0; i < m; i++) {
       total += thread_creation_times[i];
   }
   double avg = total * 1.0 / m;

   double  avg_exec = (execution_time_dynamic + execution_time_static) / 2;

    const char* out_file_path = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\outputs.txt";
    FILE* out_file = fopen(out_file_path, "a");
    if (out_file != NULL) {
        fprintf(out_file, "\n\nParallel matrix multiplication in C results:\n");
        fprintf(out_file, "\t(mm-c) Execution time - dynamic: %.6f seconds\n", execution_time_dynamic);
        fprintf(out_file, "\t(mm-c) Execution time - static: %.6f seconds\n", execution_time_static);
        fprintf(out_file, "\t(mm-c) Average execution time: %.6f seconds\n", avg_exec);
        fprintf(out_file, "\t(mm-c) Thread creation times:\n");
        for (int i = 0; i < m; i++) {
            fprintf(out_file, "\t\t Thread %d: %.9f seconds\n", i, thread_creation_times[i]);
        }
        fprintf(out_file, "\t(mm-c) Average thread creation time: %.6f seconds\n", avg);
        fprintf(out_file, "\t(mm-c) Context switch time: %.6f seconds\n", wait_time);
        fprintf(out_file, "\t(mm-c) Thread migration time: %.6f seconds\n", th_mig_time);
        fclose(out_file);
    }
    else {
        printf("Error opening the file for writing.\n");
    }

    /*printf("\nResult Matrix C:\n");
    print_matrix(C, m, n);*/

    free_matrix(A, m);
    free_matrix(B, p);
    free_matrix(C, m);

    fclose(file);
    return 0;
}


void set_thread_affinity(HANDLE thread, DWORD cpuCore) {

    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&start_th_mig);

    // Set the thread's CPU affinity mask
    DWORD_PTR affinityMask = (1 << cpuCore);  // Move 1 to the position of the specified CPU core
    if (SetThreadAffinityMask(thread, affinityMask) == 0) {
        printf("Error setting thread affinity\n");
    }
    else {
        printf("Thread affinity set to CPU core %d\n", cpuCore);
    }

    QueryPerformanceCounter(&end_th_mig);
}

DWORD WINAPI thread_function(LPVOID lpParam) {
    int* coreToUse = (int*)lpParam;

    set_thread_affinity(GetCurrentThread(), 0);

    DWORD currentCore = GetCurrentProcessorNumber();
    printf("Thread started on CPU core %d\n", currentCore);

    // Simulate some work or delay
    //Sleep(1000);  // Sleep for 1 second before moving the thread to core 1

    set_thread_affinity(GetCurrentThread(), 1);
    currentCore = GetCurrentProcessorNumber();
    printf("Thread is now running on CPU core %d\n", currentCore);

    return 0;
}