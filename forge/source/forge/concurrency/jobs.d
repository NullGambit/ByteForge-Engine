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
    auto selectedWorker = findWorker!(FindLowest.yes)();

    if (job.counter)
    {
        (*job.counter).atomicOp!"+="(1);
    }

    while (selectedWorker.appending.atomicLoad)
    {

    }

    selectedWorker.appending.atomicStore(true);

    auto selectedJob = &selectedWorker.jobs[selectedWorker.tailCursor.atomicLoad];

    *selectedJob = job;

    if (selectedWorker.tailCursor.atomicLoad >= MaxJobsPerThread)
    {
        selectedWorker.tailCursor.atomicStore(0);
    }
    else
    {
        selectedWorker.tailCursor.atomicOp!"+="(1);
    }

    selectedWorker.appending.atomicStore(false);
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

shared uint g_threadCounter;
static uint g_thisCounter;
__gshared bool g_run = true;

struct WorkerThread
{
    Job[MaxJobsPerThread] jobs;
    shared uint headCursor;
    shared uint tailCursor;
    shared bool appending;

    uint available() const pure
    {
        return tailCursor.atomicLoad - headCursor.atomicLoad;
    }
}

__gshared List!WorkerThread g_workers;

shared static this()
{
    g_workers.resize(totalCPUs);
}

static this()
{
    g_thisCounter = g_threadCounter.atomicLoad;
    g_threadCounter.atomicOp!"+="(1);
}

shared static this()
{
    foreach (i; 0..totalCPUs)
    {
        spawn(&worker);
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

    foreach (ref worker; g_workers)
    {
        auto available = worker.available;

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

bool steal(uint head, WorkerThread *thief, WorkerThread *victim)
{
    auto victimAvailable = victim.available;

    if (victimAvailable < 4)
    {
        return false;
    }

    println("found victim {}", victimAvailable);

    import core.stdc.string;
    import std.math;

    victim.appending.atomicStore(true);

    auto victimHead = victim.headCursor.atomicLoad;
    auto victimTail = victim.tailCursor.atomicLoad;

    auto n = cast(int)ceil(victimAvailable * 0.2);

    if (n <= 1)
    {
        return false;
    }

    memcpy(victim.jobs.ptr + victimHead, thief.jobs.ptr + head, Job.sizeof * n);

    victim.tailCursor.atomicOp!"-="(n);

    victim.appending.atomicStore(false);

    println("stole {} from victim", n);

    return true;
}

void worker()
{
    auto thisWorker = &g_workers[g_thisCounter];

    while (g_run)
    {

        auto head = thisWorker.headCursor.atomicLoad;
        auto tail = thisWorker.tailCursor.atomicLoad;

        if (tail - head <= 0 || thisWorker.appending.atomicLoad)
        {
            auto victim = findWorker!(FindLowest.no)();
            auto stole = steal(head, thisWorker, victim);

            if (!stole)
            {
                Thread.yield();
                continue;
            }
        }

        auto selectedJob = &thisWorker.jobs[head];

        selectedJob.fn(selectedJob.param);

        if (head >= MaxJobsPerThread)
        {
            thisWorker.headCursor.atomicStore(0);
        }
        else
        {
            thisWorker.headCursor.atomicOp!"+="(1);
        }

        if (selectedJob.counter)
        {
            (*selectedJob.counter).atomicOp!"-="(1);
        }
    }
}
