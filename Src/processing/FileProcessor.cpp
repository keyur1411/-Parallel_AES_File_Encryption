#include "processing/FileProcessor.hpp"
#include "crypto/CryptoPipeline.hpp" // We need this!
#include <filesystem>
#include <iostream>
#include <chrono>
#include <cstdio> // For remove/rename

// We pass num_threads to the ThreadPool constructor
FileProcessor::FileProcessor(const std::string& db_path, size_t num_threads)
    : m_pool(num_threads), 
      m_logger(std::make_shared<DatabaseLogger>(db_path)) {}

bool FileProcessor::init() {
    return m_logger->init();
}

void FileProcessor::processDirectory(const std::string& directory, const std::string& mode, const unsigned char key[16]) {
    
    // We need to copy the key since the C-array will go out of scope
    // A std::vector is a safe way to hold a copy.
    std::vector<unsigned char> key_copy(key, key + 16);

    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        std::cerr << "Error: Directory not found." << std::endl;
        m_logger->log(directory, "DIR_NOT_FOUND", "FAILED", 0);
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string file_path = entry.path().string();
            std::string temp_path = file_path + ".tmp";
            
            // Submit a task to the pool
            // We use a lambda to capture all the variables needed for the task
            m_pool.submit([this, file_path, temp_path, mode, key_copy] {
                // Pass the raw pointer from the vector
                this->processFileTask(file_path, temp_path, mode, key_copy.data());
            });
        }
    }
    // The destructor of m_pool will wait for all tasks to finish
}

// This function runs on a thread pool thread
void FileProcessor::processFileTask(std::string file_path, std::string temp_path, std::string mode, const unsigned char* key) {
    
    // 1. Each task creates its OWN pipeline
    CryptoPipeline pipeline;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string operation;
    std::string status;

    std::cout << "Processing: " << file_path << " on thread " << std::this_thread::get_id() << std::endl;

    if (mode == "ENCRYPT") {
        operation = "ENCRYPT";
        success = pipeline.encryptFile(file_path, temp_path, key);
    } else {
        operation = "DECRYPT";
        success = pipeline.decryptFile(file_path, temp_path, key);
    }

    // 2. Finalize
    if (success) {
        std::remove(file_path.c_str());
        std::rename(temp_path.c_str(), file_path.c_str());
        status = "SUCCESS";
    } else {
        std::remove(temp_path.c_str());
        status = "FAILED";
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_ms = end_time - start_time;

    // 3. Log
    // The shared_ptr m_logger is thread-safe for logging
    // (because sqlite3_open/exec are internally serialized or we open/close each time)
    // Note: Your DatabaseLogger is thread-safe *enough* because it opens/closes
    // the DB on every log call. If it held the m_db open, we'd need a mutex.
    m_logger->log(file_path, operation, status, duration_ms.count());
}