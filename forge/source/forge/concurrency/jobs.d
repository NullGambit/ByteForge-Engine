module forge.concurrency.jobs;

import core.atomic;
import core.thread;
import core.sync.condition;

alias JobCounter = shared(int);

enum MaxJobsPerThread = 1024;

struct Jop
{
}

struct JobData
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

void startJob(JobData job)
{
    assert (job.fn != null, "the job fn must not be null");

    // auto selectedWorker = findWorker!(FindLowest.yes)();
    import std.random;

    auto index = uniform(0, totalCPUs);

    auto selectedWorker = &g_workers[index];

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

void initJobPool(uint maxThreads = totalCPUs)
{
    if (g_run.atomicLoad)
    {
        return;
    }

    g_run.atomicStore(true);

    g_workers.resize(maxThreads);

    foreach (i; 0..maxThreads)
    {
        spawn(&worker, i);
    }
}

private:

import forge.container.list;
import std.parallelism : totalCPUs;
import std.concurrency;
import forge.mem.box;

__gshared bool g_run = false;

struct WorkerThread
{
    shared long head;
    shared long tail;

    JobData[MaxJobsPerThread] buffer;

    enum MASK = MaxJobsPerThread - 1;

    void push(JobData job)
    {
        auto t = tail.atomicLoad;

        buffer[t & MASK] = job;

        t.atomicStore(t + 1);
    }

    bool pop(out JobData job)
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

        job = buffer[h & MASK];

        return job.fn != null;
    }
}

__gshared List!WorkerThread g_workers;

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

        if (cond && available < MaxJobsPerThread)
        {
            lastAvailable = available;
            selectedWorker = &worker;
        }
    }

    return selectedWorker;
}

bool stealFromOther(uint thisIndex, out JobData job)
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
        JobData job;

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
