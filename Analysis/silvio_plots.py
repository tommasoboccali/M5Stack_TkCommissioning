from numpy import genfromtxt
import matplotlib.pyplot as plt
import os, datetime
import matplotlib.dates as mdates
xformatter = mdates.DateFormatter('%H:%M')

sensor_pairs = [(0,3), (2,5)]
#sensors_up = 1,2
#sensors_down = 3,4

inputFile = '../Data/tklog_1621851072.txt'
inputFileNew = inputFile.replace(".txt","_data.txt")
inputFileLog = inputFile.replace(".txt","_log.txt")
os.system("cat %s | grep -v Log > %s" %(inputFile,inputFileNew))
os.system("cat %s | grep Log > %s" %(inputFile,inputFileLog))
my_data = genfromtxt(inputFileNew, delimiter=' ')
mystates = {"Off":0, "Initialized":1, "Ready":2,"HeatingOn":3, "HeatingOff":4 ,"DumpResults":5}
my_curated_data = my_data[:,[0,4,8,9,10,11,12,13,14,15]]

delta_T_Above_minus_below = ( (my_curated_data[:,4]+my_curated_data[:,2])-(my_curated_data[:,7]+my_curated_data[:,5]) ) / 2

### File timestamp
timestamp_moments= {    
#    1621851072: "AAA",
}

logFile = open(inputFileLog)
for l in logFile.readlines():
    timestamp_moments[datetime.datetime.utcfromtimestamp(int(l.split(" Log ")[0]))] = l.split(" Log ")[1].replace("\n","")

print(timestamp_moments)

####

#sensors
# 0,1,2,3,4,5,6,7
# 2,3,4,5,6,7,8,9

my_dpi=96
#typically ABOVE are sensors 0,2; below sensors 3,5
fig, ax = plt.subplots(2, figsize=(1600/my_dpi, 882/my_dpi), dpi=my_dpi)

#sensors
time = [datetime.datetime.utcfromtimestamp(float(t)) for t in my_curated_data[:,0]]
#time = my_curated_data[:,0]
ax[0].plot_date(time,my_curated_data[:,2], label = 'Sensor 0')
ax[0].plot_date(time,my_curated_data[:,4], label = 'Sensor 2')
ax[0].plot_date(time,my_curated_data[:,5], label = 'Sensor 3')
ax[0].plot_date(time,my_curated_data[:,7], label = 'Sensor 5')
#ax[0].set_ylim([-10,100])
min_ = min(min(my_curated_data[:,2]),min(my_curated_data[:,4]),min(my_curated_data[:,5]),min(my_curated_data[:,7]))
max_ = max(max(my_curated_data[:,2]),max(my_curated_data[:,4]),max(my_curated_data[:,5]),max(my_curated_data[:,7]))
binSize = 2
min_ = binSize*round(min_/binSize-1)
max_ = binSize*round(max_/binSize+1)
ax[0].set_ylim([min_, max_])

trans = ax[0].get_xaxis_transform()
delta = min_

for i in timestamp_moments:
    ax[0].vlines(i,-10,100)
    ax[0].text(i,delta,timestamp_moments[i])
    delta = delta+(max_-min_)/10
    if (delta>max_):
        delta=min_

ax[0].legend()
ax[0].grid(True)

ax[1].plot_date(time,delta_T_Above_minus_below, label = 'Delta T above-below')
ax[1].legend()
ax[1].grid(True)
#ax[1].set_ylim([-10,10])
binSize = 1
min_ = binSize*round(min(delta_T_Above_minus_below) /binSize -1)
max_ = binSize*round(max(delta_T_Above_minus_below) /binSize +1)
ax[1].set_ylim([min_ ,max_ ])

trans = ax[1].get_xaxis_transform()
delta = min_

for i in timestamp_moments:
    ax[1].vlines(i,-1000,1000)
    ax[1].text(i,delta,timestamp_moments[i])
    delta = delta+(max_-min_)/10
    if (delta>max_):
        delta=min_

ax[0].set_title("Test %s"%datetime.datetime.fromtimestamp(my_data[0][0]).strftime('%d/%m %H:%M')) 
ax[1].set_xlabel("Time") 
ax[0].set_ylabel("Temperature [°C]") 
ax[1].set_ylabel("ΔT [°C]") 

plt.gcf().axes[0].xaxis.set_major_formatter(xformatter)
plt.gcf().axes[1].xaxis.set_major_formatter(xformatter)

plt.savefig("%s.png"%datetime.datetime.fromtimestamp(my_data[0][0]).strftime('%y_%m_%d_%H_%M_%S'))
plt.show()

