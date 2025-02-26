# C-vs-Java-vs-Python
Flow:  
- MS_input_generator.py will generate input for parallel merge sort, meaning an array of n random elements between 1 and 1000, and will write it in the MS_input.txt file
- MatrixMul_input_generator.py will generate the input for parallel matrix multiplication, meaning 2 matrices which can be multiplied and will write the result in the file MatrixMul_input.txt
- runPrograms.py will read the inputs from the corresponding files and will run the parallel merge sort and parallel matrix multiplication algorithms on that particular input in all 3 programming languages.
- Performance metrics, meaning execution time, thread creation time, context switch time and thread migration time for each program will be written in the output.txt file.
- From there, the results will be written in excel with ecxelWriter.py. We also plot excel graphs based on the result to make the comparison between the 3 programming languages  

Important:
- The absolute paths used in these programs are specific to my system and may not work on other machines. If you plan to run this code, please update the paths accordingly.

📄 [Read the full documentation](Documentation.pdf)
