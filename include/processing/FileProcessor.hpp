#pragma once

#include "utils/ThreadPool.hpp"
#include "logging/DatabaseLogger.hpp"
#include <string>
#include <memory> // For std::make_shared

class FileProcessor {
public:
    FileProcessor(const std::string& db_path, size_t num_threads);
    
    // Call this once to start the logging
    bool init();

    // Main function to start processing a directory
    void processDirectory(const std::string& directory, const std::string& mode, const unsigned char key[16]);

private:
    // This is the actual task that gets submitted to the pool
    void processFileTask(std::string file_path, std::string temp_path, std::string mode, const unsigned char* key);

    std::shared_ptr<DatabaseLogger> m_logger;
    ThreadPool m_pool;
    // Logger must be shared, so we use a shared_ptr
};