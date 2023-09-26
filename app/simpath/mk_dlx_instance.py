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
    if v == s or t == u:
        continue
    if s == u:
        print("#1 #v" + str(s) + " v" + str(u) + ":0 v" + str(v) + ":1 p0:" + str(u) + " p1:" + str(v))      
        continue
    elif t == v:
        print("#" + str(l) + " #v" + str(t) + " v" + str(u) + ":" + str(l-1) + " v" + str(v) + ":" + str(l) + " p" + str(l-1) + ":" + str(u) + " p" + str(l) + ":" + str(v))
        continue
    else:        
        for i in range(2, l):
            print("#" + str(i) + " v" + str(u) + ":" + str(i-1) + " v" + str(v) + ":" + str(i) + " p" + str(i-1) + ":" + str(u) + " p" + str(i) + ":" + str(v))
