#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
using namespace std;

struct PCB {
    string id;
    int priority;
    int burst_time;
    int arrival_time;
    int remaining_time;
    int completion_time;
};

// Function to add newly arrived processes to ready queue
void addArrivals(vector<PCB> &processes, map<int, queue<int> > &ready,
                 vector<int> &order, int &next_idx, int time) {
    int n = order.size();
    while (next_idx < n && processes[order[next_idx]].arrival_time <= time) {
        int idx = order[next_idx];
        ready[processes[idx].priority].push(idx);
        next_idx++;
    }
}

int main() {
    string dummy;
    int time_quantum;
    cin >> dummy >> time_quantum;

    vector<PCB> processes;
    while (true) {
        PCB p;
        if (!(cin >> p.id >> p.priority >> p.burst_time >> p.arrival_time)) break;
        p.remaining_time = p.burst_time;
        p.completion_time = -1;
        processes.push_back(p);
    }

    int n = processes.size();
    vector<int> order(n);
    for (int i = 0; i < n; i++) order[i] = i;

    // Sort indices by arrival time
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (processes[order[j]].arrival_time < processes[order[i]].arrival_time)
                swap(order[i], order[j]);

    map<int, queue<int> > ready;
    int time = 0;
    int completed = 0;
    int next_idx = 0;

    cout << "Gantt Chart:\n";

    string current_pid = "";
    int segment_start = 0;
    int segment_end = 0;
    int current_priority = 0;

    while (completed < n) {
        addArrivals(processes, ready, order, next_idx, time);

        if (ready.empty()) {
            int next_arrival = (next_idx < n) ? processes[order[next_idx]].arrival_time : time;
            int idle_time = (next_arrival > time) ? next_arrival - time : 1;

            if (current_pid != "Idle") {
                if (current_pid != "") {
                    cout << "Time " << segment_start << "-" << segment_end
                         << ": " << current_pid;
                    if (current_pid != "Idle") {
                        cout << " (Priority " << current_priority << ")";
                    }
                    cout << "\n";
                }
                current_pid = "Idle";
                segment_start = time;
                // Do not set priority for Idle
            }
            segment_end = time + idle_time;
            time += idle_time;
            continue;
        }

        // Pick highest priority
        map<int, queue<int> >::reverse_iterator it = ready.rbegin();
        int idx = it->second.front();
        it->second.pop();
        if (it->second.empty()) ready.erase(it->first);

        int run_time = (processes[idx].remaining_time < time_quantum) ? processes[idx].remaining_time : time_quantum;

        // Check for higher-priority arrivals during run
        int end_time = time + run_time;
        for (int i = next_idx; i < n; i++) {
            if (processes[order[i]].arrival_time > time && processes[order[i]].arrival_time < end_time &&
                processes[order[i]].priority > processes[idx].priority) {
                end_time = processes[order[i]].arrival_time;
                run_time = end_time - time;
                break;
            }
        }

        // Merge consecutive same process segments
        if (current_pid != processes[idx].id) {
            if (current_pid != "") {
                cout << "Time " << segment_start << "-" << segment_end
                     << ": " << current_pid;
                if (current_pid != "Idle") {
                    cout << " (Priority " << current_priority << ")";
                }
                cout << "\n";
            }
            current_pid = processes[idx].id;
            segment_start = time;
            current_priority = processes[idx].priority; // Only for real processes
        }
        segment_end = time + run_time;

        processes[idx].remaining_time -= run_time;
        time += run_time;

        addArrivals(processes, ready, order, next_idx, time);

        if (processes[idx].remaining_time == 0) {
            processes[idx].completion_time = time;
            completed++;
        } else {
            ready[processes[idx].priority].push(idx); // round-robin
        }
    }

    // Print the last segment
    if (current_pid != "")
        cout << "Time " << segment_start << "-" << segment_end
             << ": " << current_pid
             << ((current_pid != "Idle") ? " (Priority " + to_string(current_priority) + ")" : "")
             << "\n";

    // Turnaround & Waiting Times
    cout << "\nTurnaround Time\n";
    for (int i = 0; i < n; i++) {
        int tat = processes[i].completion_time - processes[i].arrival_time;
        cout << processes[i].id << " = " << tat << "\n";
    }

    cout << "\nWaiting Time\n";
    for (int i = 0; i < n; i++) {
        int wt = processes[i].completion_time - processes[i].arrival_time - processes[i].burst_time;
        cout << processes[i].id << " = " << wt << "\n";
    }

    // CPU Utilization
    int total_burst = 0;
    for (int i = 0; i < n; i++) total_burst += processes[i].burst_time;
    cout << "\nCPU Utilization Time\n" << total_burst << "/" << time << "\n";

    return 0;
}