#!/usr/bin/python
# Ben Eggers <ben.eggers36@gmail.com>
# GNU GPL'd

# This script generates data for use in testing the cover tree. It generates
# points in balls in the plane, sometimes recursing into those balls to generate
# their data in sub-balls and (sub-)*balls. It also generates some test queries,
# and writes everything out to files in "./test_data/".

import random
import math
import uuid
import heapq
import sys


# These are needed by all the recursive calls (globals suck, I know, but it's nice
# to be able to modify program behavior by just messing with things at the top).

# How much we should scale the radius of the balls when we recurse to sub-balls
RADIUS_SCALE = 4
# How many sub-ball recursions we should try. Note that there will likely be fewer
# than this many (see make_ball for the reason). This will also be the number
# of points in each bottom-level ball.
NUM_SUBBALLS = 100
# The K in KNN-search
K_VAL = 5

def main():
	# These parameters can be used to tweak the program behavior. "levels" is how
	# many levels down the ball-generating will recurse, "center" and "radius"
	# together define the circle near which all the points will fall. Note that
	# some points may fall outside of the circle, but they will be near.
	# "count" is how many KNN queries we want.
	center = (0, 0)
	radius = 100000
	levels = 1
	count = 10

	points = make_ball(center, radius, levels)
	queries = get_queries(points, count, radius)

	write_to_file(points, queries)


def make_ball(center, radius, level):
	"""
	Creates some points, all of which will be in the circle defined by the
	center and radius parameters (center should be an (x, y) tuple, radius
	a real number). If level > 1, will recursively create sub-balls inside
	the ball. If level is 1, will just populate the ball with random points.
	Returns a list of (x, y) tuples representing the points generated.
	"""
	if level is 1:
		# we bottomed out the recursion, let's get some points
		points = []
		while len(points) < NUM_SUBBALLS:
			new_point = (
				center[0] + random.randint(-radius, radius),
				center[1] + random.randint(-radius, radius)
				)
			if distance(center, new_point) < radius:
				points.append(new_point)
		return points

	else:
		sub_balls = []
		for x in range(NUM_SUBBALLS):
			# It'd be nice to get these all in the circle, but whatever
			new_center = (
				center[0] + random.randint(-radius, radius),
				center[1] + random.randint(-radius, radius))
			for old_center in sub_balls:
				# make sure there are no conflicts
				if distance(old_center, new_center) < radius / RADIUS_SCALE * 2:
					break
			else:
				sub_balls.append(new_center)

		new_points = [make_ball(b, radius/RADIUS_SCALE, level-1) for b in sub_balls]
		return [p for points in new_points for p in points]


def get_queries(points, count, bound=1000):
	"""
	Takes in a list of points, and makes some test queries on it. The optional
	bound parameter is to tell the function within what range the query points
	should lie in: ([-bound, bound], [-bound, bound]). Returns a list of length
	"count" (query, answer_list) tuples where query is some (x, y) query point
	and answer_list is the k nearest points to query, maybe more if
	multiple points are tied for last place.
	"""
	
	queries = []

	for x in range(count):
		query = (random.randint(-bound, bound), random.randint(-bound, bound))

		# Now we need the k nearest neighbors to our query, in points. Python
		# priority queues are SUPER hacky, but we'll deal with it
		ans = []
		sorted_points = []
		for p in points:
			sorted_points.append((distance(query, p), p))
		sorted_points.sort()

		k = K_VAL - 1
		while k + 1 < len(sorted_points) and sorted_points[k][0] == sorted_points[k+1][0]:
			k += 1

		for x in range(k + 1):
			ans.append(sorted_points[x][1])

		queries.append((query, ans))

	return queries


def write_to_file(points, queries):
	"""
	Write out the passed points to the file, then the passed queries to another
	file. The files will have some random name and will be in the ./test_data
	directory. The points file will have the extension .point, and the query
	file will have the extension .query. The points file will just have one
	point per line, as comma-separated values. The query file will have an integer
	at the top representing k, then some number of points. The order will be:
		k
		querypoint_1
		knn_1
		knn_2
		...
		knn_k
		querypoint_2
		...
	"""

	base_file = "./test_data/" + str(uuid.uuid4())
	if len(sys.argv) is 2:
		base_file = "./test_data/" + sys.argv[1]
		
	point_file = open(base_file + ".point", 'w')
	query_file = open(base_file + ".query", 'w')

	for p in points:
		write_point(point_file, p)

	query_file.write(str(K_VAL) + '\n')
	for q in queries:
		write_point(query_file, q[0])  # write the actual query...
		for ans in q[1]:  # ...followed by all its answers
			write_point(query_file, ans)

	point_file.close()
	query_file.close()


# Write the point out to the file in a format our stuff can parse
def write_point(file, point):
	p = str(point[0]) + ", " + str(point[1])
	file.write(p + '\n')	


# Get the distance between two 2-tuples (points)
def distance(point1, point2):
	return math.sqrt((point2[0] - point1[0])**2 + (point2[1] - point1[1])**2)


if __name__ == "__main__":
	main()