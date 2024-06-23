import numpy as np
import matplotlib.pyplot as plt

# Recebe os dados coletados no Arduino IDE
lu = open("C:/Users/yodap/Desktop/data/sensor.txt", "r")
ls = open("C:/Users/yodap/Desktop/data/solenoide.txt", "r")
lt = open("C:/Users/yodap/Desktop/data/timer.txt", "r")
u = []
s = []
t = []
# Transforma em lista para melhor manipular
for i in lu:
    u.append([int(j) for j in i.split()])
for i in ls:
    s.append([int(j) for j in i.split()])
for i in lt:
    t.append([int(j) for j in i.split()])
u = np.array(u)
s = np.array(s)
t = np.array(t)
for i in u:  # Plota cada execução da tarefa do sensor
    plt.plot([i[0], i[1]], [0, 0], linewidth=30, color="#7bccc4", solid_capstyle="butt")
for i in s:  # Plota cada execução da tarefa da válvula
    plt.plot([i[0], i[1]], [1, 1], linewidth=30, color="#4eb3d3", solid_capstyle="butt")
for i in t:  # Plota cada execução da tarefa do display
    plt.plot([i[0], i[1]], [2, 2], linewidth=30, color="#2b8cbe", solid_capstyle="butt")
# Ajusta o gráfico para melhor visualização
plt.xlim(750, 1400)
plt.ylim(-0.5, 2.500)
# Legendas e limites entre tarefas
plt.yticks([0, 1, 2], ["Sensor", "Válvula", "Timer"])
plt.axhline(y=0.5, color='#ececec', linewidth=2)
plt.axhline(y=1.5, color='#ececec', linewidth=2)
plt.axhline(y=2.5, color='#ececec', linewidth=2)
plt.xlabel("Ticks")
plt.ylabel("Tarefa")
plt.show()
