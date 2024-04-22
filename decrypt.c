/*
Homework Assignment 8: File Manipulation
Due Date: 21 April 2024
*/

#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 4
#define OUTPUT_MODE 0700    
#define TRUE 1
#define SEEK_SET 0

int main(int argc, char *argv[]) {
    int bmp_fd, out_fd, bmp_count, wt_count;
    char buffer[BUF_SIZE];
    const unsigned char eomMarker[3] = {0, 0, 0};
    unsigned char potential[3] = {0}; // Buffer to hold bytes that might be the marker
    int marker_index = 0; // Index for the end-of-message marker


    if (argc != 3) exit(1);
    bmp_fd = open(argv[1], O_RDONLY);
    if (bmp_fd < 0) exit(2);
    out_fd = creat(argv[2], OUTPUT_MODE);
    if (out_fd < 0) exit(3);

    lseek(bmp_fd, 54, SEEK_SET);

    unsigned char oneChar = 0;

    while(TRUE)
    {
        for(int bit_count = 0; bit_count < 8; bit_count++) // sets the outer loop to iterate 8 times because 8 bits = 1 byte (character)
        {
            bmp_count = read(bmp_fd, buffer, BUF_SIZE); // read four bits (R, G, B, Padding)
            if (bmp_count < 0) exit(4);
            if (bmp_count < BUF_SIZE) break;

            int buffer_index = bit_count % 3; // cycle through 0, 1, 2 elements from image buffer
            oneChar <<= 1;  // shift
            oneChar |= (buffer[buffer_index] & 1); // bit manipulations
        }
        if (bmp_count < BUF_SIZE) break;

        potential[marker_index] = oneChar;   // mark the character entered as possibly the EOM
        if(potential[marker_index] == eomMarker[marker_index]) 
        {
            marker_index++;
            if(marker_index == 3) break; // stop iterating through image
        } 
        else // not the eom, write to output file
        {
            if (marker_index > 0) 
            {
                wt_count = write(out_fd, potential, marker_index);
                if(wt_count <= 0) {perror("Error writing file"); exit(5);}
                marker_index = 0;
            }
            wt_count = write(out_fd, &oneChar, 1);
            if(wt_count <= 0) {perror("Error writing file"); exit(6);
            }
        }
    }

    close(bmp_fd);
    close(out_fd);

    exit(0);
}