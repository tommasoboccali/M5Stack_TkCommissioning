from numpy import genfromtxt
import matplotlib.pyplot as plt
my_data = genfromtxt('../Data/Feb_18th_2021.txt', delimiter=' ')
mystates = {"Off":0, "Initialized":1, "Ready":2,"HeatingOn":3, "HeatingOff":4 ,"DumpResults":5}
my_curated_data = my_data[:,[0,4,8,9,10,11,12,13,14,15]]


timestamp_moments= {
    1613655383: "V=19V A=0.49A FAN laterale",
    1613655903: "V=24V, A=0.58A",
    1613656097: "pigio sensore 1",
    1613656124: "pigio sensore 1",
    1613656252: "V=0V",
    1613656921: "metto ventola sopra, V=19V",
    1613657085: "V=0V"
    }


#sensors
# 0,1,2,3,4,5,6,7
#position in curated data
# 2,3,4,5,6,7,8,9

#typically are sensors 6,1; below sensors 3,5
    
delta_T_Above_minus_below = ( (my_curated_data[:,8]+my_curated_data[:,3])-(my_curated_data[:,7]+my_curated_data[:,5]) ) / 2
    
fig, ax = plt.subplots(2)

#sensors
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,8], label = 'Sensor 6')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,3], label = 'Sensor 1')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,5], label = 'Sensor 3')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,7], label = 'Sensor 5')
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
ax[1].set_ylim([-2,2])

trans = ax[1].get_xaxis_transform()
delta = -9

for i in timestamp_moments:
    ax[1].vlines(i,-1000,1000)
    ax[1].text(i,delta,timestamp_moments[i])
    delta = delta+.5
    if (delta>5):
        delta=-9

plt.savefig("Feb_18th_2021.png")


plt.show()


