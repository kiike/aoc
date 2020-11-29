#!/usr/bin/env python

import math

def get_distance(source, target):
    return [target[0] - source[0], target[1] - source[1]]


with open("input") as f:
    asteroid_map = f.read().lstrip().rstrip().split("\n")

asteroids = []
for x, row in enumerate(asteroid_map):
    for y, character in enumerate(row):
        if character == "#":
            asteroids.append([x, y])

seen = []
for source in asteroids:
    angles = []
    for target in asteroids:
        distance = get_distance(source, target)
        angle = math.atan2(*distance)
        angles.append(angle)
    else:
        seen.append(len(set(angles)))

print(max(seen))
