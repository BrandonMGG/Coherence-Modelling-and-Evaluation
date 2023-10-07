import matplotlib.pyplot as plt

# Leer los datos desde el archivo "stats.txt"
with open("results/stats.txt", "r") as file:
    lines = file.readlines()

# Procesar los datos desde el archivo
cores = []
inv_req_resp = []
read_req_resp = []
write_req_resp = []
incr_req_resp = []
avg_exec = []

for line in lines:
    parts = line.strip().split(",")
    core_info = parts[0].split()
    cores.append(f"Core {core_info[3]}")
    inv_req_resp.append(int(parts[1].split("=")[1]))
    read_req_resp.append(int(parts[2].split("=")[1]))
    write_req_resp.append(int(parts[3].split("=")[1]))
    incr_req_resp.append(int(parts[4].split("=")[1]))
    avg_exec.append(float(parts[5].split("=")[1]))

# Crear una figura y ejes
fig, ax = plt.subplots(figsize=(10, 6))

# Ancho de las barras
bar_width = 0.15

# Posiciones de las barras en el eje x
x = range(len(cores))

# Dibujar las barras para cada métrica
plt.bar(x, inv_req_resp, width=bar_width, label='INV_REQ_RESP')
plt.bar([i + bar_width for i in x], read_req_resp, width=bar_width, label='READ_REQ_RESP')
plt.bar([i + 2 * bar_width for i in x], write_req_resp, width=bar_width, label='WRITE_REQ_RESP')
plt.bar([i + 3 * bar_width for i in x], incr_req_resp, width=bar_width, label='INCR_REQ_RESP')
plt.bar([i + 4 * bar_width for i in x], avg_exec, width=bar_width, label='avg_exec')

# Etiquetas en el eje x
plt.xlabel('Cores')
plt.xticks([i + 2 * bar_width for i in x], cores)

# Etiquetas en el eje y
plt.ylabel('Valores')

# Título del gráfico
plt.title('Comparación de Métricas por Núcleo')

# Leyenda
plt.legend()

# Mostrar el gráfico
plt.tight_layout()
plt.show()
