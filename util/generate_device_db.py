import sys

with open(sys.argv[1], 'r') as f:
    lines = f.read().splitlines()

vID = ""

for line in lines:
    if line.strip().startswith('#'):
        continue
    if line.strip().startswith('C'):
        break
    if line.startswith("\t") and not line.startswith("\t"*2):
        dID = line.strip().split(" ")[0]
        dN = " ".join(line.strip().split(" ")[2:]).replace("\"", "'")
        print("device_" + vID + dID + ":")
        print(".start:")
        print("dw 0x" + vID + ", 0x" + dID)
        print("dw .end-.start")
        print("db \""+dN+"\"")
        print(".end:")
    elif not line.startswith("\t"*2):
        vID = line.strip().split(" ")[0]
