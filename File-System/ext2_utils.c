#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2.h"
#include <errno.h>

struct ext2_inode *get_inode(unsigned char * disk, char *abs_path){
	/*
	* find and return the inode based on given path
	*/

	const char s[2] = "/";
	char *dir_name;
	char *path = malloc(sizeof(char)*(strlen(abs_path) + 1));
	strncpy(path, abs_path, strlen(abs_path));
	if (path[0] != '/'){
		free(path);
		return NULL;
	}
	dir_name = strtok(path, s);
	
	struct ext2_group_desc *bg = (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE * 2);
	//start from root inode
	struct ext2_inode *dest_inode = (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE * bg->bg_inode_table + (EXT2_ROOT_INO-1)*sizeof(struct ext2_inode));
	unsigned int block_index = 0;

	int found = 1; /* because we are in root inode now*/
	// loop over inode blocks to find dir_name directory entry
	while(dir_name != NULL && block_index < EXT2_DIRECT_BLOCK_SIZE && dest_inode->i_block[block_index] && (dest_inode->i_mode & EXT2_S_IFDIR)){
		found = 0;
		struct ext2_dir_entry_2 *entry;
		unsigned int size = 0;
		entry = (struct ext2_dir_entry_2 *)(disk + dest_inode->i_block[block_index] * EXT2_BLOCK_SIZE);
		while((size < dest_inode->i_size) && entry->inode){
				if(strncmp(entry->name, dir_name, strlen(dir_name))==0 && strlen(dir_name) == entry->name_len){
					dest_inode = (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE * bg->bg_inode_table + (entry->inode-1)*sizeof(struct ext2_inode));
					dir_name = strtok(NULL, s);
					found = 1;
					break;
				}
			size += entry->rec_len;
			entry = (void *)entry + entry->rec_len;
		}
		if (found == 1){
			block_index = 0;
		}else {
			block_index ++;

		}
	}

	free(path);
	if (found == 1){
		return dest_inode;
	} else {
		return NULL;
	}

}

void remove_bitmap(unsigned char *disk, unsigned int index, int option){
	/*
	* remove an inode/block from inode/block bitmap
	* option = 0: inode bitmap
	* option = 1: block bitmap
	*/

	struct ext2_super_block *sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
	struct ext2_group_desc *bg = (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE * 2);
	unsigned char *bitmap;
	if (option == OPTION_INODE){
		bitmap = (unsigned char *)(disk + EXT2_BLOCK_SIZE * bg->bg_inode_bitmap);
		sb->s_free_inodes_count ++;
		bg->bg_free_inodes_count ++;

	}else{
		bitmap = (unsigned char *)(disk + EXT2_BLOCK_SIZE * bg->bg_block_bitmap);
		sb->s_free_blocks_count ++;
		bg->bg_free_blocks_count ++;
	}

	// modify bitmap
	int byte_i = (index - 1) / 8;
	int bit_i = (index - 1) % 8;
	bitmap[byte_i] &= ~(1 << bit_i);
}


void add_bitmap(unsigned char *disk, unsigned int index, int option){
	/*
	* add an inode/block to inode/block bitmap
	* option = 0: inode bitmap
	* option = 1: block bitmap
	*/

	struct ext2_super_block *sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
	struct ext2_group_desc *bg = (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE * 2);
	unsigned char *bitmap;
	if (option == OPTION_INODE){
		bitmap = (unsigned char *)(disk + EXT2_BLOCK_SIZE * bg->bg_inode_bitmap);
		sb->s_free_inodes_count --;
		bg->bg_free_inodes_count --;
	}else{
		bitmap = (unsigned char *)(disk + EXT2_BLOCK_SIZE * bg->bg_block_bitmap);
		sb->s_free_blocks_count --;
		bg->bg_free_blocks_count --;
	}

	int byte_i = (index - 1) / 8;
	int bit_i = (index - 1) % 8;
	bitmap[byte_i] |= (1 << bit_i);


}

char *get_parent_dir(char *abs_path){
	/*
	* return the parent directory path of abs_path
	*/

	char *path = malloc(sizeof(char)*(strlen(abs_path) + 1));
	strncpy(path, abs_path, strlen(abs_path));
	int i;
	for (i = strlen(path)-1; i >= 0; i--){
		if (path[i] == '/'){
			path[i+1] = '\0';
			break;
		}
	}
	return path;

}

char *get_file_name(char *abs_path, char *parent_dir){
	/*
	* return file/dir name of the abs_path
	*/

	int abs_len = strlen(abs_path);
	int diff = abs_len - strlen(parent_dir)-1;
	
	char *file_name = malloc(sizeof(char) * (diff + 1));
	file_name[diff+1] = '\0';
	for(; diff>=0; diff --){
		file_name[diff] = abs_path[abs_len-1];
		abs_len --;
	}	

	return file_name;
	
}

unsigned int get_free_bitmap(unsigned char *disk, int option){
	/*
	* return first free inode/block number(index) from inode/block bitmap
	* option = 0: inode bitmap
	* option = 1: block bitmap
	*/

	struct ext2_super_block *sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
    struct ext2_group_desc *bg = (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE * 2);
	unsigned char *bitmap;
	int count;
	if (option == OPTION_INODE){
		bitmap= (unsigned char *)(disk + EXT2_BLOCK_SIZE * bg->bg_inode_bitmap);
		count = sb->s_inodes_count;
	}else{
		bitmap = (unsigned char *)(disk + EXT2_BLOCK_SIZE * bg->bg_block_bitmap);
		count = sb->s_blocks_count;

	}
	unsigned char mask = 1;
    unsigned int j = 1;
    int k;
    int i;

    for(k=0; k< count / 8; k++){
        unsigned char byte = bitmap[k];
        for (i = 0; i < 8; i++) {
        	int bit = (byte & (mask << i)) != 0;
            if (bit == 0){
            	return j;
            }
            j++;
        }
    }
    fprintf(stderr, "Not enough space.\n");
    exit(ENOSPC);
}

