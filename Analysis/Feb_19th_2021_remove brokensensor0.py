from numpy import genfromtxt
import matplotlib.pyplot as plt
my_data = genfromtxt('../Data/Feb_19th_2021.txt', delimiter=' ')
mystates = {"Off":0, "Initialized":1, "Ready":2,"HeatingOn":3, "HeatingOff":4 ,"DumpResults":5}
my_curated_data = my_data[:,[0,4,8,9,10,11,12,13,14,15]]


timestamp_moments= {
    1613748629: "V=19V A=0.49A FAN laterale",
    1613748994: "sensor 0 problematic",
    1613749034: "serson 0 fixed",
    1613749297: "sensor 0 still problems; I go all off",
    1613749637: "V=24V A=0.59",
    1613750162: "V=30V A=0.69A (new fan PS)",
    1613750482: "V=24V",
    1613750553: "Sensor 4 in the middle",
    1613750818: "V=30V A=0.69A (new fan PS)",
    1613751077: "all off",
    }


#sensors
# 0,1,2,3,4,5,6,7
#position in curated data
# 2,3,4,5,6,7,8,9

#typically are sensors 0,4; below sensors 2,7
    
#i do not consider sensor 0
delta_T_Above_minus_below = ( (my_curated_data[:,6]+my_curated_data[:,6])-(my_curated_data[:,4]+my_curated_data[:,9]) ) / 2
    
fig, ax = plt.subplots(2)

#sensors
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,2], label = 'Sensor 0')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,6], label = 'Sensor 4')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,4], label = 'Sensor 2')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,9], label = 'Sensor 7')
ax[0].set_ylim([-10,100])

trans = ax[0].get_xaxis_transform()
delta =-8

for i in timestamp_moments:
    ax[0].vlines(i,-10,100)
    ax[0].text(i,delta,timestamp_moments[i])
    delta = delta+4
    if (delta>40):
        delta=-8

ax[0].legend()
ax[0].grid(True)


ax[1].scatter(my_curated_data[:,0],delta_T_Above_minus_below, label = 'Delta T above-below')
ax[1].legend()
ax[1].grid(True)
ax[1].set_ylim([-5,5])

trans = ax[1].get_xaxis_transform()
delta = -9

for i in timestamp_moments:
    ax[1].vlines(i,-1000,1000)
    ax[1].text(i,delta,timestamp_moments[i])
    delta = delta+.5
    if (delta>5):
        delta=-9

plt.savefig("Feb_19th_2021_removedbroken0.png")


plt.show()


