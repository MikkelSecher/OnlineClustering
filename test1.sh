#!/bin/csh
#
# OPEN-MPI EXAMPLE.
#
# Ask PBS for 4 nodes.
# 
#PBS -l nodes=2
#
# When asking for multiple nodes this job script will only run
# on the first node in the list of nodes allocated to the job.
# If executable(s) need input files and/or produce output files
# on the (other) local execution nodes this script must take care of 
# distributing input files and collecting output files (if needed).
# Some lines of (csh) shell code is provided below as an example of
# how to accomplish this task.
#
# The next line, if one "#" is removed, will specify a maximum wall-clock
# limit of 1 hour, 10 minutes. If the time is limit exceeded the job will be
# terminated. The format is: HOURS:MINUTES:SECONDS.
#
##PBS -l walltime=1:10:00
#
# The next line, if one "#" is removed, will specify that the job should be
# send to the 'workq' queue.
#
##PBS -q workq
#

#
# cd to the directory where qsub was issued.
#
cd $PBS_O_WORKDIR

#
# copy input file and mpi executable to /scratch. a.out.mpi reads 'input'.
#
cp input /scratch
cp a.out.mpi /scratch

#
# cd to /scratch.
#
cd /scratch

#
# Define a list with allocated nodes and count the number of nodes.
#
set nodelist = `cat $PBS_NODEFILE`
set nodes = $#nodelist

#
# The number of mpi-processes to start is equal to the number of
# CPU-cores allocated, which is 4 times the number of nodes for Horseshoe4
# and 8 times the number of nodes for Horseshoe5
#
set CORES = $nodes
@ CORES *= 4

#
# Distribute /scratch/input and /scratch/a.out.mpi to the other nodes.
#
shift nodelist
foreach node ($nodelist)
  rcp /scratch/input /scratch/a.out.mpi ${node}:/scratch
end

#
# Set environment variables INPF and OUPF - used by a.out.mpi on all nodes.
#
setenv INPF /scratch/input
setenv OUPF /scratch/input

#
# Start the mpi-application with 'mpirun'.
#
# -x       : export an environment variable to all nodes.
# -np      : number of proscesses to start (evenly distributed on the nodes).
# --prefix : where to find the OPEN-MPI installation on all nodes.
#
mpirun --prefix /usr/local/lib/openmpi --hostfile $PBS_NODEFILE -np $CORES -x INPF -x OUPF /scratch/a.out.mpi

#
# copy output files back to working directory. a.out.mpi has produced 'output'.
#
cp output $PBS_O_WORKDIR
