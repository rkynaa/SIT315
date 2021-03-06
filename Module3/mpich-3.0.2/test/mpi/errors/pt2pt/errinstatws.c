/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *
 *  (C) 2003 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "mpi.h"
#include <stdio.h>
#include "mpitest.h"

/*
static char MTEST_Descrip[] = "Test err in status return, using truncated \
messages for MPI_Waitsome";
*/

int main( int argc, char *argv[] )
{
    int errs = 0;
    MPI_Comm comm;
    MPI_Request r[2];
    MPI_Status  s[2];
    int         indices[2], outcount;
    int errval, errclass;
    int b1[20], b2[20], rank, size, src, dest, i, j;

    MTest_Init( &argc, &argv );

    /* Create some receive requests.  tags 0-9 will succeed, tags 10-19 
       will be used for ERR_TRUNCATE (fewer than 20 messages will be used) */
    comm = MPI_COMM_WORLD;

    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );

    src  = 1;
    dest = 0;
    if (rank == dest) {
	MPI_Errhandler_set( comm, MPI_ERRORS_RETURN );
	errval = MPI_Irecv( b1, 10, MPI_INT, src, 0, comm, &r[0] );
	if (errval) {
	    errs++;
	    MTestPrintError( errval );
	    printf( "Error returned from Irecv\n" );
	}
	errval = MPI_Irecv( b2, 10, MPI_INT, src, 10, comm, &r[1] );
	if (errval) {
	    errs++;
	    MTestPrintError( errval );
	    printf( "Error returned from Irecv\n" );
	}

	/* synchronize */
	errval = MPI_Recv(NULL, 0, MPI_INT, src, 10, comm, MPI_STATUS_IGNORE);
	if (errval) {
	    errs++;
	    MTestPrintError( errval );
	    printf( "Error returned from Recv\n" );
	}
	for (i=0; i<2; i++) {
	    s[i].MPI_ERROR = -1;
	}
	errval = MPI_Waitsome( 2, r, &outcount, indices, s );
	MPI_Error_class( errval, &errclass );
	if (errclass != MPI_ERR_IN_STATUS) {
	    errs++;
	    printf( "Did not get ERR_IN_STATUS in Waitsome.  Got %d.\n", errval );
	}
	else if (outcount != 2) {
	    errs++;
	    printf( "Wait returned outcount = %d\n", outcount );
	}
	else {
	    /* Check for success */
	    for (i=0; i<outcount; i++) {
		j = i;
		/* Indices is the request index */
		if (s[j].MPI_TAG < 10 && s[j].MPI_ERROR != MPI_SUCCESS) {
		    errs++;
		    printf( "correct msg had error class %d\n", 
			    s[j].MPI_ERROR );
		}
		else if (s[j].MPI_TAG >= 10 && s[j].MPI_ERROR == MPI_SUCCESS) {
		    errs++;
		    printf( "truncated msg had MPI_SUCCESS\n" );
		}
	    }
	}

    }
    else if (rank == src) {
	/* Send test messages, then send another message so that the test does
	   not start until we are sure that the sends have begun */
	MPI_Send( b1, 10, MPI_INT, dest, 0, comm );
	MPI_Send( b2, 11, MPI_INT, dest, 10, comm );

	/* synchronize */
	MPI_Ssend( NULL, 0, MPI_INT, dest, 10, comm );
    }

    MTest_Finalize( errs );
    MPI_Finalize();
    return 0;
  
}
