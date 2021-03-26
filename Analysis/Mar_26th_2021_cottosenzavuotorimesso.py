from numpy import genfromtxt
import matplotlib.pyplot as plt
import os

infile = '../Data/Mar_26th_2021_cottosenzavuotorimesso.txt'

# create log file and data file
os.system("rm -f tmplog tmpdat")
os.system("cat "+infile+" | awk '($2 == \"Log\"){print}' > tmplog")
os.system("cat "+infile+" | awk '($2 == \"State\"){print}' > tmpdat")

my_data = genfromtxt('tmpdat', delimiter=' ')
mystates = {"Off":0, "Initialized":1, "Ready":2,"HeatingOn":3, "HeatingOff":4 ,"DumpResults":5}
my_curated_data = my_data[:,[0,4,8,9,10,11,12,13,14,15]]


#sensors
# 0,1,2,3,4,5,6,7
# 2,3,4,5,6,7,8,9

#typically ABOVE are sensors 4,0; below sensors 7,2
fig, ax = plt.subplots(2)


delta_T_Above_minus_below = ( (my_curated_data[:,6]+my_curated_data[:,2])-(my_curated_data[:,9]+my_curated_data[:,4]) ) / 2


# load events
timestamp_moments = {}

file1 = open('tmplog', 'r')
lines = file1.readlines()
for line in lines:
    spl = line.split()
    ts = int(spl[0])
    text = " ".join(spl[1:])
    timestamp_moments[ts]=text.rstrip()

print (timestamp_moments)
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
ax[1].set_ylim([-10,10])

trans = ax[1].get_xaxis_transform()
delta = -9

for i in timestamp_moments:
    ax[1].vlines(i,-1000,1000)
    ax[1].text(i,delta,timestamp_moments[i])
    delta = delta+2
    if (delta>9):
        delta=-9

plt.savefig("Mar_26th_2021_cottosenzavuotorimesso.png")

plt.show()


