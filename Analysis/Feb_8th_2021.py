from numpy import genfromtxt
import matplotlib.pyplot as plt
my_data = genfromtxt('../Data/Feb_8th_2021.txt', delimiter=' ')
mystates = {"Off":0, "Initialized":1, "Ready":2,"HeatingOn":3, "HeatingOff":4 ,"DumpResults":5}
my_curated_data = my_data[:,[0,4,8,9,10,11,12,13,14,15]]

delta_T_Above_minus_below = (my_curated_data[:,4]+my_curated_data[:,2])-(my_curated_data[:,7]+my_curated_data[:,5])

timestamp_moments= {    1612777420: "15V, 0.4A",
                        1612778093:"touched sensor 5",
                        1612780191: "fan on, 19V, 0.5A",
                        1612780947: "heating off, fan on",
                        1612781186: "fan on, cleaned interface; 19V 0.4A",
                        1612781720: "24V, 0.58A",
                        1612781938: "added weight",
                        1612781170: "added termocondictive paste; 19V 0.4A",
                        1612778229: "24V, 0.58A",
                        1612782640: "add other paste",
                        1612783030: "24V, 0.58A",
                        1612784359: "discovered reversed fan; removed paste and added foil",
                        1612784422: "0V to cool down",
                        1612784528: "19V 0.48A",
                        1612784687:  "24V, 0.58A"                       }


#sensors
# 0,1,2,3,4,5,6,7
# 2,3,4,5,6,7,8,9

#typically ABOVE are sensors 0,2; below sensors 3,5
fig, ax = plt.subplots(2)

#sensors
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,2], label = 'Sensor 0')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,4], label = 'Sensor 2')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,5], label = 'Sensor 3')
ax[0].scatter(my_curated_data[:,0],my_curated_data[:,7], label = 'Sensor 5')
ax[0].set_ylim([-10,100])

trans = ax[0].get_xaxis_transform()
delta = .5

for i in timestamp_moments:
    ax[0].vlines(i,-10,100)
    ax[0].text(i,delta,timestamp_moments[i])
    delta = delta+4
    if (delta>40):
        delta=0.5

ax[0].legend()
ax[0].grid(True)


ax[1].scatter(my_curated_data[:,0],delta_T_Above_minus_below, label = 'Delta T above-below')
ax[1].legend()
ax[1].grid(True)
ax[1].set_ylim([-10,10])

trans = ax[1].get_xaxis_transform()
delta = .5

for i in timestamp_moments:
    ax[1].vlines(i,-1000,1000)
    ax[1].text(i,delta,timestamp_moments[i])
    delta = delta+4
    if (delta>40):
        delta=0.5



plt.show()


