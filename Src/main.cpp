#include <iostream>
#include <string>
#include <thread>
#include "processing/FileProcessor.hpp"

int main() {
    // --- 1. Get User Input (do this first) ---
    size_t num_threads = std::thread::hardware_concurrency();
    std::cout << "Initializing with " << num_threads << " worker threads." << std::endl;

    std::string directory;
    std::cout << "Enter the path to your DIRECTORY: ";
    std::getline(std::cin, directory);

    std::cout << "Enter (e) to encrypt or (d) to decrypt all files: ";
    char mode_char;
    std::cin >> mode_char;

    std::string mode_str = (mode_char == 'e') ? "ENCRYPT" : "DECRYPT";
    const unsigned char key[16] = "keyurkumarkanja";

    // --- 2. Start Processing & Wait ---
    std::cout << "Starting to process directory..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    { // <-- ADD THIS NEW SCOPE
        FileProcessor processor("file_log.db", num_threads);
        if (!processor.init()) {
            std::cerr << "Failed to initialize logging database. Exiting." << std::endl;
            return 1;
        }
        std::cout << "Logging database 'file_log.db' initialized." << std::endl;

        processor.processDirectory(directory, mode_str, key);

    } // <-- ADD THIS CLOSING SCOPE

    // -----------------------------------------------------------------
    //  At this point, 'processor' has been destroyed.
    //  Its destructor called the ThreadPool's destructor.
    //  The ThreadPool's destructor joined all 12 worker threads.
    //  Therefore, all file processing is 100% complete.
    // -----------------------------------------------------------------

    // --- 3. Report Results (Now this is correct) ---
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;

    std::cout << "All files processed." << std::endl;
    std::cout << "Total time: " << duration_ms.count() << " ms." << std::endl;

    return 0;
}