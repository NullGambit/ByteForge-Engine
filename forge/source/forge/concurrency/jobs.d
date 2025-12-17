module forge.concurrency.jobs;

import core.atomic;
import core.thread;
import core.sync.condition;

alias JobCounter = shared(int);

enum MaxJobsPerThread = 1024;

struct Job
{
    void function(void*) fn;
    void delegate() onFinish;
    void *param;
    JobCounter *counter;
}

bool isDone(JobCounter counter)
{
    return counter.atomicLoad <= 0;
}

void startJob(Job job)
{
    assert (job.fn != null, "the job fn must not be null");

    auto selectedWorker = findWorker!(FindLowest.yes)();

    assert (selectedWorker != null);

    if (job.counter)
    {
        (*job.counter).atomicFetchAdd(1);
    }

    selectedWorker.push(job);
}

void waitForJobs(JobCounter *counter)
{
    while (!isDone(*counter))
    {
        Thread.yield();
    }
}

void stopAllJobThreads()
{
    g_run = false;
}

private:

import forge.container.list;
import std.parallelism : totalCPUs;
import std.concurrency;
import forge.mem.box;

__gshared bool g_run = true;

struct WorkerThread
{
    shared long head;
    shared long tail;

    Job[MaxJobsPerThread] buffer;

    @property
    static uint mask() pure
    {
        return MaxJobsPerThread - 1;
    }

    void push(Job job)
    {
        auto t = tail.atomicLoad;

        buffer[t & mask] = job;

        tail.atomicStore(t + 1);
    }

    bool pop(out Job job)
    {
        auto h = head.atomicLoad;
        auto t = tail.atomicLoad;

        if (h >= t)
        {
            return false;
        }

        if (!cas(&head, h, h + 1))
        {
            return false;
        }

        job = buffer[h & mask];

        return true;
    }

    bool stealTop(out Job job)
    {
        auto h = atomicLoad(head);
        auto t = atomicLoad(tail);

        if (h < t)
        {
            job = buffer[h & mask];

            return cas(&head, h, h + 1);
        }

        return false;
    }
    // void push(Job job)
    // {
    //     auto b = bottom.atomicLoad;

    //     buffer[b & mask] = job;

    //     bottom.atomicStore(b + 1);
    // }

    // bool pop(out Job job)
    // {
    //     auto t = top.atomicLoad;
    //     auto b = bottom.atomicLoad;

    //     if (t >= b)
    //     {
    //         return false;
    //     }

    //     if (!cas(&top, t, t + 1))
    //     {
    //         return false;
    //     }

    //     job = buffer[t & mask];

    //     return true;
    // }

    // bool stealTop(out Job job)
    // {
    //     auto t = atomicLoad(top);
    //     auto b = atomicLoad(bottom);

    //     if (t < b)
    //     {
    //         job = buffer[t & mask];

    //         return cas(&top, t, t + 1);
    //     }

    //     return false;
    // }
}

__gshared List!WorkerThread g_workers;

shared static this()
{
    g_workers.resize(totalCPUs);
    // foreach (_; 0..totalCPUs)
    // {
    //     import core.lifetime;
    //     auto worker = WorkerThread(0);
    //     g_workers.append(move(worker));
    // }
}

shared static this()
{
    foreach (i; 0..totalCPUs)
    {
        spawn(&worker, i);
    }
}

import forge.fmt;
import std.typecons;

alias FindLowest = Flag!"FindLowest";

WorkerThread* findWorker(FindLowest FindLowestFlag)()
{
    WorkerThread *selectedWorker;

    long lastAvailable;

    static if (FindLowestFlag)
    {
        lastAvailable = uint.max;
    }

    foreach (ref worker; g_workers)
    {
        auto available = worker.tail.atomicLoad - worker.head.atomicLoad;

        bool cond;

        static if (FindLowestFlag)
        {
            cond = available < lastAvailable;
        }
        else
        {
            cond = available >= lastAvailable;
        }

        if (cond)
        {
            lastAvailable = available;
            selectedWorker = &worker;
        }
    }

    return selectedWorker;
}

bool stealFromOther(uint thisIndex, out Job job)
{
    foreach (i, ref worker; g_workers)
    {
        if (i != thisIndex && worker.pop(job))
        {
            return true;
        }
    }

    return false;
}

void worker(uint index)
{
    import forge.concurrency.util;
    import forge.fmt;

    auto name = format("worker#{}", index);

    setThreadName(name.toString);

    auto thisWorker = &g_workers[index];

    while (g_run)
    {
        Job job;

        if (!thisWorker.pop(job))
        {
            if (!stealFromOther(index, job))
            {
                Thread.yield();
                continue;
            }
        }

        job.fn(job.param);

        if (job.onFinish)
        {
            job.onFinish();
        }

        if (job.counter)
        {
            (*job.counter).atomicFetchSub(1);
        }
    }
}


// bool steal(uint head, WorkerThread *thief, WorkerThread *victim)
// {
//     auto victimAvailable = victim.available;

//     if (victimAvailable < 4)
//     {
//         return false;
//     }

//     println("found victim {}", victimAvailable);

