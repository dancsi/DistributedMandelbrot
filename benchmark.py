import subprocess
from time import perf_counter
from datetime import datetime
from os import mkdir
import numpy as np
import pandas as pd

nodes = ["localhost"]

executable_path = "build/mandelbrot"


def generate_hostfile(num_nodes, num_processes_per_node):
    with open("hosts.txt", "w") as fout:
        for node in nodes[:num_nodes]:
            print(node, "slots={}".format(num_processes_per_node), file=fout)


def get_cmdline_mpi(
        num_nodes,
        num_processes_per_node,
        num_threads_per_process):
    cmdline_mpi = "mpirun -np 1 --host localhost {0} --processor=server : --hostfile=hosts.txt -np {{total_processes}} --bynode {0} --processor={{processor}} --num_threads={{num_threads}}".format(
        executable_path)
    generate_hostfile(num_nodes, num_processes_per_node)
    return cmdline_mpi.format(
        total_processes=num_nodes * num_processes_per_node,
        num_threads=num_threads_per_process,
        processor="worker-st" if num_threads_per_process == 1 else "worker-mt")


def get_cmdline_basic(
        num_nodes,
        num_processes_per_node,
        num_threads_per_process):
    return executable_path + " --standalone=true --processor=worker-st"


def measure_once(
        num_nodes,
        num_processes_per_node,
        num_threads_per_process,
        cmdline_func):
    cmdline = cmdline_func(
        num_nodes,
        num_processes_per_node,
        num_threads_per_process)
    start = perf_counter()
    print("Running cmdline\n", cmdline)
    subprocess.run(cmdline)
    end = perf_counter()
    input("Continue?")
    return end - start


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

mkdir("logs")
results.to_csv("logs/{}.csv".format(int(datetime.now().timestamp())))
