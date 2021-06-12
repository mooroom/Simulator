#include <iostream>
#include <list>
#include <vector>
#include <queue>
// #include <stdio.h>

using namespace std;

struct Process
{
    int a;
    int b;
};

void helloProcess(Process *);
bool predicate(Process);

int main()
{
    list<Process> ls;

    Process *scheduled_process = nullptr;
    Process *running_process = nullptr;

    Process p1 = {-1};

    Process p2;
    p2.a = 6;
    p2.b = 12;

    Process p3;
    p3.a = 11;
    p3.b = 44;

    scheduled_process = &p1;

    ls.push_back(*scheduled_process);
    ls.push_back(p2);
    ls.push_back(p3);

    // for (Process p : ls)
    // {
    //     cout << p.a << " " << p.b << endl;
    // }
    // cout << endl;

    // ls.remove_if(predicate);

    // for (Process p : ls)
    // {
    //     cout << p.a << " " << p.b << endl;
    // }

    cout << scheduled_process->b << endl;

    return 0;
}

void helloProcess(Process *p)
{
    cout << "hello!" << p->a << " " << p->b << endl;
    p->a = 20;
}

bool predicate(Process p)
{
    return p.a == 6;
}