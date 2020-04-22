// ParallelJobListLocal.h
//

#pragma once


/*
================================================
idParallelJobListLocal

A job should be at least a couple of 1000 clock cycles in
order to outweigh any job switching overhead. On the other
hand a job should consume no more than a couple of
100,000 clock cycles to maintain a good load balance over
multiple processing units.
================================================
*/
class idParallelJobListLocal : public idParallelJobList {
	friend class idParallelJobManagerLocal;
public:

	virtual void					AddJob(jobRun_t function, void * data);
	virtual CellSpursJob128 *		AddJobSPURS();
	virtual void					InsertSyncPoint(jobSyncType_t syncType);

	// Submit the jobs in this list.
	virtual void					Submit(idParallelJobList * waitForJobList = NULL, int parallelism = JOBLIST_PARALLELISM_DEFAULT);
	// Wait for the jobs in this list to finish. Will spin in place if any jobs are not done.
	virtual void					Wait();
	// Try to wait for the jobs in this list to finish but either way return immediately. Returns true if all jobs are done.
	virtual bool					TryWait();
	// returns true if the job list has been submitted.
	virtual bool					IsSubmitted() const;

	// Get the number of jobs executed in this job list.
	virtual unsigned int			GetNumExecutedJobs() const;
	// Get the number of sync points.
	virtual unsigned int			GetNumSyncs() const;
	// Time at which the job list was submitted.
	virtual uint64_t				GetSubmitTimeMicroSec() const;
	// Time at which execution of this job list started.
	virtual uint64_t				GetStartTimeMicroSec() const;
	// Time at which all jobs in the list were executed.
	virtual uint64_t				GetFinishTimeMicroSec() const;
	// Time the host thread waited for this job list to finish.
	virtual uint64_t				GetWaitTimeMicroSec() const;
	// Get the total time all units spent processing this job list.
	virtual uint64_t				GetTotalProcessingTimeMicroSec() const;
	// Get the total time all units wasted while processing this job list.
	virtual uint64_t				GetTotalWastedTimeMicroSec() const;
	// Time the given unit spent processing this job list.
	virtual uint64_t				GetUnitProcessingTimeMicroSec(int unit) const;
	// Time the given unit wasted while processing this job list.
	virtual uint64_t				GetUnitWastedTimeMicroSec(int unit) const;

	// Get the job list ID
	virtual jobListId_t				GetId() const;
	// Get the color for profiling.
	virtual const idColor *			GetColor() const { return this->color; }

private:
	class idParallelJobList_Threads *	jobListThreads;
	const idColor *						color;

	idParallelJobListLocal(jobListId_t id, jobListPriority_t priority, unsigned int maxJobs, unsigned int maxSyncs, const idColor * color);
	~idParallelJobListLocal();
};


/*
================================================================================================

idJobThread

================================================================================================
*/

const int JOB_THREAD_STACK_SIZE = 256 * 1024;	// same size as the SPU local store

struct threadJobList_t {
	idParallelJobList_Threads *	jobList;
	int							version;
};

class idJobThread : public idSysThread {
public:
	idJobThread();
	~idJobThread();

	void						Start(core_t core, unsigned int threadNum);

	void						AddJobList(idParallelJobList_Threads * jobList);

private:
	threadJobList_t				jobLists[MAX_JOBLISTS];	// cyclic buffer with job lists
	unsigned int				firstJobList;			// index of the last job list the thread grabbed
	unsigned int				lastJobList;			// index where the next job list to work on will be added
	idSysMutex					addJobMutex;

	unsigned int				threadNum;

	virtual int					Run();
};

// WINDOWS LOGICAL PROCESSOR LIMITS:
//
// http://download.microsoft.com/download/5/7/7/577a5684-8a83-43ae-9272-ff260a9c20e2/Hyper-thread_Windows.doc
//
//											Physical	Logical (Cores + HT)
// Windows XP Home Edition					1			2 
// Windows XP Professional					2			4 
// Windows Server 2003, Standard Edition	4			8 
// Windows Server 2003, Enterprise Edition	8			16 
// Windows Server 2003, Datacenter Edition	32			32
//
// Windows Vista							?			?
//
// Windows 7 Starter						1			32/64
// Windows 7 Home Basic						1			32/64
// Windows 7 Professional					2			32/64
//
//
// Hyperthreading is not dead yet.  Intel's Core i7 Processor is quad-core with HT for 8 logicals.


#define MAX_JOB_THREADS		8
#define NUM_JOB_THREADS		"8"
#define JOB_THREAD_CORES	{	CORE_ANY, CORE_ANY, CORE_ANY, CORE_ANY,	\
								CORE_ANY, CORE_ANY, CORE_ANY, CORE_ANY,	\
								CORE_ANY, CORE_ANY, CORE_ANY, CORE_ANY,	\
								CORE_ANY, CORE_ANY, CORE_ANY, CORE_ANY,	\
								CORE_ANY, CORE_ANY, CORE_ANY, CORE_ANY,	\
								CORE_ANY, CORE_ANY, CORE_ANY, CORE_ANY,	\
								CORE_ANY, CORE_ANY, CORE_ANY, CORE_ANY,	\
								CORE_ANY, CORE_ANY, CORE_ANY, CORE_ANY }


idCVar jobs_numThreads("jobs_numThreads", NUM_JOB_THREADS, CVAR_INTEGER | CVAR_NOCHEAT, "number of threads used to crunch through jobs", 0, MAX_JOB_THREADS);

class idParallelJobManagerLocal : public idParallelJobManager {
public:
	virtual						~idParallelJobManagerLocal() {}

	virtual void				Init();
	virtual void				Shutdown();

	virtual idParallelJobList *	AllocJobList(jobListId_t id, jobListPriority_t priority, unsigned int maxJobs, unsigned int maxSyncs, const idColor * color);
	virtual void				FreeJobList(idParallelJobList * jobList);

	virtual int					GetNumJobLists() const;
	virtual int					GetNumFreeJobLists() const;
	virtual idParallelJobList *	GetJobList(int index);

	virtual void				RegisterJob(jobRun_t function, const char * name);

	virtual int					GetNumProcessingUnits();

	virtual void				WaitForAllJobLists();

	void						Submit(idParallelJobList_Threads * jobList, int parallelism);

private:
	idJobThread						threads[MAX_JOB_THREADS];
	unsigned int					maxThreads;
	int								numPhysicalCpuCores;
	int								numLogicalCpuCores;
	int								numCpuPackages;
	idStaticList< idParallelJobListLocal *, MAX_JOBLISTS >	jobLists;
};
