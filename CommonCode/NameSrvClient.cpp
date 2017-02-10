#include "stdafx.h"

#include "../MemoryNameSrv/MemoryNameSrv_i.h"
#include "../MemoryNameSrv/MemoryNameSrv_i.c"

#include "NameSrvClient.h"

ole_error_exception::ole_error_exception(HRESULT hr)
{
    // Could use Win API FormatMessage() call to convert HRESULT code to string message
    m_msg = "OLE communication failed with error : " + std::to_string(hr);
}

CNameSrvClient::CNameSrvClient() : m_name_server(nullptr)
{
    HRESULT hr;
    hr = ::CoInitialize(NULL);
    if (FAILED(hr))
        throw ole_error_exception(hr);

    hr = ::CoCreateInstance(CLSID_NameServer, NULL, CLSCTX_LOCAL_SERVER, IID_INameServer, (LPVOID*)&m_name_server);
    if (FAILED(hr)) {
        ::CoUninitialize();
        throw ole_error_exception(hr);
    }
}

CNameSrvClient::~CNameSrvClient()
{
    m_name_server->Release();
    ::CoUninitialize();
}

std::wstring CNameSrvClient::GetName()
{
    std::wstring name;

    BSTR bstr_name;
    HRESULT hr = m_name_server->GetSharedMemoryName(&bstr_name);
    if (FAILED(hr))
        throw ole_error_exception(hr);

    name = bstr_name;
    SysFreeString(bstr_name);

    return name;
}

std::string CNameSrvClient::GetNameUtf8()
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.to_bytes(GetName().c_str());
}
