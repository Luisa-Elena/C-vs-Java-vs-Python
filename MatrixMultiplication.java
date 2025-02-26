package org.example;

import java.io.IOException;
import java.util.Random;
import java.io.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.Locale;

public class MatrixMultiplication {
    private static final int MAX_THREADS = 64;
    private static class MultiplyRowTask extends Thread {
        private int row; // Row number in the result matrix
        private int n; // Number of columns in matrix B
        private int m; // Number of columns in matrix A and rows in matrix B
        private int[][] A; // Matrix A
        private int[][] B; // Matrix B
        private int[][] C; // Result matrix C

        public MultiplyRowTask(int row, int n, int m, int[][] A, int[][] B, int[][] C) {
            this.row = row;
            this.n = n;
            this.m = m;
            this.A = A;
            this.B = B;
            this.C = C;
        }

        @Override
        public void run() {
            for (int j = 0; j < n; j++) {
                C[row][j] = 0;
                for (int k = 0; k < m; k++) {
                    C[row][j] += A[row][k] * B[k][j];
                }
            }
        }
    }

    private static int[][] createMatrix(int rows, int cols) {
        Random rand = new Random();
        int[][] matrix = new int[rows][cols];
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                matrix[i][j] = rand.nextInt(10);  // Random values between 0 and 9
            }
        }
        return matrix;
    }

    private static void printMatrix(int[][] matrix) {
        for (int[] row : matrix) {
            for (int val : row) {
                System.out.print(val + " ");
            }
            System.out.println();
        }
    }


    public static void main(String[] args) {
        String filePath = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\MatrixMul_input.txt";

        try (BufferedReader br = new BufferedReader(new FileReader(filePath))) {
            int n, m, p;

            String inputA = br.readLine();
            String regexA = "Matrix A \\(m x p\\): (\\d+) x (\\d+)";
            Pattern patternA = Pattern.compile(regexA);
            Matcher matcherA = patternA.matcher(inputA);

            if (matcherA.find()) {
                m = Integer.parseInt(matcherA.group(1));
                p = Integer.parseInt(matcherA.group(2));
            } else {
                System.out.println("Error: Could not read dimensions of Matrix A.");
                return;
            }

            int[][] A = new int[m][p];
            for (int i = 0; i < m; i++) {
                String[] row = br.readLine().trim().split("\\s+");
                for (int j = 0; j < p; j++) {
                    A[i][j] = Integer.parseInt(row[j]);
                }
            }

            // Read dimensions of Matrix B
            String inputB = br.readLine(); // This should be "Matrix B (p x n): ..."
            String regexB = "Matrix B \\(p x n\\): (\\d+) x (\\d+)";
            Pattern patternB = Pattern.compile(regexB);
            Matcher matcherB = patternB.matcher(inputB);

            if (matcherB.find()) {
                p = Integer.parseInt(matcherB.group(1));
                n = Integer.parseInt(matcherB.group(2));
            } else {
                System.out.println("Error: Could not read dimensions of Matrix B.");
                return;
            }

            // Read Matrix B
            int[][] B = new int[p][n];
            for (int i = 0; i < p; i++) {
                String[] row = br.readLine().trim().split("\\s+");
                for (int j = 0; j < n; j++) {
                    B[i][j] = Integer.parseInt(row[j]);
                }
            }

//            System.out.println("Matrix A:");
//            printMatrix(A);
//
//            System.out.println("\nMatrix B:");
//            printMatrix(B);

            int[][] C = createMatrix(m, n);

            Thread[] threads = new Thread[m];
            double[] threadCreationTimes = new double[m];

            long startTime = System.nanoTime();

            for (int i = 0; i < m; i++) {
                long threadStartTime = System.nanoTime();
                threads[i] = new MultiplyRowTask(i, n, p, A, B, C);
                threads[i].start();
                long threadEndTime = System.nanoTime();
                threadCreationTimes[i] = (threadEndTime - threadStartTime) / 1_000_000_000.0; // Convert to seconds
            }

            long joinStartTime = System.nanoTime();
           try {
                for (int i = 0; i < m; i++) {
                    threads[i].join();  // Wait for each thread to finish
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            long joinEndTime = System.nanoTime();

            long endTime = System.nanoTime();

            double joinTimeInSeconds = (joinEndTime - joinStartTime) / 1_000_000_000.0;
            double durationInSeconds = (endTime - startTime) / 1_000_000_000.0;
            //System.out.println("\nExecution time: " + durationInSeconds + " seconds.\n");

            double total = 0.0;
            for (int i = 0; i < m; i++) {
                total += threadCreationTimes[i];
            }
            double avg = total / m;

            String outFilePath = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\outputs.txt";
            try (BufferedWriter writer = new BufferedWriter(new FileWriter(outFilePath, true))) {
                // Open the file in append mode (true for append)
                writer.write("\n\nParallel matrix multiplication in Java results:\n");
                writer.write("\t(mm-java) Execution time: " + String.format(Locale.US,"%.6f", durationInSeconds) + " seconds.\n");
                writer.write("\t(mm-java) Thread creation times (seconds):\n");
                for (int i = 0; i < m; i++) {
                    writer.write(String.format("\t\tThread %d: %.9f seconds\n", i, threadCreationTimes[i]));
                }
                writer.write(String.format(Locale.US, "\t(mm-java) Average thread creation time: %.9f seconds\n", avg));
                writer.write(String.format(Locale.US,"\t(mm-java) Context switch time: %.9f seconds\n", joinTimeInSeconds));

            } catch (IOException e) {
                // Handle any I/O errors
                System.out.println("Error writing to file: " + e.getMessage());
            }

//            System.out.println("\nResult Matrix C:");
//            printMatrix(C);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
