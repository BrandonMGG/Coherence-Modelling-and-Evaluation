
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "../../include/messages.h"


int main() {
    mqd_t mq;
    struct Message message;

    // Create and open the message queue
    mq = create_message_queue();

    srand(time(NULL));

    // Generate and send messages (for demonstration, repeat 5 times)
    for (int i = 0; i < 5; i++) {
        snprintf(message.id, sizeof(message.id), "CPU-%d", i);
        message.access = rand() % 2; // Random access type (0 or 1)
        snprintf(message.address, sizeof(message.address), "0x%x", rand());
        message.block_id = rand() % 16; // Random block ID

        // Send the message to the queue
        send_message(mq, &message);

        // Sleep briefly to simulate processing time
        sleep(2); // Sleep for 2 s
    }


    // Clean up and close the message queue
    cleanup_message_queue(mq);

    return 0;
}
