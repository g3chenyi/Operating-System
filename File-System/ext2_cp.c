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

	if(argc != 4) {
        fprintf(stderr, "Usage: ext2_cp <image file name> <file path on native> <absolute path to a file on image>\n");
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

    FILE *fp;
    fp = fopen(argv[2], "r");
    if(fp == NULL){
        fprintf(stderr, "Source file on native does not exists\n");
        exit(ENOENT);
    }

    struct ext2_inode *dest_inode = get_inode(disk, argv[3]);
    if (dest_inode != NULL){
        fprintf(stderr, "Destination file already exists\n");
        exit(EEXIST);
    }

    char *parent_dir = get_parent_dir(argv[3]);
    char *file_name = get_file_name(argv[3], parent_dir);
    struct ext2_inode *parent_inode = get_inode(disk, parent_dir);
    if (parent_inode == NULL){
        fprintf(stderr, "Destination directory does not exists\n");
        exit(ENOENT);
    }

    //to get file size
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    int num_blocks = size / EXT2_BLOCK_SIZE;
    if (size % EXT2_BLOCK_SIZE > 0){
        num_blocks ++;
    }
    if (num_blocks > 12){// need single indirect
        num_blocks ++;
    }

    struct ext2_group_desc *bg = (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE * 2);
    if (bg->bg_free_blocks_count < num_blocks){
        fprintf(stderr, "Not enough space.\n");
        exit(ENOSPC);
    }

    // init 
    int free_inode_number = get_free_bitmap(disk, OPTION_INODE);
    struct ext2_inode *new_inode = reserve_inode(disk, free_inode_number);
    new_inode -> i_mode = EXT2_S_IFREG;
    new_inode -> i_size = size;
    new_inode -> i_links_count = 1;
    new_inode -> i_blocks = 2 * num_blocks;
    init_i_blocks(new_inode);

    int i;
    void *block;
    fseek(fp, 0L, SEEK_SET);
    unsigned int free_block_number;

    // read from source file into destination file
    for (i = 0 ; i < 13 && i < num_blocks; i ++){
        free_block_number = get_free_bitmap(disk, OPTION_BLOCK);
        add_bitmap(disk, free_inode_number, OPTION_BLOCK);
        new_inode->i_block[i] = free_block_number;
        block = (void *)(disk + new_inode->i_block[i] * EXT2_BLOCK_SIZE);
        fread(block, sizeof(char), EXT2_BLOCK_SIZE/sizeof(char), fp);
    }

    if (i == 13){
        //deal with single indirect
        unsigned int *single = (unsigned int*)(disk + new_inode->i_block[12] * EXT2_BLOCK_SIZE);
        i++;
        int j;
        for (j = 0; i < num_blocks; j++, i++){
            free_block_number = get_free_bitmap(disk, OPTION_BLOCK);
            add_bitmap(disk, free_block_number, OPTION_BLOCK);
            single[j] = free_block_number;
            block = (void *)(disk + single[j] * EXT2_BLOCK_SIZE);
            fread(block, sizeof(char), EXT2_BLOCK_SIZE/sizeof(char), fp);
        }
    }

    add_entry_to_free(disk, parent_inode, free_inode_number, EXT2_FT_REG_FILE, file_name);

    munmap(disk,sb.st_size);
    close(fd);
    return 0;

 }