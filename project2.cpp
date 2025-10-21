#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
using namespace std;

// Process Control Block structure
struct PCB {
    string id;            // Process ID
    int priority;         // Process priority
    int burst_time;       // Total burst time required
    int arrival_time;     // Arrival time
    int remaining_time;   // Remaining burst time
    int completion_time;  // Completion time
};

// Function to add newly arrived processes to the ready queue
void addArrivals(vector<PCB> &processes, map<int, queue<int> > &ready,
                 vector<int> &order, int &next_idx, int time) {
    int n = order.size();
    // Add all processes that have arrived up to current time
    while (next_idx < n && processes[order[next_idx]].arrival_time <= time) {
        int idx = order[next_idx];
        ready[processes[idx].priority].push(idx); // Add process to priority queue
        next_idx++;
    }
}

int main() {
    string dummy;
    int time_quantum;

    // Read scheduling type or dummy variable and time quantum
    cin >> dummy >> time_quantum;

    vector<PCB> processes;

    // Read all processes from input
    while (true) {
        PCB p;
        if (!(cin >> p.id >> p.priority >> p.burst_time >> p.arrival_time)) break;
        p.remaining_time = p.burst_time; // Initially, remaining time = burst time
        p.completion_time = -1;          // Not yet completed
        processes.push_back(p);
    }

    int n = processes.size();
    vector<int> order(n);

    // Initialize order indices
    for (int i = 0; i < n; i++) order[i] = i;

    // Sort indices by arrival time (simple bubble sort)
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (processes[order[j]].arrival_time < processes[order[i]].arrival_time)
                swap(order[i], order[j]);

    // Ready queue: map from priority to queue of process indices
    map<int, queue<int> > ready;
    int time = 0;         // Current time
    int completed = 0;    // Number of completed processes
    int next_idx = 0;     // Index of next arriving process

    cout << "Gantt Chart:\n";

    string current_pid = "";  // Current process being executed
    int segment_start = 0;    // Start time of current segment
    int segment_end = 0;      // End time of current segment
    int current_priority = 0; // Priority of current process

    // Main scheduling loop
    while (completed < n) {
        addArrivals(processes, ready, order, next_idx, time); // Add newly arrived processes

        // If no processes are ready, CPU is idle
        if (ready.empty()) {
            int next_arrival = (next_idx < n) ? processes[order[next_idx]].arrival_time : time;
            int idle_time = (next_arrival > time) ? next_arrival - time : 1;

            // Print previous segment if different
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
            }
            segment_end = time + idle_time;
            time += idle_time;
            continue;
        }

        // Pick the highest priority process (map rbegin gives highest priority key)
        map<int, queue<int> >::reverse_iterator it = ready.rbegin();
        int idx = it->second.front();
        it->second.pop();
        if (it->second.empty()) ready.erase(it->first);

        // Determine run time (minimum of remaining time or time quantum)
        int run_time = (processes[idx].remaining_time < time_quantum) ? processes[idx].remaining_time : time_quantum;

        // Check if higher-priority process arrives during this run
        int end_time = time + run_time;
        for (int i = next_idx; i < n; i++) {
            if (processes[order[i]].arrival_time > time && processes[order[i]].arrival_time < end_time &&
                processes[order[i]].priority > processes[idx].priority) {
                end_time = processes[order[i]].arrival_time;
                run_time = end_time - time;
                break;
            }
        }

        // Merge consecutive segments of same process
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
            current_priority = processes[idx].priority;
        }
        segment_end = time + run_time;

        // Update remaining time
        processes[idx].remaining_time -= run_time;
        time += run_time;

        addArrivals(processes, ready, order, next_idx, time); // Check for new arrivals

        // If process finished, update completion time
        if (processes[idx].remaining_time == 0) {
            processes[idx].completion_time = time;
            completed++;
        } else {
            ready[processes[idx].priority].push(idx); // Round-robin: push back into queue
        }
    }

    // Print the last segment
    if (current_pid != "")
        cout << "Time " << segment_start << "-" << segment_end
             << ": " << current_pid
             << ((current_pid != "Idle") ? " (Priority " + to_string(current_priority) + ")" : "")
             << "\n";

    // Turnaround Time (Completion - Arrival)
    cout << "\nTurnaround Time\n";
    for (int i = 0; i < n; i++) {
        int tat = processes[i].completion_time - processes[i].arrival_time;
        cout << processes[i].id << " = " << tat << "\n";
    }

    // Waiting Time (Turnaround - Burst)
    cout << "\nWaiting Time\n";
    for (int i = 0; i < n; i++) {
        int wt = processes[i].completion_time - processes[i].arrival_time - processes[i].burst_time;
        cout << processes[i].id << " = " << wt << "\n";
    }

    // CPU Utilization (Total burst / total time)
    int total_burst = 0;
    for (int i = 0; i < n; i++) total_burst += processes[i].burst_time;
    cout << "\nCPU Utilization Time\n" << total_burst << "/" << time << "\n";

    return 0;
}
