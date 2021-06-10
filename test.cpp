#include <iostream>
#include <list>
// #include <stdio.h>

using namespace std;

int main()
{
    struct Process
    {
        int pid;
        string name;
    };

    list<Process> ls;

    Process p1;
    p1.pid = 1;
    p1.name = "p1";

    Process p2;
    p2.pid = 2;
    p2.name = "p2";

    ls.push_back(p1);
    ls.push_back(p2);

    for (Process val : ls)
    {
        cout << val.name << " " << val.pid << endl;
        printf("%d(%s) \n", val.pid, val.name.c_str());
    }

    return 0;
}