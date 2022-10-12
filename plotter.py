#!/usr/bin/env python

# import pandas as pd
# import matplotlib.pyplot as pl
# import serial

# ser = serial.Serial("/dev/tty.usbmodem1101", 115200, timeout=1)

# df = pd.read_csv("./data.csv")
# df.plot(x="Program Time [s]", secondary_y=["Velocity", "Acceleration", "Jerk"])

# plt.show()



from threading import Thread
import serial
import time
import collections
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import struct
import copy
import pandas as pd


class serialPlot:
    def __init__(self, serialPort='/dev/tty.usbmodem1101', serialBaud=115200, plotLength=100, dataNumBytes=2, numPlots=1):
        self.port = serialPort
        self.baud = serialBaud
        self.plotMaxLength = plotLength
        self.dataNumBytes = dataNumBytes
        self.numPlots = numPlots
        self.rawData = bytearray(numPlots * dataNumBytes)
        self.dataType = None
        if dataNumBytes == 2:
            self.dataType = 'h'     # 2 byte integer
        elif dataNumBytes == 4:
            self.dataType = 'f'     # 4 byte float
        self.data = []
        for i in range(numPlots):   # give an array for each type of data and store them in a list
            self.data.append(collections.deque([0] * plotLength, maxlen=plotLength))
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.plotTimer = 0
        self.previousTimer = 0
        # self.csvData = []

        print('Trying to connect to: ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
        try:
            self.serialConnection = serial.Serial(serialPort, serialBaud, timeout=4)
            print('Connected to ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')
        except:
            print("Failed to connect with " + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.')

    def readSerialStart(self):
        if self.thread == None:
            self.thread = Thread(target=self.backgroundThread)
            self.thread.start()
            # Block till we start receiving values
            while self.isReceiving != True:
                time.sleep(0.1)

    def getSerialData(self, frame, lines, lineValueText, lineLabel, timeText):
        currentTimer = time.perf_counter()
        self.plotTimer = int((currentTimer - self.previousTimer) * 1000)     # the first reading will be erroneous
        self.previousTimer = currentTimer
        timeText.set_text('Plot Interval = ' + str(self.plotTimer) + 'ms')
        privateData = copy.deepcopy(self.rawData[:])    # so that the 3 values in our plots will be synchronized to the same sample time
        for i in range(self.numPlots):
            data = privateData[(i*self.dataNumBytes):(self.dataNumBytes + i*self.dataNumBytes)]
            value,  = struct.unpack(self.dataType, data)
            self.data[i].append(value)    # we get the latest data point and append it to our array
            lines[i].set_data(range(self.plotMaxLength), self.data[i])
            lineValueText[i].set_text('[' + lineLabel[i] + '] = ' + str(value))
        # self.csvData.append([self.data[0][-1], self.data[1][-1], self.data[2][-1]])

    def backgroundThread(self):    # retrieve data
        time.sleep(1.0)  # give some buffer time for retrieving data
        self.serialConnection.reset_input_buffer()
        while (self.isRun):
            self.serialConnection.readinto(self.rawData)
            self.isReceiving = True
            #print(self.rawData)

    def close(self):
        self.isRun = False
        self.thread.join()
        self.serialConnection.close()
        print('Disconnected...')
        # df = pd.DataFrame(self.csvData)
        # df.to_csv('/home/rikisenia/Desktop/data.csv')


def main():
    # portName = 'COM5'
    portName = '/dev/ttyUSB0'
    baudRate = 38400
    maxPlotLength = 100     # number of points in x-axis of real time plot
    dataNumBytes = 4        # number of bytes of 1 data point
    numPlots = 3            # number of plots in 1 graph
    s = serialPlot(portName, baudRate, maxPlotLength, dataNumBytes, numPlots)   # initializes all required variables
    s.readSerialStart()                                               # starts background thread

    # plotting starts below
    pltInterval = 50    # Period at which the plot animation updates [ms]
    xmin = 0
    xmax = maxPlotLength
    ymin = -(1)
    ymax = 1
    fig = plt.figure(figsize=(10, 8))
    ax = plt.axes(xlim=(xmin, xmax), ylim=(float(ymin - (ymax - ymin) / 10), float(ymax + (ymax - ymin) / 10)))
    ax.set_title('Arduino Accelerometer')
    ax.set_xlabel("Time")
    ax.set_ylabel("Accelerometer Output")

    lineLabel = ['X', 'Y', 'Z']
    style = ['r-', 'c-', 'b-']  # linestyles for the different plots
    timeText = ax.text(0.70, 0.95, '', transform=ax.transAxes)
    lines = []
    lineValueText = []
    for i in range(numPlots):
        lines.append(ax.plot([], [], style[i], label=lineLabel[i])[0])
        lineValueText.append(ax.text(0.70, 0.90-i*0.05, '', transform=ax.transAxes))
    anim = animation.FuncAnimation(fig, s.getSerialData, fargs=(lines, lineValueText, lineLabel, timeText), interval=pltInterval)    # fargs has to be a tuple

    plt.legend(loc="upper left")
    plt.show()

    s.close()


if __name__ == '__main__':
    main()