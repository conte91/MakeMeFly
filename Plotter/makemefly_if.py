#!/usr/bin/env python
import sys

import pygtk

from collections import deque
from threading import Timer
from struct import unpack
from time import sleep
from serial import Serial
from threading import Thread
from threading import Lock
from threading import Condition
from gi.repository import Gtk
from matplotlib.figure import Figure
from matplotlib.axes import Subplot
from matplotlib.backends.backend_gtk3agg import FigureCanvasGTK3Agg as PlotCanvas
import numpy as np

class myInterface():

    def __init__(self, port, baud):
        print("Welcome to the MakeMeFly interface :D\nEnjoy your trip!")

        self.port=port
        self.baud=baud
        self.s=Serial(self.port, self.baud, timeout=0.5)
        self.inhibited=False;

        self.dataLock=Lock()
        self.printDataLock=Lock()
        self.bufLock=Lock()
        self.serialLock=Lock()
        self.saveStuffLock=Lock()
        self.buf=[]

        self.builder=Gtk.Builder()
        self.builder.add_from_file('Plotter_IF.glade')
        self.builder.connect_signals(self)

        self.squareWave=[]
        self.outputUp=[]
        self.outputDown=[]
        self.sw=0
        self.co=0
        self.ou=0
        self.od=0
        self.gb=0
        self.gp=0
        self.ur=0
        self.dr=0
        self.bM=0
        self.bm=0
        self.l=[]

        self.gainBase=[]
        self.gainPos=[]
        self.upRate=[]
        self.downRate=[]
        self.BMax=0
        self.BMin=0
        self.level=[]

        self.nData=1000
        self.BData=deque(maxlen=1000)
        self.filteredValueData=deque(maxlen=1000)
        self.coilBData=deque(maxlen=1000)
        self.BData.append(0)
        self.filteredValueData.append(0)
        self.coilBData.append(0)
        self.XData=[0.0004*x for x in range(0,1000)]

        self.window=self.builder.get_object("mainWindow")
        self.builder.get_object("sqWaveImage").set_from_stock("gtk-no", Gtk.IconSize.BUTTON)

        #Start of Matplotlib specific code
        ##########################################################################
        self.figure = Figure(figsize=(8,6), dpi=71)
        self.axisB = self.figure.add_subplot(311)
        self.axisB.set_xlabel('Time (ticks)')
        self.axisB.set_ylabel('Measured magnetic field')

        self.axisBC = self.figure.add_subplot(313)
        self.axisBC.set_xlabel('Time (s)')
        self.axisBC.set_ylabel('Emulated magnetic field')

        self.axisFV = self.figure.add_subplot(312)
        self.axisFV.set_xlabel('Time (s)')
        self.axisFV.set_ylabel('Filtered magnetic field')

        self.plotCanvas = PlotCanvas(self.figure)  # a Gtk.DrawingArea
        self.plotCanvas.set_size_request(500,600)
        a=[0]
        self.plotB=self.axisB.plot(a)[0]
        self.plotBC=self.axisBC.plot(a)[0]
        self.plotFV=self.axisFV.plot(a)[0]
        self.plotCanvas.draw()

        self.builder.get_object("plotGrid").attach(self.plotCanvas, 0,2,1,1)
        self.window.show_all()
        self.window.connect("destroy", Gtk.main_quit)

        self.newBytes=Condition()
        self.recvThread=Thread(target=self.recvBytes)
        self.recvThread.start()
        self.consumeDataThread=Thread(target=self.consumeData)
        self.consumeDataThread.start()
        self.updateIFTimer=Thread(target=self.updateIF)
        self.updateIFTimer.start()
        self.updPlotThread=Thread(target=self.updatePlot)
        self.updPlotThread.start()

    def getNext(self, requested):
        while(len(self.buf)<requested):
            self.newBytes.acquire()
            self.newBytes.wait()
            self.newBytes.release()
        self.bufLock.acquire()
        values=self.buf[:requested]
        self.buf=self.buf[requested:]
        self.bufLock.release()
        return values

    def updateIF(self):
        while(1):
            self.dataLock.acquire()
            sw=self.sw
            ou=self.ou
            od=self.od
            co=self.co
            gb=self.gb
            gp=self.gp
            ur=self.ur
            dr=self.dr
            bmax=self.bM
            bmin=self.bm
            l=self.l
            self.dataLock.release()

            if(gb!=self.gainBase):
                self.gainBase=gb
                self.builder.get_object("gainBaseLabel").set_label("Base power: "+str(self.gainBase))
            if(gp!=self.gainPos):
                self.gainPos=gp
                self.builder.get_object("gainPosLabel").set_label("Position gain: "+str(self.gainPos))
            if(ur!=self.upRate):
                self.upRate=ur
                self.builder.get_object("upRateLabel").set_label("Charging step: "+str(self.upRate))
            if(bmax!=self.BMax):
                self.BMax=bmax
                self.builder.get_object("BMaxLabel").set_label("ON field: "+str(self.BMax))
            if(bmin!=self.BMin):
                self.BMin=bmin
                self.builder.get_object("BMinLabel").set_label("Base field: "+str(self.BMin))
            if(dr!=self.downRate):
                self.downRate=dr
                self.builder.get_object("downRateLabel").set_label("Discharging step: "+str(self.downRate))
            if(l!=self.level):
                self.level=l
                self.builder.get_object("levelLabel").set_label("Level: "+str(self.level))
            if(sw!=self.squareWave):
                self.squareWave=sw
                if(sw):
                    self.builder.get_object("sqWaveImage").set_from_stock("gtk-yes", Gtk.IconSize.BUTTON)
                    self.builder.get_object("sqWaveLabel").set_label("SquareWave: ON")
                else:
                    self.builder.get_object("sqWaveImage").set_from_stock("gtk-no", Gtk.IconSize.BUTTON)
                    self.builder.get_object("sqWaveLabel").set_label("SquareWave: OFF")

            if(ou!=self.outputUp):
                self.outputUp=ou
                if(ou):
                    self.builder.get_object("outputUpImage").set_from_stock("gtk-yes", Gtk.IconSize.BUTTON)
                    self.builder.get_object("outputUpLabel").set_label("Output up: ON")
                else:
                    self.builder.get_object("outputUpImage").set_from_stock("gtk-no", Gtk.IconSize.BUTTON)
                    self.builder.get_object("outputUpLabel").set_label("Output up: OFF")

            if(od!=self.outputDown):
                self.outputDown=od
                if(od):
                    self.builder.get_object("outputDownImage").set_from_stock("gtk-yes", Gtk.IconSize.BUTTON)
                    self.builder.get_object("outputDownLabel").set_label("Output down: ON")
                else:
                    self.builder.get_object("outputDownImage").set_from_stock("gtk-no", Gtk.IconSize.BUTTON)
                    self.builder.get_object("outputDownLabel").set_label("Output down: OFF")
            if(od!=self.outputDown):
                self.outputDown=od
                if(od):
                    self.builder.get_object("outputDownImage").set_from_stock("gtk-yes", Gtk.IconSize.BUTTON)
                    self.builder.get_object("outputDownLabel").set_label("Output down: ON")
                else:
                    self.builder.get_object("outputDownImage").set_from_stock("gtk-no", Gtk.IconSize.BUTTON)
                    self.builder.get_object("outputDownLabel").set_label("Output down: OFF")
            sleep(0.1)


    def updatePlot(self):
        while(True):
            self.printDataLock.acquire()
