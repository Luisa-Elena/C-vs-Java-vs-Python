# import random
#
# def main():
#     random.seed()
#
#     n = random.randint(1, 100)
#     print(f"Merge sort n = {n}")
#
#     random_array = [random.randint(1, 1000) for _ in range(n)]
#
#     absolute_file_path = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\MS_input.txt"
#
#     with open(absolute_file_path, "w") as file:
#         file.write(f"n: {n}\n")
#         file.write("Array: " + " ".join(map(str, random_array)) + "\n")
#
#     print(f"Data written to file: {absolute_file_path}")
#
# if __name__ == "__main__":
#     main()



import random
import sys

def main():
    if len(sys.argv) != 2:
        print("Usage: python MS_input_generator.py <n>")
        return

    try:
        n = int(sys.argv[1])
        if n <= 0:
            raise ValueError("n must be a positive integer.")
    except ValueError as e:
        print(f"Invalid value for n: {e}")
        return

    random.seed()
    print(f"Merge sortttttttttt n = {n}")

    random_array = [random.randint(1, 1000) for _ in range(n)]

    absolute_file_path = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\MS_input.txt"

    with open(absolute_file_path, "w") as file:
        file.write(f"n: {n}\n")
        file.write("Array: " + " ".join(map(str, random_array)) + "\n")

    print(f"Data written to file: {absolute_file_path}")

if __name__ == "__main__":
    main()

