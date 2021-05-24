import wget, json, os
import time
import numpy as np 
import matplotlib
import datetime
#matplotlib.use('TkAgg')
from matplotlib import pyplot as plt 
import matplotlib.dates as mdates
url = 'http://192.168.0.70/'

xformatter = mdates.DateFormatter('%H:%M')

import urllib.request

folder = "dataColdBox"

try:
    os.mkdir(folder)
except:
    pass

def makePlot(folder):
	dataFiles = os.listdir(folder)
	x = []
	y = []
	yD = []
	for dataFile in sorted(dataFiles):
		fName = "%s/%s"%(folder, dataFile)
		#print(fName)
		if int(dataFile)>int(time.time()-60*60*3):
			data = json.load(open(fName))
			#x.append(int(dataFile))
			x.append(datetime.datetime.utcfromtimestamp(int(dataFile)))
			y.append(data["ColdBox"]["Temperature"])
			yD.append(data["ColdBox"]["DewPoint"])
	#print(x,y)
	plt.clf()
	plt.plot(np.array(x),np.array(y))
	plt.plot(np.array(x),np.array(yD))
	plt.legend(['Temperature', 'Dew Point'])
	plt.title("Cold box temperature") 
	plt.xlabel("Day time") 
	plt.ylabel("Temperature [°C]") 
	plt.gcf().axes[0].xaxis.set_major_formatter(xformatter)
	return plt
	
#        data = json.loads(open(folder))
#        return data

while True:
	timestamp = int(time.time())
	#download data
	wget.download(url, "%s/%d"%(folder, timestamp))
	#make plot
	plt = makePlot(folder)
	print()
	print("updating plot...")
	plt.savefig("mygraph.png")
	#plt.show()
	time.sleep(20)




import numpy as np 
from matplotlib import pyplot as plt 

#24.8V 2.46A 17V 1.34A
#28V 2.17A 34.5V 2.63A 19.4V 
#nuovo test con sensore 0 sopra allo scaldino, al centro
