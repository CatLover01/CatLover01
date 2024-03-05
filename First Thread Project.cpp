#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

void inputFunc(std::mutex& mut, std::condition_variable& cv, std::string& input) {
    std::cout << "Enter something: ";
    std::string userInput;
    std::cin >> userInput;

    std::lock_guard<std::mutex> lock(mut);
    input = std::move(userInput);
    cv.notify_one();
}

void timeFunc(std::mutex& mut, std::condition_variable& cv, std::string& str, bool& bCheck) {
    std::unique_lock<std::mutex> lock(mut);
    auto now = std::chrono::system_clock();
    if (cv.wait_for(lock, std::chrono::seconds(5), [&str]() {return !str.empty(); })) {
       std::cout << "You entered " << str << "!\n";
    }
    else {
       std::cout << "\nYou didn't enter something..." << '\n';
    }
    bCheck = true;
}

int main() {
    //var declaration
    std::mutex mut;
    std::condition_variable cv;
    std::string input;
    bool bCheck{ false };

    //jthreads
    std::stop_source source1;
    std::jthread evaluateTimeThread(timeFunc, std::ref(mut), std::ref(cv), std::ref(input), std::ref(bCheck));
    std::jthread inputThread(inputFunc, std::ref(mut), std::ref(cv), std::ref(input));
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

    //only work in this case but shouldn't be used normally
    inputThread.detach();
    
    return 0;
}
