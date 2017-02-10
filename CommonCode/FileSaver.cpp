#include "stdafx.h"

#include "FileSaver.h"

#include <boost/filesystem.hpp>
#include <iostream>

CFileSaver::CFileSaver(std::wstring folder) : m_folder(folder)
{
    if (!boost::filesystem::is_directory(folder)) {
        throw cannot_find_folder_exception(folder);
    }
}

std::unique_ptr<IFileSaver> FileSaver_create(std::wstring folder)
{
    return CFileSaver::Create(folder);
}

std::unique_ptr<IFileSaver> CFileSaver::Create(std::wstring folder)
{
    return std::make_unique<CFileSaver>(folder);
}

void CFileSaver::OpenFile(std::wstring filename, std::streamsize size, unsigned long from_proc)
{
    boost::filesystem::path path(m_folder);
    path /= filename;
    
    // Only one file per client
    m_files.erase(from_proc);

    if (boost::filesystem::is_directory(path))
        throw cannot_save_file_exception(path.wstring());

    // Check if file already exists.
    // Could generate a new name based on filename but let's just delete for now.
    if (boost::filesystem::exists(path))
        boost::filesystem::remove(path);
    
    std::ifstream stream(path.wstring(), std::ofstream::out | std::ofstream::trunc | std::ifstream::binary);
    if (!stream.is_open()) {
        std::wcerr << "Cannot open file " << path.wstring() << L" for writing" << std::endl;
        //throw cannot_save_file_exception(path.wstring());
        return;
    }
    stream.close();

    FileInfo_t info = { path.wstring(), size, 0 };
    m_files[from_proc] = info;

    std::wcerr << "Receiving file " << filename << " to " << m_folder << std::endl;
}

void CFileSaver::SaveData(unsigned long from_proc, const char * data, size_t size)
{
    auto info = m_files.find(from_proc);
    if (info != m_files.end()) {
        boost::filesystem::path path(info->second.filename);
        std::ofstream stream(path.wstring(), std::ofstream::app | std::ifstream::binary);
        if (!stream.is_open()) {
            std::wcerr << "Cannot open file for writing" << std::endl;
            std::wcerr << L"  The rest of data for the file " << path.wstring() << L" will be dropped." << std::endl;
            m_files.erase(from_proc);
            //throw cannot_save_file_exception(path.wstring());
            return;
        }

        try {
            stream.write(data, size);
        }
        catch (std::ios::failure& e) {
            std::cerr << "Cannot write to file: " << e.what() << std::endl;
            std::wcerr << L"  The rest of data for the file " << path.wstring() << L" will be dropped." << std::endl;
            m_files.erase(from_proc);
            //throw cannot_save_file_exception(path.wstring());
            return;
        }
        
        info->second.saved_bytes += size;
        
        if (info->second.saved_bytes >= info->second.size) {
            m_files.erase(from_proc);
            std::wcerr << L"All done for " << path.wstring() << std::endl;
        }
    }
}
