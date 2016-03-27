all : ext2_cp ext2_ln ext2_ls ext2_mkdir ext2_rm

ext2_cp: ext2_cp.c 
	gcc -Wall -g -o $@ $^

ext2_ln: ext2_ln.c 
	gcc -Wall -g  -o $@ $^

ext2_ls: ext2_ls.c 
	gcc -Wall -g -o $@ $^

ext2_mkdir: ext2_mkdir.c 
	gcc -Wall -g -o $@ $^

ext2_rm: ext2_rm.c
	gcc -Wall -g -o $@ $^

clean:
	rm -f ext2_cp ext2_ln ext2_ls ext2_mkdir ext2_rm