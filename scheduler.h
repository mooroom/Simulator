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
    bool is_io_waiting;
    int current_code;
    queue<vector<int>> codes;
};

int execute_io_pid = -1;
int wake_sleep_pid = -1;

bool predicateSleepDone(Process p)
{
    return p.pid == wake_sleep_pid;
}
bool predicateIoExec(Process p)
{
    return p.pid == execute_io_pid;
}

class Scheduler
{
public:
    bool END = false;

    string INPUT_PATH;
    int TOTAL_EVENT_NUM;
    int VM_SIZE;
    int PM_SIZE;
    int PAGE_FRAME_SIZE;

    FILE *scheduler_txt;
    // FILE *memory_txt;

    int pid_counter = 0;
    int cycle = 0;

    Process temp;
    Process *scheduled_process = nullptr;
    Process *running_process = nullptr;

    queue<Process> print_scheduled_process;
    queue<Process> print_running_process;
    // Process null_process = {-1};
    // Process scheduled_process = null_process;
    // Process running_process = null_process;

    queue<vector<string>> input_processes;
    list<Process> q0, q1, q2, q3, q4, q5, q6, q7, q8, q9;
    vector<list<Process>> run_queue = {q0, q1, q2, q3, q4, q5, q6, q7, q8, q9};
    list<Process> sleep_list, ioWait_list;

    void getInput(string);
    vector<string> split(string);
    vector<int> split_int(string);

    void checkSleepTermination();
    void executeIO(int);
    void createProcess(string, int);
    void decideProcess();
    void executeProcessCode();

    void op0_mem_alloc(int);
    void op1_mem_access(int);
    void op2_mem_release(int);
    void op3_non_mem(int);
    void op4_sleep(int);
    void op5_io_wait(int);

    void printSchedulerInfo();
    void printMemoryInfo();

    void runCycle(int);
    void start();
};

vector<string> Scheduler::split(string input)
{
    vector<string> res;
    stringstream ss(input);
    string tmp;

    while (getline(ss, tmp, ' '))
    {
        res.push_back(tmp);
    }

    return res;
}

