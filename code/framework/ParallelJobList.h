/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").  

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#ifndef __PARALLELJOBLIST_H__
#define __PARALLELJOBLIST_H__

struct CellSpursJob128;
class idColor;

typedef void ( * jobRun_t )( void * );

enum jobSyncType_t {
	SYNC_NONE,
	SYNC_SIGNAL,
	SYNC_SYNCHRONIZE
};

// NOTE: keep in sync with jobNames[]
enum jobListId_t {
	JOBLIST_RENDERER	= 0,
	JOBLIST_GAME		= 1,
	JOBLIST_UTILITY				= 9,			// won't print over-time warnings

	MAX_JOBLISTS				= 32			// the editor may cause quite a few to be allocated
};

//compile_time_assert( CONST_ISPOWEROFTWO( MAX_JOBLISTS ) );

enum jobListPriority_t {
	JOBLIST_PRIORITY_NONE,
	JOBLIST_PRIORITY_LOW,
	JOBLIST_PRIORITY_MEDIUM,
	JOBLIST_PRIORITY_HIGH
};

enum jobListParallelism_t {
	JOBLIST_PARALLELISM_DEFAULT			= -1,	// use "jobs_numThreads" number of threads
	JOBLIST_PARALLELISM_MAX_CORES		= -2,	// use a thread for each logical core (includes hyperthreads)
	JOBLIST_PARALLELISM_MAX_THREADS		= -3	// use the maximum number of job threads, which can help if there is IO to overlap
};

#define assert_spu_local_store( ptr )
#define assert_not_spu_local_store( ptr )

/*
================================================
idParallelJobList

A job should be at least a couple of 1000 clock cycles in
order to outweigh any job switching overhead. On the other
hand a job should consume no more than a couple of
100,000 clock cycles to maintain a good load balance over
multiple processing units.
================================================
*/
class idParallelJobList {
public:

	virtual void					AddJob( jobRun_t function, void * data ) = 0;
	virtual CellSpursJob128 *		AddJobSPURS() = 0;
	virtual void					InsertSyncPoint( jobSyncType_t syncType ) = 0;

	// Submit the jobs in this list.
	virtual void					Submit( idParallelJobList * waitForJobList = NULL, int parallelism = JOBLIST_PARALLELISM_DEFAULT ) = 0;
	// Wait for the jobs in this list to finish. Will spin in place if any jobs are not done.
	virtual void					Wait() = 0;
	// Try to wait for the jobs in this list to finish but either way return immediately. Returns true if all jobs are done.
	virtual bool					TryWait() = 0;
	// returns true if the job list has been submitted.
	virtual bool					IsSubmitted() const = 0;

	// Get the number of jobs executed in this job list.
	virtual unsigned int			GetNumExecutedJobs() const = 0;
	// Get the number of sync points.
	virtual unsigned int			GetNumSyncs() const = 0;
	// Time at which the job list was submitted.
	virtual uint64_t				GetSubmitTimeMicroSec() const = 0;
	// Time at which execution of this job list started.
	virtual uint64_t				GetStartTimeMicroSec() const = 0;
	// Time at which all jobs in the list were executed.
	virtual uint64_t				GetFinishTimeMicroSec() const = 0;
	// Time the host thread waited for this job list to finish.
	virtual uint64_t				GetWaitTimeMicroSec() const = 0;
	// Get the total time all units spent processing this job list.
	virtual uint64_t				GetTotalProcessingTimeMicroSec() const = 0;
	// Get the total time all units wasted while processing this job list.
	virtual uint64_t				GetTotalWastedTimeMicroSec() const = 0;
	// Time the given unit spent processing this job list.
	virtual uint64_t				GetUnitProcessingTimeMicroSec( int unit ) const = 0;
	// Time the given unit wasted while processing this job list.
	virtual uint64_t				GetUnitWastedTimeMicroSec( int unit ) const = 0;

	// Get the job list ID
	virtual jobListId_t				GetId() const = 0;
	// Get the color for profiling.
	virtual const idColor *			GetColor() const = 0;
};

/*
================================================
idParallelJobManager

This is the only interface through which job lists
should be allocated or freed.
================================================
*/
class idParallelJobManager {
public:
	virtual						~idParallelJobManager() {}

	virtual void				Init() = 0;
	virtual void				Shutdown() = 0;

	virtual idParallelJobList *	AllocJobList( jobListId_t id, jobListPriority_t priority, unsigned int maxJobs, unsigned int maxSyncs, const idColor * color ) = 0;
	virtual void				FreeJobList( idParallelJobList * jobList ) = 0;

	virtual int					GetNumJobLists() const = 0;
	virtual int					GetNumFreeJobLists() const = 0;
	virtual idParallelJobList *	GetJobList( int index ) = 0;

	virtual void				RegisterJob(jobRun_t function, const char * name) = 0;

	virtual int					GetNumProcessingUnits() = 0;

	virtual void				WaitForAllJobLists() = 0;
};

extern idParallelJobManager *	parallelJobManager;

#endif // !__PARALLELJOBLIST_H__
