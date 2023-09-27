#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>

#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 1024

// Define a message structure
struct Message {
    char id[64];
    int access;
    char address[128];
    int block_id;
};

int main() {
    mqd_t mq;
    struct mq_attr attr;
    struct Message message;
    int exit_flag = 0;

    // Define message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = sizeof(struct Message);
    attr.mq_curmsgs = 0;

    // Create a message queue
    mq = mq_open("/myqueue", O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    // Producer: Send messages to the queue
    strcpy(message.id, "core1");
    message.access = 1;  // AccessType.readmiss
    strcpy(message.address, "0x12345678");
    message.block_id = 42;

    if (mq_send(mq, (char *)&message, sizeof(struct Message), 0) == -1) {
        perror("mq_send");
        exit(1);
    }

    // Consumer: Receive messages from the queue
    while (!exit_flag) {
        if (mq_receive(mq, (char *)&message, sizeof(struct Message), NULL) == -1) {
            perror("mq_receive");
            exit(1);
        }
        
        if (message.access == 1) {
            printf("Current core: %s \n", message.id);
            printf("AccessType.readmiss\n");
            printf("Current address: %s \n", message.address);

        } else {
            printf("Unknown access type\n");
        }
        
        // Check for an exit condition
        // For example, set exit_flag to 1 when you want to exit the loop
        exit_flag = 1;
    }

    // Close and unlink the message queue
    mq_close(mq);
    mq_unlink("/myqueue");

    return 0;
}
