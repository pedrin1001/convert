#include <opencv2/imgproc/imgproc_c.h>
#define TRUE 1
#define FALSE 0
#define NUM_THREADS 4

//Thread's Arguments
struct thread_data
{
    int number, resize_number;
    char *original_address;
    IplImage *original, *destination;
};

//Thread method to convert image using opencv
void *ConvertImages(void *threadarg);

//Main function of convert.c, take all inputs and process them to the threads
int convert (char *input_folder, char *output_folder, int width, int height, int call);
