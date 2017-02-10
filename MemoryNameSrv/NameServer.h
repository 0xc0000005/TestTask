// NameServer.h : Declaration of the CNameServer

#pragma once
#include "resource.h"       // main symbols

#include "MemoryNameSrv_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

// CNameServer

class ATL_NO_VTABLE CNameServer :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CNameServer, &CLSID_NameServer>,
    public IDispatchImpl<INameServer, &IID_INameServer, &LIBID_MemoryNameSrvLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    CNameServer() {};

    DECLARE_REGISTRY_RESOURCEID(IDR_NAMESERVER)

    BEGIN_COM_MAP(CNameServer)
        COM_INTERFACE_ENTRY(INameServer)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

public:

    STDMETHOD(GetSharedMemoryName)(BSTR* pRet);

private:
    static std::wstring GenerateRandomName(const int length);
    static const std::wstring m_shared_memory_name;
};

OBJECT_ENTRY_AUTO(__uuidof(NameServer), CNameServer)
