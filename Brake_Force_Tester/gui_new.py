#
# Use Canvas to create a basic gauge
#
from tkinter import *
import random
import time
from PIL import Image, ImageDraw
from tkinter.ttk import Progressbar
import serial
from datetime import datetime
buffer_string = ''
ser = serial.Serial('/dev/ttyACM0',9600,timeout=0.3)
ser1 = serial.Serial('/dev/ttyUSB0',9600,timeout=0.3)
ser.flush()
ser1.flush()
data2 = ser1.readline()
inByte2 = data2.decode('utf-8')
str1=inByte2[:8]
inByte3=(float(str1))
Tempdisp=inByte3
#Tempdisp=100
#print(Tempdisp)
#ser.flush()
data2 = ser.read(ser.inWaiting())
#print(data2)
inByte2 = data2.decode('utf-8')
lines1=inByte2.split('\n')
#print(lines1)
inByte6=lines1[-2]
inByte=inByte6
#inByte="60+80+5"
#print(inByte)
parse=inByte.split("+")
pressure_value=parse[0];
pressure1_value=parse[1];
displacement_value=parse[2];
displacement_value=(int(displacement_value))
temp_disp=displacement_value;
pressure_value=0;
pressure1_value=0;
#displacement_value=0;
load_value=0;
variable=0;
fpressure_value=0
fpressure_value=0
fdisplacement_value=0
file = open("2log.csv","a")
file.write("Pressure1" + "," + "Pressure2" + "," +  "Displacement" + "," +  "Load"+ "\n")
file.close()
    
def update_gauge():
    global variable;
    ser.flush()
    
    data = ser1.readline()
    inByte3 = data.decode('utf-8')
    lines=inByte3.split('\n')
    #print(lines)
    inByte5=lines[-2]
    #inByte5 = 5000
    #print(inByte5)
    ser.flush()
    data2 = ser.read(ser.inWaiting())
    inByte2 = data2.decode('utf-8')
    lines1=inByte2.split('\n')
    inByte6=lines1[-2]
    #inByte="60+80+5"
    #print(inByte)
    parse=inByte.split("+")
    pressure_value=parse[0];
    pressure1_value=parse[1];
    displacement_value=parse[2];

    #print(displacement_value)
    str2=inByte5[:8]
    inByte4=(float(str2))
    Tempdisp1=inByte4
    Load = Tempdisp-Tempdisp1
    Load = round(Load, 2)
    if(Load<=0):
     Load=0.00;
    #print(Load)
    pressure_value=(int(pressure_value))
 
    pressure1_value=(int(pressure1_value))
    displacement_value=(int(displacement_value))
    #displacement_value=temp_disp-displacement_value
    file = open('%Y-%m-%d' + "_" '%H:%M:%S',"a")
    file.write("{0:0.1f}".format(pressure_value) + ",")
    file.write("{0:0.1f}".format(pressure1_value) + ",")
    file.write("{0:0.1f}".format(displacement_value) + ",")
    file.write("{0:0.1f}".format(Load) + ",")
    file.write(datetime.today().strftime('%Y-%m-%d' + "," '%H:%M:%S')+"\n")
    file.close()
    #print(pressure_value)
   # data1 = ser1.readline()
    #inByte1 = data1.decode('utf-8')
    #inByte1 = inByte1*(-1)
   # inByte2 = inByte1
    #load_value=inByte1
    
    #newvalue = random.randint(low_r,hi_r)
    
    # Rescale value to angle range (0%=120deg, 100%=30 deg)
    fpressure_value = pressure_value-variable
    fpressure_value1 = pressure1_value
    fdisplacement_value = Load
    angle = 270 * (hi_r - fpressure_value)/(hi_r - low_r) + -45
    angle1 = 270 * (hi_r - fpressure_value1)/(hi_r - low_r) + -45
    angle2 = 270 * (hi_r1 - fdisplacement_value)/(hi_r1 - low_r1) + -45
    cnvs.itemconfig(id_needle,start = angle)
    cnvs.itemconfig(id_needle1,start = angle1)
    cnvs.itemconfig(id_needle2,start = angle2)
    #print(newvalue)
    bar['value'] = displacement_value
    value=displacement_value;
    #print(displacement_value)
    Pressure = Button(root ,text=fpressure_value, width=8,
             height=1 , bd='2', bg="grey")
    Pressure.place(x=95, y=251)
    Pressure1 = Button(root ,text=fpressure_value1, width=8,
             height=1 , bd='2', bg="grey")
    Pressure1.place(x=385, y=251)
    Load = Button(root ,text=fdisplacement_value, width=8,
             height=1 , bd='2', bg="grey")
    Load.place(x=665, y=251)
    
    displacement = Button(root , font="Times 10  bold", text=displacement_value, width=10,
             height=2 , bd='2', bg="grey")
    displacement.place(x=681, y=415)
    root.after(50, update_gauge)
    


    
