#include <iostream>
#include <string>
#include "scheduler.h"

using namespace std;

string PAGE_ALGORITHM = "";
string PATH = "";

void option_page(string);
void option_path(string);

int main(int argc, char *argv[])
{
    // get options
    if (argc == 1)
    {
        PAGE_ALGORITHM = "lru";
        PATH = "";
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            string option = argv[i];

            if (option[1] == 'p')
                option_page(option);
            else if (option[1] == 'd')
                option_path(option);
        }
    }

    Scheduler scheduler;
    scheduler.getInput(PATH);
    // vector<string> vec = {"0", "program1", "6"};
    // scheduler.createProcess(vec);

    //
    string file_memeory = PATH == "" ? "memory.txt" : PATH + "/memory.txt";
    FILE *memory = fopen(file_memeory.c_str(), "w");
    //

    return 0;
}

int find_equal(string option)
{
    int equal_index;

    for (int i = 0; i < option.length(); i++)
    {
        if (option[i] == '=')
        {
            equal_index = i;
            break;
        }
    }

    return equal_index;
}

void option_page(string option)
{
    int split_idx = find_equal(option);
    string opt = option.substr(split_idx + 1);

    cout << "page option: " << opt << endl;

    PAGE_ALGORITHM = opt;
}

void option_path(string option)
{
    int split_idx = find_equal(option);
    string opt = option.substr(split_idx + 1);

    cout << "path option: " << opt << endl;

    PATH = opt;
}
