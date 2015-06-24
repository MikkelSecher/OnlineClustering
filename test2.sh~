#!/bin/csh
#
# PBS jobscript example for running a job on one node.
#
# A PBS batch job script is simply like a shell script - except
# that PBS will pick up commands disguised as comments in the first
# block of comments in the script, as shown here.
#
# The next line requests on node to be used for this batch job.
#
# Remember that the nodes in the cluster has multiple cores
# thus your application should be parallelized in some fashion and
# prepared to use all 4 cores per node.
#
#PBS -l nodes=8
#
# The next line requests that PBS sends an e-mail notification at the
# start of execution and at the end of execution.
#
##PBS -m be
#
# The next line, if one "#" is removed, will specify a maximum wall-clock
# limit of 1 hour, 10 minutes. If the time limit is exceeded the job will be
# terminated. The format is: HOURS:MINUTES:SECONDS.
#
#PBS -l walltime=48:00:00
#
# The next line, if one "#" is removed, will specify that the job should be
# send to the 'workq' queue.
#
##PBS -q express
#

#
# Change to the directory where the qsub command was issued.
#
cd $PBS_O_WORKDIR
cd $HOME/mikkel/mpitests/largerun/OnlineClustering
mpirun -np 128 ./OnlineClustering 9 8 1.61 11 400

#

