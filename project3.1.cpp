/* Martin Jaime
 * CS 370
 * Project 3: a CPU Time Lord
 * 2015-03-03
 */ 

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>

#define MINSLICE 10;
#define MAXSLICE 300;

using namespace std;
using namespace boost;

class process
{
    public:
    int pid,
        arrival_t,
        start_t,
        end_t,
        priority,
        nice,
        cpu_count,
        t_slice,
        finish_t,
        cur_CPUburst,   // Currently running CPU burst time
        cur_IOburst,    // Currently running IO burst time
        TAT,            // Turn around time
        TCT,            // Total cpu time: sum of all cpu bursts
        TIT,            // Total I/O time
        WT,             // Waiting time: TAT - TCT - TIT(Total IO time)
        CUT,            // % of CPU utilization time = TCT / TAT (nearest 10th)
        b_index;
    vector<int> cpu_bursts, 
        io_bursts;  // io_bursts should be cpu_bursts-1
                    
    process()
    {
        pid = 0; arrival_t = 0; start_t = 0; end_t = 0; priority = 0;
        nice = 0; cpu_count = 0; t_slice = 0; cur_CPUburst = 0; cur_IOburst = 0;
        TAT = 0; TCT = 0; TIT = 0; WT = 0; CUT = 0, b_index = 0;
        vector<int>().swap(cpu_bursts);
        vector<int>().swap(io_bursts); 
    }
    
    bool isExpired();
    void calcPriority();
    void calcTSlice();
    void decTSlice(); 
    void decIOburst();
    bool isBurstDone();
    bool isIOBurstDone();
    // add an insert function to insert in appropriate spot. 

};

void ioexpireit(vector<process>&, process, int);
void printEnterActive(process, int);
void printPreempt(vector<process>, int);        // Print message about premtipn
void printEnterCPU(process, int);               // Print message about p entering cpu
void expireit(vector<process>&, process, int);  // Send a process form 
void doIO(vector<process>&, process, int);
void finishP(vector<process>&, vector<process>&, int);
void decIO(vector<process>&);
void qInsert(vector<process>&, process);
int writeStartup(vector<process>&);
bool complt_arrival(process, process);
bool complt_priority(process, process);

