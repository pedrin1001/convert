#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pthread.h>
#include "convert.h"

void print_error ()
{
    printf("not enough arguments!\n\n");
    printf("correct format:\n\n");
    printf("inputFolder outputFolder numberOfConversions widthxheight1 widthxheight2 ...\n\n");
}

void get_dimensions (char *input_size, int *dimensions)
{
    int i, j;
    char width[5], height[5];
    for (i=0; input_size[i] != 'x'; i++)
    {
        width[i] = input_size[i];
    }
    dimensions[0] = atoi(width);
    j=0;
    i++;
    while (input_size[i] != '\0')
    {
        height[j] = input_size[i];
        i++;
        j++;
    }
    dimensions[1] = atoi(height);
}

int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        print_error();
        return EXIT_FAILURE;
    }
    int num_conversions = atoi(argv[3]);
    //user must give one size for each conversion
    if (argc < (4 + num_conversions))
    {
        print_error();
        return EXIT_FAILURE;
    }

    int dimensions[1], i;

    //calls convert function for all thumbnails sizes
    //each at a time
    for(i=0; i<num_conversions; i++)
    {
        get_dimensions(argv[4+i], dimensions);
        printf("generating thumbnails %d...\n", i+1);
        if(!(convert (argv[1], argv[2], dimensions[0], dimensions[1], (i+1))))
        {
            printf("error while converting!");
            return EXIT_FAILURE;
        }
    }

    pthread_exit(NULL);
}
