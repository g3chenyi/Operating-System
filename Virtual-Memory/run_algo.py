if __name__ == '__main__':
     import os
     memsize = ["50", "100", "150", "200"]
     swapsize = "10000"
     algorithm = ["rand", "fifo", "lru", "clock","opt"]
     tracefile = ["traceprogs/tr-simpleloop.ref", "traceprogs/tr-matmul.ref", "traceprogs/tr-blocked.ref", "traceprogs/tr-merge_sort.ref"]
     
     for i in range(len(tracefile)):
          for j in range(len(algorithm)):
               print("===============================================\n")
               for k in range(len(memsize)):
                    cmd = "./sim -f "+tracefile[i] + " -m "+memsize[k] + " -s "+swapsize+" -a "+algorithm[j]
                    os.system(cmd)
               print("===============================================\n")