int main()
{
    vector<process> startup, ioqueue, *active, 
        *expired, finished, *tempQ;            // Queues. 
    vector<process> cpu;
    int clock = 0;
    int p_count;
    int avg_TAT = 0, avg_WT = 0, avg_CUT = 0; active = new vector<process>;
    expired  = new vector<process>;

    p_count = writeStartup(startup);
    // Sort processes by arrival
    sort(startup.begin(), startup.end(), complt_arrival);
    /*for(vector<process>::iterator i = startup.begin(); i != startup.end();  ++i)
        cout << (*i).arrival_t << endl;*/
    while (true)
    {
        // Check startup q for arriving processes and place those in active q
        for(vector<process>::size_type i = 0; i < startup.size(); i++)
        {
            if (startup[i].arrival_t == clock) 
            {
                startup[i].calcPriority();
                startup[i].calcTSlice();
                printEnterActive(startup[i], clock);
                qInsert(*active, startup[i]);
                startup.erase(startup.begin() + i);
                i--;
            }
            else // otherwise, no other elements should qualify to run. 
                break;
        }
        //sort((*active).begin(), (*active).end(), complt_priority); 
        // Run highest priority process if cpu is empty.
        if (cpu.size() == 0 && (*active).size() != 0) // check if cpu is empty
        {
            cpu.push_back((*active)[0]);
            printEnterCPU(cpu[0], clock);
            (*active).erase((*active).begin()); 
        }
        // Or check if current process needs to be preempted. 
        else if((*active).size() > 0 && (*active)[0].priority < cpu[0].priority)
        {   
            cpu.push_back((*active)[0]);
            printPreempt(cpu, clock);
            active->erase(active->begin());
            qInsert(*active, cpu[0]);
        }
        decIO(ioqueue);
        if (cpu.size() > 0) // if there's a process on the cpu
        {
            cpu[0].decTSlice();
            if (cpu[0].isBurstDone()) // if current burst is exhausted 
            {
                if (cpu[0].cpu_bursts.size() == 0) // If all brusts are done.
                    finishP(finished, cpu, clock); // Then process is done.
                else 
                    doIO(ioqueue, cpu[0], clock);     // Otherwise, it has IO.
            }
            else if (cpu[0].isExpired()) 
            {
                expireit((*expired), cpu[0], clock);
                cpu.erase(cpu.begin());
            }
        }
        for(vector<process>::iterator i=ioqueue.begin(); i!= ioqueue.end(); ++i)
        { // If there is any process in the IO Q 
            if (i->isIOBurstDone()) // that is finished with cur IO
            { 
                if (i->isExpired()) // If expired. 
                    ioexpireit(*expired, *i, clock);
                else 
                {
                    qInsert(*active, *i);
                    printf("[%5d] <%d> Finishes IO and moves to the Ready Queue\n", 
                            clock, i->pid);
                }
                ioqueue.erase(i);
            }
        }
        if(startup.size() + active->size() + expired->size() + 
                ioqueue.size() + cpu.size() == 0) // If these are empty, sim done.
            break;
        if (active->size() + cpu.size() == 0 && expired->size() != 0)
        { // if p is not running and active queue is empty and expired queue is not,
            tempQ = active;
            active = expired;   // swap active and expired labels. 
            expired = tempQ;    // swap pointers.
            printf("[%5d] *** Queue Swap \n", clock);
        }
        clock++;
    }
    
    cout << endl << endl;
    for(vector<process>::iterator i = finished.begin(); i != finished.end(); ++i)
    {
        i->TAT = i->finish_t - i->arrival_t;
        i->CUT = ((float)i->TCT) / ((float)(i->TAT));
        i->WT = i->TAT - i->TCT - i->TIT;
        cout << i->TCT << " " <<  i->TAT << endl;
        printf("Turn around time = %d\n", i->TAT);
        printf("Total CPU time = %d\n", i->TCT);
        printf("Waiting Time = %d\n", i->WT);
        printf("Percentage of CPU utilization time = %.1f\n\n", i->CUT * 100.0);
    }

    for(vector<process>::iterator i = finished.begin(); i != finished.end(); ++i)
    {
        avg_TAT += i->TAT;
        avg_WT += i->WT;
        avg_CUT += i->CUT;
    }

    printf("Average Waiting time: %.3f\n", avg_WT / ((float)p_count));
    printf("Average Turnaround time: %.3f\n", avg_TAT  / ((float)p_count));
    printf("Average CPU Utilization: %.3f\n", avg_CUT  / ((float)p_count));

    return 0;
}
    //-while(true)
    //-    if a process is to start at this clock tick
    //-       insert processes to the active queue
    //-        calculate priority and timeslice.
    //         set current cpu burst time
    //-    if the cpu is empty, the lowest priority process in the active queue
    //-        is put into the cpu. (If more than two processes have the same 
    //-        priority then FIFO algorithm is used. 
    //-    If the lowest priority process in the active queue has a lower priority
    //-        than the process in the cpu. If so then preempt. 
    //-        Preempted process goes back to the active queue if its time slice is
    //-        not over. 
    //
    //-    Preform CPU (decrement the timeslice of the process in the CPU)
    //-    Perform I/O (decrement the I/O burst for all processes in the I/O queue)
    //-    if there is a process p in the CPU 
    //-        if curent CPU burst for p is exhausted
    //-            if p is done with ALL cpu bursts
    //-                send to the finished queue.
    //-            else p is not done with all CPU bursts (which probably means
    //-                there is still some I/O burst) send to the I/O queue. 
    //-        else if p's timeslice is exhausted send to the expired queue and recalc
    //-            priority and timeslice
    //-    if thre is any process in the I/O queue that is finished with its I/O
    //-        burst (there may be more than one process p).
    //-        if p's timceslice is exhausted move to the expired queue and 
    //-            recalc the priority and tyimeslice. 
    //-        if p still has a timeslice left insert p into the active queue. 
    //-    if the startup queue, ready queue, expired queue, I/O queue and the CPU
    //-        are all empty, then break out of the while loop. (sim complete). 
    //     if the ready queue and the CPU are empty and the expired queue is not, 
    //         then switch the expired and active queues. Simply swap pointers. 
    //     clock++

void printEnterActive(process p, int clock)
{
    printf("[%5d] <%d> Enters ready queue (Priority: %d, TimeSlice: %d)\n", 
            clock, p.pid, p.priority, p.t_slice);
}

void printPreempt(vector<process> cpu, int clock)
{
    printf("[%5d] <%d> Preempts process %d \n", clock, cpu[1].pid, cpu[0].pid);
}

void printEnterCPU(process p, int clock)
{
    printf("[%5d] <%d> Enters the CPU\n", clock, p.pid);
}

void ioexpireit(vector<process> &expQ, process p, int clock)
{
    p.calcPriority();
    p.calcTSlice();
    qInsert(expQ, p);
    printf("[%5d] <%d> Finished IO and moves to the Expired Queue\
(Priority: %d, TimeSlice: %d\n", clock, p.pid, p.priority, p.t_slice);
    return;
}                   