//     import core.stdc.string;
//     import std.math;

//     auto victimHead = victim.headCursor.atomicLoad;
//     auto victimTail = victim.tailCursor.atomicLoad;

//     auto n = cast(int)ceil(victimAvailable * 0.2);

//     if (n <= 1)
//     {
//         return false;
//     }

//     memcpy(victim.jobs.ptr + victimHead, thief.jobs.ptr + head, Job.sizeof * n);

//     victim.tailCursor.atomicOp!"-="(n);

//     println("stole {} from victim", n);

//     return true;
// }

// module forge.concurrency.jobs;

// import core.atomic;
// import core.thread;
// import core.sync.condition;

// alias JobCounter = shared(int);

// struct Job
// {
//     void function(void*) fn;
//     void delegate() onFinish;
//     void *param;
//     JobCounter *counter;
// }

// bool isDone(JobCounter counter)
// {
//     return counter.atomicLoad!(MemoryOrder.raw)() <= 0;
// }

// void startJob(Job job)
// {
//     assert (job.fn != null, "the job fn must not be null");

//     auto selectedWorker = findWorker!(FindLowest.yes)();

//     assert (selectedWorker != null);

//     if (job.counter)
//     {
//         (*job.counter).atomicFetchAdd(1);
//     }

//     selectedWorker.acquireLock();

//     selectedWorker.backBuffer.append(job);

//     selectedWorker.pendingJobs.atomicFetchAdd(1);

//     selectedWorker.releaseLock();
// }

// void waitForJobs(JobCounter *counter)
// {
//     while (!isDone(*counter))
//     {
//         Thread.yield();
//     }
// }

// void stopAllJobThreads()
// {
//     g_run = false;
// }

// private:

// import forge.container.list;
// import std.parallelism : totalCPUs;
// import std.concurrency;
// import forge.mem.box;

// shared uint g_threadCounter;
// static uint g_thisCounter;
// __gshared bool g_run = true;

// enum WorkerState
// {
//     Executing = 1 << 0,
//     Swapping = 1 << 1,
//     Queueing = 1 << 2
// }

// struct WorkerThread
// {
//     List!Job backBuffer;
//     List!Job executingJobs;

//     shared uint pendingJobs;

//     shared bool lock;

//     void swap()
//     {
//         import core.lifetime;

//         acquireLock();

//         auto temp = move(backBuffer);

//         move(executingJobs, backBuffer);
//         move(temp, executingJobs);

//         pendingJobs.atomicStore(0);

//         releaseLock();
//     }

//     void acquireLock(bool Yield = false)()
//     {
//         while (true)
//         {
//             if (lock.atomicLoad == false)
//             {
//                 if (cas(&lock, false, true))
//                 {
//                     break;
//                 }
//             }

//             static if (Yield)
//             {
//                 Thread.yield();
//             }
//         }
//     }

//     void releaseLock()
//     {
//         lock.atomicStore(false);
//     }
// }

// __gshared List!WorkerThread g_workers;

// shared static this()
// {
//     g_workers.resize(totalCPUs);
//     // foreach (_; 0..totalCPUs)
//     // {
//     //     import core.lifetime;
//     //     auto worker = WorkerThread(0);
//     //     g_workers.append(move(worker));
//     // }
// }

// static this()
// {
//     g_thisCounter = g_threadCounter.atomicLoad;
//     g_threadCounter.atomicOp!"+="(1);
// }

// shared static this()
// {
//     foreach (i; 0..totalCPUs)
//     {
//         spawn(&worker);
//     }
// }

// import forge.fmt;
// import std.typecons;

// alias FindLowest = Flag!"FindLowest";

// WorkerThread* findWorker(FindLowest FindLowestFlag)()
// {
//     WorkerThread *selectedWorker;

//     uint lastAvailable;

//     static if (FindLowestFlag)
//     {
//         lastAvailable = uint.max;
//     }

//     while (selectedWorker == null)
//     {
//         foreach (ref worker; g_workers)
//         {
//             auto available = worker.pendingJobs.atomicLoad;

//             bool cond;

//             static if (FindLowestFlag)
//             {
//                 cond = available < lastAvailable;
//             }
//             else
//             {
//                 cond = available >= lastAvailable;
//             }

//             if (cond)
//             {
//                 lastAvailable = available;
//                 selectedWorker = &worker;
//             }
//         }
//     }

//     return selectedWorker;
// }

// void worker()
// {
//     auto thisWorker = &g_workers[g_thisCounter];

//     while (g_run)
//     {
//         if (thisWorker.executingJobs.isEmpty)
//         {
//             if (thisWorker.pendingJobs.atomicLoad > 0)
//             {
//                 thisWorker.swap();
//             }
//             else
//             {
//                 Thread.yield();
//                 continue;
//             }
//         }

//         auto selectedJob = thisWorker.executingJobs.pop();

//         selectedJob.fn(selectedJob.param);

//         if (selectedJob.onFinish)
//         {
//             selectedJob.onFinish();
//         }

//         if (selectedJob.counter)
//         {
//             (*selectedJob.counter).atomicFetchSub(1);
//         }
//     }
// }
