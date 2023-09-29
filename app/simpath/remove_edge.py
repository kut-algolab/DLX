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

remove_e = int(input())
RE = []
for i in range(remove_e):
    u, v = map(int, input().split())
    RE.append((u, v))
    RE.append((v, u))

removed_E = []
for u, v in E:
    if (u, v) not in RE:
        removed_E.append((u, v))

# print(removed_E)

print('p edge ' + str(n) + ' ' + str(len(removed_E[::2])))
for u, v in removed_E[::2]:
    print('e ' + str(u) + ' ' + str(v))

print(' '.join(limit))
print(' '.join(ts))