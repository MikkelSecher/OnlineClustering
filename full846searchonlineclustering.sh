#! /bin/bash
#
#SBATCH --account sdudm_slim      # account
#SBATCH --nodes 72                # number of nodes
#SBATCH --ntasks-per-node 24      # number of MPI tasks per node
#SBATCH --time 24:00:00            # max time (HH:MM:SS)

echo Running on "$(hostname)"
echo Available nodes: "$SLURM_NODELIST"
echo Slurm_submit_dir: "$SLURM_SUBMIT_DIR"
echo Start time: "$(date)"

# Load the modules previously used when compiling the application
module purge
module add gcc/4.8-c7 openmpi/1.8.4

# Start in total 2*24 MPI ranks on all available CPU cores
srun ./OnlineClustering 6 6 1.63 84 10000

echo Done.