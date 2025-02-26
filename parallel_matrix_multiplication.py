import random
import threading
import time

# Function to create a matrix with random values between 0 and 9
def create_matrix(rows, cols):
    return [[random.randint(0, 9) for _ in range(cols)] for _ in range(rows)]

# Function to print a matrix
def print_matrix(matrix):
    for row in matrix:
        print(" ".join(map(str, row)))

# Thread function to multiply a single row
def multiply_row(data):
    row, n, m, A, B, C = data
    for j in range(n):
        C[row][j] = sum(A[row][k] * B[k][j] for k in range(m))


def read_matrices_from_file(file_path):
    with open(file_path, "r") as file:
        lines = file.readlines()

    header_A = lines[0].strip().split(":")[1].strip()
    m, p = map(int, header_A.split("x"))

    matrix_A = []
    idx = 1
    for _ in range(m):
        matrix_A.append(list(map(int, lines[idx].strip().split())))
        idx += 1

    header_B = lines[idx].strip().split(":")[1].strip()
    p_B, n = map(int, header_B.split("x"))
    idx += 1

    if p != p_B:
        raise ValueError("Matrix A's columns do not match Matrix B's rows!")

    matrix_B = []
    for _ in range(p):
        matrix_B.append(list(map(int, lines[idx].strip().split())))
        idx += 1

    return m, n, p, matrix_A, matrix_B


def main():

    file_path = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\MatrixMul_input.txt"

    m, n, p, A, B = read_matrices_from_file(file_path)

    C = [[0] * n for _ in range(m)]

    # print("Matrix A:")
    # print_matrix(A)
    # print("\nMatrix B:")
    # print_matrix(B)

    start_time = time.time();
    threads_creation_times = []
    threads = []
    for i in range(m):
        thread_start = time.time()
        thread_data = (i, n, p, A, B, C)
        thread = threading.Thread(target=multiply_row, args=(thread_data,))
        threads.append(thread)
        thread.start()
        thread_end = time.time()  # Record thread creation end time
        threads_creation_times.append(thread_end - thread_start)

    join_start_time = time.time()
    for thread in threads:
        thread.join()
    join_end_time = time.time()

    end_time = time.time();
    execution_time = end_time - start_time
    join_time = join_end_time - join_start_time
    # print(f"\nExecution Time: {execution_time:.6f} seconds")

    total = 0.0
    for i, t_time in enumerate(threads_creation_times):
        total += t_time
    avg = total / m

    out_file_path = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\outputs.txt"
    try:
        with open(out_file_path, 'a') as file:
            file.write(f"\n\nParallel matrix multiplication in Python results:\n")
            file.write(f"\t(mm-py) Execution time: {execution_time:.6f} seconds.\n")
            file.write("\t(mm-py) Thread creation times:\n")
            for i, t_time in enumerate(threads_creation_times):
                file.write(f"\t\tThread {i}: {t_time:.6f} seconds\n")
            file.write(f"\t(mm-py) Average thread creation time: {avg:.6f} seconds\n")
            file.write(f"\t(mm-py) Context switch time: {join_time:.6f} seconds\n")

    except Exception as e:
        print(f"Error writing to file: {e}")

    # print("\nResult Matrix C:")
    # print_matrix(C)

if __name__ == "__main__":
    main()