#Note for the reader: 
#While I was writing this code, a thief came into my house and tried stealing my car; I bravely run to him shouting and made him run away. Don't expect this code to be clean anymore.
# (True story!)
            self.axisB.set_xbound(lower=0,upper=len(self.BData))
            self.axisB.set_ybound(lower=min(self.BData), upper=len(self.BData))
            self.plotB.set_xdata(np.arange(len(self.BData)))
            self.plotB.set_ydata(np.array(self.BData))
            #self.plotB.set_xdata(self.XData[:n])
            #self.plotB.set_ydata(self.BData)

            self.axisBC.set_xbound(lower=0,upper=len(self.coilBData))
            self.axisBC.set_ybound(lower=min(self.coilBData), upper=len(self.coilBData))
            self.plotBC.set_xdata(np.arange(len(self.coilBData)))
            self.plotBC.set_ydata(np.array(self.coilBData))

            self.axisFV.set_xbound(lower=0,upper=len(self.filteredValueData))
            self.axisFV.set_ybound(lower=min(self.filteredValueData), upper=len(self.filteredValueData))
            self.plotFV.set_xdata(np.arange(len(self.filteredValueData)))
            self.plotFV.set_ydata(np.array(self.filteredValueData))
            self.plotCanvas.draw()
            self.printDataLock.release()
            sleep(0.2)
    def consumeData(self):
        while(not self.inhibited):
