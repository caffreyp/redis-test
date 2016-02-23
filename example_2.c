#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hiredis.h>

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
    printf("PING: %s\n", reply->str);
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
    // Dump the page
    //
    for( j=0; j<1024; j++ )
    {
      printf("%02X ", (unsigned char)page_1[j] );
      if( (j-15)%16 == 0 ) printf("\n");
    }

    //
    // SET the key using binary safe API
    //
    reply = redisCommand(c,"SET %b %b", "page_1", (size_t) 6, page_1, (size_t) 1024);
    printf("SET (binary API): %s\n", reply->str);
    freeReplyObject(reply);

    //
    // GET the key
    //
    reply = redisCommand(c,"GET page_1");
    //printf("GET foo: %s\n", reply->str);

    //
    // Dump the page
    //
    for( j=0; j<1024; j++ )
    {
      printf("%02X ", (unsigned char)reply->str[j] );
      if( (j-15)%16 == 0 ) printf("\n");
    }


    freeReplyObject(reply);

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
