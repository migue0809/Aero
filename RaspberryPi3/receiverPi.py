#!/usr/bin/python
# -*- coding: utf-8 -*-

import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
from lib_nrf24 import NRF24
import time
import spidev
import urllib, urllib2

def toGx(x):
    ixx=round(-1.214/100000000*x**3+1.844/100000*x**2-0.002251*x-2,2)
    return ixx

def toGy(y):
    iyy=round(-1.268/100000000*y**3+1.945/100000*y**2-0.002715*y-2.001,2)
    return iyy

def toGz(z):
    izz=round(-1.306/100000000*z**3+1.99/100000*z**2-0.002778*z-1.999,2)
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
sendBuffer2=[]
sendBuffer3=[]
sendBuffer4=[]

while True:
    while radio.available():
        print("Radio available")
        received = []
        radio.read(received, radio.getDynamicPayloadSize())

        if len(received)>0:
            string=""
            for n in received:
                if (n>=32 and n<=126):
                    string += chr(n)
            print(string)
            if string[0]=='r':
                ind1=string.find('r')
                ind2=string.find('r',ind1+1)

                rpm=string[ind1+1:ind2]
                print("Speed = "+ rpm + " RPM")
                
                try:
                    irpm=int(rpm)
                    
                    if irpm==0:
                        rpm="0.0"
                        
                    try:
                        f = urllib2.urlopen("http://sistelemetria-sistelemetria.rhcloud.com/save_aero.php?type=rpm&rpm="+rpm)
                            
                    except:
                        print("There is no internet connection")
                        sendBuffer3.append(rpm)
                        sendBuffer4=sendBuffer3[:]
                            
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
                
                    sendG= [ toGx(x) for x in ivalues[0:3] ]
                    sendG+=[ toGy(x) for x in ivalues[3:6] ]
                    sendG+=[ toGz(x) for x in ivalues[6:]  ]
                
                    sendIter= [str(x) for x in sendG]
                    print(sendIter)

                    #GET aceleraciones a base de datos
                    try:
                        f=urllib2.urlopen("http://sistelemetria-sistelemetria.rhcloud.com/save_aero.php?type=acc&maxx="+sendIter[0]+"&mx="+sendIter[1]+"&minx="+sendIter[2]+"&maxy="+sendIter[3]+"&my="+sendIter[4]+"&miny="+sendIter[5]+"&maxz="+sendIter[6]+"&mz="+sendIter[7]+"&minz="+sendIter[8])
                        
                    except:
                        print("There is no internet connection")
                        sendBuffer1.append(sendIter)
                        sendBuffer2=sendBuffer1[:]
            
                except:
                    print("There was an error in communication with Accelerometer")
                
    #print("Radio unavailable")
    #time.sleep(0.01)

    while len(sendBuffer4)>0:
        try:
            for sendRPM in sendBuffer3:
                f = urllib2.urlopen("http://sistelemetria-sistelemetria.rhcloud.com/save_aero.php?type=rpm&rpm="+sendRPM)

                if sendRPM in sendBuffer4:
                    sendBuffer4.remove(sendRPM)

            sendBuffer3=sendBuffer4[:]

        except:
            print("There is no internet connection")

    while len(sendBuffer2)>0:
        try:
            for sendAcc in sendBuffer1:
                f=urllib2.urlopen("http://sistelemetria-sistelemetria.rhcloud.com/save_aero.php?type=acc&maxx="+sendAcc[0]+"&mx="+sendAcc[1]+"&minx="+sendAcc[2]+"&maxy="+sendAcc[3]+"&my="+sendAcc[4]+"&miny="+sendAcc[5]+"&maxz="+senAcc[6]+"&mz="+sendAcc[7]+"&minz="+sendAcc[8])

                if sendAcc in sendBuffer2:
                    sendBuffer2.remove(sendAcc)

            sendBuffer1=sendBuffer2[:]

        except:
            print("There is no internet connection")

    time.sleep(0.01)
