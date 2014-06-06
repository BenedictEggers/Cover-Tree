# Ben Eggers
# GNU GPL'd

# This script generates data for use in testing the cover tree. It generates
# points in balls in the plane, sometimes recursing into those balls to generate
# their data in sub-balls and (sub-)*balls. It also generates some test queries,
# and writes everything out to a file in "./test_data/".

def main():
	# These parameters can be used to tweak the program behavior. "levels" is how
	# many levels down the ball-generating will recurse, and "center", "radius"
	# together define the circle within which all the points will fall.
	center = (0, 0)
	radius = 10
	levels = 1

	points = make_ball(center, radius, levels)
	queries = get_queries(points)


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
		# need to recurse


def get_queries(points, bound=1000):
	"""
	Takes in a list of points, and makes some test queries on it. The optional
	bound parameter is to tell the function within what range the query points
	should lie in: ([-bound, bound], [-bound, bound]). Returns a list of
	(query, k, answer_list) tuples where query is some (x, y) query point,
	k is the length of answer_list, and answer_list is the k nearest points
	to query, maybe more if multiple points are tied for last place.
	"""


if __name__ == "__main__":
	main()