
#include "../include/messages.h"

// Create and open the message queue
mqd_t create_message_queue() {
    mqd_t mq;
    struct mq_attr attr;

    // Set message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = sizeof(struct Message);
    attr.mq_curmsgs = 0;

    // Create and open the message queue
    mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0666, &attr);
    
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    return mq;
}

// Send a message to the message queue
void send_message(mqd_t mq, struct Message *message) {
    if (mq_send(mq, (char *)message, sizeof(struct Message), 0) == -1) {
        perror("mq_send");
        exit(1);
    }
}

// Close and unlink the message queue
void cleanup_message_queue(mqd_t mq) {
    mq_close(mq);
    mq_unlink(MQ_NAME);
}
