//#include <windows.h>
//#include <iostream>
//#include <string>
//#include <tlhelp32.h>
//#include <psapi.h>
//#include <thread>
//
//using namespace std;
//
//// Знаходження PID за іменем процесу
//int GetProcess_ID(string procname)
//{
//    HANDLE hSnapshot = NULL;
//    PROCESSENTRY32 pe;
//    int pid = 0;
//    BOOL hResult;
//
//    // Знімок усіх процесів у системі
//    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//    if (INVALID_HANDLE_VALUE == hSnapshot) {
//        return 0;
//    }
//    // Ініціалізація розміру: потрібно для використання Process32First
//    pe.dwSize = sizeof(PROCESSENTRY32);
//    // Інформація про перший знайдений процес у системному знімку
//    hResult = Process32First(hSnapshot, &pe);
//    // Отримання інформації про процеси та виходження у разі невдачі
//    while (hResult) {
//        // Якщо ми знайшли процес: повертаємо PID
//        if (procname == pe.szExeFile) {
//            pid = pe.th32ProcessID;
//            break;
//        }
//        hResult = Process32Next(hSnapshot, &pe);
//    }
//    // Закриваємо знімок
//    CloseHandle(hSnapshot);
//
//    return pid;
//}
//
//void killProcessByName(const string& filename)
//{
//    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
//    PROCESSENTRY32 pEntry;
//    pEntry.dwSize = sizeof(pEntry);
//    BOOL hRes = Process32First(hSnapShot, &pEntry);
//
//    while (hRes)
//    {
//        if (filename == pEntry.szExeFile)
//        {
//            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, pEntry.th32ProcessID);
//            if (hProcess != NULL)
//            {
//                TerminateProcess(hProcess, 9);
//                CloseHandle(hProcess);
//            }
//        }
//        hRes = Process32Next(hSnapShot, &pEntry);
//    }
//    CloseHandle(hSnapShot);
//}
//
//SIZE_T GetProcessMemoryUsage(int processID)
//{
//    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
//    if (hProcess == NULL) {
//        return 0;
//    }
//
//    PROCESS_MEMORY_COUNTERS_EX pmc;
//    if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
//        return pmc.WorkingSetSize;
//    }
//
//    CloseHandle(hProcess);
//    return 0;
//}
//
//int main() {
//    SetConsoleOutputCP(CP_UTF8);
//    string process_name;
//    int process_PID = 0;
//
//    int number = 0;
//
//    cout << "Ласкаво просимо у програму контролю процесів на вашому комп'ютері." << endl;
//    cout << "1 - Почати роботу." << endl;
//    cout << "2 - Завершити роботу." << endl;
//
//    cin >> number;
//
//    if (number == 1)
//    {
//        cout << "Введіть назву процесу: ";
//        cin >> process_name;
//
//        process_PID = GetProcess_ID(process_name);
//
//        if (process_PID)
//        {
//            cout << "PID: " << process_PID << endl;
//
//            long long limit = 0;
//            SIZE_T memoryUsage = GetProcessMemoryUsage(process_PID);
//
//            cout << "Введіть ліміт (байт) використання програмою оперативної пам'яті" << endl;
//            cin >> limit;
//
//            while(true)
//            {
//                if (memoryUsage > static_cast<SIZE_T>(limit))
//                {
//                    cout << "Використання пам'яті процесом перевищує ліміт. Завершення процесу..." << endl;
//                    killProcessByName(process_name);
//                    break;
//                }
//                else
//                {
//                    cout << "Використання пам'яті процесом у межах ліміту." << endl;
//                }
//                std::this_thread::sleep_for(std::chrono::seconds(3));
//            }
//        }
//        else
//        {
//            cout << "Процес з іменем \"" << process_name << "\" не знайдено." << endl;
//        }
//    }
//    else if (number == 2)
//    {
//        return 0;
//    }
//    else
//    {
//        cout << "Ви ввели неправильну цифру" << endl;
//    }
//
//    return 0;
//}



#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#else
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fstream>
#endif

using namespace std;

