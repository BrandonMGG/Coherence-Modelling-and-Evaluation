#include "../../include/messages.h"

int main() {
    mqd_t mq;
    struct Message message;

    // Open the message queue
    mq = mq_open(MQ_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    while (1) {
        // Receive a message from the queue
        if (mq_receive(mq, (char *)&message, sizeof(struct Message), NULL) == -1) {
            perror("mq_receive");
            break;
        }

        // Process the received message (e.g., trigger cache coherence actions)
        printf("Received message: id=%s, access=%d, address=%s, block_id=%d\n",
               message.id, message.access, message.address, message.block_id);
    }

    // Close the message queue
    mq_close(mq);

    return 0;
}
