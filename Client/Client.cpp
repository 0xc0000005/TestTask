// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/filesystem/path.hpp>
#include <boost/interprocess/exceptions.hpp>
using boost::interprocess::interprocess_exception;

#include "../CommonCode/Interfaces.h"

int wmain(int argc, wchar_t** argv)
{
    if (argc != 2) {
        boost::filesystem::path exe(argv[0]);
        std::wcout << L"Usage: " << exe.filename() << L" <file to transfer>" << std::endl;
        return 0;
    }

    std::wstring filename(argv[1]);

    std::wcerr << L"Sending file " << filename << L"...";

    try {
        auto driver = ShrdMemExch_CreateClient();
        auto client = FileExch_CreateClient(std::move(driver));
        if (!client->SendFile(filename))
            std::cerr << std::endl << "Server was terminated" << std::endl;
        else
            std::cerr << " done." << std::endl;
    }
    catch (interprocess_exception& e) {
        std::cout << std::endl << "Interprocess error: " << e.what() << std::endl;

    }
    catch (cannot_read_file_exception& e) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
        auto filename = utf8_conv.from_bytes(e.what());
        std::wcout << std::endl << "Cannot read data from file: " << filename << std::endl;
    }
    catch (std::exception& e) {
        std::cout << std::endl << e.what() << std::endl;
    }

    return 0;
}
