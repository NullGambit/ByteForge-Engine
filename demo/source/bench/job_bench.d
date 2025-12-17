module bench.job_bench;

import forge.fmt;

private struct FibonacciRange
{
    // States of the Fibonacci generator
    int a = 1, b = 1;

    // The fibonacci range never ends
    enum empty = false;

    // Peek at the first element
    int front() const @property
    {
        return a;
    }

    // Remove the first element
    void popFront()
    {
        auto t = a;
        a = b;
        b = t + b;
    }
}

private void job(int FibMax)(void*)
{
    FibonacciRange fib;

    import std.range : drop, take;
    import std.algorithm.iteration :
        filter, sum;

    auto fib10 = fib.take(FibMax);
}

void jobBench()
{
    import std.datetime.stopwatch;

    enum RunCount = 1;
    enum MaxJobsToPush = 4096 * 2;
    enum FibMax = 1000;

    void runForgeJob1(string name)()
    {
        import forge.concurrency.jobs;

        auto doBench = ()
        {

            JobCounter[MaxJobsToPush] counters;

            foreach (i; 0..MaxJobsToPush)
            {
                startJob(Job(fn: &job!(FibMax), counter: &counters[i]));
            }

            foreach (ref counter; counters)
            {
                waitForJobs(&counter);
            }

            stopAllJobThreads();
        };

        auto results = benchmark!doBench(RunCount);

        println("{}: {}", name, results[0]);
    }

    void runStdJob()
    {
        auto doBench = ()
        {
            import std.parallelism;

            auto pool = new TaskPool(totalCPUs);

            foreach (i; 0..MaxJobsToPush)
            {
                auto t = task!(job!(FibMax))(null);
                pool.put(t);
            }

            pool.finish();

            pool.stop();
        };

        auto results = benchmark!doBench(RunCount);

        println("std: {}", results[0]);
    }

    runForgeJob1!("jobs1")();
    runStdJob();

}