def getinput():  
    x1 = '0'
    print("0")
    data3= x1.encode('utf-8')
    ser.write(data3)
    #print(data3)
    
def getinput0():  
    x1 = '1'
    print("x1=0")
    root.after(100, update_gauge)
    
    
def getinput1():  
    x1 = '1'
    print("x1=2")
    xx1 = x1.encode('utf-8')
    ser.write(xx1)
    root.mainloop()
   
    
def getinput2():  
    x1 = '2'
    print("x1=3")
    xx1 = x1.encode('utf-8')
    ser.write(xx1)

def plotgraph():
    x1 = '2'
    print("x1=3")
    

def tare1():
    global variable
    variable = pressure_value;
    pressure_value1 = pressure_value-variable
 
# Create Canvas objects    
 
canvas_width = 848
canvas_height = 480

 
root = Tk()
root.title("                                                                                                         Brake Chamber GUI")
 
cnvs = Canvas(root, width=canvas_width, height=canvas_height, bd=2, bg="black", highlightcolor="green" )
cnvs.grid(row=2, column=1)

bar = Progressbar(root, length=200)
bar.place(x=640, y=390) 

coord1 = 4.5, 35, 290.5, 317 #define the size of the gauge
coord2 = 287, 35, 573, 317 
coord3 = 568.75, 35, 854.75, 317
low_r = 0 # chart low range
hi_r = 100 # chart hi range
sub =  200


low_l = 0 # chart low range
hi_l = 100 
low_r1 = 0 # chart low range
hi_r1 = 4000 
#newvalue = random.randint(low_r,hi_r)

# Create a background arc and a pointer (very narrow arc)

 
# add hi/low bands
#cnvs.create_arc(coord, start=30, extent=120, outline="green", style= "arc", width=40)
#cnvs.create_arc(coord, start=30, extent=20, outline="red", style= "arc", width=40)
#cnvs.create_arc(coord, start=50, extent=20, outline="yellow", style= "arc", width=40)
# add needle/value pointer
    
 
# add hi/low bands
#cnvs.create_arc(coord1, start=30, extent=120, outline="green", style= "arc", width=40)
#cnvs.create_arc(coord1, start=30, extent=20, outline="red", style= "arc", width=40)
#cnvs.create_arc(coord1, start=50, extent=20, outline="yellow", style= "arc", width=40)


#id_needle = cnvs.create_arc(coord1, start= 119, extent=1, width=3)

photo = PhotoImage(master=root, file = "/home/pi/Desktop/Brake_Force_Tester/images/button_left.png")
photo1 = PhotoImage(master=root, file = "/home/pi/Desktop/Brake_Force_Tester/images/button_right.png")
cnvs.create_image(375, 355, anchor=NE, image=photo)
cnvs.create_image(645, 355, anchor=NE, image=photo1)
btn = Button(root, text='MOVE LEFT', width=9,
             height=2 , bd='2', command=getinput1, bg="yellow")
btn1 = Button(root, text='STOP', width=9, justify = LEFT,
             height=2 , bd='2', command=getinput, bg="red")
btn2 = Button(root, text='MOVE RIGHT', width=9,
             height=2 , bd='2', command=getinput2, bg="yellow")
