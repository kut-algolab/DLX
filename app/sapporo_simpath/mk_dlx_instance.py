#!/usr/bin/env python

line = input().split()
while "c" == line[0]:
    line = input().split()

n = int(line[2])
m = int(line[3])

E = []

for i in range(m):
    e = input().split()
    u = int(e[1])
    v = int(e[2])
    E.append((u, v))
    E.append((v, u))

limit = input().split()
l = int(limit[1]) # The maximum path length
ts = input().split()
s = int(ts[1])
t = int(ts[2])

G = [[] for i in range(n+1)]
for u, v in E:
    G[u].append(v)

INF = 1 << 60
from_s = [INF for i in range(n+1)]
que = []

from_s[s] = 0
que.append(s)

while que:
    v = que.pop(0)
    for u in G[v]:
        if INF == from_s[u]:
            from_s[u] = from_s[v] + 1
            que.append(u)

from_t = [INF for i in range(n+1)]
from_t[t] = 0
que.append(t)

while que:
    v = que.pop(0)
    for u in G[v]:
        if INF == from_t[u]:
            from_t[u] = from_t[v] + 1
            que.append(u)

# Print the primary items
for prim in range(1, l+1):
    print("#" + str(prim) + " ", end='')

print("#v" + str(s) + " #v" + str(t), end='')
print(" | ", end='')

# Print the secondary items
for second in range(1, n+1):
    print("v" + str(second) + " ", end='')

for pathnum in range(0, l+1):
    print("p" + str(pathnum) + " ", end='')

print()

for (u, v) in E:
    # print(u, v)
    if v == s or t == u:
        continue
    if s == u:
        print("#1 #v" + str(s) + " v" + str(u) + ":0 v" + str(v) + ":1 p0:" + str(u) + " p1:" + str(v))      
        continue
    elif t == v:
        print("#" + str(l) + " #v" + str(t) + " v" + str(u) + ":" + str(l-1) + " v" + str(v) + ":" + str(l) + " p" + str(l-1) + ":" + str(u) + " p" + str(l) + ":" + str(v))
        continue
    else:        
        for i in range(from_s[u]+1, l-from_t[v]+1):
            print("#" + str(i) + " v" + str(u) + ":" + str(i-1) + " v" + str(v) + ":" + str(i) + " p" + str(i-1) + ":" + str(u) + " p" + str(i) + ":" + str(v))
# print(from_s)
# print(s, t)