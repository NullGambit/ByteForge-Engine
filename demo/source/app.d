import forge.container;
import core.stdc.string;
import std.conv;
import std.conv : to;
import forge.fmt;
import std.typecons;
import forge.mem.allocators;
import forge.mem.box;
import forge.mem.arc;
import forge.math.vector;

class Data
{
    int score;
    String name;

    this(S)(int score, auto ref S name)
    {
        this.score = score;
        this.name = String(name);
    }
}

struct Data2
{
    int score;
    String name;

    this(S)(int score, auto ref S name)
    {
        this.score = score;
        this.name = String(name);
    }
}

struct Fields
{
    Data data;
}

import std.traits;

void call(Fn, Args...)(auto ref Fn fn, auto ref Args args)
if (isCallable!Fn)
{
    fn(args);
}

struct Functor
{
    int x = 0;
    char padding;

    void opCall(int y)
    {
        println("hello {}", x + y);
    }
}

struct SignalDelegate(R, Args...)
{
    void *obj;
    R delegate(Args) fnPtr;
}

auto makeDelegate()
{
    SignalDelegate!(void, int) del;

    auto fn = newObj!Functor();

    fn.x = 10;

    del.obj = fn;
    del.fnPtr = &fn.opCall;

    return del;
}

shared uint g_jobsDone;

void job(void*)
{
    import core.atomic;
    import std.random;
    import core.thread;
    import forge.concurrency.util;

    auto delay = uniform(0, 15);

    Thread.sleep(delay.msecs);

    println("finished job #{} on thread {}", g_jobsDone.atomicFetchAdd(1), getThreadName());
}

void main()
{
    import bench.job_bench;

    jobBench();
    // import core.sync.condition;
    // import forge.mem.box;

    // import forge.concurrency.jobs;

    // enum MaxJobsToPush = 126;

    // JobCounter[MaxJobsToPush] counters;

    // foreach (i; 0..MaxJobsToPush)
    // {
    //     startJob(Job(fn: &job, counter: &counters[i]));
    // }

    // foreach (ref counter; counters)
    // {
    //     waitForJobs(&counter);
    // }

    // println("finished all jobs");
    // println(g_jobsDone);

    // assert(g_jobsDone == MaxJobsToPush);

    // stopAllJobThreads();
}
