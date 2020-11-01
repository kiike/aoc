#!/usr/bin/env/python

import itertools
import subprocess


found = []
with open("input") as f:
    program = f.read()

settings = [str(e) for e in range(5)]

for setting in itertools.permutations(settings):
    cmd = ["./intcode"] + list(setting)
    result = subprocess.run(cmd, input=program, text=True, capture_output=True)
    found.append(int(result.stdout))

print(max(found))
