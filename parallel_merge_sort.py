import threading
import time

THRESHOLD = 5  # Minimum size for parallel sort

def merge(array, left, mid, right):
    left_size = mid - left + 1
    right_size = right - mid

    left_array = array[left:left + left_size]
    right_array = array[mid + 1:mid + 1 + right_size]

    i = j = 0
    k = left

    while i < left_size and j < right_size:
        if left_array[i] <= right_array[j]:
            array[k] = left_array[i]
            i += 1
        else:
            array[k] = right_array[j]
            j += 1
        k += 1

    while i < left_size:
        array[k] = left_array[i]
        i += 1
        k += 1

    while j < right_size:
        array[k] = right_array[j]
        j += 1
        k += 1

def merge_sort_thread(params):
    array, left, right = params
    if left < right:
        mid = left + (right - left) // 2

        if (right - left) <= THRESHOLD:
            # Sequential sort if below threshold
            merge_sort_thread((array, left, mid))
            merge_sort_thread((array, mid + 1, right))
        else:
            # Parallel sort if above threshold
            left_thread = threading.Thread(target=merge_sort_thread, args=((array, left, mid),))
            right_thread = threading.Thread(target=merge_sort_thread, args=((array, mid + 1, right),))

            left_thread.start()
            right_thread.start()

            left_thread.join()
            right_thread.join()

        merge(array, left, mid, right)


shared_variable = 0
mutex = threading.Lock()
def thread_func(thread_id):
    global shared_variable
    mutex.acquire()
    shared_variable += 1
    mutex.release()


def main():

    file_path = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\MS_input.txt"

    try:
        with open(file_path, 'r') as file:

            first_line = file.readline().strip()
            n = int(first_line.split(':')[1].strip())

            second_line = file.readline().strip()
            array = list(map(int, second_line.split(':')[1].strip().split()))

        if len(array) != n:
            raise ValueError("Mismatch between the number of elements and the size specified in the file.")

        # print(f"Number of elements: {n}")
        # print("Unsorted array:")
        # print(" ".join(map(str, array)))

        start_time = time.time()
        merge_sort_thread((array, 0, n - 1))
        end_time = time.time()

        execution_time = end_time - start_time
        #print(f"\nExecution time: {execution_time:.6f} seconds.")

        thread_creation_time = 0.0
        start_context_switch = time.time()
        threads = []

        for i in range(n):
            thread_id = i + 1

            thread_start_time = time.time()
            thread = threading.Thread(target=thread_func, args=(thread_id,))
            thread.start()
            threads.append(thread)
            thread_end_time = time.time()

            thread_creation_time += (thread_end_time - thread_start_time)

        for thread in threads:
            thread.join()

        end_context_switch = time.time()  # Record end time for context switch

        context_switch_time = end_context_switch - start_context_switch
        avg_th_creation_time = thread_creation_time / n

        out_file_path = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\outputs.txt"
        try:
            with open(out_file_path, 'a') as file:
                file.write(f"\nParallel merge sort in Python results\n")
                file.write(f"\t(ms-py) Execution time: {execution_time:.6f} seconds.\n")
                file.write(f"\t(ms-py) Average thread creation time: {avg_th_creation_time:.6f} seconds.\n")
                file.write(f"\t(ms-py) Context switch time: {context_switch_time:.6f} seconds.\n")
        except Exception as e:
            print(f"Error writing to file: {e}")

        # print("\nSorted array:")
        # print(" ".join(map(str, array)))

    except FileNotFoundError:
        print(f"Error: File not found at {file_path}")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()
