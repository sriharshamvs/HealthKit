from datetime import datetime
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import cufflinks as cf

names=['sensorType','uid','epoch','sensorReading']
df = pd.read_csv('bpm.csv',names = names)

df= df.replace(0,80)

df['epoch'] = pd.to_datetime(df.epoch,unit='s')

#df['date'] = pd.to_datetime(df['epoch']).dt.date
#df['time'] = pd.to_datetime(df['epoch']).dt.time

#df = df.drop(columns=['sensorType','epoch'])
df = df.drop(columns=['sensorType'])
#df = df[['uid','date','time','sensorReading']]

print(df)

#df.plot(x='time',y='sensorReading',style=['o'],linestyle='-')
df.plot(x='epoch',y='sensorReading',style=['o'],linestyle = '-')
plt.title('Heart Rate')
plt.xlabel('Date and Time')
plt.ylabel('BPM')
plt.show()
