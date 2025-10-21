#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
using namespace std;

struct PCB {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int completion_time;
};

// Move this OUTSIDE of main
struct SortByArrival {
    vector<PCB> *p;
    bool operator()(int a, int b) const {
        return (*p)[a].arrival_time < (*p)[b].arrival_time;
    }
};

void addArrivals(vector<PCB> &processes, map<int, queue<int> > &ready,
                 int &next_idx, int time, const vector<int> &order) {
    while (next_idx < (int)order.size() &&
           processes[order[next_idx]].arrival_time <= time) {
        int idx = order[next_idx];
        ready[processes[idx].priority].push(idx);
        next_idx++;
    }
}

int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<PCB> processes(n);
    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        cout << "Arrival time, Burst time, Priority for process " << i + 1 << ": ";
        cin >> processes[i].arrival_time >> processes[i].burst_time >> processes[i].priority;
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].completion_time = -1;
    }

    vector<int> order(n);
    for (int i = 0; i < n; i++) order[i] = i;

    SortByArrival cmp;
    cmp.p = &processes;
    sort(order.begin(), order.end(), cmp);

    map<int, queue<int> > ready;
    int time = 0, completed = 0, next_idx = 0;
    addArrivals(processes, ready, next_idx, time, order);

    cout << "\nGantt Chart:\n";

    while (completed < n) {
        if (ready.empty()) {
            time++;
            addArrivals(processes, ready, next_idx, time, order);
            continue;
        }

        map<int, queue<int> >::reverse_iterator it = ready.rbegin();
        int idx = it->second.front();
        it->second.pop();
        if (it->second.empty())
            ready.erase(it->first);

        cout << "P" << processes[idx].pid << " ";

        processes[idx].remaining_time--;
        time++;

        addArrivals(processes, ready, next_idx, time, order);

        if (processes[idx].remaining_time == 0) {
            processes[idx].completion_time = time;
            completed++;
        } else {
            ready[processes[idx].priority].push(idx);
        }
    }

    cout << "\n\nPID\tArrival\tBurst\tPriority\tCompletion\tTurnaround\tWaiting\n";
    for (int i = 0; i < n; i++) {
        int turnaround = processes[i].completion_time - processes[i].arrival_time;
        int waiting = turnaround - processes[i].burst_time;
        cout << processes[i].pid << "\t"
             << processes[i].arrival_time << "\t"
             << processes[i].burst_time << "\t"
             << processes[i].priority << "\t\t"
             << processes[i].completion_time << "\t\t"
             << turnaround << "\t\t"
             << waiting << endl;
    }

    return 0;
}