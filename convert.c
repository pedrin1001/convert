#include <pthread.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include "convert.h"

struct thread_data thread_data_array[NUM_THREADS];

//Thread's Method
void *ConvertImages(void *threadarg)
{
    //Gets thread arguments
    struct thread_data *this_thread;
    this_thread = (struct thread_data *) threadarg;
    if(!(this_thread->original = cvLoadImage(this_thread->original_address, CV_LOAD_IMAGE_COLOR)))
    {
        printf("Error loading file named %s\n", this_thread->original_address);
        pthread_exit(NULL);
    }
    char file_name[40];
    //create file name
    sprintf(file_name, "thumbnail_%d_resize_%d.jpg",
            this_thread->number, this_thread->resize_number);
    //resize image using linear interpolation (opencv default)
    cvResize (this_thread->original, this_thread->destination, CV_INTER_LINEAR);
    if(! (cvSaveImage (file_name, this_thread->destination, 0)))
    {
        printf("Error saving file %s", file_name);
    }
    cvReleaseImage(&(this_thread->original));
    pthread_exit(NULL);
}

int convert (char *input_folder, char *output_folder, int width, int height, int call)
{
    int i, first_calls = TRUE;
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;
    pthread_attr_t attr;
    void *status;
    //configure to make threads joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int THUMBNAIL_NUMBER=0;
    DIR *dir;
    struct dirent *ent;
    //stores a sample image to get channels and depth
    IplImage *sample_image;
    //uses sample image and the desired size to create blank images
    IplImage **thread_destination_images;
    //one for each thread
    thread_destination_images = (IplImage**) malloc(NUM_THREADS * sizeof (IplImage*));
    //changes directory to output
    if (chdir (output_folder) == -1)
    {
        printf ("error opening output folder");
        return 0;
    }
    if ((dir = opendir (input_folder)) != NULL)
    {
        i=0;
        char *base_address = input_folder, file_address[100];
        int j;
        //get last position of string a
        for(j=0; base_address[j] != '\0'; j++);
        //add '/' to directory address if needed
        if(base_address[j-1] != '/') sprintf(base_address, "%s/", input_folder);
        while ((ent = readdir (dir)) != NULL)
        {
            if(NUM_THREADS == i)
            {
                i=0;
                if(first_calls) first_calls = FALSE;
            }
            if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            sprintf(file_address, "%s%s", base_address, ent->d_name);
            //waits for thread to complete to assign new image to it
            if(!first_calls)
            {
                rc = pthread_join(threads[i], &status);
                if (rc)
                {
                    printf("Error; return code from pthread_join() is %d\n", rc);
                    exit(-1);
                }
            }
            if(first_calls)
            {
                //reads first image to get depth and channels
                if(!(sample_image = cvLoadImage(file_address, CV_LOAD_IMAGE_COLOR)))
                {
                    printf("Error loading file named %s\n", file_address);
                    return 0;
                }
                //create images with user specified size
                //length and width given by user
                //depth and channels taken from sample_image

                thread_destination_images[i] = cvCreateImage
                                               ( cvSize(width , height),
                                                 sample_image->depth, sample_image->nChannels );
                cvReleaseImage(&sample_image);

            }
            //set thread arguments
            thread_data_array[i].destination = thread_destination_images[i];
            thread_data_array[i].number = THUMBNAIL_NUMBER;
            thread_data_array[i].resize_number = call;
            thread_data_array[i].original_address = file_address;
            rc = pthread_create(&threads[i], &attr, ConvertImages, (void *) &thread_data_array[i]);
            if (rc)
            {
                printf("Error; return code from pthread_create() is %d\n", rc);
                return 0;
            }
            i++;
            THUMBNAIL_NUMBER++;
        }
        closedir (dir);
    }
    else
    {
        //could not open directory
        perror ("");
        return 0;
    }

    //Free attribute and wait for the other threads
    pthread_attr_destroy(&attr);
    for(t=0; t<NUM_THREADS; t++)
    {
        rc = pthread_join(threads[t], &status);
        if (rc)
        {
            printf("Error; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

    //release memory
    for(i=0; i<NUM_THREADS; i++) cvReleaseImage(&(thread_destination_images[i]));
    cvReleaseImage(&sample_image);

    return 1;
}
