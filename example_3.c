#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hiredis.h>
#include <time.h>

//
// This function gived the difference between two times (timespecs)
//
struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0)
    {
      temp.tv_sec = end.tv_sec-start.tv_sec-1;
      temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    }
    else
    {
      temp.tv_sec = end.tv_sec-start.tv_sec;
      temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

//
// Main program
//
int main(int argc, char **argv) 
{
    unsigned int j;
    redisContext *c;
    redisReply *reply;
    const char *hostname = "127.0.0.1";
    int port = 6379;
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    char *page_1 = NULL;
    char *page_2 = NULL;
    struct timespec start_time, end_time, diff_time;

    //
    // Connect to REDIS service, returns a connection context
    //
    c = redisConnectWithTimeout(hostname, port, timeout);
    if (c == NULL || c->err) 
    {
        if (c) 
        {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } 
        else printf("Connection error: can't allocate redis context\n");
        exit(1);
    }

    //
    // Ping REDIS service
    //
    reply = redisCommand(c,"PING");
    //printf("PING: %s\n", reply->str);
    freeReplyObject(reply);

    //
    // Allocate some memory for test
    //
    page_1 = malloc( 1024 );

    if ( page_1 == NULL ) 
    {
      printf("Error allocating memory for testing\n");
      exit(1);
    }

    //
    // Put something usefull in the page
    //
    for( j=0; j<1024; j++ )
    {
      page_1[j] = j%256;
    }

    //
    // SET the key using binary safe API
    //
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    for( j=0; j<10000; j++ )
      reply = redisCommand(c,"SET %b %b", "page_1", (size_t) 6, page_1, (size_t) 1024);
    freeReplyObject(reply);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    diff_time = diff(start_time, end_time);
    printf(" Time for 10000 SET commands %ld (us) %f us per SET\n", diff_time.tv_nsec/1000, diff_time.tv_nsec / (10000.0 * 1000) );

    //
    // GET the key
    //
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    for( j=0; j<10000; j++ )
      reply = redisCommand(c,"GET page_1");
    freeReplyObject(reply);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    diff_time = diff(start_time, end_time);
    printf(" Time for 10000 GET commands %ld (us) %f us per GET\n", diff_time.tv_nsec/1000, diff_time.tv_nsec / (10000.0 * 1000) );

    //
    // Disconnects and frees the context
    //
    redisFree(c);

    //
    // Free up temp memory
    //
    free( page_1 );

    return 0;
}
