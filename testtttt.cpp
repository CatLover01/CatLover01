#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <stop_token>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

void ErrorExit(LPSTR);
void KeyEventProc(KEY_EVENT_RECORD ker);

// Global variables are here for example, avoid that.
DWORD fdwSaveOldMode;
HANDLE hStdin;

void inputFunc(std::mutex& mut, std::condition_variable& cv, std::string& input, std::stop_token stop_token, DWORD bufferSize, DWORD fdwMode, DWORD cNumRead, INPUT_RECORD irInBuf) {
    std::cout << "Enter something: ";
    while (!stop_token.stop_requested()) {
        if (bufferSize > 0) {
            if (!ReadConsoleInput(
                hStdin,      // input buffer handle
                irInBuf,     // buffer to read into
                128,         // size of read buffer
                &cNumRead)) // number of records read
                ErrorExit("ReadConsoleInput");

            // This code is not rock solid, you should iterate over
            // irInBuf to get what you want, the last event may not contain what you expect
            // Once again you'll find an event constant list on Microsoft documentation
            if (irInBuf[cNumRead - 1].EventType == KEY_EVENT) {
                KeyEventProc(irInBuf[cNumRead - 1].Event.KeyEvent);
                Sleep(2000);
            }
        }
    }
    //std::string userInput;
    //std::cin >> userInput;

    std::lock_guard<std::mutex> lock(mut);
    //input = std::move(userInput);
    cv.notify_one();
}

void timeFunc(std::mutex& mut, std::condition_variable& cv, std::string& str, bool& bCheck) {
    std::unique_lock<std::mutex> lock(mut);
    auto now = std::chrono::system_clock();
    if (cv.wait_for(lock, std::chrono::seconds(5), [&str]() {return !str.empty(); })) {
       std::cout << "You entered " << str << "!\n";
    }
    else {
       std::cout << "You didn't enter something..." << '\n';
    }
    bCheck = true;
}

int main() {


    printf("\033[H\033[J");
    int i = 0;
    char* s = "*";

    DWORD fdwMode, cNumRead;
    INPUT_RECORD irInBuf[128];
    DWORD bufferSize = 0;

    //var declaration
    std::mutex mut;
    std::condition_variable cv;
    std::string input;
    bool bCheck{ false };

    //jthreads
    std::stop_source source1;
    std::jthread evaluateTimeThread(timeFunc, std::ref(mut), std::ref(cv), std::ref(input), std::ref(bCheck));
    std::jthread inputThread(inputFunc, std::ref(mut), std::ref(cv), std::ref(input), std::ref(bufferSize), std::ref(fdwMode), std::ref(cNumRead), std::ref(irInBuf));
    evaluateTimeThread.join();
    


    //alway going to stop but we could do other things in here...
    if (!bCheck) {
        std::cout << "ettttt";
        inputThread.join();
    } 
    else {
        //stoping here it is impossible...   
        //Problem is: when the computer is waiting for input, the thread is working and can't exit in anyways we can't even use source/token function to stop the thread...
        inputThread.request_stop();
    }

    return 0;
}

void ErrorExit(LPSTR lpszMessage)
{
    fprintf(stderr, "%s\n", lpszMessage);

    // Restore input mode on exit.

    SetConsoleMode(hStdin, fdwSaveOldMode);

    ExitProcess(0);
}

void KeyEventProc(KEY_EVENT_RECORD ker)
{
    printf("Key event: \"%c\" ", ker.uChar.AsciiChar);

    if (ker.bKeyDown)
        printf("key pressed\n");
    else printf("key released\n");
}
