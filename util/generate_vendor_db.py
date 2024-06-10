import sys

with open(sys.argv[1], 'r') as f:
    lines = f.read().splitlines()

vID = ""

for line in lines:
    if line.strip().startswith('#'):
        continue
    if line.strip().startswith('C'):
        break
    if len(line.strip()) == 0:
        continue
    if line.startswith("\t") and not line.startswith("\t"*2):
        continue
    elif not line.startswith("\t"*2):
        vID = line.strip().split(" ")[0]
        vN = " ".join(line.strip().split(" ")[2:]).replace("\"", "'")
        print("vendor_" + vID + ":")
        print(".start:")
        print("dw 0x" + vID)
        print("dw .end-.start")
        print("db \""+vN+"\"")
        print(".end:")
