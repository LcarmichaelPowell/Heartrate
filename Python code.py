import time
import serial

from Tkinter import *
import threading
import tkMessageBox
import smtplib


globvar = 0
BPM = 0
score = 0
def beenClicked():
    radioValue = CheckBoxVal.get()
    tkMessageBox.showinfo("You picked option", radioValue)
    return

def ResponseY():
    self.app.destroy()
    return
def ResponseN():
    self.app.destroy()
    return 
def See_Status() :
    global score
    score = CheckBoxVal.get()
    adioValue = "Z"

    if   score == 1:
        tkMessageBox.showinfo("Blood Pressure","Safe Blood Pressure is 117/77")
        answer=tkMessageBox.askquestion("Alert","Would you like to recieve a warning email at dangerous blood pressures?")
    elif score == 2:
        tkMessageBox.showinfo("Blood Pressure","Safe Blood Pressure is 121/80")
        answer=tkMessageBox.askquestion("Alert","Would you like to recieve a warning email at dangerous blood pressures?")
    elif score == 3:
        tkMessageBox.showinfo("Blood Pressure","Safe Blood Pressure is 123/82")
        answer=tkMessageBox.askquestion("Alert","Would you like to recieve a warning email at dangerous blood pressures?")
    elif score == 4:
        tkMessageBox.showinfo("Blood Pressure","Safe Blood Pressure is 127/84")
        answer=tkMessageBox.askquestion("Alert","Would you like to recieve a warning email at dangerous blood pressures?")
    elif score == 5:
        tkMessageBox.showinfo("Blood Pressure","Safe Blood Pressure is 131/86")
        answer=tkMessageBox.askquestion("Alert","Would you like to recieve a warning email at dangerous blood pressures?")
    else:
        tkMessageBox.showinfo("Blood Pressure","Safe Blood Pressure is 134/87")
        answer=tkMessageBox.askquestion("Alert","Would you like to recieve a warning email at dangerous blood pressures?")
    if answer == 'yes' :
        print "Yes"
        global globvar    # Needed to modify global copy of globvar
        globvar = 1
    else :
        print "No"
        globvar = 0
        
    return

def Send_Text() :
    server = smtplib.SMTP( "smtp.gmail.com", 587 )
    server.starttls()
    server.login( 'LcarmichaelPowellProject@Gmail.com', '!Freddy1' )
    server.sendmail( 'Leighton', 'LcarmichaelPowell@Gmail.com' , 'High Blood Pressure' )
    return

#Create Window
app = Tk()
#modify root window
app.title("Simple Gui")
app.geometry("1000x600")


#label
labelText=StringVar()
labelText.set("Please select an age range")

label1=Label(app,textvariable=labelText, height=4)
label1.pack()

#Code for Check Boxes
CheckBoxVal = IntVar()
CheckBox1 = Radiobutton(app, variable=CheckBoxVal, text = "14-19",value="1",command= See_Status).pack()
CheckBox2 = Radiobutton(app, variable=CheckBoxVal, text = "20-30",value="2",command= See_Status).pack()
CheckBox3 = Radiobutton(app, variable=CheckBoxVal, text = "30-39",value="3",command= See_Status).pack()
CheckBox4 = Radiobutton(app, variable=CheckBoxVal, text = "40-50",value="4",command= See_Status).pack()
CheckBox5 = Radiobutton(app, variable=CheckBoxVal, text = "51-60",value="5",command= See_Status).pack()
CheckBox6 = Radiobutton(app, variable=CheckBoxVal, text = "60+",value="6",command= See_Status).pack()



#Main loop
app.mainloop()

# configure the serial port
ser = serial.Serial(
 port='COM6',
 baudrate=115200,
 parity=serial.PARITY_NONE,
 stopbits=serial.STOPBITS_TWO,
 bytesize=serial.EIGHTBITS
)
ser.isOpen()
while 1 :
 strin = ser.readline()
 print strin
 print score
 
 if globvar == 1 :
    if score == 1:
     if float(strin) > 90:
         Send_Text()
    if score == 2:
     if float (strin) >= 124:
        Send_Text()
    if score == 3:
     if float (strin) >= 125:
         Send_Text()
    if score == 4:
     if float (strin) >= 130:
         Send_Text()
    if score == 5:
     if float (strin) >= 134:
         Send_Text()
    if score == 6:
     if float (strin) >= 138:
         Send_Text()


    
 
