/*
Homework Assignment 8: File Manipulation
Due Date: 21 April 2024
*/

#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]);

#define BUF_SIZE 4
#define OUTPUT_MODE 0700    
#define TRUE 1

int main(int argc, char *argv[])
{
    int bmp_fd, text_fd, out_fd, bmp_count, text_count, wt_count;
    const unsigned char end_of_message_marker[3] = {0, 0, 0}; // to mark the end of the encrypted message
    char buffer[BUF_SIZE]; // read in 4 bytes at a time (R, G, B, padding)
    unsigned char textBuffer[1]; //read in one char at a time

    if(argc != 4) exit(1); 
    bmp_fd = open(argv[1], O_RDONLY);
    if (bmp_fd < 0) exit(2);
    text_fd = open(argv[2], O_RDONLY);
    if (text_fd < 0) exit(3);
    out_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, OUTPUT_MODE); // wouldn't output with just OUTPUT_MODE
    if (out_fd < 0) exit(4);

    int x = 0;

    while(x <= 53) // must iterate through and copy the file data in order to copy the image
    {
        bmp_count = read(bmp_fd, buffer, 1);
        if(bmp_count <= 0) break;

        wt_count = write(out_fd, buffer, bmp_count);
        if (wt_count <= 0) exit(5);
        x++;
    }

    while((text_count = read(text_fd, textBuffer, 1)) > 0) // while there are still characters remaining and reads next character
    {
        for(int bit_count = 0; bit_count < 8; bit_count++) // 8 bits = 1 character byte
        {
            bmp_count = read(bmp_fd, buffer, BUF_SIZE); // read four bits (R, G, B, Padding)
            if (bmp_count < BUF_SIZE)
            {
                wt_count = write(out_fd, buffer, bmp_count); // eof handling - reads however many bits are left
                if(wt_count <=0) exit(6);
            }
            for(int i = 0; i < 3; i++)  // iterate through the first 3 bytes in the image buffer
            {
                unsigned char mask = ~0x01; // clear the least significant bit
                unsigned char shifted = textBuffer[0] >> (7-bit_count);
                unsigned char isolated = shifted & 1;
                
                buffer[i] &= mask;
                buffer[i] |= isolated;  //perform the necessary bitwise operations
            }
            wt_count = write(out_fd, buffer, bmp_count);
            if (wt_count <= 0) 
            {
                perror("Failed to write to output file");
                exit(7);
            }
        }
    }
    for (int j = 0; j < sizeof(end_of_message_marker); j++) //mark the end of the message
    {
        textBuffer[0] = end_of_message_marker[j];
        for (int bit_count = 0; bit_count < 8; bit_count++) 
        {
            bmp_count = read(bmp_fd, buffer, BUF_SIZE);
            if (bmp_count < BUF_SIZE) 
            {
                wt_count = write(out_fd, buffer, bmp_count);
                if(wt_count <= 0) exit(8);
            }
            for(int i = 0; i < 3; i++)
            {
                unsigned char mask = ~0x01; // clear the least significant bit
                unsigned char shifted = textBuffer[0] >> (7-bit_count);
                unsigned char isolated = shifted & 1;
                
                buffer[i] &= mask;
                buffer[i] |= isolated;
            }
            wt_count = write(out_fd, buffer, bmp_count);
            if (wt_count <= 0) 
            {
                perror("Failed to write to output file");
                exit(9);
            }
        }
    }

    while((bmp_count = read(bmp_fd, buffer, BUF_SIZE)) > 0) // read the rest of the file (if necessary) and copy it
    {
        wt_count = write(out_fd, buffer, bmp_count);
        if(wt_count <= 0) exit(10);
    }

    if (bmp_count < 0) 
    {
        perror("Read failed on BMP file");
        exit(11);
    }
    else
    {
        close(bmp_fd);
        close(text_fd);
        close(out_fd);
        exit(0);
    }


}
