from numpy import genfromtxt
import matplotlib.pyplot as plt
my_data = genfromtxt('../Data/Feb_8th_2021.txt', delimiter=' ')
mystates = {"Off":0, "Initialized":1, "Ready":2,"HeatingOn":3, "HeatingOff":4 ,"DumpResults":5}
my_curated_data = my_data[:,[0,4,8,9,10,11,12,13,14,15]]

delta_T_Above_minus_below = (my_curated_data[:,4]+my_curated_data[:,2])-(my_curated_data[:,7]+my_curated_data[:,5])

#sensors
# 0,1,2,3,4,5,6,7
# 2,3,4,5,6,7,8,9
fig, ax = plt.subplots(2)

#sensors
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,2], label = 'Sensor 0')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,4], label = 'Sensor 2')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,5], label = 'Sensor 3')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,7], label = 'Sensor 5')


ax[0].legend()
ax[0].grid(True)


ax[1].scatter(my_curated_data[:,0],delta_T_Above_minus_below, label = 'Delta T above-below')
ax[1].legend()
ax[1].grid(True)

plt.show()


