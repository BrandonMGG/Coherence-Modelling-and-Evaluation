#ifndef MESSAGES_H
#define MESSAGES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#define MQ_NAME "/cache_coherence_queue"
#define MAX_MESSAGES 10

struct Message {
    char id[64];
    int access;
    char address[128];
    int block_id;
};

mqd_t create_message_queue();
void send_message(mqd_t mq, struct Message *message);
void cleanup_message_queue(mqd_t mq);

#endif
