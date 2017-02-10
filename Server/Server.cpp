// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <thread>

#include <boost/interprocess/exceptions.hpp>
using boost::interprocess::interprocess_exception;

#include "../CommonCode/Interfaces.h"
#include "../CommonCode/Terminator.h"

void ServerThread(ITerminator* terminator)
{
    try {
        auto driver = ShrdMemExch_CreateServer(terminator);
        auto file_saver = FileSaver_create(L"./input");
        auto server = FileExch_CreateServer(std::move(driver), std::move(file_saver));

        server->ReceiveFiles();
    }
    catch (interprocess_exception& e) {
        std::cout << std::endl << "Interprocess error: " << e.what() << std::endl;

    }
    catch (cannot_save_file_exception& e) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
        auto filename = utf8_conv.from_bytes(e.what());
        std::wcout << std::endl << L"Cannot save data to file: " << filename << std::endl;
    }
    catch (cannot_find_folder_exception& e) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
        auto folder = utf8_conv.from_bytes(e.what());
        std::wcout << std::endl << L"Cannot find folder: " << folder << std::endl;
    }
    catch (std::exception& e) {
        std::cout << std::endl << e.what() << std::endl;
    }
}

int main()
{
    CTerminator terminator;
    std::thread srv_thread(ServerThread, &terminator);

    std::cout << "Press [Enter] to stop server..." << std::endl;
    std::cin.get();

    terminator.Terminate();

    srv_thread.join();

    return 0;
}
