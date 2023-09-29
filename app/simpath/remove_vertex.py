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

V = list(map(int, input().split()))
RE = []
for u, v in E:
    if u not in V and v not in V:
        RE.append((u, v))
# print(RE)

print('p edge ' + str(n-len(V)) + ' ' + str(len(RE[::2])))
for u, v in RE[::2]:
    print('e ' + str(u) + ' ' + str(v))

print(' '.join(limit))

if s in V:
    print('There is not "s"')
elif t in V:
    print('There is not "t"')
elif s in V and t in V:
    print('There is not "s" and "t"')
else:
    print(' '.join(ts))