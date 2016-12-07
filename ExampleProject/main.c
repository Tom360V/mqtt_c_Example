#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "clientSock.h"
#include "PubSubClient.h"
#include "x86.h"
#include "Client.h"

#define LOG_ERROR(...)     \
                        perror(__FUNCTION__);                   \
                        printf("ERROR::%s:%s:", __FUNCTION__, strerror(errno) );    \
                        printf(__VA_ARGS__);                    \
                        printf("\r\n");                         \
                        fflush(stdout);

#define LOG_INFO(...)     \
                        printf("INFO ::%s: ",__FUNCTION__);     \
                        printf(__VA_ARGS__);                    \
                        printf("\r\n");                         \
                        fflush(stdout);

void callback(char* topic, uint8_t* payload, unsigned int length);

void callback(char* topic, uint8_t* payload, unsigned int length)
{
    char* p = (char*)malloc(length+1);

    memcpy(p, payload, length);
    p[length]=0;
    printf(">>Received: %s::%s\r\n", topic, p);

    free(p);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    clientSocket_init();

    PubSubClient_initHostCallback((Client_t*)&clientSock, "test.mosquitto.org",   1883, callback);
//  PubSubClient_initHostCallback((Client_t*)&clientSock, "data.sparkfun.com",    1883, callback);

    millis_init();
    uint8_t interval = 8;
    while(1)
    {
        LOG_INFO("  Connecting");
        if (PubSubClient_connectId("mqttClient"))
        {
            LOG_INFO("  Subscribing");
            PubSubClient_subscribe("bla");
            sleep(1);

            while(PubSubClient_connected())
            {
                PubSubClient_loop();
                interval++;
                sleep(1);
                if(interval>10)
                {
                    interval=0;
                    LOG_INFO("  Publish");
                    char msg[]="hello world";
                    if(PubSubClient_publish("dummy", (uint8_t*)msg, sizeof(msg)))
                    {
                        LOG_INFO("  Publish succeed");
                    }
                    else
                    {
                        LOG_INFO("  Publish failed");
                        PubSubClient_disconnect();
                    }
                }
            }
        }
        sleep(2);
    }
    PubSubClient_disconnect();

    return 0;
}
