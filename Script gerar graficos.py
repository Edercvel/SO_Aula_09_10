import matplotlib.pyplot as plt
import numpy as np

# Configuração de dados
threads = [1, 2, 4, 8, 12]

# Tempos Médios (ms)
t_pequena = [8.2284, 6.2888, 3.6783, 2.5696, 2.7703]
t_media   = [38.0685, 18.2625, 12.7917, 11.9265, 10.2122]
t_grande  = [138.7817, 82.4334, 47.6602, 28.5889, 37.8455]

# Speedups
s_pequena = [1.0000, 1.3084, 2.2370, 3.2022, 2.9702]
s_media   = [1.0000, 2.0845, 2.9760, 3.1919, 3.7277]
s_grande  = [1.0000, 1.6836, 2.9119, 4.8543, 3.6671]

# Eficiências (%)
e_pequena = [100.00, 65.42, 55.93, 40.03, 24.75]
e_media   = [100.00, 104.23, 74.40, 39.90, 31.06]
e_grande  = [100.00, 84.18, 72.80, 60.68, 30.56]

# Criando figura com 3 subplots
fig, axs = plt.subplots(1, 3, figsize=(18, 5))

# Gráfico 1: Tempo de Execução
axs[0].plot(threads, t_pequena, marker='o', label='Pequena (N=2000)')
axs[0].plot(threads, t_media, marker='s', label='Média (N=10000)')
axs[0].plot(threads, t_grande, marker='^', label='Grande (N=50000)')
axs[0].set_title('Tempo de Execução Médio (ms)')
axs[0].set_xlabel('Número de Threads')
axs[0].set_ylabel('Tempo (ms)')
axs[0].set_xticks(threads)
axs[0].grid(True, linestyle='--', alpha=0.6)
axs[0].legend()

# Gráfico 2: Speedup
axs[1].plot(threads, s_pequena, marker='o', label='Pequena')
axs[1].plot(threads, s_media, marker='s', label='Média')
axs[1].plot(threads, s_grande, marker='^', label='Grande')
axs[1].plot(threads, threads, color='gray', linestyle=':', label='Speedup Ideal (Linear)')
axs[1].set_title('Speedup ($S_p$)')
axs[1].set_xlabel('Número de Threads')
axs[1].set_ylabel('Speedup')
axs[1].set_xticks(threads)
axs[1].grid(True, linestyle='--', alpha=0.6)
axs[1].legend()

# Gráfico 3: Eficiência
axs[2].plot(threads, e_pequena, marker='o', label='Pequena')
axs[2].plot(threads, e_media, marker='s', label='Média')
axs[2].plot(threads, e_grande, marker='^', label='Grande')
axs[2].axhline(y=100, color='gray', linestyle=':', label='Eficiência Ideal (100%)')
axs[2].set_title('Eficiência ($E_p$) %')
axs[2].set_xlabel('Número de Threads')
axs[2].set_ylabel('Eficiência (%)')
axs[2].set_xticks(threads)
axs[2].grid(True, linestyle='--', alpha=0.6)
axs[2].legend()

plt.tight_layout()
plt.savefig('graficos_desempenho.png', dpi=300)
plt.show()