#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <queue>
#include <list>

#include <sstream>
#include <fstream>

using namespace std;

struct Process
{
    int pid;
    string name;
    int priority;
    int num_codes;
    bool is_sleeping;
    int sleep_cycle;
    int current_code;
    queue<vector<int>> codes;
};

class Scheduler
{
public:
    bool END = false;

    string INPUT_PATH;
    int TOTAL_EVENT_NUM;
    int VM_SIZE;
    int PM_SIZE;
    int PAGE_FRAME_SIZE;

    int pid_counter = 0;
    int cycle = 0;

    Process scheduled_process;
    Process running_process;

    queue<vector<string>> input_processes;
    vector<list<Process>> run_queue;
    // queue<Process> q1, q2, q3, q4, q5, q6, q7, q8, q9;
    list<Process> sleep_list, ioWait_list;

    void getInput(string);
    vector<string> split(string);
    vector<int> split_int(string);

    void checkSleepTermination();
    void executeIO(int);
    void createProcess(string, int);
    void decideProcess();
    void executeProcessCode(Process);

    void printSchedulerInfo(Process);
    void printMemoryInfo(Process);

    void runCycle(int);
    void start();
};

vector<string> Scheduler::split(string input)
{
    vector<string> res;
    stringstream ss(input);
    string temp;

    while (getline(ss, temp, ' '))
    {
        res.push_back(temp);
    }

    return res;
}

vector<int> Scheduler::split_int(string input)
{
    vector<int> res;
    stringstream ss(input);
    string temp;

    while (getline(ss, temp, ' '))
    {
        res.push_back(stoi(temp));
    }

    return res;
}

void Scheduler::getInput(string path)
{
    INPUT_PATH = path;
    string file_input = path == "" ? "input" : path + "/input";

    // C-style input--------
    // FILE *input = fopen(file_input.c_str(), "r");
    // FILE *scheduler_file = fopen(file_scheduler.c_str(), "w");

    // char *input_settings = NULL;
    // size_t len = 0;
    // getline(&input_settings, &len, input);

    ifstream input_file(file_input);
    string input_settings;
    vector<string> settings;

    getline(input_file, input_settings);

    settings = split(input_settings);

    TOTAL_EVENT_NUM = stoi(settings[0]);
    VM_SIZE = stoi(settings[1]);
    PM_SIZE = stoi(settings[2]);
    PAGE_FRAME_SIZE = stoi(settings[3]);

    for (int i = 0; i < TOTAL_EVENT_NUM; i++)
    {
        getline(input_file, input_settings);
        input_processes.push(split(input_settings));
    }
}

void Scheduler::checkSleepTermination()
{
    for (Process p : sleep_list)
    {
        if (p.is_sleeping && p.sleep_cycle == 0)
        {
            int num_q = p.priority;
            p.is_sleeping = false;

            Process temp = p;
            sleep_list.remove(p);
            run_queue[num_q].push_back(temp);
        }
        else if (p.is_sleeping && p.sleep_cycle > 0)
        {
            p.sleep_cycle--;
        }
    }
}

void Scheduler::executeIO(int pid)
{
    for (Process p : ioWait_list)
    {
        if (pid == p.pid)
        {
            int num_q = p.priority;
            Process temp = p;
            ioWait_list.remove(p);
            run_queue[num_q].push_back(temp);
        }
    }
}

void Scheduler::createProcess(string name, int priority)
{
    Process process;

    process.pid = pid_counter;
    pid_counter++;

    process.name = name;
    process.priority = priority;
    process.is_sleeping = false;
    process.current_code = 0;

    string code_input = INPUT_PATH + "/" + process.name;
    ifstream input_file(code_input);
    string line;

    getline(input_file, line);
    process.num_codes = stoi(line);

    for (int i = 0; i < process.num_codes; i++)
    {
        getline(input_file, line);
        process.codes.push(split_int(line));
    }

    run_queue[priority].push_back(process);
}

void Scheduler::decideProcess()
{
}

void Scheduler::printSchedulerInfo(Process p)
{

    string file_scheduler = INPUT_PATH == "" ? "scheduler.txt" : INPUT_PATH + "/scheduler.txt";
    FILE *scheduler_txt = fopen(file_scheduler.c_str(), "w");

    int cycle = 0;
    while (true)
    {
        // Line 1
        fprintf(scheduler_txt, "[%d Cycle] Scheduled Process: ", cycle);
        if (Scheduled Process Exists)
            fprintf(scheduler_txt, "%d %s (priority %d)\n", pid, name, priority);
        else
            fprintf(scheduler_txt, "None\n");

        // Line 2
        fprintf(scheduler_txt, "Running Process: ");
        if (Running Process Exists)
            fprintf(scheduler_txt, "Process%d(%d) running code %s line %d(op %d arg %d)\n", pid, priority, name, line, op, arg);
        else
            fprintf(scheduler_txt, "None\n");

        // Line 3
        for (int i = 0; i < run_queue.size(); i++)
        {
            fprintf(scheduler_txt, "RunQueue %d: ", i);
            if (run_queue[i].empty())
                fprintf(scheduler_txt, "Empty");
            else
                for (Process p : run_queue[i])
                {
                    fprintf(scheduler_txt, "%d(%s) ", p.pid, p.name.c_str());
                }

            fprintf(scheduler_txt, "\n");
        }

        // Line 4
        fprintf(scheduler_txt, "SleepList: ");
        if (sleep_list.empty())
            fprintf(scheduler_txt, "Empty");
        else
            for (Process p : sleep_list)
            {
                fprintf(scheduler_txt, "%d(%s) ", p.pid, p.name.c_str());
            }

        fprintf(scheduler_txt, "\n");

        // Line 5
        fprintf(scheduler_txt, "IOWait List: ");
        if (ioWait_list.empty())
            fprintf(scheduler_txt, "Empty");
        else
            for (Process p : ioWait_list)
            {
                fprintf(scheduler_txt, "%d(%s) ", p.pid, p.name);
            }
        fprintf(scheduler_txt, "\n");

        // for the next cycle
        fprintf(scheduler_txt, "\n");
    }
}

void Scheduler::runCycle(int current_cycle)
{
    checkSleepTermination();

    bool input_in_current_cycle = true;
    while (input_in_current_cycle)
    {
        vector<string> input = input_processes.front();
        int input_arrival_time = stoi(input[0]);
        if (current_cycle == input_arrival_time)
        {
            input_processes.pop();
            char c = input[1][0];
            c == 'I' ? executeIO(stoi(input[2])) : createProcess(input[1], stoi(input[2]));
        }
        else
        {
            input_in_current_cycle = false;
        }
    }

    decideProcess();
    executeProcessCode(running_process);
    printSchedulerInfo(running_process);
    printMemoryInfo(running_process);

    running_process.current_code++;
}

void Scheduler::start()
{
    while (!END)
    {
        runCycle(cycle);
        cycle++;
        sleep(3);
    }
}