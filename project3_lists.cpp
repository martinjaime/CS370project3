/* Martin Jaime
 * CS 370
 * Project 3: a CPU Time Lord
 * 2015-03-03
 */ 

#include <iostream>
#include <string>
#include <stdlib.h>
#include <list>
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
        cur_CPUburst,   // Currently running CPU burst time
        cur_IOburst,    // Currently running IO burst time
        TAT,            // Turn around time
        TCT,            // Total cpu time: sum of all cpu bursts
        TIT,            // Total I/O time
        WT,             // Waiting time: TAT - TCT - TIT(Total IO time)
        CUT;            // % of CPU utilization time = TCT / TAT (nearest 10th)
    list<int> cpu_bursts, 
        io_bursts;  // io_bursts should be cpu_bursts-1
                    
    process()
    {
        pid = 0; arrival_t = 0; start_t = 0; end_t = 0; priority = 0;
        nice = 0; cpu_count = 0; t_slice = 0; TAT = 0; TCT = 0; TIT = 0;
        WT = 0; CUT = 0;
        list<int>().swap(cpu_bursts);
        list<int>().swap(io_bursts); 
    }
    void calcPriority();
    void calcTSlice();
    void decTSlice(); 
    void decIOburst();
    // add an insert function to insert in appropriate spot. 

};

void decIO(list<process>&);
void qInsert(list<process>&, process);
void writeStartup(list<process>&);
bool complt_arrival(process, process);
bool complt_priority(process, process);

int main()
{
    list<process> startup, ioqueue, *active, *expired, *tempQ; // Queues. 
    list<process> cpu;
    int clock = 0;
    active = new list<process>;
    expired  = new list<process>;

    writeStartup(startup);
    // Sort processes by arrival
    sort(startup.begin(), startup.end(), complt_arrival);
    /*for(list<process>::iterator i = startup.begin(); i != startup.end();  ++i)
        cout << (*i).arrival_t << endl;*/
    while (true)
    {
        // Check startup q for arriving processes and place those in active q
        for(list<process>::iterator i=startup.begin(); i != startup.end(); ++i) 
        {
            if ((*i).arrival_t == clock) 
            {
                (*i).calcPriority();
                (*i).calcTSlice();
                qInsert(*active, *i);
                startup.erase(*i);
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
            (*active).erase((*active).begin()); 
        }
        // Or check if current process needs to be preempted. 
        else if ((*active)[0].priority < cpu[0].priority) 
        {   
            cpu.push_back((*active)[0]);
            if (cpu[0].t_slice != 0)
                qInsert(*active, cpu[0]);
            else
                qInsert(*expired, cpu[0]);
            cpu.erase(cpu.begin());
        }
        cpu[0].decTSlice();
        decIO(ioqueue);
        
        if (cpu.size() > 0)
        {

        }

        if (clock > 250) break;

    //- while(true)
    //-     if a process is to start at this clock tick
    //-         insert processes to the active queue
    //-         calculate priority and timeslice.
    //          set current cpu burst time
    //-     if the cpu is empty, the lowest priority process in the active queue
    //-         is put into the cpu. (If more than two processes have the same 
    //-         priority then FIFO algorithm is used. 
    //-     If the lowest priority process in the active queue has a lower priority
    //-         than the process in the cpu. If so then preempt. 
    //-         Preempted process goes back to the active queue if its time slice is
    //-         not over. 
    //-     Preform CPU (decrement the timeslice of the process in the CPU)
    //-     Perform I/O (decrement the I/O burst for all processes in the I/O queue)
    //     if there is a process p in the CPU 
    //         if curent CPU burst for p is exhausted
    //             if p is done with ALL cpu bursts
    //                 send to the finished queue.
    //             else p is not done with all CPU bursts (which probably means
    //                 there is still some I/O burst) send to the I/O queue. 
    //         if p's timeslice is exhausted send to the expired queue and recalc
    //             priority and timeslice
    //     if thre is any process in the I/O queue that is finished with its I/O
    //         burst (there may be more than one process p).
    //         if p's timceslice is exhausted move to the expired queue and 
    //             recalc the priority and tyimeslice. 
    //         if p still has a timeslice left insert p into the active queue. 
    //     if the startup queue, ready queue, expired queue, I/O queue and the CPU
    //         are all empty, then break out of the while loop. (sim complete). 
    //     if the ready queue and the CPU are empty and the expired queue is not, 
    //         then switch the expired and active queues. Simply swap pointers. 
    //     clock++
        clock++;
    }


    return 0;
}

void writeStartup(list<process> &startup)
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
        pid_count++;
    }

    return;
}

void qInsert(list<process> &q, process newQ)
/* Insert a new process object into a priority queue in its proper position
 * based on its priority. 
 */
{
    if (newQ.t_slice == 0)
    {
        newQ.calcTSlice();
        newQ.calcPriority();
    }
    for(list<process>::iterator i = q.begin(); i != q.end(); ++i)
    {
        if(newQ.priority < (*i).priority)
        {
           q.insert(i, newQ);
           return;;
        }
    }
    q.push_back(newQ);
    return;
}

void decIO(list<process> &q)
{
    for(list<process>::iterator i=q.begin(); i != q.end(); ++i)
    {
        if((*i).io_bursts.back() > 0)
            (*i).cur_IOburst--; // decrement last io burst. 
    }
}

bool complt_arrival(process a, process b)
{
    return a.arrival_t < b.arrival_t; 
}

bool complt_priority(process a, process b)
{
    return a.priority < b.priority;
}

/*******************************************************************************
 *                 Member functions for process class                          *
 ******************************************************************************/

void process::calcPriority()
{
    int bonus;
    if (TCT == 0)
        bonus = 0;
    else if (TCT < TIT)
        bonus = (int)(((1 - TCT/((double)TIT)) * -5) - 0.5);
    else 
        bonus = (int)(((1 - TIT/((double)TCT)) *  5) + 0.5);
    priority = (int)((nice + 20)/39.0 * 30 + 0.5) + 105 + bonus;
    return;
}

void process::calcTSlice()
/* Calculates time slice based on priority level and operates on next cpu
 * cpu burst appropriately. 
 */
{
    t_slice = (int)((1 - priority/140.0) * 290 + 0.5) + 10;
    cur_CPUburst = cpu_bursts[0];
    cpu_bursts.erase(cpu_bursts.begin());
    return;
}

void process::decTSlice()
{
    t_slice--; 
    cur_CPUburst--;
    return;
}

void process::decIOburst()
{
    cur_IOburst--;
    return;
}
