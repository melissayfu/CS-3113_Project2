#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
using namespace std;

// Structure to store process details
struct PCB {
    string id;            // Process ID
    int priority;         // Priority of the process
    int burst_time;       // Total CPU burst time required
    int arrival_time;     // Time at which process arrives
    int remaining_time;   // Remaining burst time (for preemption/round-robin)
    int completion_time;  // Time at which process completes execution
};

// Function to add newly arrived processes to the ready queue
void addArrivals(vector<PCB> &processes, map<int, queue<int> > &ready,
                 vector<int> &order, int &next_idx, int time) {
    int n = order.size();
    // Add all processes that have arrived by current 'time' to the ready queue
    while (next_idx < n && processes[order[next_idx]].arrival_time <= time) {
        int idx = order[next_idx];
        ready[processes[idx].priority].push(idx); // Queue processes by priority
        next_idx++;
    }
}

int main() {
    string dummy;
    int time_quantum;
    cin >> dummy >> time_quantum; // Read time quantum (for round-robin scheduling)

    vector<PCB> processes;
    // Read process details from input
    while (true) {
        PCB p;
        if (!(cin >> p.id >> p.priority >> p.burst_time >> p.arrival_time)) break;
        p.remaining_time = p.burst_time; // Initialize remaining time
        p.completion_time = -1;          // Initialize completion time
        processes.push_back(p);
    }

    int n = processes.size();
    vector<int> order(n);
    for (int i = 0; i < n; i++) order[i] = i;

    // Sort process indices by arrival time
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (processes[order[j]].arrival_time < processes[order[i]].arrival_time)
                swap(order[i], order[j]);

    map<int, queue<int> > ready; // Ready queues organized by priority
    int time = 0;                // Current simulation time
    int completed = 0;           // Number of processes completed
    int next_idx = 0;             // Index of next arriving process

    // Variables to track CPU segments for output
    string current_pid = "";
    int segment_start = 0;
    int segment_end = 0;
    int current_priority = 0;

    // Main scheduling loop: continue until all processes complete
    while (completed < n) {
        addArrivals(processes, ready, order, next_idx, time);

        if (ready.empty()) { // No ready processes => CPU is idle
            int next_arrival = (next_idx < n) ? processes[order[next_idx]].arrival_time : time;
            int idle_time = (next_arrival > time) ? next_arrival - time : 1;

            // Output previous segment if it exists
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

        // Pick the highest-priority process from the ready queues
        map<int, queue<int> >::reverse_iterator it = ready.rbegin();
        int idx = it->second.front();
        it->second.pop();
        if (it->second.empty()) ready.erase(it->first);

        // Determine how long process can run (time quantum or remaining time)
        int run_time = (processes[idx].remaining_time < time_quantum) ? processes[idx].remaining_time : time_quantum;

        // Check if a higher-priority process will arrive during this run
        int end_time = time + run_time;
        for (int i = next_idx; i < n; i++) {
            if (processes[order[i]].arrival_time > time && processes[order[i]].arrival_time < end_time &&
                processes[order[i]].priority > processes[idx].priority) {
                end_time = processes[order[i]].arrival_time;
                run_time = end_time - time;
                break;
            }
        }

        // Merge consecutive segments of the same process
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

        processes[idx].remaining_time -= run_time; // Update remaining time
        time += run_time;                          // Advance simulation time

        addArrivals(processes, ready, order, next_idx, time);

        if (processes[idx].remaining_time == 0) { // Process finished
            processes[idx].completion_time = time;
            completed++;
        } else { // Process not finished, re-add to ready queue (round-robin)
            ready[processes[idx].priority].push(idx);
        }
    }

    // Print the final segment
    if (current_pid != "")
        cout << "Time " << segment_start << "-" << segment_end
             << ": " << current_pid
             << ((current_pid != "Idle") ? " (Priority " + to_string(current_priority) + ")" : "")
             << "\n";

    // Calculate and print Turnaround Times
    cout << "\nTurnaround Time\n";
    for (int i = 0; i < n; i++) {
        int tat = processes[i].completion_time - processes[i].arrival_time;
        cout << processes[i].id << " = " << tat << "\n";
    }

    // Calculate and print Waiting Times
    cout << "\nWaiting Time\n";
    for (int i = 0; i < n; i++) {
        int wt = processes[i].completion_time - processes[i].arrival_time - processes[i].burst_time;
        cout << processes[i].id << " = " << wt << "\n";
    }

    // Calculate and print CPU Utilization
    int total_burst = 0;
    for (int i = 0; i < n; i++) total_burst += processes[i].burst_time;
    cout << "\nCPU Utilization Time\n" << total_burst << "/" << time << "\n";

    return 0;
}