import random

def main():
    random.seed()

    m = random.randint(1, 20)  # Rows in Matrix A and C
    p = random.randint(1, 20)  # Columns in Matrix A, Rows in Matrix B
    n = random.randint(1, 20)  # Columns in Matrix B and C

    print(f"Matrix multiplication m = {m}, n = {n}, p ={p}")

    matrix_A = [[random.randint(0, 9) for _ in range(p)] for _ in range(m)]
    matrix_B = [[random.randint(0, 9) for _ in range(n)] for _ in range(p)]

    absolute_file_path = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\MatrixMul_input.txt"

    with open(absolute_file_path, "w") as file:
        file.write(f"Matrix A (m x p): {m} x {p}\n")
        for row in matrix_A:
            file.write(" ".join(map(str, row)) + "\n")
        file.write(f"Matrix B (p x n): {p} x {n}\n")
        for row in matrix_B:
            file.write(" ".join(map(str, row)) + "\n")

    print(f"Data written to file: {absolute_file_path}")

if __name__ == "__main__":
    main()
