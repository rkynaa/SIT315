/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "mpidimpl.h"

/* FIXME - HOMOGENEOUS SYSTEMS ONLY -- no data conversion is performed */

/*
 * MPID_Send()
 */
#undef FUNCNAME
#define FUNCNAME MPID_Send
#undef FCNAME
#define FCNAME MPIDI_QUOTE(FUNCNAME)
int MPID_Send(const void * buf, int count, MPI_Datatype datatype, int rank, 
	      int tag, MPID_Comm * comm, int context_offset,
	      MPID_Request ** request)
{
    MPIDI_msg_sz_t data_sz;
    int dt_contig;
    MPI_Aint dt_true_lb;
    MPID_Datatype * dt_ptr;
    MPID_Request * sreq = NULL;
    MPIDI_VC_t * vc;
#if defined(MPID_USE_SEQUENCE_NUMBERS)
    MPID_Seqnum_t seqnum;
#endif    
    int mpi_errno = MPI_SUCCESS;    
    MPIDI_STATE_DECL(MPID_STATE_MPID_SEND);

    MPIDI_FUNC_ENTER(MPID_STATE_MPID_SEND);

    MPIU_DBG_MSG_FMT(CH3_OTHER,VERBOSE,(MPIU_DBG_FDEST,
                "rank=%d, tag=%d, context=%d", 
		rank, tag, comm->context_id + context_offset));

    if (rank == comm->rank && comm->comm_kind != MPID_INTERCOMM)
    {
	mpi_errno = MPIDI_Isend_self(buf, count, datatype, rank, tag, comm, 
				     context_offset, MPIDI_REQUEST_TYPE_SEND, 
				     &sreq);

	/* In the single threaded case, sending to yourself will cause 
	   deadlock.  Note that in the runtime-thread case, this check
	   will not be made (long-term FIXME) */
#       ifndef MPICH_IS_THREADED
	{
	    if (sreq != NULL && sreq->cc != 0) {
		MPIU_ERR_SETANDJUMP(mpi_errno,MPI_ERR_OTHER,
				    "**dev|selfsenddeadlock");
	    }
	}
#	endif
	if (mpi_errno != MPI_SUCCESS) { MPIU_ERR_POP(mpi_errno); }
	goto fn_exit;
    }

    if (rank == MPI_PROC_NULL)
    {
	goto fn_exit;
    }

    MPIDI_Comm_get_vc_set_active(comm, rank, &vc);

#ifdef ENABLE_COMM_OVERRIDES
    if (vc->comm_ops && vc->comm_ops->send)
    {
	mpi_errno = vc->comm_ops->send( vc, buf, count, datatype, rank, tag, comm, context_offset, &sreq);
	goto fn_exit;
    }
#endif

    MPIDI_Datatype_get_info(count, datatype, dt_contig, data_sz, dt_ptr, 
			    dt_true_lb);


    if (data_sz == 0)
    {
	MPIDI_CH3_Pkt_t upkt;
	MPIDI_CH3_Pkt_eager_send_t * const eager_pkt = &upkt.eager_send;

	MPIU_DBG_MSG(CH3_OTHER,VERBOSE,"sending zero length message");
	MPIDI_Pkt_init(eager_pkt, MPIDI_CH3_PKT_EAGER_SEND);
	eager_pkt->match.parts.rank = comm->rank;
	eager_pkt->match.parts.tag = tag;
	eager_pkt->match.parts.context_id = comm->context_id + context_offset;
	eager_pkt->sender_req_id = MPI_REQUEST_NULL;
	eager_pkt->data_sz = 0;
	
	MPIDI_VC_FAI_send_seqnum(vc, seqnum);
	MPIDI_Pkt_set_seqnum(eager_pkt, seqnum);
	
	MPIU_THREAD_CS_ENTER(CH3COMM,vc);
	mpi_errno = MPIDI_CH3_iStartMsg(vc, eager_pkt, sizeof(*eager_pkt), &sreq);
	MPIU_THREAD_CS_EXIT(CH3COMM,vc);
	/* --BEGIN ERROR HANDLING-- */
	if (mpi_errno != MPI_SUCCESS)
	{
	    MPIU_ERR_SETANDJUMP(mpi_errno,MPI_ERR_OTHER,"**ch3|eagermsg");
	}
	/* --END ERROR HANDLING-- */
	if (sreq != NULL)
	{
	    MPIDI_Request_set_seqnum(sreq, seqnum);
	    MPIDI_Request_set_type(sreq, MPIDI_REQUEST_TYPE_SEND);
	    /* sreq->comm = comm;
	      MPIR_Comm_add_ref(comm); -- not necessary for blocking functions */
	}
	
	goto fn_exit;
    }
    
    /* FIXME: flow control: limit number of outstanding eager messsages 
       containing data and need to be buffered by the receiver */
#ifdef USE_EAGER_SHORT
    if (dt_contig && data_sz <= MPIDI_EAGER_SHORT_SIZE) {
	mpi_errno = MPIDI_CH3_EagerContigShortSend( &sreq, 
					       MPIDI_CH3_PKT_EAGERSHORT_SEND,
					       (char *)buf + dt_true_lb,
					       data_sz, rank, tag, comm, 
					       context_offset );
    }
    else
#endif
    if (data_sz + sizeof(MPIDI_CH3_Pkt_eager_send_t) <=	
	vc->eager_max_msg_sz) {
	if (dt_contig) {
 	    mpi_errno = MPIDI_CH3_EagerContigSend( &sreq, 
						   MPIDI_CH3_PKT_EAGER_SEND,
						   (char *)buf + dt_true_lb,
						   data_sz, rank, tag, comm, 
						   context_offset );
	}
	else {
	    MPIDI_Request_create_sreq(sreq, mpi_errno, goto fn_exit);
	    MPIDI_Request_set_type(sreq, MPIDI_REQUEST_TYPE_SEND);
	    mpi_errno = MPIDI_CH3_EagerNoncontigSend( &sreq, 
                                                      MPIDI_CH3_PKT_EAGER_SEND,
                                                      buf, count, datatype,
                                                      data_sz, rank, tag, 
                                                      comm, context_offset );
	}
    }
    else {
	MPIDI_Request_create_sreq(sreq, mpi_errno, goto fn_exit);
	MPIDI_Request_set_type(sreq, MPIDI_REQUEST_TYPE_SEND);
	mpi_errno = vc->rndvSend_fn( &sreq, buf, count, datatype, dt_contig,
                                     data_sz, dt_true_lb, rank, tag, comm, 
                                     context_offset );
	/* Note that we don't increase the ref count on the datatype
	   because this is a blocking call, and the calling routine 
	   must wait until sreq completes */
    }

 fn_fail:
 fn_exit:
    *request = sreq;

    MPIU_DBG_STMT(CH3_OTHER,VERBOSE,
    {
	if (mpi_errno == MPI_SUCCESS) {
	    if (sreq) {
		MPIU_DBG_MSG_P(CH3_OTHER,VERBOSE,
			 "request allocated, handle=0x%08x", sreq->handle);
	    }
	    else
	    {
		MPIU_DBG_MSG(CH3_OTHER,VERBOSE,
			     "operation complete, no requests allocated");
	    }
	}
    }
		  );
    
    MPIDI_FUNC_EXIT(MPID_STATE_MPID_SEND);
    return mpi_errno;
}
