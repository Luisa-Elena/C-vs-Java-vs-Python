#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#define THRESHOLD 5  // Minimum size for parallel sort

double wait_time = 0;

typedef struct {
    int* array;
    int left;
    int right;
} SortParams;


void merge(int* array, int left, int mid, int right) {
    int left_size = mid - left + 1;
    int right_size = right - mid;
    int* left_array = (int*)malloc(left_size * sizeof(int));
    int* right_array = (int*)malloc(right_size * sizeof(int));

    for (int i = 0; i < left_size; i++)
        left_array[i] = array[left + i];
    for (int i = 0; i < right_size; i++)
        right_array[i] = array[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < left_size && j < right_size) {
        if (left_array[i] <= right_array[j]) {
            array[k++] = left_array[i++];
        }
        else {
            array[k++] = right_array[j++];
        }
    }
    while (i < left_size) {
        array[k++] = left_array[i++];
    }
    while (j < right_size) {
        array[k++] = right_array[j++];
    }

    free(left_array);
    free(right_array);
}


DWORD WINAPI merge_sort(LPVOID params) {
    SortParams* args = (SortParams*)params;
    int left = args->left;
    int right = args->right;
    int* array = args->array;

    if (left < right) {
        int mid = left + (right - left) / 2;

        if ((right - left) <= THRESHOLD) {
            // Sequential sort if below threshold
            SortParams left_params = { array, left, mid };
            SortParams right_params = { array, mid + 1, right };
            merge_sort(&left_params);
            merge_sort(&right_params);
        }
        else {
            // Parallel sort if above threshold
            HANDLE left_thread, right_thread;
            DWORD left_thread_id, right_thread_id;

            SortParams left_params = { array, left, mid };
            SortParams right_params = { array, mid + 1, right };

            left_thread = CreateThread(NULL, 0, merge_sort, &left_params, 0, &left_thread_id);
            right_thread = CreateThread(NULL, 0, merge_sort, &right_params, 0, &right_thread_id);
            
            WaitForSingleObject(left_thread, INFINITE);
            WaitForSingleObject(right_thread, INFINITE);
           
            CloseHandle(left_thread);
            CloseHandle(right_thread);
        }

        merge(array, left, mid, right);
    }
    return 0;
}


int sharedVariable = 0;
HANDLE mutex;
DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    int threadId = *(int*)lpParam;

    WaitForSingleObject(mutex, INFINITE);
    //printf("Thread %d: Acquired mutex\n", threadId);

    //printf("Thread %d: Shared variable before increment: %d\n", threadId, sharedVariable);
    sharedVariable++;
    //printf("Thread %d: Shared variable after increment: %d\n", threadId, sharedVariable);
    
    ReleaseMutex(mutex);

    return 0;
}


void set_thread_affinity(HANDLE thread, DWORD cpuCore);
DWORD WINAPI thread_function(LPVOID lpParam);

int static_array[100];

LARGE_INTEGER end_th_mig, start_th_mig, f;

