#pragma once

#include <Objbase.h>

#include <boost/utility.hpp>
#include <boost/exception/all.hpp>

struct ole_error_exception :
    std::exception
{
    virtual const char* what() const override { return m_msg.c_str(); }
    ole_error_exception(HRESULT hr);

private:
    std::string m_msg;
};

struct INameServer;

class CNameSrvClient :
    private boost::noncopyable
{
public:
    CNameSrvClient();
    ~CNameSrvClient();
    std::wstring GetName();
    std::string GetNameUtf8();

private:
    INameServer* m_name_server;
};
