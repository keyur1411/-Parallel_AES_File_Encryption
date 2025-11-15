#include <iostream>
#include <string>
#include <thread> // For hardware_concurrency
#include "processing/FileProcessor.hpp"

int main() {
    // --- 1. Init FileProcessor ---
    // Use the optimal number of threads for this system
    size_t num_threads = std::thread::hardware_concurrency();
    std::cout << "Initializing with " << num_threads << " worker threads." << std::endl;
    
    FileProcessor processor("file_log.db", num_threads);
    if (!processor.init()) {
        std::cerr << "Failed to initialize logging database. Exiting." << std::endl;
        return 1;
    }
    std::cout << "Logging database 'file_log.db' initialized." << std::endl;

    // --- 2. Get User Input ---
    std::string directory;
    std::cout << "Enter the path to your DIRECTORY: ";
    std::getline(std::cin, directory);

    std::cout << "Enter (e) to encrypt or (d) to decrypt all files: ";
    char mode_char;
    std::cin >> mode_char;

    std::string mode_str = (mode_char == 'e') ? "ENCRYPT" : "DECRYPT";
    const unsigned char key[16] = "keyurkumarkanja";
    
    // --- 3. Start Processing ---
    std::cout << "Starting to process directory... (This will run in the background)" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    processor.processDirectory(directory, mode_str, key);
    
    // --- 4. Wait for completion ---
    // The FileProcessor destructor will block until all tasks are done
    // when 'processor' goes out of scope.
    
    std::cout << "All files processed." << std::endl;
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;
    
    std::cout << "Total time: " << duration_ms.count() << " ms." << std::endl;

    return 0;
}