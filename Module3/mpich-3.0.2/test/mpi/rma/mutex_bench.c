/*
 * Copyright (C) 2013. See COPYRIGHT in top-level directory.
 */

/** MPI Mutex test -- James Dinan <dinan@mcs.anl.gov>
  *
  * All processes create a mutex then lock+unlock it N times.
  */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <mpi.h>
#include "mcs-mutex.h"

#define NUM_ITER    1000
#define NUM_MUTEXES 1

const int verbose = 0;
double delay_ctr = 0.0;

int main(int argc, char ** argv) {
  int rank, nproc, i;
  double t_mpix_mtx, t_mcs_mtx;
  MPI_Comm mtx_comm;
  MCS_Mutex mcs_mtx;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

#ifdef USE_WIN_SHARED
  MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, rank,
                      MPI_INFO_NULL, &mtx_comm);
#else
  mtx_comm = MPI_COMM_WORLD;
#endif

  MCS_Mutex_create(0, mtx_comm, &mcs_mtx);

  MPI_Barrier(MPI_COMM_WORLD);
  t_mcs_mtx = MPI_Wtime();

  for (i = 0; i < NUM_ITER; i++) {
    /* Combining trylock and lock here is helpful for testing because it makes
     * CAS and Fetch-and-op contend for the tail pointer. */
    if (rank % 2) {
      int success = 0;
      while (!success) {
        MCS_Mutex_trylock(mcs_mtx, &success);
      }
    }
    else {
        MCS_Mutex_lock(mcs_mtx);
    }
    MCS_Mutex_unlock(mcs_mtx);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  t_mcs_mtx = MPI_Wtime() - t_mcs_mtx;

  MCS_Mutex_free(&mcs_mtx);

  if (rank == 0) {
      if (verbose) {
          printf("Nproc %d, MCS Mtx = %f us\n", nproc, t_mcs_mtx/NUM_ITER*1.0e6);
      }
  }

  if (mtx_comm != MPI_COMM_WORLD)
      MPI_Comm_free(&mtx_comm);

  MTest_Finalize(0);
  MPI_Finalize();

  return 0;
}
