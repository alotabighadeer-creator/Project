#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int start_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
};

void print_results(const vector<Process>& processes) {
    int n = processes.size();
    float avg_turnaround_time = 0;
    float avg_waiting_time = 0;
    float avg_response_time = 0;

    cout << setprecision(2) << fixed;
    cout << "\nPID\tArrival\tBurst\tPriority\tStart\tCompletion\tTurnaround\tWaiting\tResponse\n";
    cout << "--------------------------------------------------------------------------------------------------------\n";

    for (const auto& p : processes) {
        cout << p.pid << "\t" << p.arrival_time << "\t" << p.burst_time << "\t" << p.priority << "\t\t"
             << p.start_time << "\t" << p.completion_time << "\t\t" << p.turnaround_time << "\t\t"
             << p.waiting_time << "\t\t" << p.response_time << "\n";
        avg_turnaround_time += p.turnaround_time;
        avg_waiting_time += p.waiting_time;
        avg_response_time += p.response_time;
    }

    cout << "\nAverage Turnaround Time: " << avg_turnaround_time / n << endl;
    cout << "Average Waiting Time: " << avg_waiting_time / n << endl;
    cout << "Average Response Time: " << avg_response_time / n << endl;
}

void fcfs(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    int current_time = 0;
    for (auto& p : processes) {
        p.start_time = max(current_time, p.arrival_time);
        p.completion_time = p.start_time + p.burst_time;
        p.turnaround_time = p.completion_time - p.arrival_time;
        p.waiting_time = p.turnaround_time - p.burst_time;
        p.response_time = p.start_time - p.arrival_time;
        current_time = p.completion_time;
    }
    cout << "\n--- FCFS Scheduling ---";
    print_results(processes);
}

void sjf(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    vector<Process> completed_processes;
    vector<Process> ready_queue;
    int current_time = 0;
    int completed = 0;
    int n = processes.size();

    while(completed < n) {
        for(auto& p : processes) {
            if(p.arrival_time <= current_time && find_if(completed_processes.begin(), completed_processes.end(), [&](const Process& cp){ return cp.pid == p.pid; }) == completed_processes.end() && find_if(ready_queue.begin(), ready_queue.end(), [&](const Process& rq){ return rq.pid == p.pid; }) == ready_queue.end()) {
                ready_queue.push_back(p);
            }
        }

        if(ready_queue.empty()) {
            current_time++;
            continue;
        }

        sort(ready_queue.begin(), ready_queue.end(), [](const Process& a, const Process& b) {
            return a.burst_time < b.burst_time;
        });

        Process current_process = ready_queue.front();
        ready_queue.erase(ready_queue.begin());

        current_process.start_time = current_time;
        current_process.completion_time = current_time + current_process.burst_time;
        current_process.turnaround_time = current_process.completion_time - current_process.arrival_time;
        current_process.waiting_time = current_process.turnaround_time - current_process.burst_time;
        current_process.response_time = current_process.start_time - current_process.arrival_time;
        
        current_time = current_process.completion_time;
        completed_processes.push_back(current_process);
        completed++;
    }
    
    sort(completed_processes.begin(), completed_processes.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });

    cout << "\n--- SJF (Non-Preemptive) Scheduling ---";
    print_results(completed_processes);
}

