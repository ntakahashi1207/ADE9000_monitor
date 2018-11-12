# -*- coding: utf-8 -*-
import sys
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QApplication, QWidget, QSizePolicy
from form.ADE9000_form import Ui_Form
import serial

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure


class ADE9000Monitor(QWidget):
    arduinoData: str
    Vrms: float
    Arms: float
    Watt: float
    Freq: float
    VTHD: float
    ITHD: float
    PF: float
    AngVAIA: float
    Temp: float

    Full_scale_Vrms = 566.4
    Full_scale_Vrms_code = 52702092
    Full_scale_Arms = 208.0
    Full_scale_Arms_code = 52702092
    Full_scale_Watt = Full_scale_Vrms * Full_scale_Arms
    Full_scale_Watt_code = 20694066

    def __init__(self, parent=None):

        super(ADE9000Monitor, self).__init__(parent)
        self.ui = Ui_Form()
        self.ui.setupUi(self)
        self.canvas = PlotCanvas(self, width=6.8, height=3.7)
        self.canvas.move(10, 220)
        self.timer = QTimer(self)
        self.timer.start(1000)
        self.timer.timeout.connect(self.get_value)

    def get_value(self):
        ser.write(b'r')
        self.arduinoData = ser.readline().decode('ascii')

        self.arduinoData = self.arduinoData.split()

        self.Vrms = self.arduinoData[0]
        self.Vrms = int(self.Vrms) * self.Full_scale_Vrms / self.Full_scale_Vrms_code
        self.ui.lcd_Voltage.display(self.Vrms)

        self.Arms = self.arduinoData[1]
        self.Arms = int(self.Arms) * self.Full_scale_Arms / self.Full_scale_Arms_code
        self.ui.lcd_Current.display(self.Arms)

        self.Watt = self.arduinoData[2]
        self.Watt = int(self.Watt) * self.Full_scale_Watt / self.Full_scale_Watt_code
        self.Watt = abs(self.Watt)
        self.ui.lcd_Power.display(self.Watt)

        self.Freq = self.arduinoData[3]
        self.ui.lcd_Freq.display(self.Freq)

        self.VTHD = self.arduinoData[4]
        # print(self.VTHD)
        self.VTHD = float(self.VTHD)
        self.ui.lcd_VTHD.display(self.VTHD)

        self.ITHD = self.arduinoData[5]
        # print(self.ITHD)
        self.ITHD = float(self.ITHD)
        self.ui.lcd_ITHD.display(self.ITHD)

        self.PF = self.arduinoData[6]
        self.PF = float(self.PF) * 100
        self.ui.lcd_PF.display(self.PF)

        # self.AngVAIA = self.arduinoData[7]
        # self.AngVAIA = float(self.AngVAIA) * 0.02109375
        # print(self.AngVAIA)
        # self.ui.lcd_AngVAIA.display(self.AngVAIA)

        self.Temp = self.arduinoData[8]
        self.ui.lcd_Temp.display(self.Temp)

        self.canvas.plot(*self.arduinoData)


class PlotCanvas(FigureCanvas):
    arduinoData: str
    Full_scale_Vresampled = 801.0
    Full_scale_Vresampled_code = 18196
    Full_scale_Iresampled = 294.1
    Full_scale_Iresampled_code = 18196

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = self.fig.add_subplot(1, 2, 1)

        FigureCanvas.__init__(self, self.fig)
        self.setParent(parent)

        FigureCanvas.setSizePolicy(self,
               QSizePolicy.Expanding,
               QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)

        self.ax1 = self.figure.add_subplot(1, 2, 1)
        self.ax2 = self.figure.add_subplot(1, 2, 2)

    def plot(self, *args):
        self.V = args[9::2]
        self.V = [int(s) / self.Full_scale_Vresampled_code * self.Full_scale_Vresampled for s in self.V]

        self.I = args[10::2]
        self.I = [int(s) / self.Full_scale_Iresampled_code * self.Full_scale_Iresampled for s in self.I]

        self.ax1.clear()
        self.ax1.set_title('Voltage')
        self.ax1.set_xlabel("Sample")
        self.ax1.set_ylabel("Volt")
        self.ax1.plot(self.V)

        self.ax2.clear()
        self.ax2.set_title('Current')
        self.ax2.set_xlabel("Sample")
        self.ax2.set_ylabel("Ampere")
        self.ax2.plot(self.I)

        self.fig.subplots_adjust(top=0.9, bottom=0.2, wspace=0.5)
        self.draw()


if __name__ == '__main__':
    ser = serial.Serial('COM9', baudrate=115200, timeout=1)
    app = QApplication(sys.argv)
    window = ADE9000Monitor()
    window.show()
    sys.exit(app.exec_())
