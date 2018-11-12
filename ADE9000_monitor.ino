#include <SPI.h>
#include  <ADE9000RegMap.h>
#include <ADE9000API.h>

/*Basic initializations*/
ADE9000Class ade9000;
#define SPI_SPEED 5000000     //SPI Speed
#define CS_PIN 16 //8-->Arduino Zero. 16-->ESP8266 
#define ADE9000_RESET_PIN 5 //Reset Pin on HW
#define PM_1 15              //PM1 Pin: 4-->Arduino Zero. 15-->ESP8266 

/*Structure decleration */
struct CurrentRMSRegs curntRMSRegs;
struct VoltageRMSRegs vltgRMSRegs;
struct ActivePowerRegs powerRegs;
struct PeriodRegs periodRegs; //Period, Frequency
struct VoltageTHDRegs vltgTHDRegs;
struct CurrentTHDRegs curntTHDRegs;
struct PowerFactorRegs powerFactorRegs;
struct AngleRegs angleRegs;
struct TemperatureRegnValue tempRegs;
struct ResampledWfbData resampledData; // Resampled Data


/*Function Decleration*/
void readRegisterData(void);
void readResampledData(void);
void resetADE9000(void);
char userInput;

void setup() 
{
  Serial.begin(115200);
  pinMode(PM_1, OUTPUT);    //Set PM1 pin as output 
  digitalWrite(PM_1, LOW);   //Set PM1 select pin low for PSM0 mode
  pinMode(ADE9000_RESET_PIN, OUTPUT);
  digitalWrite(ADE9000_RESET_PIN, HIGH); 
  void resetADE9000(); 
  delay(1000);
  ade9000.SPI_Init(SPI_SPEED,CS_PIN); //Initialize SPI
  ade9000.SetupADE9000();             //Initialize ADE9000 registers according to values in ADE9000API.h
  //ade9000.SPI_Write_16(ADDR_RUN,0x1); //Set RUN=1 to turn on DSP. Uncomment if SetupADE9000 function is not used
  Serial.print("RUN Register: ");
  Serial.println(ade9000.SPI_Read_16(ADDR_RUN),HEX);
}

void loop() {
if(Serial.available()>0){  // Serial入力があった場合のみ実行
  userInput=Serial.read();
  if(userInput == 'r'){
    readRegisterData();
    }
  }
}

void readRegisterData()
{
  /*Read and Print RMS Register using ADE9000 Read Library*/
  /* arduinoData[0] */
  ade9000.ReadVoltageRMSRegs(&vltgRMSRegs);    //Template to read Power registers from ADE9000 and store data in Arduino MCU
  Serial.print(vltgRMSRegs.VoltageRMSReg_A);  //Print AVRMS register
  Serial.print(" ");

  /* arduinoData[1] */
  ade9000.ReadCurrentRMSRegs(&curntRMSRegs);
  Serial.print(curntRMSRegs.CurrentRMSReg_A);
  Serial.print(" ");

  /* arduinoData[2] */
  ade9000.ReadActivePowerRegs(&powerRegs);
  Serial.print(powerRegs.ActivePowerReg_A);
  Serial.print(" ");

  /* arduinoData[3] */
  ade9000.ReadPeriodRegsnValues(&periodRegs);
  Serial.print(periodRegs.FrequencyValue_A); 
  Serial.print(" ");

  /* arduinoData[4] */
  ade9000.ReadVoltageTHDRegsnValues(&vltgTHDRegs);
  Serial.print(vltgTHDRegs.VoltageTHDValue_A);
  Serial.print(" ");

  /* arduinoData[5] */
  ade9000.ReadCurrentTHDRegsnValues(&curntTHDRegs);
  Serial.print(curntTHDRegs.CurrentTHDValue_A);
  Serial.print(" ");

  /* arduinoData[6] */
  ade9000.ReadPowerFactorRegsnValues(&powerFactorRegs);
  Serial.print(powerFactorRegs.PowerFactorValue_A);
  Serial.print(" ");

  /* arduinoData[7] */
  ade9000.ReadAngleRegsnValues(&angleRegs);
  Serial.print(angleRegs.AngleValue_VA_IA);
  Serial.print(" ");

  /* arduinoData[8] */
  ade9000.ReadTempRegnValue(&tempRegs);
  Serial.print(tempRegs.Temperature);
  Serial.print(" ");

  readResampledData();
}


void readResampledData()
{
  uint32_t temp;
  /*Read and Print Resampled data*/
  /*Start the Resampling engine to acquire 4 cycles of resampled data*/
  ade9000.SPI_Write_16(ADDR_WFB_CFG,0x1000);
  ade9000.SPI_Write_16(ADDR_WFB_CFG,0x1010);
  delay(100); //approximate time to fill the waveform buffer with 4 line cycles  
  /*Read Resampled data into Arduino Memory*/
  ade9000.SPI_Burst_Read_Resampled_Wfb(0x800,WFB_ELEMENT_ARRAY_SIZE,&resampledData);   // Burst read function

  /* arduinoData[9::2] for Voltage */
  /* arduinoData[10::2] for Current */

 for(temp=0;temp<WFB_ELEMENT_ARRAY_SIZE;temp++)
    {
      Serial.print(resampledData.VA_Resampled[temp]);
      Serial.print(' ');
      Serial.print(resampledData.IA_Resampled[temp]);
      Serial.print(' ');
   } 
}


void resetADE9000(void)
{
 digitalWrite(ADE9000_RESET_PIN, LOW);
 delay(50);
 digitalWrite(ADE9000_RESET_PIN, HIGH);
 delay(1000);
 Serial.println("Reset Done");
}