#Waits for sync
            while(True):
                if(self.getNext(1)==[0x4C]):
                    if(self.getNext(1)==[0x56]):
                        if(self.getNext(1)==[0x54]):
                            break
            data=self.getNext(14)
            self.dataLock.acquire()
            self.l=int(data[0])*16;
            self.gp=float(data[2]*256+data[1]+(-65536 if data[2] & 0x80 else 0))/10000.0
            self.gb=float(data[4]*256+data[3]+(-65536 if data[4] & 0x80 else 0))/1000.0+1
            self.ur=float(data[6]*256+data[5]+(-65536 if data[6] & 0x80 else 0))/10000.0
            self.dr=float(data[8]*256+data[7]+(-65536 if data[8] & 0x80 else 0))/10000.0
            self.bM=(data[10]*256+data[9])
            self.bm=(data[12]*256+data[11])
            state=data[13]
            self.sw=state & 0x01
            self.ou=state & 0x02
            self.od=state & 0x04
            self.co=state & 0x08
            self.dataLock.release()
            if((self.ou and self.co) or (self.od and not self.co)):
                values=self.getNext(6)
                filteredValue=float(values[1]*256+data[0]+(-65536 if data[1] & 0x80 else 0))/256.0
                coilB=float(values[3]*256+data[2]+(-65536 if data[1] & 0x80 else 0))/256.0
                B=float(values[5]*256+data[4]+(-65536 if data[1] & 0x80 else 0))/256.0
                #print(str(filteredValue)+" "+str(coilB)+" "+str(B))
                self.printDataLock.acquire()
                self.BData.append(B)
                self.coilBData.append(coilB)
                self.filteredValueData.append(filteredValue)
                self.printDataLock.release()

    def recvBytes(self):
        self.s.flushInput()
        a=[x for x in range(256)]
        while(True):
            n=self.s.readinto(a)
            self.bufLock.acquire()
            self.buf.extend(a[:n])
            if(len(self.buf)>1000):
                self.buf=self.buf[-500:]
            self.bufLock.release()
            self.newBytes.acquire()
            self.newBytes.notify()
            self.newBytes.release()


    def updateNData(self,a):
        value=int(self.builder.get_object("nDataInput").get_text())
        print("New data: " + str(value))
        if(value>65536 or value<0):
            print("Invalid value")
        else:
            if(value!=self.nData):
                self.nData=value
                self.printDataLock.acquire()
                bd=self.BData
                fd=self.filteredValueData
                cbd=self.coilBData
                self.XData=[0.0004*x for x in range(0,value)]
                self.axisB.set_xbound(lower=self.XData[0], upper=self.XData[-1])
                self.BData=deque(maxlen=value)
                self.BData.extend(bd)
                self.filteredValueData=deque(maxlen=value)
                self.filteredValueData.extend(fd)
                self.coilBData=deque(maxlen=value)
                self.coilBData.extend(cbd)
                self.printDataLock.release()

    def changeSquareWave(self,a):
        print("SquareWave change")
        self.serialLock.acquire()
        self.s.write([ord('s') ,0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def changeOutputUp(self,a):
        print("Change output up")
        self.serialLock.acquire()
        self.s.write([ord('O'),0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def changeOutputDown(self,a):
        print("Change output down")
        self.serialLock.acquire()
        self.s.write([ord('o') ,0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def gainBaseUp(self,a):
        print("Gain base up")
        self.serialLock.acquire()
        self.s.write([ord('P') ,0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def gainBaseDown(self,a):
        print("Gain base down")
        self.serialLock.acquire()
        self.s.write([ord('p') ,0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def gainPosUp(self,a):
        print("Gain pos up")
        self.serialLock.acquire()
        self.s.write([ord('G') ,0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def gainPosDown(self,a):
        print("Gain pos down")
        self.serialLock.acquire()
        self.s.write([ord('g') ,0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def upRateUp(self,a):
        print("Up rate up")
        self.serialLock.acquire()
        self.s.write([ord('U') ,0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def upRateDown(self,a):
        print("Up rate down")
        self.serialLock.acquire()
        self.s.write([ord('u') ,0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def downRateUp(self,a):
        print("Down rate up")
        self.serialLock.acquire()
        self.s.write([ord('D') ,0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def downRateDown(self,a):
        print("Down rate down")
        self.serialLock.acquire()
        self.s.write([ord('d') ,0x0A,0x0D])
        self.s.flush()
        self.serialLock.release()

    def saveStuff(self,a):
        print("Saving data to FLASH memory")
        self.inhibited=True
        self.saveStuffLock.acquire()
        self.serialLock.acquire()
        self.s.write([ord('W') ,0x0A,0x0D])
        while(True):
            if(self.getNext(1)==[ord('A')]):
                if(self.getNext(1)==[ord('C')]):
                    if(self.getNext(1)==[ord('K')]):
                        break
        print("Success!")
        self.inhibited=False
        self.serialLock.release()
        self.saveStuffLock.release()

if __name__=="__main__":
    aaa=[]
    if(len(sys.argv)==3):
        aaa=myInterface(sys.argv[1],int(sys.argv[2]))
    else:
        aaa=myInterface("/dev/ttyACM0", 1000000)
    Gtk.main()
