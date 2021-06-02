#include <iostream>
#include <string>

using namespace std;

string PAGE_ALGORITHM = "";
string PATH = "";

int TOTAL_EVENT_NUM;
int VM_SIZE;
int PW_SIZE;
int PAGE_FRAME_SIZE;

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

    string file_input = PATH == "" ? "input" : PATH + "/input";
    string file_scheduler = PATH == "" ? "scheduler.txt" : PATH + "/scheduler.txt";
    string file_memeory = PATH == "" ? "memory.txt" : PATH + "/memory.txt";

    FILE *input = fopen(file_input.c_str(), "r");
    FILE *scheduler = fopen(file_scheduler.c_str(), "w");
    FILE *memory = fopen(file_memeory.c_str(), "w");

    char *input_settings = NULL;
    char *settings2 = NULL;
    size_t len = 0;

    getline(&input_settings, &len, input);

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
