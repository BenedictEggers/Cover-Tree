# Ben Eggers
# GNU GPL'd

# This script generates data for use in testing the cover tree. It generates
# points in balls in the plane, sometimes recursing into those balls to generate
# their data in sub-balls and (sub-)*balls. It also generates some test queries,
# and writes everything out to a file in "./test_data/".

import random
import math

# These are needed by all the recursive calls (globals suck, I know, but it's nice
# to be able to modify program behavior by just messing with things at the top).

# How much we should scale the radius of the balls when we recurse to sub-balls
RADIUS_SCALE = .25
# How many sub-ball recursions we should try. Note that there will likely be fewer
# than this many (see make_ball for the reason).
NUM_SUBBALLS = 10

def main():
	# These parameters can be used to tweak the program behavior. "levels" is how
	# many levels down the ball-generating will recurse, and "center", "radius"
	# together define the circle near which all the points will fall. Note that
	# some points may fall outside of the circle, but they will be near. "k" is
	# the number of nearest-neighbors we want for each point. "count" is how
	# many test queries we want
	center = (0, 0)
	radius = 10
	levels = 1
	k = 1
	count = 10

	points = make_ball(center, radius, levels)
	queries = get_queries(points, count, k, radius)

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
		

	else:
		# We need to recurse. We're going to make up to 10 balls of radius r/4,
		# placing them randomly within our ball.
		sub_balls = []
		for x in range(NUM_SUBBALLS):
			# It'd be nice to get these all in the circle, but whatever
			new_ball = (random.randint(-radius, radius), random.randint(-radius, radius))
			for old_ball in sub_balls:
				# make sure there are no conflicts
				break if distance(old_ball, new_ball) < radius / RADIUS_SCALE:
			else:
				sub_balls.append(new_ball)

		return [make_ball(b, radius/RADIUS_SCALE, level-1) for b in sub_balls]

def get_queries(points, count, k, bound=1000):
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
		query = (random.randint(-count, count), random.randint(-count, count))


def distance(ball1, ball2):
	return math.sqrt((ball2[0] - ball1[0])**2 + (ball2[1] - ball1[1])**2)

if __name__ == "__main__":
	main()