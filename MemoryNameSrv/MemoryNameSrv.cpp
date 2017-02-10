// MemoryNameSrv.cpp : Implementation of WinMain

#include "stdafx.h"
#include "resource.h"
#include "MemoryNameSrv_i.h"
#include "xdlldata.h"

using namespace ATL;

class CMemoryNameSrvModule : public ATL::CAtlExeModuleT< CMemoryNameSrvModule >
{
public:
    DECLARE_LIBID(LIBID_MemoryNameSrvLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MEMORYNAMESRV, "{CBA074DB-8A09-4B3D-9E07-8946C894229F}")
};

CMemoryNameSrvModule _AtlModule;

//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
    LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}
