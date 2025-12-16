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

    selectedWorker.pushBottom(job);
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
    shared uint head;
    shared uint tail;

    Job[MaxJobsPerThread] buffer;

    @property
    static uint mask() pure
    {
        return MaxJobsPerThread - 1;
    }

    void pushBottom(Job job)
    {
        auto t = tail.atomicLoad;
        auto h = head.atomicLoad;

        assert (t - h < MaxJobsPerThread, "buffer overflow");

        buffer[t & mask] = job;

        tail.atomicStore(t + 1);
    }

    bool popBottom(out Job job)
    {
        auto h = head.atomicLoad;
        auto t = tail.atomicLoad;

        if (h >= t)
        {
            return false;
        }

        job = buffer[h & mask];

        head.atomicStore(h + 1);

        if (h == t)
        {
            if (cas(&head, t, t + 1) != h)
            {
                tail.atomicStore(t + 1);
                return false;
            }

            tail.atomicStore(t + 1);
        }

        return true;
    }

    bool stealTop(out Job job)
    {
        auto h = atomicLoad(head);
        auto t = atomicLoad(tail);

        if (h >= t)
        {
            return false;
        }

        job = buffer[h & mask];

        if (cas(&head, h, h + 1) != h)
        {
            return false;
        }

        return true;
    }
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

    uint lastAvailable;

    static if (FindLowestFlag)
    {
        lastAvailable = uint.max;
    }

    while (selectedWorker == null)
    {
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
    }

    return selectedWorker;
}

bool stealFromOther(uint thisIndex, out Job job)
{
    foreach (i, ref worker; g_workers)
    {
        if (i != thisIndex && worker.stealTop(job))
        {
            return true;
        }
    }

    return false;
}

void worker(uint index)
{
    auto thisWorker = &g_workers[index];

    while (g_run)
    {
        Job job;

        if (!thisWorker.popBottom(job))
        {
            Thread.yield();
            continue;
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
