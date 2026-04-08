module forge.concurrency.local_jobs;

// a local job system that supports at most one producer but many consumers
// this allows for extremely fast and no contention scheduling and execution of jobs
// meaning every job is a high priority job
// as the name suggests this is ideal for local job systems such as ones used by a single subsystem internally

struct LocalJobs(size_t MaxJobsPerThread = 1024)
{
    private
    {
        shared bool m_shouldRun = false;
        List!WorkerThread m_workers;
    }
}
