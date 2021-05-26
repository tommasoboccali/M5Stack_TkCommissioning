from numpy import genfromtxt
import matplotlib.pyplot as plt
import os, datetime
from math import floor, ceil
import matplotlib.dates as mdates
from pytz import timezone
xformatter = mdates.DateFormatter('%H:%M')

os.system("mkdir -p plots")

sensor_config = {
    '../Data/tklog_1619104693.txt': ([(7,3),],""), #(0,1),(2,4),(5,6)
#    '../Data/tklog_1621846908.txt': ([(0,7)],""),
#    '../Data/tklog_1621847007.txt': ([(0,7)],""),
#    '../Data/tklog_1621848040.txt': ([(0,7)],""),
#    '../Data/tklog_1621848543.txt': ([(0,7)],""),
#    '../Data/tklog_1621848738.txt': ([(0,7)],""),
#    '../Data/tklog_1621849015.txt': ([(0,7)],""),
#    '../Data/tklog_1621849264.txt': ([(0,7)],""),
#    '../Data/tklog_1621850280.txt': ([(0,7)],""),
#    '../Data/tklog_1621850596.txt': ([(0,7)],""),
#    '../Data/tklog_1621851072.txt': ([(0,7)],""),
#    '../Data/tklog_1621875533.txt': ([(0,7)],""),
#    '../Data/tklog_1621932290.txt': ([(0,7)],""),
#    '../Data/tklog_1621938103.txt': ([(0,7),(1,3)],""),
}

for inputFile in sensor_config:
    sensor_pairs = sensor_config[inputFile][0]
    title = sensor_config[inputFile][1]
    #sensors_up = 1,2
    #sensors_down = 3,4
    sensors = [i[0] for i in sensor_pairs] + [i[1] for i in sensor_pairs]
    
    inputFileNew = inputFile.replace("tklog_","data_")
    inputFileLog = inputFile.replace("tklog_","log_")
    os.system("cat %s | grep -v Log > %s" %(inputFile,inputFileNew))
    os.system("cat %s | grep Log > %s" %(inputFile,inputFileLog))
    my_data = genfromtxt(inputFileNew, delimiter=' ')
    mystates = {"Off":0, "Initialized":1, "Ready":2,"HeatingOn":3, "HeatingOff":4 ,"DumpResults":5}
    my_curated_data = my_data[:,[0,4,8,9,10,11,12,13,14,15]]

    delta_T_Above_minus_below = {}
    for sensor_pair in sensor_pairs:
        delta_T_Above_minus_below[sensor_pair] = ( my_curated_data[:,sensor_pair[0]+2] - my_curated_data[:,sensor_pair[1]+2] )

    ### File timestamp
    timestamp_moments= {    
    #    1621851072: "AAA",
    }

    logFile = open(inputFileLog)
    for l in logFile.readlines():
        timestamp_moments[datetime.datetime.utcfromtimestamp(int(l.split(" Log ")[0]))] = l.split(" Log ")[1].replace("\n","") #,timezone('Europe/Amsterdam')

#    print(timestamp_moments)

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
    for sensor in sensors:
        ax[0].plot_date(time,my_curated_data[:,sensor+2], label = 'Sensor #%d'%sensor)
    #ax[0].set_ylim([-10,100])
    min_ = min([min(my_curated_data[:,i+2]) for i in sensors])
    max_ = max([max(my_curated_data[:,i+2]) for i in sensors])
    binSize = 1
    min_ = binSize*floor(min_ /binSize )
    max_ = binSize*ceil(max_/binSize)
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

    for sensor_pair in sensor_pairs:
        ax[1].plot_date(time,delta_T_Above_minus_below[sensor_pair], label = 'ΔT (#%d - #%d)'%(sensor_pair[0],sensor_pair[1]))
    ax[1].legend()
    ax[1].grid(True)
    #ax[1].set_ylim([-10,10])
    min_ = min([min(delta_T_Above_minus_below[sensor_pair]) for sensor_pair in sensor_pairs])
    max_ = max([max(delta_T_Above_minus_below[sensor_pair]) for sensor_pair in sensor_pairs])
    binSize = 0.2
    min_ = binSize*floor(min_ /binSize )
    max_ = binSize*ceil(max_/binSize)
    ax[1].set_ylim([min_ ,max_ ])

    trans = ax[1].get_xaxis_transform()
    delta = min_

    for i in timestamp_moments:
        ax[1].vlines(i,-1000,1000)
        ax[1].text(i,delta,timestamp_moments[i])
        delta = delta+(max_-min_)/10
        if (delta>max_):
            delta=min_

    if title != "":
        ax[0].set_title(title + " [%s]"%datetime.datetime.utcfromtimestamp(my_data[0][0]).strftime('%d/%m %H:%M')) 
    else:
        ax[0].set_title("Test %s"%datetime.datetime.utcfromtimestamp(my_data[0][0]).strftime('%d/%m %H:%M')) 
    ax[1].set_xlabel("Time") 
    ax[0].set_ylabel("Temperature [°C]") 
    ax[1].set_ylabel("ΔT [°C]") 

    plt.gcf().axes[0].xaxis.set_major_formatter(xformatter)
    plt.gcf().axes[1].xaxis.set_major_formatter(xformatter)

    outFile = "plots/%s.png"%datetime.datetime.utcfromtimestamp(my_data[0][0]).strftime('%y_%m_%d_%H_%M_%S')
    print("%s saved, generated from %s"%(outFile,inputFile))
    plt.savefig(outFile)
#    plt.show()

