#!/usr/bin/env python

from collections import Counter


width = 25
height = 6
size = width * height

transformations = {
    '0': "░",
    '1': "█"
}

def get_layers(pixels):
    return [pixels[layer:layer+size] for layer in range(0, len(pixels), size)]


def get_pixel(pixel, layers):
    for layer in layers:
        if layer[pixel] != '2':
            return transformations[layer[pixel]]


def print_p1_solution(layers):
    counts = []
    for layer in layers:
        counts.append(Counter(layer))
    counts = sorted(counts, key=lambda c: int(c['0']))
    print("Part 1 solution:", counts[0]['1'] * counts[0]['2'])

            
pixels = input().strip()
layers = get_layers(pixels)

print_p1_solution(layers)

composite = []
for pixel in range(size):
    composite.append(get_pixel(pixel, layers))

for index, pixel in enumerate(composite):
    if index != 0 and index % width == 0:
        print()
    print(pixel, end="")
else:
    print()
