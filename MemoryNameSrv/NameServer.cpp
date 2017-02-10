// NameServer.cpp : Implementation of CNameServer

#include "stdafx.h"
#include "NameServer.h"

const int name_length = 10;
const std::wstring CNameServer::m_shared_memory_name = CNameServer::GenerateRandomName(name_length);

std::wstring CNameServer::GenerateRandomName(const int length)
{
    const std::wstring allowed_chars = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    //srand(::GetCurrentProcessId());
    srand(static_cast<unsigned int>(time(NULL)));
    std::wstring name;
    name.reserve(length);
    for (int i = 0; i < length; ++i)
        name.append(1, allowed_chars.at(rand() % allowed_chars.length()));
    return name;
}

STDMETHODIMP CNameServer::GetSharedMemoryName(BSTR* pRet)
{
    if (!pRet)
        return E_POINTER;
    *pRet = ::SysAllocString(m_shared_memory_name.c_str());
    return *pRet != NULL ? S_OK : E_OUTOFMEMORY;
}
