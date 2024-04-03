import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from mpl_toolkits.mplot3d import Axes3D  
import pandas as pd

num_steps = 1000

def load_particle_positions(step):
    filename = f'/home/yo/Documents/NBodySimulations/BH/files/positions_{step}.csv'
    data = pd.read_csv(filename)
    x_positions = data['X'].values
    y_positions = data['Y'].values
    z_positions = data['Z'].values
    return x_positions, y_positions, z_positions

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')  
ax.set_xlim([-20, 20])
ax.set_ylim([-20, 20])
ax.set_zlim([-20, 20])
ax.set_facecolor('black')  
particles, = ax.plot([], [], [], 'wo', ms=0.5)  
def init():
    particles.set_data([], [])
    particles.set_3d_properties([])  
    return particles,

def animate(step):
    x, y, z = load_particle_positions(step)
    particles.set_data(x, y)
    particles.set_3d_properties(z)  
    return particles,

ani = animation.FuncAnimation(fig, animate, init_func=init, frames=num_steps, interval=10, blit=False)

plt.show()

ani.save('/home/yo/Documents/NBodySimulations/BH/files/simul_3D.gif', writer='pillow', fps=100)

