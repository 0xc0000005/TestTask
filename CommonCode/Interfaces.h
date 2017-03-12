#pragma once

#include <memory>

struct file_exception_proto {
    file_exception_proto(const std::string& path) : m_path(path) {}
    file_exception_proto(const std::wstring& path) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
        m_path = utf8_conv.to_bytes(path);
    }

protected:
    std::string m_path;
};

struct cannot_read_file_exception : file_exception_proto, std::exception {
    virtual const char* what() const override { return m_path.c_str(); }
    cannot_read_file_exception(const std::wstring& path) : file_exception_proto(path) {}
    cannot_read_file_exception(const std::string& path) : file_exception_proto(path) {}
};

struct cannot_save_file_exception : file_exception_proto, std::exception {
    virtual const char* what() const override { return m_path.c_str(); }
    cannot_save_file_exception(const std::wstring& path) : file_exception_proto(path) {}
    cannot_save_file_exception(const std::string& path) : file_exception_proto(path) {}
};

struct cannot_find_folder_exception : file_exception_proto, std::exception {
    virtual const char* what() const override { return m_path.c_str(); }
    cannot_find_folder_exception(const std::wstring& path) : file_exception_proto(path) {}
    cannot_find_folder_exception(const std::string& path) : file_exception_proto(path) {}
};

struct data_too_long_exception : std::exception {
    virtual const char* what() const override { return "Data block is too large to send/receive"; }
};

class ITerminator
{
public:
    virtual void Terminate() = 0;
    virtual bool IsTerminated() = 0;

    ITerminator() {}
    virtual ~ITerminator() {}
};

class IExchangeClient
{
public:

    virtual bool Send(const char* data, size_t len) = 0;
    virtual size_t GetBufSize() const = 0;

    IExchangeClient() {}
    virtual ~IExchangeClient() {}
};

class IExchangeServer
{
public:

    virtual bool Receive(char* data, size_t len) = 0;
    virtual size_t GetBufSize() const = 0;

    IExchangeServer() {}
    virtual ~IExchangeServer() {}
};

class IFileExchangeServer
{
public:

    virtual void ReceiveFiles() = 0;

    IFileExchangeServer() {}
    virtual ~IFileExchangeServer() {}
};

class IFileExchangeClient
{
public:

    virtual bool SendFile(const std::wstring& path) = 0;

    IFileExchangeClient() {}
    virtual ~IFileExchangeClient() {}
};

class IFileSaver
{
public:
    virtual void OpenFile(std::wstring filename, std::streamsize size, unsigned long from_proc) = 0;
    virtual void SaveData(unsigned long from_proc, const char* data, size_t size) = 0;

    IFileSaver() {}
    virtual ~IFileSaver() {}
};

std::unique_ptr<IFileExchangeServer> FileExch_CreateServer(std::unique_ptr<IExchangeServer> driver, std::unique_ptr<IFileSaver> saver);
std::unique_ptr<IFileExchangeClient> FileExch_CreateClient(std::unique_ptr<IExchangeClient> driver);

std::unique_ptr<IExchangeServer> ShrdMemExch_CreateServer(ITerminator* terminator);
std::unique_ptr<IExchangeClient> ShrdMemExch_CreateClient();

std::unique_ptr<IFileSaver> FileSaver_create(std::wstring folder);