void remove_inode(struct ext2_inode *dest_inode, unsigned char *disk, unsigned int inode_number){
	/*
	* remove the given inode from the disk and free all its blocks
	*/
	remove_bitmap(disk, inode_number, OPTION_INODE);
    int block_index = 0;
    while(block_index < EXT2_DIRECT_BLOCK_SIZE && dest_inode->i_block[block_index]){
            remove_bitmap(disk, dest_inode->i_block[block_index], OPTION_BLOCK);
            block_index ++;
    }
    // to deal with single indirect
    if (dest_inode->i_block[block_index]){
        unsigned int *single = (unsigned int*)(disk + dest_inode->i_block[block_index] * EXT2_BLOCK_SIZE);
        block_index = 0;
        while(single[block_index]){
            remove_bitmap(disk, single[block_index], OPTION_BLOCK);
            block_index ++;
        }
        remove_bitmap(disk, dest_inode->i_block[EXT2_DIRECT_BLOCK_SIZE], OPTION_BLOCK);
    }
    dest_inode = 0;
}

struct ext2_inode *reserve_inode(unsigned char *disk, unsigned int inode_number){
	/*
	* reserve an inode at given position(inode_number), and return it
	*/
	struct ext2_group_desc *bg = (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE * 2);
	struct ext2_inode *new_inode = (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE * bg->bg_inode_table + (inode_number-1)*sizeof(struct ext2_inode));
	add_bitmap(disk, inode_number,OPTION_INODE);

	return new_inode;
}

void init_i_blocks(struct ext2_inode *inode){
	/*
	* init i_blocks of given inode, set all to 0
	*/

	int i = 0;
    while(i < 15){
        inode -> i_block[i] = 0;
        i ++;
    }
}

unsigned short add_padding(unsigned short len){
	/*
	* return a rec_len that's multiple of 4, if necessary
	*/
	unsigned short new;
	if (len % 4 !=0){
		new = len + 4 - (len % 4);
	} else {
		new = len;
	}
	return new;
}

void add_entry_to_free(unsigned char *disk, struct ext2_inode *parent_inode, unsigned int inode, unsigned char file_type, char *dir_name){
	/*
	* find a free space in the parent inode and add a new directory entery to it
	* parent_inode: the inode that we want to add the entry to
	* dir_name: new entry's dir/file name
	* file_type: new entry's file type
	* inode: new_entry's inode number
	*/
	unsigned int block_index = 0;
    int found = 0;
    unsigned short parent_entry_len;
    unsigned short new_entry_len = add_padding(sizeof(struct ext2_dir_entry_2) + strlen(dir_name));
    struct ext2_dir_entry_2 *parent_entry;
    struct ext2_dir_entry_2 *new_entry;
    while(block_index < EXT2_DIRECT_BLOCK_SIZE && parent_inode->i_block[block_index]){
        unsigned int size = 0;
        parent_entry = (struct ext2_dir_entry_2 *)(disk + parent_inode->i_block[block_index] * EXT2_BLOCK_SIZE);
        while((size < parent_inode->i_size) && parent_entry->inode){
            parent_entry_len = add_padding(sizeof(struct ext2_dir_entry_2) + parent_entry->name_len);
            if (parent_entry_len + new_entry_len <= parent_entry->rec_len){
                found = 1;
                break;
            }
                size += parent_entry->rec_len;
                parent_entry = (void *)parent_entry + parent_entry->rec_len;                   
        }
        if (found == 1){
            break;
        }
        block_index ++;
    }
    if(parent_inode->i_block[block_index]){
    	/*
    	* [entry]_rec_len: rec_len an entry holds now
    	* [entry]_len : rec_len an entry actually needs
    	*/   
        unsigned short parent_entry_rec_len = parent_entry -> rec_len;
        new_entry = (void *)parent_entry + parent_entry_len;
        new_entry -> inode = inode;
        new_entry -> name_len = strlen(dir_name);
        new_entry -> file_type = file_type;
        new_entry -> rec_len = parent_entry_rec_len - parent_entry_len;
        strncpy(new_entry->name, dir_name, new_entry->name_len);
        parent_entry -> rec_len = parent_entry_len;

    }else{
        //no space on previous blocks
        //need to init a new block and add new entry to it.
        unsigned int free_block_number = get_free_bitmap(disk, OPTION_BLOCK);
        add_bitmap(disk, free_block_number, OPTION_BLOCK);
        parent_inode -> i_block[block_index] = free_block_number;
        new_entry = (struct ext2_dir_entry_2 *)(disk + parent_inode->i_block[block_index] * EXT2_BLOCK_SIZE);
        new_entry -> inode = inode;
        new_entry -> name_len = strlen(dir_name);
        new_entry -> file_type = file_type;
        new_entry -> rec_len = EXT2_BLOCK_SIZE;
        strncpy(new_entry->name, dir_name, new_entry->name_len);
        parent_inode -> i_blocks += 2;
        parent_inode -> i_size += EXT2_BLOCK_SIZE;
            
    }
}

