#pragma once

#include <map>

#include <boost/core/noncopyable.hpp>
#include <boost/filesystem.hpp>

#include "Interfaces.h"
class CFileSaver :
    public IFileSaver,
    private boost::noncopyable
{
public:
    CFileSaver(std::wstring folder);
    virtual ~CFileSaver() {};

    static std::unique_ptr<IFileSaver> Create(std::wstring folder);

    virtual void OpenFile(std::wstring filename, std::streamsize size, unsigned long from_proc);
    virtual void SaveData(unsigned long from_proc, const char* data, size_t size);

private:

    std::wstring m_folder;

    typedef struct {
        boost::filesystem::path filename;
        std::streamsize size;
        std::streamsize saved_bytes;
    } FileInfo_t;

    typedef std::map<unsigned long, FileInfo_t> FileInfoMap_t;
    FileInfoMap_t m_files;
};

