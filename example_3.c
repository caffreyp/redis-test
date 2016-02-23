#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hiredis.h>
#include <time.h>

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
    clock_t start_time=0, end_time =0;

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
    start_time = clock();
    for( j=0; j<10000; j++ )
      reply = redisCommand(c,"SET %b %b", "page_1", (size_t) 6, page_1, (size_t) 1024);
    freeReplyObject(reply);

    end_time = clock();
    printf(" Time for 10000 SET commands %ld (us) %ld us per SET\n", end_time - start_time, (end_time - start_time) / 10000 );

    //
    // GET the key
    //
    start_time = clock();
    for( j=0; j<10000; j++ )
      reply = redisCommand(c,"GET page_1");
    freeReplyObject(reply);

    end_time = clock();
    printf(" Time for 10000 GET commands %ld (us) %ld us per GET\n", end_time - start_time, (end_time - start_time) / 10000 );

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

