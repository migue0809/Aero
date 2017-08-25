#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Example program to receive packets from the radio link
#


import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
from lib_nrf24 import NRF24
import time
import spidev
import urllib, urllib2

def toGx(x):
    ixx=round(-1.214/100000000*ix**3+1.844/100000*ix**2-0.002251*ix-2,2)
    return ixx

def toGy(y):
    iyy=round(-1.268/100000000*ix**3+1.945/100000*ix**2-0.002715*ix-2.001,2)
    return iyy

def toGz(z):
    izz=round(-1.306/100000000*ix**3+1.99/100000*ix**2-0.002778*ix-1.999,2)
    return izz


GPIO.setwarnings(False)

#Configure Radio
pipes = [[0xe7, 0xd3, 0xf0, 0x35, 0xff], [0xe7, 0xd3, 0xf0, 0x35, 0xc2]]

radio = NRF24(GPIO, spidev.SpiDev())
radio.begin(0, 17)

radio.setRetries(15,15)
radio.setPALevel(NRF24.PA_MAX)
radio.setAutoAck(True)
radio.enableDynamicPayloads()
radio.setCRCLength(16)
radio.enableAckPayload()
radio.openReadingPipe(1, pipes[0])
radio.openReadingPipe(2, pipes[1])

radio.startListening()

#Open sending buffers
send=[]
sendBuffer1=[]
sendBUffer2=[]
sendBuffer3=[]
sendBuffer4=[]

while True:
    while radio.available():
        received = []
        radio.read(received, radio.getDynamicPayloadSize())

        if len(received)>0:
            string=""
            for n in received:
                if (n>=32 and n<=126):
                    string += chr(n)
            
            if string[0]=='r':
                ind1=string.find('r')
                ind2=string.find('r',ind1+1)

                rpm=string[ind1+1:ind2]
                print("Speed = "+ rpm + " RPM")
                
                try:
                    irpm=int(rpm)
                    
                    if irpm==0:
                        rpm="0.0"
                        sendBuffer3.append(rpm)
                        sendBuffer4=sendBuffer3[:]
                        
                        try:
                            for sendRPM in sendBuffer3:
                                f = urllib2.urlopen("http://sistelemetria-sistelemetria.rhcloud.com/save_aero.php?type=rpm&rpm="+sendRPM)
                                
                                if sendElem in sendBuffer4:
                                    sendBuffer4.remove(sendRPM)
                                    sendBuffer3=sendBuffer4[:]
                        except:
                            print("There is no internet connection")
                            
                except:
                    print("There was an error in communication with the RPM sensor.")
            
            if string[0]=='X'and len(string)>27:
                indX=string.find('X')
                indY=string.find('Y')
                indZ=string.find('Z')
                indF=string.find('F')
                
                mx=string[indX+1:indX+4]
                maxx=string[indX+4:indX+7]
                minx=string[indX+7:indX+10]
                
                my=string[indY+1:indY+4]
                maxy=string[indY+4:indY+7]
                miny=string[indY+7:indY+10]
                
                mz=string[indZ+1:indZ+4]
                maxz=string[indZ+4:indZ+7]
                minz=string[indZ+7:indZ+10]
                
                values=[maxx,mx,minx,maxy,my,miny,maxz,mz,minz]

            try:
                ivalues= [int(x) for x in values]
                
                sendG= [toGx(x) for x in ivalues[0:3] ]
                sendG.append([toGy(x) for x in ivalues[3:6] ])
                sendG.append([toGz(x) for x in ivalues[6:] ])
                
                send= [str(x) for x in sendG]
                sendBuffer1.append(send)
                sendBuffer2=sendBuffer1[:]
                
                #GET aceleraciones a base de datos
                try:
                    for sendIter in sendBuffer1:
                        f=urllib2.urlopen("http://sistelemetria-sistelemetria.rhcloud.com/save_aero.php?type=acc&maxx="+sendIter[0]+"&mx="+sendIter[1]+"&minx="+sendIter[2]+"&maxy="+sendIter[3]+"&my="+sendIter[4]+"&miny="+sendIter[5]+"&maxz="+sendIter[6]+"&mz="+sendIter[7]+"&minz="+sendIter[8])
                        
                        if sendIter in sendBuffer2:
                            sendBuffer2.remove(sendIter)
                            sendBuffer1=sendBuffer2[:]
                except:
                    print("There is no internet connection")
            
            except:
                print("There was an error in communication with Accelerometer")
                
    time.sleep(0.01)
