import requests
#import nodes

"""
Run this script after running wrk.sh to validate that all nodes were
generated.
"""

nodes = requests.get('http://localhost:18080/nodes').json()
#nodes = nodes.nodes


arr = nodes['nodes']

all_in1 = [n for n in arr if n.startswith('in_1')]
all_in1.sort()

v1, *vals = all_in1

deltas = []
missing = []
for v2 in vals:
    delta = int(v2[3:]) - int(v1[3:])
    if (delta != 1):
        missing.append((v1, v2))
    deltas.append(delta)
    v1 = v2

print(deltas)
print(missing)
print(sum(deltas))
print(len(arr))
