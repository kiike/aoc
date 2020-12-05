#!/usr/bin/env python

from collections import namedtuple
import logging
import math
import sys

TEST = False
LEVEL=logging.ERROR

Target = namedtuple("Target", ["angle", "position", "distance"])

def get_distance(source, target):
    return [source[0] - target[0], source[1] - target[1]]


def get_abs_distance(distance):
    return abs(distance[0]) + abs(distance[1])


def get_angle(distance):
    """
    Thanks to https://math.stackexchange.com/questions/707673/find-angle-in-degrees-from-one-point-to-another-in-2d-space
    """

    angle = math.atan2(distance[1], distance[0]) * 180 / math.pi
    if angle < 0:
        return 360 + angle
    else:
        return angle


logging.basicConfig(level=LEVEL)

if TEST:
    input_file = "test"
else:
    input_file = "input"

with open(input_file) as f:
    asteroid_map = f.read().lstrip().rstrip().split("\n")

asteroids = []
for y, line in enumerate(asteroid_map):
    for x, character in enumerate(line):
        if character == "#":
            asteroids.append([x, y])

best = {
    "position": [],
    "visible": 0,
}

for source in asteroids:
    angles = []
    for target in asteroids:
        distance = get_distance(source, target)
        angle = get_angle(distance)
        angles.append(angle)
    else:
        visible = (len(set(angles)))
        if visible > best["visible"]:
            best = {
                "position": source,
                "visible": visible
            }
            
# Part 1 solution
print(f"Best position: {best['position']} with {best['visible']} targets")

targets = asteroids.copy()
targets.remove(best["position"])
source = best["position"]

targets_with_info = []
for target in targets:
    position = target
    distance = get_distance(source, target)
    abs_distance = get_abs_distance(distance)
    angle = get_angle(distance)
    targets_with_info.append(Target(angle=angle, position=position, distance=abs_distance))

targets = targets_with_info
targets_sorted_by_distance = sorted(targets, key=lambda x: x.distance)
targets_sorted_by_angle_and_distance = sorted(targets_sorted_by_distance, key=lambda x: x.angle)

targets = targets_sorted_by_angle_and_distance

shot_angle = 89.999999
shot_asteroids = []

while len(shot_asteroids) != len(targets):
    for target in targets:
        if target in shot_asteroids:
            continue

        if target.angle > shot_angle:
            logging.info("\033[93m" + "shooting", target)
            shot_angle = target.angle
            shot_asteroids.append(target)

        else:
            logging.info("\033[91m" + "skipping", target)
            continue
    else:
        logging.info("\033[0m" + "reset")
        shot_angle = -1.0

shot_asteroid_200 = shot_asteroids[199]
print("Asteroid vaporization #200:", shot_asteroid_200.position[0]*100 + shot_asteroid_200.position[1])