btn3 = Button(root, text='START', width=10,
             height=2 , bd='2', command=getinput0, bg="green")
btn4 = Button(root, text=fpressure_value, width=8,
             height=1 , bd='2', command=getinput2, bg="white")
btn5 = Button(root, text=fpressure_value, width=8,
             height=1 , bd='2', command=tare1, bg="white")
btn6 = Button(root, text=fpressure_value, width=8,
             height=1 , bd='2', command=getinput2, bg="white")
btn7 = Button(root, text=fpressure_value, width=8,
             height=1 , bd='2', command=getinput2, bg="white")

btn8 = Button(root, text='GRAPH', width=10,
             height=2 , bd='2', command=plotgraph, bg="maroon")

var = StringVar()

#displacement = Button(root , font="Times 10  bold", text=value, width=10,
           #  height=2 , bd='2', bg="grey")
Pressure = Button(root ,text=fpressure_value, width=8,
             height=1 , bd='2', bg="grey")
Pressure1 = Button(root ,text=fpressure_value, width=8,
             height=1 , bd='2', bg="grey")
Load = Button(root ,text=fdisplacement_value, width=8,
             height=1 , bd='2', bg="grey")

label = Label( root, textvariable=var, relief=RAISED )

  
btn.place(x=310, y=375)
btn1.place(x=405, y=375)
btn2.place(x=485, y=375)
btn3.place(x=25, y=375)

btn4.place(x=681, y=415)
btn5.place(x=95, y=325)
btn6.place(x=385, y=325)
btn7.place(x=665, y=325)

btn8.place(x=145, y =375)
#displacement.place(x=791, y=426)
Pressure.place(x=95, y=251)
Pressure1.place(x=385, y=251)
Load.place(x=665, y=251)
filename = PhotoImage(master=root, file = "/home/pi/Desktop/Brake_Force_Tester/images/new_gauge.png")
cnvs.create_image(286.75, 37, anchor=NE, image=filename)
cnvs.create_image(568.5, 37, anchor=NE, image=filename)
cnvs.create_image(850.25, 37, anchor=NE, image=filename)
id_needle = cnvs.create_arc(coord1, start= 225, extent=0, width=4)
id_needle1 = cnvs.create_arc(coord2, start= 225, extent=0, width=4)
id_needle2 = cnvs.create_arc(coord3, start= 225, extent=0, width=4)
#cnvs.create_arc(coord, start=-90, extent=358, fill="white",  width=2) 
#id_needle = cnvs.create_arc(coord, start= 119, extent=1, width=7)
 

# Add some labels
cnvs.create_text(150,15,font="Times 20  bold", text="Pressure1", fill="white")
cnvs.create_text(435,15,font="Times 20  bold", text="Pressure2", fill="white")
cnvs.create_text(714,15,font="Times 20  bold", text="Load", fill="white")
cnvs.create_text(735,375,font="Times 20  bold", text="Displacement", fill="white")
cnvs.create_text(645,430,font="Times 15  bold", text="L1", fill="white")
cnvs.create_text(815,430,font="Times 15  bold", text="L2", fill="white")
cnvs.create_text(95,231,font="Times 12 bold", text=low_r)
cnvs.create_text(205,231,font="Times 12 bold", text=hi_r)
cnvs.create_text(377,231,font="Times 12 bold", text=low_r)
cnvs.create_text(487,231,font="Times 12 bold", text=hi_r)
cnvs.create_text(658,231,font="Times 12 bold", text=low_r)
cnvs.create_text(768,231,font="Times 12 bold", text=hi_r)
cnvs.create_text(145,315,font="Times 12 bold", text="PSI", fill="white")
cnvs.create_text(435,315,font="Times 12 bold", text="PSI", fill="white")
cnvs.create_text(715,315,font="Times 12 bold", text="LB", fill="white")
newvalue=cnvs.create_text(50,35,font="Times 15 bold", fill="white")
id_text=cnvs.create_text(170,265,font="Times 15 bold",fill="white")

 
root.mainloop()