void expireit(vector<process> &expQ, process p, int clock)
{
    p.calcPriority();
    p.calcTSlice();
    qInsert(expQ, p);
    printf("[%5d] <%d> Finished its time slice and moves to the Expired Queue \
(Priority: %d, Timeslice: %d)\n", clock, p.pid, p.priority, p.t_slice);
    return;
}

void finishP(vector<process> &q, vector<process> &cpu, int clock)
{
    printf("[%5d] <%d> Finishes and moves to the Finished Queue\n", clock, 
            cpu[0].pid);
    cpu[0].finish_t = clock;
    q.push_back(cpu[0]);
    cpu.pop_back();
    return;
}

void doIO(vector<process> &ioQ, process p, int clock)
{
    printf("[%5d] <%d> Moves to the IO Queue\n", clock, p.pid);
    vector<int>::iterator i = p.io_bursts.begin();
    p.cur_IOburst = *i;
    p.io_bursts.erase(i); 
    ioQ.push_back(p);
    return;
}

int writeStartup(vector<process> &startup)
{
    process temp_p;
    int temp;
    int pid_count = 0;
    string line;
    tokenizer<char_separator<char> >::iterator iter;
    char_separator<char> sep(" ");

    // Read input
    getline(cin, line);
    while(line != "***")
    {
        pid_count++;
        temp_p.pid = pid_count;
        tokenizer<char_separator<char> > tok(line, sep);   // assign pid
        iter = tok.begin();
        temp_p.nice = atoi((*iter).c_str()); iter++;       // read nice value 
        temp_p.arrival_t = atoi((*iter).c_str()); iter++;  // read arrival time
        temp_p.cpu_count = atoi((*iter).c_str()); iter++;  // read # cpu bursts
        temp = atoi((*iter).c_str()); iter++;
        temp_p.cpu_bursts.push_back(temp);          // read cpu_burst time
        while(iter != tok.end()) // read I/O if available
        {   // First grab io_burst time, and then cpu_burst time
            temp = atoi((*iter).c_str()); iter++;
            temp_p.io_bursts.push_back(temp);
            temp = atoi((*iter).c_str()); iter++;
            temp_p.cpu_bursts.push_back(temp);
        }
        // store processes into startup queue
        startup.push_back(temp_p);
        temp_p = process();     // Clear contents of temp_p
        getline(cin, line);
        // cout << "cur pid is " << startup[pid_count].pid << endl;
    }

    return pid_count; 
}

void qInsert(vector<process> &q, process newQ)
/* Insert a new process object into a priority queue in its proper position
 * based on its priority. 
 */
{
    if (newQ.t_slice == 0)
    {
        newQ.calcTSlice();
        newQ.calcPriority();
    }
    for(vector<process>::iterator i = q.begin(); i != q.end(); ++i)
        if(newQ.priority < (*i).priority)
        {
           q.insert(i, newQ);
           return;;
        }
    q.push_back(newQ);
    return;
}

void decIO(vector<process> &q)
{
    for(vector<process>::iterator i=q.begin(); i != q.end(); ++i)
        (*i).decIOburst();
}

bool complt_arrival(process a, process b) { return a.arrival_t < b.arrival_t; }
bool complt_priority(process a, process b) { return a.priority < b.priority; }

/*******************************************************************************
 *                 Member functions for process class                          *
 ******************************************************************************/

void process::calcPriority()
{
    int bonus = 0;
    if (TCT == 0)
        bonus = 0;
    else if (TCT < TIT)
        bonus = (int)(((1 - TCT/((double)TIT)) * -5) - 0.5);
    else 
        bonus = (int)(((1 - TIT/((double)TCT)) *  5) + 0.5);
    priority = (int)((nice + 20)/39.0 * 30 + 0.5) + 105 + bonus;
}

void process::calcTSlice()
/* Calculates time slice based on priority level and operates on next cpu
 * cpu burst appropriately. 
 */
{
    t_slice = (int)((1 - priority/140.0) * 290 + 0.5) + 10;
}

void process::decTSlice()
{
    if (cur_CPUburst == 0)
    {
        cur_CPUburst = cpu_bursts[0];
        cpu_bursts.erase(cpu_bursts.begin());
    }
    cur_CPUburst--;
    TCT++;
    t_slice--; 
}

void process::decIOburst()
{
    if (cur_IOburst == 0 && io_bursts.size() != 0)
    {
        cur_IOburst = io_bursts[0];
        io_bursts.erase(io_bursts.begin());
    }
    cur_IOburst--;
    TIT++;
}

bool process::isBurstDone() { return cur_CPUburst == 0; }
bool process::isIOBurstDone() { return cur_IOburst == 0; } 
bool process::isExpired() { return t_slice == 0; }
