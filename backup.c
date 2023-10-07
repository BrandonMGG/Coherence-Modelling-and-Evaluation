#include <stdio.h>
#include <stdlib.h>

struct Stats {
    int INV;
    int READ_REQ_RESP;
    int WRITE_REQ_RESP;
    int INCR_REQ_RESP;
    double avg_exec;
};

void saveStatsToFile(const char* filename, int coreId, const struct Stats* stats) {
    FILE* file = fopen(filename, "w"); 
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Format and write stats to the file in the specified format
    fprintf(file, "PROTOCOL %d CORE %d,INV_REQ_RESP=%d,READ_REQ_RESP=%d,WRITE_REQ_RESP=%d,INCR_REQ_RESP=%d,avg_exec=%.2lf\n",
            coreId, coreId, stats->INV, stats->READ_REQ_RESP, stats->WRITE_REQ_RESP, stats->INCR_REQ_RESP, stats->avg_exec);

    fclose(file);
}

int main() {
    // Create sample struct Stats for a core
    struct Stats coreStats = {10, 20, 30, 40, 5.67};

    for (int i = 0; i < 3; i++)
    {
        saveStatsToFile("results/stats.txt", i, &coreStats);
    }
    
    
    const char *command = "python src_C/graph_results.py"; // Replace with the actual script name and arguments
    
    // Execute the command
    int result = system(command);
    

    return 0;
}
