from numpy import genfromtxt
import matplotlib.pyplot as plt
import os, datetime
from math import floor, ceil
import matplotlib.dates as mdates
from pytz import timezone
xformatter = mdates.DateFormatter('%H:%M')

os.system("mkdir -p plots")

sensor_config = {
#### primi test (Feb 5)
#    '../Data/tklog_1612542074.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1612545297.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Feb 8)
#    '../Data/tklog_1612777420.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1612780183.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1612781179.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1612782690.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1612783572.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1612783710.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1612784403.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Feb 9)
#    '../Data/tklog_1612884983.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Feb 17)
#    '../Data/tklog_1613571107.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1613571198.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1613573229.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Feb 18)
#    '../Data/tklog_1613654915.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1613655383.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1613656571.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1613656843.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Feb 19)
#    '../Data/tklog_1613744620.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1613749472.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Feb 22)
#    '../Data/tklog_1614008551.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614008861.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614009145.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614009300.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614009674.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614009880.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614010458.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614010548.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614010755.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614010943.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614011016.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614011084.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614012434.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614013287.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614013399.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614013442.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614013606.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614013732.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614013890.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614014045.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614014532.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614014738.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614014960.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614015096.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614015260.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614016013.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1614017781.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Feb 24)
#    '../Data/tklog_1614172794.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Mar 10)
#    '../Data/tklog_1615374687.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1615374753.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1615374827.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Mar 23)
#    '../Data/tklog_1616518108.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1616518155.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1616518205.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Mar 25)
#    '../Data/tklog_1616674114.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1616674188.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1616674304.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Mar 26)
#    '../Data/tklog_1616752352.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1616752647.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1616753605.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1616754630.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1616757007.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1616757831.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Apr 19)
#    '../Data/tklog_1618844856.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    '../Data/tklog_1618845418.txt': ([(0,1),(2,3),(4,5),(6,7),],""),

#### primi test (Apr 21)
#    '../Data/tklog_1618996352.txt': ([(0,1),(2,3),(4,5),(6,7),],""),
#    
#### primi test con i sensori avvitati sul modulo (Apr 22)
#    '../Data/tklog_1619104693.txt': ([(7,3),],"colla blu, sensori avvitati"), #(0,1),(2,4),(5,6) 
#    '../Data/tklog_1619105936.txt': ([(7,3),],"colla blu, sensori avvitati"), #(0,1),(2,4),(5,6)
#    '../Data/tklog_1619106339.txt': ([(7,3),],"colla blu, sensori avvitati"), #(0,1),(2,4),(5,6)
#    '../Data/tklog_1619107077.txt': ([(7,3),(0,1),(2,4),(5,6)],"raffreddamento"), #(0,1),(2,4),(5,6)
#    '../Data/tklog_1619107186.txt': ([(5,2),],"panino secco, sensori avvitati"), #(0,1),(3,4),(6,7)
#    '../Data/tklog_1619107964.txt': ([(5,2),],"panino secco, sensori avvitati"), #(0,1),(3,4),(6,7)
#    '../Data/tklog_1619108587.txt': ([(5,2),],"panino secco, sensori avvitati"), #(0,1),(3,4),(6,7)
#    '../Data/tklog_1619109439.txt': ([(5,2),],"raffreddamento"), #(0,1),(3,4),(6,7)

#### primi test con il julabo (Apr 23) - inutile, piastre scaccate a causa della vite ###
#    '../Data/tklog_1619166827.txt': ([(5,2),(3,4),(1,6),(0,7),],"test inutile (piastre distaccate a causa della vite)"),
#    '../Data/tklog_1619166924.txt': ([(5,2),(3,4),(1,6),(0,7),],"test inutile (piastre distaccate a causa della vite)"),
#    '../Data/tklog_1619167683.txt': ([(5,2),(3,4),(1,6),(0,7),],"test inutile (piastre distaccate a causa della vite)"),
#    '../Data/tklog_1619168877.txt': ([(5,2),(3,4),(1,6),(0,7),],"test inutile (piastre distaccate a causa della vite)"),
#    '../Data/tklog_1619169158.txt': ([(5,2),(3,4),(1,6),(0,7),],"test inutile (piastre distaccate a causa della vite)"),
#    '../Data/tklog_1619170382.txt': ([(5,2),(3,4),(1,6),(0,7),],"test inutile (piastre distaccate a causa della vite)"),
#    '../Data/tklog_1619171337.txt': ([(5,2),(3,4),(1,6),(0,7),],"test inutile (piastre distaccate a causa della vite)"),
#    '../Data/tklog_1619172110.txt': ([(5,2),(3,4),(1,6),(0,7),],"test inutile (piastre distaccate a causa della vite)"),
#    '../Data/tklog_1619175021.txt': ([(5,2),(3,4),(1,6),(0,7),],"test inutile (piastre distaccate a causa della vite)"),
#    '../Data/tklog_1619175259.txt': ([(5,2),(3,4),(1,6),(0,7),],"test inutile (piastre distaccate a causa della vite)"),

#### test con il julabo (May 7) ###
#    '../Data/tklog_1620373673.txt': ([(2,0),(4,6),(3,5),(1,7),],"niente"),
#    '../Data/tklog_1620373781.txt': ([(2,0),(4,6),(3,5),(1,7),],"niente"),
#    '../Data/tklog_1620373922.txt': ([(2,0),(4,6),(3,5),(1,7),],"julabo"),

#### test con il julabo (May 10) ###
#    '../Data/tklog_1620634689.txt': ([(2,0),(4,6),(3,5),(1,7),],"julabo"),

### test con il julabo (May 19) ###
#    '../Data/tklog_1621438259.txt': ([(2,0),(2,1)],"niente"),
#    '../Data/tklog_1621439089.txt': ([(2,0),(2,1)],"niente"),
#    '../Data/tklog_1621439794.txt': ([(2,0),(2,1)],"niente"),
#    '../Data/tklog_1621440013.txt': ([(2,0),(2,1)],"work in progress"),
    '../Data/tklog_1621440159.txt': ([(2,0),(2,1)],""),
    '../Data/tklog_1621442300.txt': ([(2,0),(2,1)],""),

### test con il julabo (May 21) ###
    '../Data/tklog_1621592934.txt': ([(5,2),(3,4),],"niente"),
    '../Data/tklog_1621597071.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621608807.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621610316.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621610372.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621611018.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621614135.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621614450.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621616484.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621617011.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621617305.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621617551.txt': ([(5,2),(3,4),],""),
    '../Data/tklog_1621617785.txt': ([(5,2),(3,4),],""),

## test con il julabo (May 24) ###
    '../Data/tklog_1621846908.txt': ([(0,7),(6,5)],"niente"),
    '../Data/tklog_1621847007.txt': ([(0,7),(6,5)],""),
    '../Data/tklog_1621848040.txt': ([(0,7),(6,5)],""),
    '../Data/tklog_1621848543.txt': ([(0,7),(6,5)],""),
    '../Data/tklog_1621848738.txt': ([(0,7),(6,5)],""),
    '../Data/tklog_1621849015.txt': ([(0,7),(6,5)],"raffreddamento"),
    '../Data/tklog_1621849264.txt': ([(0,7),(6,5)],"niente"),
    '../Data/tklog_1621850280.txt': ([(0,7),(6,5)],"niente"),
    '../Data/tklog_1621850596.txt': ([(0,7),(6,5)],""),
    '../Data/tklog_1621851072.txt': ([(0,7),(6,5)],""),
    '../Data/tklog_1621875533.txt': ([(0,7),(6,5)],""),

## test con il julabo (May 25) ###
    '../Data/tklog_1621932290.txt': ([(0,7),(6,5)],""),
    '../Data/tklog_1621938103.txt': ([(0,7),(6,5),(1,3)],""),
}

for inputFile in sensor_config:
    sensor_pairs = sensor_config[inputFile][0]
    title = sensor_config[inputFile][1]
    #sensors_up = 1,2
    #sensors_down = 3,4
    sensors = [i[0] for i in sensor_pairs] + [i[1] for i in sensor_pairs]
    sensors = set(sensors)
    
    inputFileNew = inputFile.replace("tklog_","data_")
    inputFileLog = inputFile.replace("tklog_","log_")
    os.system("cat %s | grep -a -v Log > %s" %(inputFile,inputFileNew))
    os.system("cat %s | grep -a Log > %s" %(inputFile,inputFileLog))
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

    logFile = open(inputFileLog,'r', encoding='ISO-8859-1')
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
        ax[0].vlines(i,-1000,1000)
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

