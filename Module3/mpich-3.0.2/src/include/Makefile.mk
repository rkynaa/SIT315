## -*- Mode: Makefile; -*-
## vim: set ft=automake :
##
## (C) 2011 by Argonne National Laboratory.
##     See COPYRIGHT in top-level directory.
##

include $(top_srcdir)/src/include/thread/Makefile.mk

# nodist_ b/c these are created by config.status and should not be distributed
nodist_include_HEADERS += src/include/mpi.h

## Internal headers that are created by config.status from a corresponding
## ".h.in" file.  This ensures that these files are _not_ distributed, which is
## important because they contain lots of info that is computed by configure.
nodist_noinst_HEADERS +=     \
    src/include/glue_romio.h \
    src/include/mpichinfo.h \
    src/include/mpichconf.h  \
    src/include/mpichtimer.h

## listed here in BUILT_SOURCES to ensure that if glue_romio.h is out of date
## that it will be rebuilt before make recurses into src/mpi/romio and runs
## make.  This isn't normally necessary when automake is tracking all the
## dependencies, but that's not true for SUBDIRS packages
BUILT_SOURCES += src/include/glue_romio.h

noinst_HEADERS +=                   \
    src/include/bsocket.h           \
    src/include/mpi_attr.h          \
    src/include/mpi_f77interface.h  \
    src/include/mpi_fortlogical.h   \
    src/include/mpi_lang.h          \
    src/include/mpiallstates.h      \
    src/include/mpibase.h           \
    src/include/mpibsend.h          \
    src/include/mpich_param_vals.h  \
    src/include/mpichconfconst.h    \
    src/include/mpidbg.h            \
    src/include/mpierror.h          \
    src/include/mpierrs.h           \
    src/include/mpiext.h            \
    src/include/mpifunc.h           \
    src/include/mpihandlemem.h      \
    src/include/mpiimpl.h           \
    src/include/mpiimplthread.h     \
    src/include/mpiimplthreadpost.h \
    src/include/mpiinstr.h          \
    src/include/mpiiov.h            \
    src/include/mpimem.h            \
    src/include/mpir_nbc.h          \
    src/include/mpishared.h         \
    src/include/mpistates.h         \
    src/include/mpitimerimpl.h      \
    src/include/mpitypedefs.h       \
    src/include/mpiu_ex.h           \
    src/include/mpiu_thread.h       \
    src/include/mpiu_utarray.h      \
    src/include/mpiutil.h           \
    src/include/nopackage.h         \
    src/include/oputil.h            \
    src/include/pmi.h               \
    src/include/pmi2.h              \
    src/include/rlog.h              \
    src/include/rlog_macros.h       \
    src/include/oputil.h            \
    src/include/mpiinfo.h

src/include/param_vals.h: src/util/param/params.yml
	$(top_srcdir)/maint/genparams
