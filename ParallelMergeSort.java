package org.example;

import java.io.*;
import java.util.Locale;
import java.util.concurrent.locks.ReentrantLock;

public class ParallelMergeSort {
    private static final int THRESHOLD = 5;

    private static int sharedVariable = 0;
    private static final ReentrantLock mutex = new ReentrantLock();

    static class WorkerThread extends Thread {
        private final int threadId;

        public WorkerThread(int threadId) {
            this.threadId = threadId;
        }

        @Override
        public void run() {
            mutex.lock();
            try {
                sharedVariable++;
            } finally {
                mutex.unlock();
            }
        }
    }

    private static void merge(int[] array, int left, int mid, int right) {
        int leftSize = mid - left + 1;
        int rightSize = right - mid;

        int[] leftArray = new int[leftSize];
        int[] rightArray = new int[rightSize];

        System.arraycopy(array, left, leftArray, 0, leftSize);
        System.arraycopy(array, mid + 1, rightArray, 0, rightSize);

        int i = 0, j = 0, k = left;

        while (i < leftSize && j < rightSize) {
            if (leftArray[i] <= rightArray[j]) {
                array[k++] = leftArray[i++];
            } else {
                array[k++] = rightArray[j++];
            }
        }

        while (i < leftSize) {
            array[k++] = leftArray[i++];
        }

        while (j < rightSize) {
            array[k++] = rightArray[j++];
        }
    }

    // MergeSortTask thread class
    private static class MergeSortTask extends Thread {
        private int[] array;
        private int left;
        private int right;

        public MergeSortTask(int[] array, int left, int right) {
            this.array = array;
            this.left = left;
            this.right = right;
        }

        @Override
        public void run() {
            if (left < right) {
                int mid = left + (right - left) / 2;

                if (right - left <= THRESHOLD) {
                    mergeSort(array, left, mid);
                    mergeSort(array, mid + 1, right);
                } else {
                    MergeSortTask leftTask = new MergeSortTask(array, left, mid);
                    MergeSortTask rightTask = new MergeSortTask(array, mid + 1, right);

                    leftTask.start();
                    rightTask.start();

                    try {
                        leftTask.join();
                        rightTask.join();
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    }
                }

                merge(array, left, mid, right);
            }
        }

        // Classical merge sort for array with length below TRESHOLD
        private void mergeSort(int[] array, int left, int right) {
            if (left < right) {
                int mid = left + (right - left) / 2;

                mergeSort(array, left, mid);
                mergeSort(array, mid + 1, right);

                merge(array, left, mid, right);
            }
        }
    }

    private static void printArray(int[] array) {
        for (int i : array) {
            System.out.print(i + " ");
        }
        System.out.println();
    }

    public static void main(String[] args) {
        String filePath = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\MS_input.txt";

        try (BufferedReader br = new BufferedReader(new FileReader(filePath))) {
            // Read n
            String firstLine = br.readLine().trim();
            int n = Integer.parseInt(firstLine.split(":")[1].trim());

            // Read array
            String secondLine = br.readLine().trim();
            String[] arrayStrings = secondLine.split(":")[1].trim().split(" ");
            int[] array = new int[n];
            for (int i = 0; i < n; i++) {
                array[i] = Integer.parseInt(arrayStrings[i]);
            }

//            System.out.println("Number of elements: " + n);
//            System.out.print("Unsorted array: ");
//            printArray(array);

            long startTime = System.nanoTime();
            MergeSortTask task = new MergeSortTask(array, 0, n - 1);
            task.start();

            try {
                task.join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                System.out.println("Merge sort interrupted");
            }

            long endTime = System.nanoTime();

            double durationInSeconds = (endTime - startTime) / 1_000_000_000.0;

            Thread[] threads = new Thread[n];
            long startContextSwitch = System.nanoTime();

            double totalThreadCreationTime = 0.0;

            for (int i = 0; i < n; i++) {
                final int threadId = i + 1;

                long threadStartTime = System.nanoTime();
                threads[i] = new WorkerThread(threadId);
                threads[i].start();
                long threadEndTime = System.nanoTime();

                totalThreadCreationTime += (threadEndTime - threadStartTime) / 1_000_000_000.0;
            }

            for (int i = 0; i < n; i++) {
                try {
                    threads[i].join();
                } catch (InterruptedException e) {
                    System.err.println("Thread " + i + " was interrupted.");
                }
            }

            long endContextSwitch = System.nanoTime();

            double avgThreadCreationTime = totalThreadCreationTime / n;
            double contextSwitchTime = (endContextSwitch - startContextSwitch) / 1_000_000_000.0;

            String outFilePath = "C:\\Users\\luisa\\Desktop\\SCS\\SCS_Project\\outputs.txt";
            try (BufferedWriter writer = new BufferedWriter(new FileWriter(outFilePath, true))) {
                writer.write("\nParallel merge sort in Java results:\n");
                writer.write("\t(ms-java) Execution time: " + String.format(Locale.US,"%.6f", durationInSeconds) + " seconds.\n");
                writer.write("\t(ms-java) Average thread creation time: " + String.format(Locale.US,"%.6f", avgThreadCreationTime) + " seconds.\n");
                writer.write("\t(ms-java) Context switch time: " + String.format(Locale.US,"%.6f", contextSwitchTime) + " seconds.\n");
            } catch (IOException e) {
                System.out.println("Error writing to file: " + e.getMessage());
            }

//            System.out.print("Sorted array: ");
//            printArray(array);

        } catch (IOException e) {
            System.out.println("Error reading file: " + e.getMessage());
        } catch (NumberFormatException e) {
            System.out.println("Error parsing input: " + e.getMessage());
        }
    }
}
