from numpy import genfromtxt
import matplotlib.pyplot as plt
my_data = genfromtxt('../Data/tklog_1612545297.txt', delimiter=' ')
mystates = {"Off":0, "Initialized":1, "Ready":2,"HeatingOn":3, "HeatingOff":4 ,"DumpResults":5}
my_curated_data = my_data[:,[0,4,8,9,10,11,12,13,14,15]]
#sensors
# 0,1,2,3,4,5,6,7
# 2,3,4,5,6,7,8,9

#sensors
plt.scatter(my_curated_data[:,0],my_curated_data[:,2])
plt.scatter(my_curated_data[:,0],my_curated_data[:,4])
plt.scatter(my_curated_data[:,0],my_curated_data[:,5])
plt.scatter(my_curated_data[:,0],my_curated_data[:,7])
