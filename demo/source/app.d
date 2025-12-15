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

void job1(void*)
{
    println("job1");
}

void job2(void*)
{
    println("job2");
}

void job3(void*)
{
    println("job3");
}

void main()
{
    import core.sync.condition;
    import forge.mem.box;

    auto m = Box!Mutex();
    auto cv = Box!Condition(m);

    cv.notify();

    import forge.concurrency.jobs;

    foreach (_; 0..1)
    {
        JobCounter counter;

        startJob(Job(fn: &job1, counter: &counter));
        startJob(Job(fn: &job1, counter: &counter));
        startJob(Job(fn: &job1, counter: &counter));
        startJob(Job(fn: &job1, counter: &counter));
        startJob(Job(fn: &job2, counter: &counter));
        startJob(Job(fn: &job2, counter: &counter));
        startJob(Job(fn: &job2, counter: &counter));
        startJob(Job(fn: &job3, counter: &counter));
        startJob(Job(fn: &job3, counter: &counter));
        startJob(Job(fn: &job3, counter: &counter));

        waitForJobs(&counter);

        println("finished all jobs");
    }

    stopAllJobThreads();
}
