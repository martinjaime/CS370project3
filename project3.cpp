/* Martin Jaime
 * CS 370
 * Project 3: CPU Scheduling 
 * 2015-03-03
 */ 

#include <iostream>
#include <string>

using namespace std;

class process
{
    private:
        int pid,
            start_t,
            end_t,
            priority,
            t_slice,
                        

}

int main()
{
    // read input
    // clock = 0
    // while (true) 
    //     If processes are to start at this clock tick
    //         insert processes to the active queue
    //         calculate priority and timeslice.
    //     if the cpu is empty, the lowest priority process in the active queue is
    //         put into the cpu. (If more than two processes have the same priority
    //         then FIFO algorithm is used. 
    //     If the lowest priority process in the active queue has a lower priority
    //         than the process in the cpu. If so then preempt. 
    //         Preempted process goes back to the active queue if its time slice is
    //         over. 
    //     Preform CPU (decrement the timeslice of the process in the CPU)
    //     Perform I/O (decrement the I/O burst for all processes in the I/O queue)
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
    //         if p's timceslice is exhausted move to the expired queue and recalc 
    //             the priority and tyimeslice. 
    //         if p still has a timeslice left insert p into the active queue. 
    //     if the startup queue, ready queue, expired queue, I/O queue and the CPU
    //         are all empty, then break out of the while loop. (sim complete). 
    //     if the ready queue and the CPU are empty and the expired queue is not, 
    //         then switch the expired and active queues. Simply swap pointers. 
    //     clock++


    return 0;
}