void priority(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    vector<Process> completed_processes;
    vector<Process> ready_queue;
    int current_time = 0;
    int completed = 0;
    int n = processes.size();

    while(completed < n) {
        for(auto& p : processes) {
            if(p.arrival_time <= current_time && find_if(completed_processes.begin(), completed_processes.end(), [&](const Process& cp){ return cp.pid == p.pid; }) == completed_processes.end() && find_if(ready_queue.begin(), ready_queue.end(), [&](const Process& rq){ return rq.pid == p.pid; }) == ready_queue.end()) {
                ready_queue.push_back(p);
            }
        }

        if(ready_queue.empty()) {
            current_time++;
            continue;
        }

        sort(ready_queue.begin(), ready_queue.end(), [](const Process& a, const Process& b) {
            return a.priority < b.priority;
        });

        Process current_process = ready_queue.front();
        ready_queue.erase(ready_queue.begin());

        current_process.start_time = current_time;
        current_process.completion_time = current_time + current_process.burst_time;
        current_process.turnaround_time = current_process.completion_time - current_process.arrival_time;
        current_process.waiting_time = current_process.turnaround_time - current_process.burst_time;
        current_process.response_time = current_process.start_time - current_process.arrival_time;
        
        current_time = current_process.completion_time;
        completed_processes.push_back(current_process);
        completed++;
    }

    sort(completed_processes.begin(), completed_processes.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });

    cout << "\n--- Priority (Non-Preemptive) Scheduling ---";
    print_results(completed_processes);
}

void round_robin(vector<Process> processes, int quantum) {
    vector<Process> ready_queue;
    vector<Process> completed_processes;
    vector<int> remaining_burst_time(processes.size() + 1);
    vector<bool> in_queue(processes.size() + 1, false);

    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });

    for(const auto& p : processes) {
        remaining_burst_time[p.pid] = p.burst_time;
    }

    int current_time = 0;
    int completed = 0;
    int n = processes.size();
    int process_idx = 0;

    while(completed < n) {
        while(process_idx < n && processes[process_idx].arrival_time <= current_time) {
            if(!in_queue[processes[process_idx].pid]) {
                ready_queue.push_back(processes[process_idx]);
                in_queue[processes[process_idx].pid] = true;
            }
            process_idx++;
        }

        if(ready_queue.empty()) {
            current_time++;
            continue;
        }

        Process current_process = ready_queue.front();
        ready_queue.erase(ready_queue.begin());
        in_queue[current_process.pid] = false;

        auto& original_process = *find_if(processes.begin(), processes.end(), [&](Process& p){ return p.pid == current_process.pid; });

        if (remaining_burst_time[current_process.pid] == original_process.burst_time) {
            original_process.start_time = current_time;
            original_process.response_time = original_process.start_time - original_process.arrival_time;
        }

        int time_slice = min(quantum, remaining_burst_time[current_process.pid]);
        current_time += time_slice;
        remaining_burst_time[current_process.pid] -= time_slice;

        while(process_idx < n && processes[process_idx].arrival_time <= current_time) {
            if(!in_queue[processes[process_idx].pid]) {
                ready_queue.push_back(processes[process_idx]);
                in_queue[processes[process_idx].pid] = true;
            }
            process_idx++;
        }

        if (remaining_burst_time[current_process.pid] > 0) {
            ready_queue.push_back(current_process);
            in_queue[current_process.pid] = true;
        } else {
            original_process.completion_time = current_time;
            original_process.turnaround_time = original_process.completion_time - original_process.arrival_time;
            original_process.waiting_time = original_process.turnaround_time - original_process.burst_time;
            completed++;
        }
    }

    cout << "\n--- Round Robin (RR) Scheduling (Quantum=" << quantum << ") ---";
    print_results(processes);
}

int main() {
    int n;
    cout << "Enter the number of processes: ";
    cin >> n;

    vector<Process> processes(n);
    for (int i = 0; i < n; ++i) {
        processes[i].pid = i + 1;
        cout << "\nProcess " << processes[i].pid << ":\n";
        cout << "  Enter Arrival Time: ";
        cin >> processes[i].arrival_time;
        cout << "  Enter Burst Time: ";
        cin >> processes[i].burst_time;
        cout << "  Enter Priority: ";
        cin >> processes[i].priority;
    }

    int quantum;
    cout << "\nEnter the time quantum for Round Robin: ";
    cin >> quantum;

    fcfs(processes);
    sjf(processes);
    priority(processes);
    round_robin(processes, quantum);

    return 0;
}
