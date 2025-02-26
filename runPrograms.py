import subprocess
import random


def execute_c(exe_path):
    result = subprocess.run([exe_path], capture_output=True, text=True)
    print(result.stdout)

def execute_py(py_path, *args):
    command = ['python', py_path, *map(str, args)]
    result = subprocess.run(command, capture_output=True, text=True)
    print(result.stdout)

def execute_java(java_class_path):
    result = subprocess.run(['java', java_class_path], capture_output=True, text=True, check=True)
    print(result.stdout)

def main():
    random.seed()
    n_values = sorted(random.sample(range(1, 101), 15))

    for n in n_values:
        print(f"\nProcessing for n = {n}\n" + "-" * 30)
        execute_py("C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\input_generator\\MS_input_generator.py", n)
        execute_py("C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\input_generator\\MatrixMul_input_generator.py")

        execute_c("C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\C\\Parallel_Merge_Sort\\x64\\Debug\\Parallel_Merge_Sort.exe")
        execute_py("C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\Python\\Parallel_Merge_Sort\\main.py")
        execute_java("C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\Java\\Parallel_Merge_Sort\\src\\main\\java\\org\\example\\ParallelMergeSort.java")

        execute_c("C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\C\\Parallel_Matrix_Multiplication\\x64\\Debug\\Parallel_Matrix_Multiplication.exe")
        execute_py("C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\Python\\Parallel_Matrix_Multiplication\\main.py")
        execute_java("C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\Java\\Parallel_Matrix_Multiplication\\src\\main\\java\\org\\example\\MatrixMultiplication.java")


if __name__ == "__main__":
    main()
