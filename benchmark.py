import subprocess
from time import perf_counter
from datetime import datetime
from os import makedirs, remove
import numpy as np
import pandas as pd
import argparse
from os.path import normpath, exists

executable_path = normpath("build/mandelbrot")
temp_hostfile_name = "temp-hosts.txt"
real_hostfile = "hosts.txt"

nodes = []
if not exists(real_hostfile):
	nodes = ["localhost"]
else:
	with open(real_hostfile) as fin:
		for line in fin:
			nodes.append(line.strip())

parser = argparse.ArgumentParser()
parser.add_argument(
    "--display-stdout",
    help="Displays stdout from the processes being run",
    action='store_true')
args = parser.parse_args()


def generate_hostfile(num_nodes, num_processes_per_node):
    with open(temp_hostfile_name, "w") as fout:
        for node in nodes[:num_nodes]:
            print(node, "slots={}".format(num_processes_per_node), file=fout)


def get_cmdline_mpi(
        num_nodes,
        num_processes_per_node,
        num_threads_per_process):

    generate_hostfile(num_nodes, num_processes_per_node)

    cmdline_mpi = ["mpiexec",
                   "-np",
                   "1",
                   "--host",
                   "localhost",
                   executable_path,
                   "--processor=server",
                   ":",
                   "--hostfile",
                   temp_hostfile_name,
                   "-np",
                   str(num_nodes * num_processes_per_node),
                   "--map-by",
                   "node",
                   executable_path,
                   "--processor={}".format("worker-st" if num_threads_per_process == 1 else "worker-mt"),
                   "--num_threads={}".format(num_threads_per_process)]

    return cmdline_mpi


def get_cmdline_basic(
        num_nodes,
        num_processes_per_node,
        num_threads_per_process):
    return [executable_path, "--standalone=true", "--processor=worker-st"]


def measure_once(
        num_nodes,
        num_processes_per_node,
        num_threads_per_process,
        cmdline_func):
    cmdline = cmdline_func(
        num_nodes,
        num_processes_per_node,
        num_threads_per_process)

    print("Running cmdline\n", cmdline)
    display_stdout = args.display_stdout

    start = perf_counter()
    subprocess.run(
        cmdline,
        stdout=None if args.display_stdout else subprocess.DEVNULL)
    end = perf_counter()

    elapsed = end - start
    print("Elapsed: {:0.4f}s".format(elapsed))
    return elapsed


def measure_many(
        n_measurements,
        num_nodes,
        num_processes_per_node,
        num_threads_per_process,
        cmdline_func):
    times = np.zeros(n_measurements)
    for i in range(n_measurements):
        t = measure_once(
            num_nodes,
            num_processes_per_node,
            num_threads_per_process,
            cmdline_func)
        times[i] = t
    return times.mean(), times.std()

max_num_nodes = len(nodes)
max_num_processes_per_node = 8
max_num_threads_per_process = 8
num_measurements = 5

baseline_t, baseline_sd = measure_many(
    num_measurements, 1, 1, 1, get_cmdline_basic)
results = pd.DataFrame({'num_nodes': [1],
                        'num_processes_per_node': [1],
                        'num_threads_per_process': [1],
                        'mpi': [0],
                        'mt': [0],
                        'time': [baseline_t],
                        'sd': [baseline_sd]})

for num_nodes in range(1, max_num_nodes + 1):
    for num_processes_per_node in range(1, max_num_processes_per_node + 1):
        for num_threads_per_process in range(
                1, max_num_threads_per_process + 1):
            t, sd = measure_many(
                num_measurements,
                num_nodes,
                num_processes_per_node,
                num_threads_per_process,
                get_cmdline_mpi)
            results = results.append({'num_nodes': num_nodes,
                                      'num_processes_per_node': num_processes_per_node,
                                      'num_threads_per_process': num_threads_per_process,
                                      'mpi': 1,
                                      'mt': 1,
                                      'time': t,
                                      'sd': sd}, ignore_index=True)

makedirs("logs", exist_ok=True)
results.to_csv("logs/{}.csv".format(int(datetime.now().timestamp())))

remove(temp_hostfile_name)