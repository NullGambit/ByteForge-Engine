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

    enum MASK = MaxJobsPerThread - 1;

    void push(Job job)
    {
        auto t = tail.atomicLoad;

        buffer[t & MASK] = job;

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

        job = buffer[h & MASK];

        return true;
    }
}

__gshared List!WorkerThread g_workers;

shared static this()
{
    g_workers.resize(totalCPUs);
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
