if __name__ == "__main__":
    
    fil = open("readme.txt", "r")
    l = fil.readline()
    d = []
    d2 = []
    i = 0
    while l:
        l = l.strip()
        if len(l) > 0:
            if l.startswith("trace"):
                l = l.split(",")
                l2 = l[0] +", "+ l[2]
                if (l2 not in d):
                    d.append(l2)
                    print(l2)
                    print("memory size		Hit rate	Hit count	Miss count	Overall evictions	Clean evictions	Dirty evictions")
            else:
                l = l.split(": ")
                print "\t" + l[1] ,
                i = i + 1
                if i == 6:
                    i = 0
                    print ""
        l = fil.readline()