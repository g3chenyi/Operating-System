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
        fprintf(stderr, "Usage: ext2_rm <image file name> <absolute path to a file/link on image>\n");
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
    } else if (dest_inode->i_mode & EXT2_S_IFDIR){
        fprintf(stderr, "Usage: ext2_rm <image file name> <absolute path to a file/link on image>\n");
        exit(1);
    } else {

        char * parent_dir = get_parent_dir(argv[2]);
        char * file_name = get_file_name(argv[2], parent_dir);
        struct ext2_inode *parent_inode = get_inode(disk, parent_dir);

        unsigned int block_index = 0;
        int found = 0;
        unsigned int inode_number;
        while(block_index < EXT2_DIRECT_BLOCK_SIZE && parent_inode->i_block[block_index]){
            struct ext2_dir_entry_2 *entry;
            struct ext2_dir_entry_2 *next_entry;
            unsigned int size = 0;
            //find the entry before the one we want to remove and make it to jump over
            entry = (struct ext2_dir_entry_2 *)(disk + parent_inode->i_block[block_index] * EXT2_BLOCK_SIZE);
            next_entry = (void *)entry + entry->rec_len;
            while((size < parent_inode->i_size) && entry->inode && next_entry->inode){
                if(strncmp(file_name, next_entry->name, next_entry->name_len)==0 && strlen(file_name) == next_entry->name_len){   
                    found = 1;
                    entry->rec_len += next_entry->rec_len;
                    inode_number = next_entry->inode;
                    memset(next_entry, 0, sizeof(struct ext2_dir_entry_2));
                    break;
                }
                size += entry->rec_len;
                entry = (void *)entry + entry->rec_len;
                next_entry = (void *)entry + entry->rec_len;
            }
            if (found == 1){
               break;
            }
            block_index ++;
        }
        free(parent_dir);
        free(file_name);

        dest_inode->i_links_count --;


        // remove inode and its associated blocks.
        if (dest_inode->i_links_count == 0){
            remove_inode(dest_inode, disk, inode_number);
        }

    }

    munmap(disk,sb.st_size);
    close(fd);
    return 0;
}