int main() {

    FILE* file = fopen("C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\MS_input.txt", "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    int n;
    if (fscanf(file, "n: %d\nArray: ", &n) != 1) {
        fprintf(stderr, "Error reading the size of the array\n");
        fclose(file);
        return 1;
    }

    int* array = (int*)malloc(n * sizeof(int));
    if (!array) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return 1;
    }

    //printf("Number of elements: %d\n", n);

    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%d", &array[i]) != 1) {
            fprintf(stderr, "Error reading array elements\n");
            free(array);
            fclose(file);
            return 1;
        }
        static_array[i] = array[i];
    }
    fclose(file);

    /*printf("\nUnsorted array:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }*/

    LARGE_INTEGER freq, start, end;
    LARGE_INTEGER frequency1, start1, end1;

    SortParams params = { array, 0, n - 1 };
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    merge_sort(&params);
    QueryPerformanceCounter(&end);

    SortParams params1 = { static_array, 0, n - 1 };
    QueryPerformanceFrequency(&frequency1);
    QueryPerformanceCounter(&start1);
    merge_sort(&params1);
    QueryPerformanceCounter(&end1);


    double execution_time_dynamic = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    double execution_time_static = (double)(end1.QuadPart - start1.QuadPart) / frequency1.QuadPart;
    /*printf("Parallel merge sort in C results:\n");
    printf("Execution time: %.6f seconds\n", execution_time);*/

    // ------------------------------------------------------------------------------------------------- Context switch

    mutex = CreateMutex(NULL, FALSE, NULL);
    if (mutex == NULL) {
        printf("Mutex creation failed with error %d\n", GetLastError());
        return 1;
    }

    HANDLE threads[100];
    int threadIds[100];
    LARGE_INTEGER startTime, endTime, frequency;
    LARGE_INTEGER contextStartTime, contextEndTime;
    QueryPerformanceFrequency(&frequency);
    double thread_creation_time = 0.0;

    QueryPerformanceCounter(&contextStartTime);

    for (int i = 0; i < n; i++) {
        threadIds[i] = i + 1;
        QueryPerformanceCounter(&startTime);
        threads[i] = CreateThread(NULL, 0, ThreadFunc, &threadIds[i], 0, NULL);
        QueryPerformanceCounter(&endTime);
        if (threads[i] != NULL) {
            thread_creation_time += (double)(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
            WaitForSingleObject(threads[i], INFINITE);
            CloseHandle(threads[i]);
        }
        else {
            printf("Thread %d creation failed.\n", i);
        }
    }

    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    CloseHandle(mutex);
    for (int i = 0; i < 2; i++) {
        CloseHandle(threads[i]);
    }

    QueryPerformanceCounter(&contextEndTime);

    double context_switch_time = (double)(contextEndTime.QuadPart - contextStartTime.QuadPart) / frequency.QuadPart;
    double avg_th_creation_time = thread_creation_time * 1.0 / n;

    // ------------------------------------------------------------------------------------------------- Thread migration
    HANDLE thread = CreateThread(NULL, 0, thread_function, NULL, 0, NULL);

    if (thread == NULL) {
        printf("Error creating thread\n");
        return 1;
    }

    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    double th_mig_time = (double)(end_th_mig.QuadPart - start_th_mig.QuadPart) / frequency.QuadPart;
    double  avg_exec = (execution_time_dynamic + execution_time_static) / 2;

    // ---------------------------------------------------------------------------------------------------

    const char* out_file_path = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\outputs.txt";
    FILE* out_file = fopen(out_file_path, "a");
    if (out_file != NULL) {
        fprintf(out_file, "\n(n) n = %d elements\n", n);
        fprintf(out_file, "Parallel merge sort in C results:\n");
        fprintf(out_file, "\t(ms-c) Execution time - dynamic: %.6f seconds\n", execution_time_dynamic);
        fprintf(out_file, "\t(ms-c) Execution time - static: %.6f seconds\n", execution_time_static);
        fprintf(out_file, "\t(ms-c) Average execution time: %.6f seconds\n", avg_exec);
        fprintf(out_file, "\t(ms-c) Average thread creation time: %.6f seconds\n", avg_th_creation_time);
        fprintf(out_file, "\t(ms-c) Context switch time: %.6f seconds\n", context_switch_time);
        fprintf(out_file, "\t(ms-c) Thread migration time: %.6f seconds\n", th_mig_time);
        fclose(out_file);
    }
    else {
        printf("Error opening the file for writing.\n");
    }

    /*printf("\nSorted array:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");*/



    free(array);
    return 0;
}


void set_thread_affinity(HANDLE thread, DWORD cpuCore) {

    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&start_th_mig);

    // Set the thread's CPU affinity mask
    DWORD_PTR affinityMask = (1 << cpuCore);  // Move 1 to the position of the specified CPU core
    if (SetThreadAffinityMask(thread, affinityMask) == 0) {
        printf("Error setting thread affinity\n");
    } else {
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
