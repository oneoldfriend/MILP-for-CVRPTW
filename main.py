from shutil import copyfile
import numpy as np
import os
import subprocess
import uuid
import argparse


def new_instances_processor(instances):
    copyfile(instances, instances.split("/")[-1])
    return instances.split("/")[-1]


parser = argparse.ArgumentParser()
parser.add_argument("--instance", type=str, default="")
args = parser.parse_args()
file_name = new_instances_processor(args.instance)
print(file_name)
with subprocess.Popen(["./milp", file_name], stdout=subprocess.PIPE, universal_newlines=True) as p:
    for _ in p.stdout:
        continue
    os.remove(file_name)
