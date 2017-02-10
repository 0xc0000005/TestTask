#include "stdafx.h"
#include "SharedMemExchange.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/chrono.hpp>


using namespace boost::interprocess;
using namespace boost::posix_time;

std::unique_ptr<IExchangeServer> ShrdMemExch_CreateServer(ITerminator* terminator = nullptr)
{
    return CSharedMemExchangeServer::Create(terminator);
}

std::unique_ptr<IExchangeClient> ShrdMemExch_CreateClient()
{
    return CSharedMemExchangeClient::Create();
}

std::unique_ptr<IExchangeServer> CSharedMemExchangeServer::Create(ITerminator* terminator = nullptr)
{
    auto name_server = std::make_unique<CNameSrvClient>();
    std::string name = name_server->GetNameUtf8();
    windows_shared_memory shmem(create_only, name.c_str(), read_write, sizeof(SharedMemoryBuf_t));
    mapped_region region(shmem, read_write);
    void* dataptr = region.get_address();

    auto server = std::make_unique<CSharedMemExchangeServer>();
    server->m_buf = new (dataptr) SharedMemoryBuf_t;

    server->m_terminator = terminator;
    server->m_name_server.swap(name_server); // to keep name server running
    server->m_shmem.swap(shmem);
    server->m_region.swap(region);

    return std::unique_ptr<IExchangeServer>(std::move(server));
}

bool CSharedMemExchangeServer::Receive(char * data, size_t len)
{
    if (len > GetBufSize())
        throw data_too_long_exception();

    scoped_lock<interprocess_mutex> lock(m_buf->mutex);

    while (!isTerminated() &&
           !m_buf->data_ready.timed_wait(lock, boost::posix_time::microsec_clock::universal_time() + milliseconds(10)))
        m_buf->server_ready.notify_one();

    if (isTerminated()) {
        m_buf->server_stopped = true;
        m_buf->server_ready.notify_all();
        return false;
    }

    memcpy(data, m_buf->buf, len);
    return true;
}

std::unique_ptr<IExchangeClient> CSharedMemExchangeClient::Create()
{
    CNameSrvClient name_server;
    std::string name = name_server.GetNameUtf8();
    windows_shared_memory shmem(open_only, name.c_str(), read_write);
    mapped_region region(shmem, read_write);
    void* dataptr = region.get_address();

    auto client = std::make_unique<CSharedMemExchangeClient>();
    client->m_buf = static_cast<SharedMemoryBuf_t*>(dataptr);

    client->m_shmem.swap(shmem);
    client->m_region.swap(region);

    return std::unique_ptr<IExchangeClient>(std::move(client));
}

bool CSharedMemExchangeClient::Send(const char * data, size_t len)
{
    if (len > GetBufSize())
        throw data_too_long_exception();

    scoped_lock<interprocess_mutex> lock(m_buf->mutex);
  
    m_buf->server_ready.wait(lock);
    if (m_buf->server_stopped)
        return false;

    memcpy(m_buf->buf, data, len);

    m_buf->data_ready.notify_one();

    return true;
}
