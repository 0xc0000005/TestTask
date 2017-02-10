#include "stdafx.h"

#include <boost/filesystem.hpp>
#include <boost/log/attributes/current_process_id.hpp>
using boost::log::attributes::current_process_id;

#include "FileExchange.h"

std::unique_ptr<IFileExchangeServer> FileExch_CreateServer(std::unique_ptr<IExchangeServer> server, std::unique_ptr<IFileSaver> saver)
{
    return CFileExchangeServer::Create(std::move(server), std::move(saver));
}

std::unique_ptr<IFileExchangeClient> FileExch_CreateClient(std::unique_ptr<IExchangeClient> client)
{
    return CFileExchangeClient::Create(std::move(client));
}

std::unique_ptr<IFileExchangeServer> CFileExchangeServer::Create(std::unique_ptr<IExchangeServer> driver, std::unique_ptr<IFileSaver> saver)
{
    auto server = std::make_unique<CFileExchangeServer>(std::move(driver), std::move(saver));
    return std::unique_ptr<IFileExchangeServer>(std::move(server));
}

void CFileExchangeServer::ReceiveFiles()
{
    auto buf_size = m_driver->GetBufSize();
    auto buf = std::make_unique<char[]>(buf_size);
    auto msg_ptr = reinterpret_cast<Message_t*>(&buf[0]);

    while (m_driver->Receive(buf.get(), buf_size)) {
        
        unsigned long prod_id = msg_ptr->header.proc_id;
        switch (msg_ptr->header.msg_type) {
        case FILEINFO:
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
            auto filename = utf8_conv.from_bytes(msg_ptr->buf);
            auto file_size = msg_ptr->header.size;
            auto proc_id = msg_ptr->header.proc_id;
            m_saver->OpenFile(filename, file_size, proc_id);
            break;
        }
        case FILEDATA:
        {
            auto proc_id = msg_ptr->header.proc_id;
            auto data_size = msg_ptr->header.size;
            m_saver->SaveData(proc_id, msg_ptr->buf, static_cast<size_t>(data_size));
            break;
        }
        default:
            break; // drop unknown
        }
    }
}

std::unique_ptr<IFileExchangeClient> CFileExchangeClient::Create(std::unique_ptr<IExchangeClient> driver)
{
    auto client = std::make_unique<CFileExchangeClient>(std::move(driver));
    return std::unique_ptr<IFileExchangeClient>(std::move(client));
}

bool CFileExchangeClient::SendFile(const std::wstring & path)
{
    // file opened at the end to calculate file size
    std::ifstream stream(path, std::ifstream::ate | std::ifstream::binary);
    if (!stream.is_open())
        throw cannot_read_file_exception(path);

    std::streampos filesize = stream.tellg();
    stream.seekg(0); // seek file to the begining

    boost::filesystem::path filename(path);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    std::string path_utf8 = utf8_conv.to_bytes(filename.filename().wstring());

    const size_t buf_size = m_driver->GetBufSize();
    const size_t data_size = buf_size - sizeof(Message_t);

    if (path_utf8.length() + 1 > data_size)
        throw data_too_long_exception();

    auto buf = std::make_unique<char[]>(buf_size);

    auto msg_ptr = reinterpret_cast<Message_t*>(&buf[0]);
    msg_ptr->header.proc_id = current_process_id().get().native_id();
    msg_ptr->header.msg_type = FILEINFO;
    msg_ptr->header.size = filesize;
    memcpy(msg_ptr->buf, path_utf8.c_str(), path_utf8.length() + 1);

    m_driver->Send(buf.get(), buf_size);

    std::streampos data_get = 0;
    do {
        msg_ptr->header.msg_type = FILEDATA;
        stream.read(msg_ptr->buf, data_size);
        data_get = stream.gcount();
        if (data_get > 0) {
            msg_ptr->header.size = data_get;
            if (!m_driver->Send(buf.get(), buf_size)) {
                return false;
            }
        }
    } while (data_get >= data_size); // last time we read and sent less than buffer allows
    
    return true;
}
