#pragma once

#include <boost/core/noncopyable.hpp>

#include "Interfaces.h"

typedef enum { FILEINFO, FILEDATA } MessageType_t;

typedef struct {
    unsigned long proc_id;
    MessageType_t msg_type;
    std::streampos size; // contain file size or amount of data in buffer
} MsgHeader_t;

typedef struct {
    MsgHeader_t header;
    char buf[1];
} Message_t;

class CFileExchangeServer :
    public IFileExchangeServer,
    private boost::noncopyable
{
public:
    CFileExchangeServer(std::unique_ptr<IExchangeServer> driver,
                        std::unique_ptr<IFileSaver> saver) : m_driver(std::move(driver)), m_saver(std::move(saver)) {}
    virtual ~CFileExchangeServer() {};

    static std::unique_ptr<IFileExchangeServer> Create(std::unique_ptr<IExchangeServer> driver, std::unique_ptr<IFileSaver> saver);

    virtual void ReceiveFiles();

private:
    std::unique_ptr<IExchangeServer> m_driver;
    std::unique_ptr<IFileSaver> m_saver;
};

class CFileExchangeClient :
    public IFileExchangeClient,
    private boost::noncopyable
{
public:
    CFileExchangeClient(std::unique_ptr<IExchangeClient> driver) : m_driver(std::move(driver)) {};
    virtual ~CFileExchangeClient() {};

    static std::unique_ptr<IFileExchangeClient> Create(std::unique_ptr<IExchangeClient> driver);

    virtual bool SendFile(const std::wstring& path);

private:
    std::unique_ptr<IExchangeClient> m_driver;
};
