import matplotlib.pyplot as plt

# Function to parse the data file and extract statistics
def parse_data_file(filename):
    protocol_data = {}

    with open(filename, 'r') as file:
        for line in file:
            if line.startswith("PROTOCOL"):
                parts = line.split(',')
                protocol = parts[0].split()[1]
                stats = {}

                
                for part in parts[1:-1]:
                    key, value = part.split('=')
                    stats[key] = int(value)

                # Process the last key-value pair separately
                last_part = parts[-1].strip() 
                key, value = last_part.split('=')
                stats[key] = float(value)  # Convert the last value to float

                protocol_data[protocol] = stats

    return protocol_data


# Function to generate graphs based on the statistics
def generate_graphs(data):
    # Extract data for plotting
    protocols = list(data.keys())
    inv_req_resp = [data[protocol]['INV_REQ_RESP'] for protocol in protocols]
    read_req_resp = [data[protocol]['READ_REQ_RESP'] for protocol in protocols]
    write_req_resp = [data[protocol]['WRITE_REQ_RESP'] for protocol in protocols]
    incr_req_resp = [data[protocol]['INCR_REQ_RESP'] for protocol in protocols]
    avg_exec = [data[protocol]['avg_exec'] for protocol in protocols]
    
    # Create bar plots for each statistic
    plt.figure(figsize=(12, 6))
    
    plt.subplot(2, 2, 1)
    plt.bar(protocols, inv_req_resp)
    plt.title('INV_REQ_RESP per core')
    
    plt.subplot(2, 2, 2)
    plt.bar(protocols, read_req_resp)
    plt.title('READ_REQ_RESP per core')
    
    plt.subplot(2, 2, 3)
    plt.bar(protocols, write_req_resp)
    plt.title('WRITE_REQ_RESP per core')
    
    plt.subplot(2, 2, 4)
    plt.bar(protocols, incr_req_resp)
    plt.title('INCR_REQ_RESP per core')
    plt.savefig("results/req_reps.png")
    plt.tight_layout()
    
    
    # save avg exec time per core
    plt.figure(figsize=(8, 4))
    plt.bar(protocols, avg_exec)
    plt.title('avg_exec per core')
    plt.savefig("results/avg_exec.png")
    

if __name__ == "__main__":
    filename = "results/stats.txt"  # Replace with the actual data file path
    data = parse_data_file(filename)
    generate_graphs(data)

