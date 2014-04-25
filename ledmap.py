#!/usr/bin/env python

m = [-1] * 48 * 12

for i in range(0, 100 * 3):
	x = i % 50
	if x == 0 or x == 25: continue
	y = i / 25
	if x > 25:
		x = 49 - x
	else:
		x -= 1
	m[y * 48 + (x + 24)] = i
	m[y * 48 + (23 - x)] = i + 400

used = dict()

print "int16_t LedMap[ 48 * 12 ] = {"
for i in m:
	assert(i >= 0)
	assert(i not in used)
	used[i] = 1
	print str(i) + ",",
print "};\n"

assert(len(used) == len(m))
