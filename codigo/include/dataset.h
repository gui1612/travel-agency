#pragma once

#include <vector>
#include <string>
#include <fstream>

using FileEntry = std::vector<unsigned long>;
class File;

#include "entities/graph.h"

/** @brief Manages file operations */
class File {

    std::vector<unsigned long> header;

    /** @brief Vector containing all entry data */
    std::vector<FileEntry> entries;

    /**
     * @brief Reads file header
     * 
     * @param file The file to be read from
     * @param delim The character that delimits entries
     * @return true if data could be retrieved
     * @return false if data could not be retrieved
     */
    bool read_header(std::ifstream &file, char delim);
    
    /**
     * @brief Reads file body
     * 
     * @param file The file to be read from
     * @param delim The character that delimits entries
     * @return true if data could be retrieved
     * @return false if data could not be retrieved
     */
    bool read_body(std::ifstream &file, char delim);

public:
    /**
     * @brief Construct a new File object
     * 
     * @param path The file's absolute path
     * @param delim The character that delimits the data 
     */
    File(std::string path, char delim = ' ');

    /**
     * @brief Reads the header from the target file
     * 
     * @return A vector holding the header values
     */
    std::vector<unsigned long> get_header() const;

    /**
     * @brief Reads all the entries from the target file
     * 
     * @return A vector holding all the entries
     */
    std::vector<FileEntry> get_entries() const;
};

