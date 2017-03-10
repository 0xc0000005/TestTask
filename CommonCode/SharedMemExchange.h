#pragma once

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

#include "NameSrvClient.h"
#include "Interfaces.h"

typedef struct {
    enum { buf_size = 0xFFFF }; // use static size for now
    boost::interprocess::interprocess_mutex mutex;
    boost::interprocess::interprocess_condition data_ready;
    boost::interprocess::interprocess_condition server_ready;
    bool server_stopped = false;
    char buf[buf_size];
} SharedMemoryBuf_t;

typedef struct {
    boost::interprocess::windows_shared_memory m_shmem;
    boost::interprocess::mapped_region m_region;
} SharedMem_t;

class CSharedMemExchangeServer :
    public IExchangeServer,
    private SharedMem_t,
    private boost::noncopyable
{
public:
    virtual ~CSharedMemExchangeServer() {}

    static std::unique_ptr<IExchangeServer> Create(ITerminator* terminator);

    virtual bool Receive(char* data, size_t len) override;
    virtual size_t GetBufSize() const override { return SharedMemoryBuf_t::buf_size; }

private:
    bool isTerminated() {
        return m_terminator && m_terminator->IsTerminated();
    }

    SharedMemoryBuf_t* m_buf = nullptr;
    std::unique_ptr<CNameSrvClient> m_name_server;
    ITerminator* m_terminator = nullptr;
};

class CSharedMemExchangeClient :
    public IExchangeClient,
    private SharedMem_t,
    private boost::noncopyable
{
public:
    virtual ~CSharedMemExchangeClient() {}

    static std::unique_ptr<IExchangeClient> Create();

    virtual bool Send(const char* data, size_t len) override;
    virtual size_t GetBufSize() const override { return SharedMemoryBuf_t::buf_size; }

private:
    SharedMemoryBuf_t* m_buf = nullptr;
};
