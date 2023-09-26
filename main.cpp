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

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) {
        return 0;
    }

    pe.dwSize = sizeof(PROCESSENTRY32);
    hResult = Process32First(hSnapshot, &pe);

    while (hResult) {
        if (procname == pe.szExeFile) {
            pid = pe.th32ProcessID;
            break;
        }
        hResult = Process32Next(hSnapshot, &pe);
    }

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
    string command = "pgrep " + processName;
    ifstream pipe(command.c_str());

    if (!pipe.is_open())
        return 0;

    int processID = 0;
    pipe >> processID;
    pipe.close();

    return processID;
}

void killProcessByName(const std::string& fileName)
{
    std::string command = "pkill -f " + fileName;
    system(command.c_str());
}

size_t GetProcessMemoryUsage(int processID)
{
    string command = "pmap " + to_string(processID) + " | grep total";
    ifstream pipe(command.c_str());

    if (!pipe.is_open())
        return 0;

    string line;
    getline(pipe, line);
    pipe.close();

    size_t memoryUsage = 0;
    size_t pos = line.find("total");

    if (pos != std::string::npos)
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
