#!/usr/bin/python
# Ben Eggers <ben.eggers36@gmail.com>
# GNU GPL'd

# This script takes in a file name (should be in test_data) with NO EXTENSION. It
# will plot the points found in filename.point, then the queries found in
# filename.query in a different color, and the answers to the knn queries for that
# query in a third color. See gen_data.py for more detail on the data format.

import numpy as np
import matplotlib.pyplot as plt
import sys
import math

# Get the distance between two 2-tuples (points)
def distance(point1, point2):
	return math.sqrt((point2[0] - point1[0])**2 + (point2[1] - point1[1])**2)

f = sys.argv[1]

pf = open(f + ".point", 'r')
qf = open(f + ".query", 'r')

# Read in all the stuff
points = [(int(p.split(',')[0]), int(p.split(',')[1])) for p in pf.read().split('\n')[:-1]]
k = int(qf.readline())
queries = [(int(p.split(',')[0]), int(p.split(',')[1])) for p in qf.read().split('\n')[:-1]]

# Split the data points into x and y for pyplot
px = [p[0] for p in points]
py = [p[1] for p in points]
plt.plot(px, py, 'ro')

# Now separate the queries from their answers and plot them both, with a circle
# around the query to make it clear that the answers are actually correct

for x in xrange(0, len(queries), k + 1):
	current = queries[x : x + k + 1]
	query = current[0]
	answers = current[1:]

	plt.plot(query[0], query[1], 'ko')
	max_radius = 0
	for a in answers:
		plt.plot(a[0], a[1], 'bo')

		dist = distance(query, a)
		if dist > max_radius:
			max_radius = dist
	circle = plt.Circle(query, max_radius, color='k', fill=False)
	plt.gcf().gca().add_artist(circle)


plt.show()