#if defined(_WIN32) || defined(_WIN64)
int GetProcessPID(string procname)
{
    HANDLE hSnapshot = NULL;
    PROCESSENTRY32 pe;
    int pid = 0;
    BOOL hResult;

    // Знімок усіх процесів у системі
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) {
        return 0;
    }
    // Ініціалізація розміру: потрібно для використання Process32First
    pe.dwSize = sizeof(PROCESSENTRY32);
    // Інформація про перший знайдений процес у системному знімку
    hResult = Process32First(hSnapshot, &pe);
    // Отримання інформації про процеси та виходження у разі невдачі
    while (hResult) {
        // Якщо ми знайшли процес: повертаємо PID
        if (procname == pe.szExeFile) {
            pid = pe.th32ProcessID;
            break;
        }
        hResult = Process32Next(hSnapshot, &pe);
    }
    // Закриваємо знімок
    CloseHandle(hSnapshot);

    return pid;
}

void killProcessByName(const string& filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);

    while (hRes)
    {
        if (filename == pEntry.szExeFile)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}

size_t GetProcessMemoryUsage(int processID)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (hProcess == NULL) {
        return 0;
    }

    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }

    CloseHandle(hProcess);
    return 0;
}

void SetProcessPriority(int processID, int priority)
{
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, processID);
    if (hProcess != NULL)
    {
        SetPriorityClass(hProcess, priority);
        CloseHandle(hProcess);
    }
}
#else
int GetProcessPID(const std::string& processName)
{
    string command = "pgrep " + processName;  //find process by name
    ifstream pipe(command.c_str()); //stream to write command

    if (!pipe.is_open())
        return 0;

    int processID = 0;
    pipe >> processID;  //read data from stream
    pipe.close();

    return processID;
}

void killProcessByName(const std::string& fileName)
{
    std::string command = "pkill -f " + fileName; // Використовуємо `pkill -f` для знаходження та завершення процесу за назвою файлу
    system(command.c_str());
}

size_t GetProcessMemoryUsage(int processID)
{
    string command = "pmap " + to_string(processID) + " | grep total"; //pmap - information for usage memory, grep - find string total
    ifstream pipe(command.c_str());

    if (!pipe.is_open())
        return 0;


    string line;
    getline(pipe, line);
    pipe.close();

    size_t memoryUsage = 0;
    size_t pos = line.find("total");

    if (pos != std::string::npos) //npos - pos is not find
    {
        pos = line.find_first_of("0123456789", pos);

        if (pos != std::string::npos)
        {
            size_t endPos = line.find_first_not_of("0123456789", pos);

            if (endPos != std::string::npos)
            {
                std::string memoryValue = line.substr(pos, endPos - pos);
                memoryUsage = std::stoull(memoryValue);
            }
        }
    }

    return memoryUsage;
}

void SetProcessPriority(int processID, int priority)
{
    // Змінювати пріоритет процесів у Linux можна, наприклад, за допомогою "nice" або "renice" команд.
    // Наприклад, "nice -n 10 -p <PID>" збільшить пріоритет процесу з PID на 10 одиниць.
    string command = "renice " + to_string(priority) + " -p " + to_string(processID);
    system(command.c_str());
}
#endif

int main() {
    string processName;
    int processPID = 0;

    cout << "Welcome to the process control program on your computer." << endl;
    cout << "1 - Start" << endl;
    cout << "2 - Exit." << endl;

    int number;
    cin >> number;

    if (number == 1) {
        cout << "Enter the process name: ";
        cin >> processName;

        processPID = GetProcessPID(processName);

        if (processPID)
        {
            cout << "PID: " << processPID << endl;

            size_t limit = 0;
            size_t memoryUsage = GetProcessMemoryUsage(processPID);

            cout << "Enter the memory usage limit (bytes):" << endl;
            cin >> limit;

            int priority = 0;
            cout << "Enter the new priority for the process:" << endl;
            cin >> priority;

            int num = 0;
            cout << "Enter 10, start monitoring." << endl;
            cin >> num;

            while (true)
            {
                cout << "Enter 11, change priority of process.";
                if(num == 10)
                {
                    if (memoryUsage > limit)
                    {
                        cout << "Process memory usage exceeds the limit. Terminating the process..." << endl;
                        killProcessByName(processName);
                        break;
                    }
                    else
                    {
                        cout << "Process memory usage within the limit." << endl;
                    }
                }
                else if(num == 11)
                {
                    SetProcessPriority(processPID, priority);
                }
                else
                {
                    cout << "You wrote inncorrect number !";
                }


                this_thread::sleep_for(chrono::seconds(3));
                memoryUsage = GetProcessMemoryUsage(processPID);
            }
        }
        else
        {
            cout << "Process with name \"" << processName << "\" not found." << endl;
        }
    }
    else if (number == 2)
    {
        return 0;
    }
    else
    {
        cout << "You entered an incorrect number." << endl;
    }

    return 0;
}
