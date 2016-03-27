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
        fprintf(stderr, "Usage: ext2_ln <image file name> <absolute path to a file on image> <absolute path to a file on image>\n");
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

    struct ext2_inode *source = get_inode(disk, argv[2]);
    struct ext2_inode *dest = get_inode(disk, argv[3]);

    if (source == NULL){
        fprintf(stderr, "No such file or diretory\n");
        exit(ENOENT);
    }

    if ((dest != NULL) && (dest->i_mode  & EXT2_S_IFREG)){
        fprintf(stderr, "Target file already exits\n");
        exit(EEXIST);
    }

    if ((source->i_mode & EXT2_S_IFDIR) || ((dest != NULL) && (dest->i_mode & EXT2_S_IFDIR))){
        fprintf(stderr,"Cannot link directoires\n");
        exit(EISDIR);

    }

    char *source_parent_dir = get_parent_dir(argv[2]);
    char *source_file_name = get_file_name(argv[2], source_parent_dir);
    char *dest_parent_dir  = get_parent_dir(argv[3]);
    char *dest_file_name = get_file_name(argv[3], dest_parent_dir);

    struct ext2_inode *source_parent_inode = get_inode(disk, source_parent_dir);
    struct ext2_dir_entry_2 *source_entry;
    unsigned int block_index = 0;
    int found = 0;
    while(block_index < EXT2_DIRECT_BLOCK_SIZE && source_parent_inode->i_block[block_index]){
        struct ext2_dir_entry_2 *entry;
        unsigned int size = 0;
        //find the source file entry
        entry = (struct ext2_dir_entry_2 *)(disk + source_parent_inode->i_block[block_index] * EXT2_BLOCK_SIZE);
        while((size < source_parent_inode->i_size) && entry->inode){
            if(strncmp(source_file_name, entry->name, entry->name_len)==0 && strlen(source_file_name) == entry->name_len){   
                found = 1;
                source_entry = entry;
                break;
            }
            size += entry->rec_len;
            entry = (void *)entry + entry->rec_len;
            }
        if (found == 1){
            break;
        }
        block_index ++;
    }

    struct ext2_inode *dest_parent_inode = get_inode(disk, dest_parent_dir);
    // use source file entry info to create an new entry, and add to destiantion directory entry
    add_entry_to_free(disk, dest_parent_inode, source_entry->inode , EXT2_FT_REG_FILE, dest_file_name);
    dest_parent_inode->i_links_count ++;

    free(source_parent_dir);
    free(source_file_name);
    free(dest_parent_dir);
    free(dest_file_name);
    munmap(disk,sb.st_size);
    close(fd);
    return 0;
}