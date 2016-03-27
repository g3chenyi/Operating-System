#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2_utils.c"
#include <errno.h>

unsigned char *disk;

int main(int argc, char **argv) {

	if(argc != 3) {
        fprintf(stderr, "Usage: ext2_ls <image file name> <absolute path on image>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);
    struct stat sb;
    fstat(fd, &sb);
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
	   perror("mmap");
	   exit(1);
    }
    
    struct ext2_inode *dest_inode = get_inode(disk, argv[2]);
    if (dest_inode == NULL){
        fprintf(stderr,"No such file or diretory\n");
        exit(ENOENT);
    } else if (dest_inode->i_mode & EXT2_S_IFREG){
        printf("%s\n", argv[2]);
    } else{
        unsigned int block_index = 0;
        while(block_index < EXT2_DIRECT_BLOCK_SIZE && dest_inode->i_block[block_index]){
            struct ext2_dir_entry_2 *entry;
            unsigned int size = 0;
            entry = (struct ext2_dir_entry_2 *)(disk + dest_inode->i_block[block_index] * EXT2_BLOCK_SIZE);
            while((size < dest_inode->i_size) && entry->inode){
                char file_name[entry->name_len+1];
                memcpy(file_name, entry->name, entry->name_len);
                file_name[entry->name_len] = 0;
                printf("%s\n", file_name);
                size += entry->rec_len;
                entry = (void *)entry + entry->rec_len;
            }
            block_index ++;
        }         

    }
    
    munmap(disk,sb.st_size);
    close(fd);
    return 0;
}