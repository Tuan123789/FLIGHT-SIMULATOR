from re import A
from socket import timeout
from tracemalloc import stop
import serial # import module for serial communcation
import pyautogui # import module for keyboard/mouse control
import io # import module to to use universal newline mode
import time
from numpy import asarray

import mysql.connector

[                            ]
pyautogui.FAILSAFE = False # set failsafe for manual exit
width, height = pyautogui.size() # set size of screen
pyautogui.moveTo(960, 540); # set mouse to middle

landing_gear = 0
flap_up = 0
flap_down = 0
stop = 0
len_gas = 0
xuong_gas = 0
alive = True
dem = 0
count = 0

#time.sleep(0.2)
while (alive): # kill switch not asserted
    connection = mysql.connector.connect(host='localhost',
                                         user='root',
                                         password='',
                                         database='mpu1')

    sql_select_Query = "select * from data_mpu1 "
    cursor = connection.cursor()
    cursor.execute(sql_select_Query)
    # get all records
    data1 = cursor.fetchall()
    data=[10]
    for row in data1:
        if (landing_gear==0):
            a =int(row[1])
        else: a = ~int(row[1])
        if (flap_up==0):
            b =int(row[2])
        else: b = ~int(row[2])
        if (flap_down==0):
            c =int(row[3])
        else: c = ~int(row[3])
        if (stop==0):
            d =int(row[4])
        else: d =~int(row[4])
        if (len_gas==0):
            e =int(row[5])
        else: e = ~int(row[5])
        if (xuong_gas==0):
            f =int(row[6])
        else: f = ~int(row[6])
        g =row[7]
        x_avg =row[8]
        y_avg =row[9]
         #time.sleep(0.05)
    # landing gear
    if a==-1:
        pyautogui.press('g')
        landing_gear = ~landing_gear

    # flap up
    if b==-1:
        pyautogui.press('[')
        flap_up = ~flap_up

    # flaps down\
    if c==-1:
        pyautogui.press(']')
        flap_down = ~flap_down

    # stop
    if d==2:
        pyautogui.press('space')
        stop =~stop 
    # len gas
    if e==-1:
        pyautogui.keyDown('pageup')
        len_gas = ~len_gas
        dem = 1
    else: pyautogui.keyUp('pageup')
 
    # xuong gas
    if f==-1:
        pyautogui.keyDown('pagedown')
        xuong_gas = ~xuong_gas 
    else: pyautogui.keyUp('pagedown')

    # exit
    if int(g)==-1:
        pyautogui.press('esc')
       # mysql_close(conn);
        alive = False

    # roll, pitch
    # che do auto
    if dem>0:
        x = -float(x_avg)
        y = -float(y_avg) 
        if x>90:
            x = 90
        if x<-90:
            x = -90
        if y>90:
            y = 90
        if y<-90:
            y = -90
        xn = y*10+960
        yn = x*6+540
        print (a,b,c,d,e,f,g,x,y)
        pyautogui.moveTo(xn, yn)