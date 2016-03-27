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
        fprintf(stderr, "Usage: ext2_mkdir <image file name> <absolute path on image>\n");
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
    if (dest_inode != NULL && (dest_inode->i_mode & EXT2_S_IFDIR)){
        fprintf(stderr, "Directory already exists. \n");
        exit(EEXIST);

    } else {
        char *parent_dir = get_parent_dir(argv[2]);
        struct ext2_inode *parent_inode = get_inode(disk, parent_dir);
        if (parent_inode == NULL){
            fprintf(stderr, "Parent Directory does not exists. \n");
            free(parent_dir);
            exit(ENOENT);
        }else{
            char *dir_name = get_file_name(argv[2], parent_dir);
            unsigned int free_inode_number = get_free_bitmap(disk, OPTION_INODE); 
            struct ext2_inode *new_inode = reserve_inode(disk, free_inode_number);
            
            //initialization
            new_inode -> i_mode = EXT2_S_IFDIR;
            new_inode -> i_size = EXT2_BLOCK_SIZE;
            new_inode -> i_links_count = 2;
            new_inode -> i_blocks = 2;
            unsigned int free_block_number = get_free_bitmap(disk, OPTION_BLOCK);
            add_bitmap(disk, free_block_number, OPTION_BLOCK);
            init_i_blocks(new_inode);
            new_inode -> i_block[0] = free_block_number;

            struct ext2_dir_entry_2 *new_entry = (struct ext2_dir_entry_2 *)(disk + new_inode->i_block[0] * EXT2_BLOCK_SIZE);
            new_entry -> inode = free_inode_number;
            new_entry -> name_len = 1;
            unsigned short rec_len = add_padding(sizeof(struct ext2_dir_entry_2) + new_entry->name_len);
            new_entry -> rec_len = rec_len;
            new_entry -> file_type = EXT2_FT_DIR;
            strncpy(new_entry -> name, ".", new_entry->name_len);

            new_entry = (void*)(new_entry) + new_entry->rec_len;
            new_entry -> inode = ((struct ext2_dir_entry_2 *)(disk + parent_inode->i_block[0] * EXT2_BLOCK_SIZE))->inode;
            new_entry -> name_len = 2;
            new_entry -> rec_len = EXT2_BLOCK_SIZE - rec_len;
            new_entry -> file_type = EXT2_FT_DIR;
            strncpy(new_entry -> name, "..", new_entry->name_len);

            //find a free space in parent direcoty entry and add new_entry
            add_entry_to_free(disk,parent_inode, free_inode_number, EXT2_FT_DIR, dir_name);

            free(dir_name);
            free(parent_dir);
        }
    }

    munmap(disk,sb.st_size);
    close(fd);
    return 0;
}