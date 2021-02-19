from numpy import genfromtxt
import matplotlib.pyplot as plt
my_data = genfromtxt('../Data/Feb_9th_2021.txt', delimiter=' ')
mystates = {"Off":0, "Initialized":1, "Ready":2,"HeatingOn":3, "HeatingOff":4 ,"DumpResults":5}
my_curated_data = my_data[:,[0,4,8,9,10,11,12,13,14,15]]


timestamp_moments= {
    1612885161: "V=19V, A=0.48A; fan on",
    1612885732: "V=24V, A=0.59A",
    1612886020: "heating off, cambio basetta carbonio",
    1612886153: "V=19V, A=0.48A",
    1612886591: "V=24V, A=0.59A",
    1612887293: "heating off"
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
ax[1].set_ylim([-20,20])

trans = ax[1].get_xaxis_transform()
delta = -9

for i in timestamp_moments:
    ax[1].vlines(i,-1000,1000)
    ax[1].text(i,delta,timestamp_moments[i])
    delta = delta+2
    if (delta>9):
        delta=-9

plt.savefig("Feb_9th_2021.png")


plt.show()