vector<int> Scheduler::split_int(string input)
{
    vector<int> res;
    stringstream ss(input);
    string tmp;

    while (getline(ss, tmp, ' '))
    {
        res.push_back(stoi(tmp));
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

    string file_scheduler = INPUT_PATH == "" ? "scheduler.txt" : INPUT_PATH + "/scheduler.txt";
    scheduler_txt = fopen(file_scheduler.c_str(), "w");

    // string file_memeory = PATH == "" ? "memory.txt" : PATH + "/memory.txt";
    // memory_txt = fopen(file_memeory.c_str(), "w");
}

void Scheduler::checkSleepTermination()
{
    cout << "checkSleepTermination start ok." << endl;

    list<Process>::iterator iter;

    for (iter = sleep_list.begin(); iter != sleep_list.end(); iter++)
    {
        if (iter->is_sleeping && iter->sleep_cycle > 0)
        {
            iter->sleep_cycle--;
        }

        if (iter->is_sleeping && iter->sleep_cycle == 0)
        {
            int num_q = iter->priority;
            iter->is_sleeping = false;
            wake_sleep_pid = iter->pid;
            run_queue[num_q].push_back(*iter);
        }
    }

    sleep_list.remove_if(predicateSleepDone);
    wake_sleep_pid = -1;
}

void Scheduler::executeIO(int pid)
{
    cout << "excuteIO start ok." << endl;
    cout << ioWait_list.front().name << endl;

    list<Process>::iterator iter;

    for (iter = ioWait_list.begin(); iter != ioWait_list.end(); iter++)
    {
        if (pid == iter->pid)
        {
            int num_q = iter->priority;
            iter->is_io_waiting = false;

            execute_io_pid = pid;
            run_queue[num_q].push_back(*iter);

            break;
        }
    }

    ioWait_list.remove_if(predicateIoExec);
    execute_io_pid = -1;
}

void Scheduler::createProcess(string name, int priority)
{
    cout << "createProcess start ok." << endl;
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
    cout << "decideProcess start ok." << endl;
    if (running_process == nullptr)
    {
        for (int i = 0; i < run_queue.size(); i++)
        {
            if (!run_queue[i].empty())
            {
                temp = run_queue[i].front();
                run_queue[i].pop_front();
                scheduled_process = &temp;
                Process p = *scheduled_process;
                print_scheduled_process.push(p);
                break;
            }
        }
    }
    else
    {
        Process p = *running_process;
        int current_process_priority = p.priority;

        for (int i = 0; i < run_queue.size(); i++)
        {
            if (i >= current_process_priority)
                break;

            if (!run_queue[i].empty())
            {
                temp = run_queue[i].front();
                run_queue[i].pop_front();
                run_queue[i].push_back(p);
                scheduled_process = &temp;
                Process p = *scheduled_process;
                print_scheduled_process.push(p);
                break;
            }
        }
    }
}

void Scheduler::op0_mem_alloc(int arg)
{
    cout << "op0 start ok." << endl;
    running_process->current_code++;
}

void Scheduler::op1_mem_access(int arg)
{
    cout << "op1 start ok." << endl;
    running_process->current_code++;
}

void Scheduler::op2_mem_release(int arg)
{
    cout << "op2 start ok." << endl;
    running_process->current_code++;
}

void Scheduler::op3_non_mem(int arg)
{
    cout << "op3 start ok." << endl;
    running_process->current_code++;
}

void Scheduler::op4_sleep(int arg)
{
    cout << "op4 start ok." << endl;
    if (running_process->codes.size() == 1)
        return;

    running_process->is_sleeping = true;
    running_process->sleep_cycle = arg;
    running_process->codes.pop();
    running_process->current_code++;

    sleep_list.push_back(*running_process);

    // consider if the process' last operation is sleep op.
}

void Scheduler::op5_io_wait(int arg)
{
    cout << "op5 start ok." << endl;
    if (running_process->codes.size() == 1)
        return;

    running_process->is_io_waiting = true;
    running_process->codes.pop();
    running_process->current_code++;

    ioWait_list.push_back(*running_process);

    // consider if the process' last operation is io op.
}

void Scheduler::executeProcessCode()
{
    cout << "executeProcessCode start ok." << endl;

    int op = running_process->codes.front()[0];
    int arg = running_process->codes.front()[1];

    // print for check
    // cout << "Process name: " << running_process->name << " operation #" << op << " excuted with argument #" << arg << endl;
    switch (op)
    {
    case 0:
        op0_mem_alloc(arg);
        running_process->codes.pop();
        break;
    case 1:
        op1_mem_access(arg);
        running_process->codes.pop();
        break;
    case 2:
        op2_mem_release(arg);
        running_process->codes.pop();
        break;
    case 3:
        op3_non_mem(arg);
        running_process->codes.pop();
        break;
    case 4:
        op4_sleep(arg);
        break;
    case 5:
        op5_io_wait(arg);
        break;
    }
}

void Scheduler::printSchedulerInfo()
{
    cout << "printSchedulerInfo start ok." << endl;

    // Line 1
    fprintf(scheduler_txt, "[%d Cycle] Scheduled Process: ", cycle);
    if (!print_scheduled_process.empty())
        fprintf(scheduler_txt, "%d %s (priority %d)\n", print_scheduled_process.front().pid, print_scheduled_process.front().name.c_str(), print_scheduled_process.front().priority);
    else
        fprintf(scheduler_txt, "None\n");

    // Line 2
    fprintf(scheduler_txt, "Running Process: ");
    if (!print_running_process.empty())
        fprintf(scheduler_txt, "Process#%d(%d) running code %s line %d(op %d, arg %d)\n", print_running_process.front().pid, print_running_process.front().priority, print_running_process.front().name.c_str(), print_running_process.front().current_code + 1, print_running_process.front().codes.front()[0], print_running_process.front().codes.front()[1]);
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
            fprintf(scheduler_txt, "%d(%s) ", p.pid, p.name.c_str());
        }
    fprintf(scheduler_txt, "\n");

    // for the next cycle
    fprintf(scheduler_txt, "\n");
}

void Scheduler::runCycle(int current_cycle)
{
    cout << "runCycle start ok." << endl;
    checkSleepTermination();

    bool input_in_current_cycle = true;
    while (!input_processes.empty() && input_in_current_cycle)
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
    if (scheduled_process != nullptr)
    {
        running_process = scheduled_process;
        scheduled_process = nullptr;
    }

    if (running_process != nullptr)
    {
        Process p = *running_process;
        print_running_process.push(p);
        executeProcessCode();

        // when to remove a running process
        if (running_process->is_sleeping || running_process->is_io_waiting || running_process->codes.empty())
            running_process = nullptr;
    }

    printSchedulerInfo();
    if (!print_scheduled_process.empty())
        print_scheduled_process.pop();
    if (!print_running_process.empty())
        print_running_process.pop();
    // printMemoryInfo();

    //  when to terminate scheduler program
    bool run_queues_empty = true;
    for (list<Process> q : run_queue)
    {
        if (!q.empty())
        {
            run_queues_empty = false;
            break;
        }
    }

    if (input_processes.empty() && run_queues_empty && running_process == nullptr && scheduled_process == nullptr && ioWait_list.empty() && sleep_list.empty())
    {
        cout << "program will be end now..." << endl;
        END = true;
    }
}

void Scheduler::start()
{
    while (!END)
    // for (int i = 0; i < 34; i++)
    {
        cout << "cycle: " << cycle << endl;
        runCycle(cycle);
        cycle++;
        cout << "==========" << endl;
    }
    fclose(scheduler_txt);
}