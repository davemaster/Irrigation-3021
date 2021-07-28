
/****************************************************************************************************************************
 * Project     :    Irrigation 3021
 * Programmer  :    David Elías Flores Escalante
 * github      :
 * Description :    Arduino sketch for Irrigation 3021 system for save water in irrigation applications in home, agriculture, 
 *                  etc. This for Contest C++ Builder Code for Good 2021
 **************************************************************************************************************************** 
 */


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>


//********************************* Define an LCD object using I2C interface  ***********************************************

//LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x20,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//**********************************************Variables RTC DS3231 ********************************************************
DS3231 clock;

byte Year;
byte Month;
byte Date;
//byte DoW;
byte Hour;
byte Minute;
byte Second;
int lastSecond = 20; //Initial absurd second value

bool h12Flag;
bool pmFlag;

//***************************************************************************************************************************

//******************************************Variables RTC DS3231 ************************************************************
int sensorHumedad = A0;                 //   Humidity Sensor
int sensorNivelAgua = A1;               //   Water Level Sensor
int sensorLluvia = A2;                  //   Rain Intensitivity Sensor
int valvePin = 7;                       //   Valve for water
int valveTanquePin = 8;                 //   valve of tank of water from rain
//int ledPin = 13;

// countdown timer vars
int openMinutes = 5;                    //   time in minutes during valve will be open
int openSeconds = 0;
RTClib myRTC;
DateTime dTime;
long startTime, currentTime;
//long endTime;



// setpoints
const int defaultHumedad = 400;         //   default humidity sensor setPoint
const int defaultNivel   = 500;         //   default water level sensor setPoint
const int defaultLluvia  = 500;         //   default rain intenwitivity sensor setPoint

int setPointHumedad      = 400;         //   current humidity sensor setPoint
int setPointNivelAgua    = 500;         //   current water level sensor setPoint
int setPointLluvia       = 500;         //   current rain intenwitivity sensor setPoint

const int BTN_OPEN       = 3;           //   button arduino pin for open valve
const int BTN_CLOSE      = 2;           //   button arduino pin for close valve

int buttonState          = 0;           //   variable for reading the pushbutton status
int valveStatus          = 0;           //   variable valve status 1: open     0: closed
int valveTanqueStatus    = 0;           //   variable valve status 1: open     0: closed

long sensorValueHumedad;                //   value of Analog port for humidity
long sensorValueNivelAgua;              //   value of Analog Port for Water Level
long sensorValueLluvia;                 //   value of Analog port for Rain Intensitivity

//int limit = 300;

#define SAMPLES 10                      //   number of samples for Analog ports lectures

/***************************************
  SERIAL BLUETOOTH LINK
**************************************/

//int cmd = 0;
int Modo = 3;
byte prevVal1, prevVal2, val, dataBuffer[256];
boolean dataIn = false;
int dataPos = 0;

/*******************************************************************************************

   Function setup, executed ONCE for set Arduino features

********************************************************************************************/
void setup()
{
  /***********************************************************************
     buttons open & close valve into interrupted fired ports (2 & 3 for Arduino UNO)
  ***********************************************************************/
  // digital pin 2 - open valve button
  attachInterrupt(digitalPinToInterrupt(BTN_OPEN), btnValveOpen, CHANGE);

  // digital pin 3 - close valve button
  attachInterrupt(digitalPinToInterrupt(BTN_CLOSE), btnValveClose, CHANGE);

  loadConfiguration();
}

/*******************************************************************************************

   Function for Arduino settings

********************************************************************************************/
void loadConfiguration()
{
  //pinMode(ledPin, OUTPUT);
  pinMode(valvePin, OUTPUT);              // set valvePin to OUTPUT
  pinMode(valveTanquePin, OUTPUT);        // set valveTanquePin to OUTPUT

  lcd.init();                             // initialize the LCD-I2C

  lcd.backlight();                        // turn LCD-I2C backlight on
  lcd.setCursor(0, 0);
  lcd.print("DaveWare Design.");
  lcd.setCursor(0, 1);
  lcd.print("Irrigation 3021 ");
  delay(3000);
  lcd.clear();

  //lcd.noBacklight();                    // turn LCD-I2C backlight off

  Serial.begin(9600);                     // set Serial port (Bluetooth attached here) to 9600 bps spped

  // Finally the hour, minute, and second
  Serial.println(" ");
  Serial.print(clock.getHour(h12Flag, pmFlag), DEC);
  Serial.print(" ");
  Serial.print(clock.getMinute(), DEC);
  Serial.print(" ");
  Serial.print(clock.getSecond(), DEC);
  
}

/*******************************************************************************************

   Function that read dataBuffer and get humidity setpoint sent by pc/android application

********************************************************************************************/
void getHumidityFromBT(int& humditySP)
{
  byte Temp1, Temp2;                      // temp variables for convert bytes with time values
  byte Temp3, Temp4;                      // temp variables for convert bytes with time values

  // Reading from Most to less significant digits
  Temp1 = dataBuffer[0] - 48;
  Temp2 = dataBuffer[1] - 48;
  Temp3 = dataBuffer[2] - 48;
  Temp4 = dataBuffer[3] - 48;
  humditySP = Temp1 * 1000 + Temp2 * 100 + Temp3 * 10 + Temp4;

}

/*******************************************************************************************

   Function that read dataBuffer and get Water Level setpoint sent by pc/android application

********************************************************************************************/
void getLevelFromBT(int& LevelSP)
{
  byte Temp1, Temp2;                      // temp variables for convert bytes with time values
  byte Temp3, Temp4;                      // temp variables for convert bytes with time values

  // Reading from Most to less significant digits
  Temp1 = dataBuffer[0] - 48;
  Temp2 = dataBuffer[1] - 48;
  Temp3 = dataBuffer[2] - 48;
  Temp4 = dataBuffer[3] - 48;
  LevelSP = Temp1 * 1000 + Temp2 * 100 + Temp3 * 10 + Temp4;
}


/*******************************************************************************************

   Function that read dataBuffer and get Rain Intenisivity setpoint sent by pc/android application

********************************************************************************************/
void getRainFromBT(int& RainSP)
{
  byte Temp1, Temp2;                      // temp variables for convert bytes with time values
  byte Temp3, Temp4;                      // temp variables for convert bytes with time values

  // Reading from Most to less significant digits
  Temp1 = dataBuffer[0] - 48;
  Temp2 = dataBuffer[1] - 48;
  Temp3 = dataBuffer[2] - 48;
  Temp4 = dataBuffer[3] - 48;
  RainSP = Temp1 * 1000 + Temp2 * 100 + Temp3 * 10 + Temp4;
}

/***********************************************************************************************

   Function that read dataBuffer and get Time of Valve Operation sent by pc/android application

***********************************************************************************************/
void getMinutesFromBT(int& vMinutes)
{
  byte Temp1, Temp2;                      // temp variables for convert bytes with time values
  byte Temp3, Temp4;                      // temp variables for convert bytes with time values

  // Reading from Most to less significant digits
  Temp1 = dataBuffer[0] - 48;
  Temp2 = dataBuffer[1] - 48;
  Temp3 = dataBuffer[2] - 48;
  Temp4 = dataBuffer[3] - 48;
  vMinutes = Temp1 * 1000 + Temp2 * 100 + Temp3 * 10 + Temp4;
}

/*******************************************************************************************

   Function that read dataBuffer when a "Sincronize time" is sent by pc/android application

********************************************************************************************/
void getDateFromBT(byte& Year, byte& Month, byte& Day,
                   byte& Hour, byte& Minute, byte& Second)
{
  // Call this if you notice something coming in on
  // the serial port. The stuff coming in should be in
  // the order YYMMDDHHMMSS, with an 'x' at the end.

  byte Temp1, Temp2;                      // temp variables for convert bytes with time values

  // Read Year first
  Temp1 = dataBuffer[0] - 48;
  Temp2 = dataBuffer[1] - 48;
  Year = Temp1 * 10 + Temp2;
  // now month
  Temp1 = dataBuffer[2] - 48;
  Temp2 = dataBuffer[3] - 48;
  Month = Temp1 * 10 + Temp2;
  // now date
  Temp1 = dataBuffer[4] - 48;
  Temp2 = dataBuffer[5] - 48;
  Day = Temp1 * 10 + Temp2;
  // now Day of Week
  //DoW = (byte)dataBuffer[6] - 48;
  // now Hour
  Temp1 = dataBuffer[6] - 48;
  Temp2 = dataBuffer[7] - 48;
  Hour = Temp1 * 10 + Temp2;
  // now Minute
  Temp1 = dataBuffer[8] - 48;
  Temp2 = dataBuffer[9] - 48;
  Minute = Temp1 * 10 + Temp2;
  // now Second
  Temp1 = dataBuffer[10] - 48;
  Temp2 = dataBuffer[11] - 48;
  Second = Temp1 * 10 + Temp2;
}

/*******************************************************************************************

   Function loop, this is executed continously

********************************************************************************************/
void loop()
{
  /***************************************************
     Humidity Sensor Readings
  ***************************************************/
  //set value of humidity sensor to zero
  sensorValueHumedad = 0;
  //reading samples to average measurements
  for (int ind = 0; ind < SAMPLES; ind++)
  {
    int value = analogRead(sensorHumedad);
    sensorValueHumedad = sensorValueHumedad + value;
    delay(10);                                   // wait 10 milliseconds
  }

  //humidity sensor average value
  sensorValueHumedad = sensorValueHumedad / SAMPLES;

  /***************************************************
     Water Level Sensor Readings
  ***************************************************/
  //set value of Water Level sensor to zero
  sensorValueNivelAgua = 0;

  //reading samples to average measurements
  for (int ind = 0; ind < SAMPLES; ind++)
  {
    int value = analogRead(sensorNivelAgua);     // Read the analog value form sensor
    sensorValueNivelAgua = sensorValueNivelAgua + value;
    delay(10);                                   // wait 10 milliseconds
  }
  //Water Level sensor average value
  sensorValueNivelAgua /= SAMPLES;

  /***************************************************
     Rain Intensitivity Sensor Readings
  ***************************************************/
  //set value of Rain Intensitivity sensor to zero
  sensorValueLluvia = 0;

  //reading samples to average measurements
  for (int ind = 0; ind < SAMPLES; ind++)
  {
    int value = analogRead(sensorLluvia);        // Read the analog value form sensor
    sensorValueLluvia = sensorValueLluvia + value;
    delay(10);                                   // wait 10 milliseconds
  }
  //Rain Intensitivity sensor average value
  sensorValueLluvia /= SAMPLES;

  if (Modo == 3) //Normal Mode / Modo Normal
  {
    char buffer[20];
    int curSecond = clock.getSecond();

    if (lastSecond != curSecond)
    {
      sprintf(buffer, "Hora--> %02d:%02d:%02d", clock.getHour(h12Flag, pmFlag)
              , clock.getMinute()
              , lastSecond); //,clock.getSecond());
      lastSecond = curSecond;

      //Serial.println(buffer);
      lcd.setCursor(0, 1);
      lcd.print(buffer);
    }

    if (sensorValueLluvia > setPointLluvia)
    {
      if (sensorValueNivelAgua < setPointNivelAgua - 100)
      {
        digitalWrite(valveTanquePin, LOW);
        valveTanqueStatus = 0;
        if (sensorValueHumedad < setPointHumedad - 100)
        {
          digitalWrite(valvePin, LOW);
          valveStatus = 0;
          lcd.setCursor(0, 0);
          lcd.print("Valvula   ");
          lcd.setCursor(8, 0);
          lcd.print("Cerrada   ");
        }
        else if (sensorValueHumedad > setPointHumedad)
        {
          digitalWrite(valvePin, HIGH);
          valveStatus = 1;
          lcd.setCursor(0, 0);
          lcd.print("Valvula   ");
          lcd.setCursor(8, 0);
          lcd.print("Abierta   ");
        }
      }
      else if (sensorValueNivelAgua > setPointNivelAgua)
      {
        digitalWrite(valvePin, LOW);
        valveStatus = 0;

        if (sensorValueHumedad < setPointHumedad - 100)
        {
          digitalWrite(valveTanquePin, LOW);
          valveTanqueStatus = 0;
          lcd.setCursor(0, 0);
          lcd.print("Tanque   ");
          lcd.setCursor(8, 0);
          lcd.print("Cerrado   ");
        }
        else if (sensorValueHumedad > setPointHumedad)
        {
          digitalWrite(valveTanquePin, HIGH);
          valveTanqueStatus = 1;
          lcd.setCursor(0, 0);
          lcd.print("Tanque   ");
          lcd.setCursor(8, 0);
          lcd.print("Abierto   ");
        }
      }
    }
    else //if(sensorValueLluvia<setPointLluvia)
    {
      lcd.setCursor(8, 0);
      lcd.print("Lluvia  ");

      digitalWrite(valvePin, LOW);
      valveStatus = 0;
      digitalWrite(valveTanquePin, LOW);
      valveTanqueStatus = 0;
    }
  }

  else if (Modo == 2) //modo test
  {
    char buffer[20];

    Serial.print("humedad Value : ");
    Serial.println(sensorValueHumedad);
    Serial.print("Nivel Value : ");
    Serial.println(sensorValueNivelAgua);
    Serial.print("Lluvia Value : ");
    Serial.println(sensorValueLluvia);

    int valor = sensorValueHumedad;
    sprintf(buffer, "H: %4u", valor);
    lcd.setCursor(0, 0);
    lcd.print(buffer);

    lcd.setCursor(0, 1);
    valor = sensorValueNivelAgua;
    sprintf(buffer, "N: %4u", valor);
    lcd.print(buffer);

    lcd.setCursor(8, 1);
    valor = sensorValueLluvia;
    sprintf(buffer, "LL: %4u", valor);
    lcd.print(buffer);

    lcd.setCursor(8, 0);
    lcd.print("Prueba    ");

  }

  else if (Modo == 1) //modo setup: cambia setpoints, humedad,nivel, lluvia
  {
    Serial.print("humedad Value : ");
    Serial.println(sensorValueHumedad);
    Serial.print("Nivel Value : ");
    Serial.println(sensorValueNivelAgua);
    Serial.print("Lluvia Value : ");
    Serial.println(sensorValueLluvia);

    // lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("H: ");   lcd.print(sensorValueHumedad);
    lcd.setCursor(8, 0);
    lcd.print("Setup    ");
    lcd.setCursor(0, 1);
    lcd.print("N: "); lcd.print(sensorValueNivelAgua);
    lcd.setCursor(8, 1);
    lcd.print("LL: "); lcd.print(sensorValueLluvia);
    //delay(3000);
    // lcd.noBacklight();
  }

  else if (Modo == 5) //Sincronize time in Ds3231 from Phone/PC by Bluetooth
  {
    char buffer[20];

    getDateFromBT(Year, Month, Date,  Hour, Minute, Second);

    clock.setClockMode(false);    // set to 24h
    //setClockMode(true);         // set to 12h

    clock.setYear(Year);
    clock.setMonth(Month);
    clock.setDate(Date);
    //clock.setDoW(DoW);
    clock.setHour(Hour);
    clock.setMinute(Minute);
    clock.setSecond(Second);

    sprintf(buffer, "Valve Sincronized");

    lcd.setCursor(0, 0);
    lcd.print(buffer);

    if (Serial)
      Serial.println("Date & Time sinchronized");

    delay(3000);
    Modo = 3;                     // return to NORMAL mode
  }

  else if (Modo == 6) //change Humidity SetPoint
  {
    char buffer[20];

    getHumidityFromBT(setPointHumedad);

    sprintf(buffer, "Humidity ->  %d", setPointHumedad);

    //Serial.println(buffer);
    lcd.setCursor(0, 0);
    lcd.print(buffer);

    delay(3000);
    Modo = 3;

  }

  else if (Modo == 7) //change Water Level SetPoint
  {
    char buffer[20];

    getLevelFromBT(setPointNivelAgua);

    sprintf(buffer, "Water SP ->  %d", setPointNivelAgua);

    //Serial.println(buffer);
    lcd.setCursor(0, 0);
    lcd.print(buffer);

    delay(3000);
    Modo = 3;
  }

  else if (Modo == 8) //change Rain Intenistivity SetPoint
  {
    char buffer[20];

    getRainFromBT(setPointLluvia);

    sprintf(buffer, "Rain  SP ->  %d", setPointLluvia);

    lcd.setCursor(0, 0);
    lcd.print(buffer);

    delay(3000);
    Modo = 3;
  }

  else if (Modo == 9) //open valve during a time set by app/pc
  {
    //RTClib myRTC;
    char buffer[20];
    /*DateTime*/ dTime = myRTC.now();

    getMinutesFromBT(openMinutes);

    openSeconds = openMinutes * 60;

    startTime = dTime.unixtime();

    digitalWrite(valvePin, HIGH);                //   Open valve of water

    sprintf(buffer, "Valve Open %d Min", openMinutes);

    lcd.setCursor(0, 0);
    lcd.print(buffer);

    delay(3000);
    Modo = 99;
  }

  else if (Modo == 10) //Valve Open
  {
    char buffer[20];

    digitalWrite(valvePin, HIGH);                //   Open valve of water

    sprintf(buffer, "Test - Valve Open");

    lcd.setCursor(0, 0);
    lcd.print(buffer);

    delay(3000);
    //Modo=3;
  }

  else if (Modo == 11) //Close Valve
  {
    char buffer[20];

    digitalWrite(valvePin, LOW);                //   Open valve of water

    sprintf(buffer, "Test - Close Valve");

    lcd.setCursor(0, 0);
    lcd.print(buffer);

    delay(3000);
    Modo = 3;
  }

  else if (Modo == 99) //check new second value from DS3231
  {
    //RTClib myRTC;
    char buffer[20];
    /*DateTime*/ dTime = myRTC.now();

    currentTime = dTime.unixtime();

    //long elapsedTime = currentTime - startTime;

    if(currentTime>startTime)
	{
          openSeconds--;
		  startTime=currentTime;
	}

    if (openSeconds <=0)
    {
      digitalWrite(valvePin, LOW);                //   Close water valve after time is elapsed

      sprintf(buffer,"Test-Close Valve");

      currentTime=0;

      lcd.setCursor(0, 0);
      lcd.print(buffer);
	   

      delay(3000);
      Modo = 3;
    }
    else
	{
		sprintf(buffer,"Valve Open- %u",openSeconds);		

		lcd.setCursor(0, 0);
		lcd.print(buffer);
		
		sprintf(buffer, "Hora--> %02d:%02d:%02d", clock.getHour(h12Flag, pmFlag)
												, clock.getMinute()
												, lastSecond); //,clock.getSecond());		

		//Serial.println(buffer);
		lcd.setCursor(0, 1);
		lcd.print(buffer);
	
      Modo = 99;
	}
  }
}

/*******************************************************************************************

   Function btnValveOpen, executed with BUTON_OPEN interrupt in port 2

********************************************************************************************/
void btnValveOpen()
{
  if (Modo == 3) //en modo Normal, no opera
    return;
  buttonState = digitalRead(BTN_OPEN);
  if (buttonState == LOW)
  {
    buttonState = digitalRead(BTN_OPEN);
    if (buttonState == LOW)
    {
      digitalWrite(valvePin, HIGH);
      valveStatus = 1;
      delay(25);
    }
  }
}

/*******************************************************************************************

   Function btnValveOpen, executed with BUTON_CLOSE interrupt in port 3

********************************************************************************************/
void btnValveClose()
{
  if (Modo == 3) //en modo Normal, no opera
    return;
  buttonState = digitalRead(BTN_CLOSE);
  if (buttonState == LOW)
  {
    buttonState = digitalRead(BTN_CLOSE);
    if (buttonState == LOW)
    {
      digitalWrite(valvePin, LOW);
      valveStatus = 0;
      delay(25);

    }
  }
}

/*******************************************************************************************

   Function that manages serial port interrupt (received data) from Bluetooth HC-05 module

********************************************************************************************/
void serialEvent()
{
  while (Serial.available() > 0)
  {
    prevVal1 = prevVal2;
    prevVal2 = val;
    val = Serial.read();
    char caracter = val;
    Serial.print(caracter);

    /*********************************************
       Modo 1: SETUP / CONFIGURACION
    **********************************************/
    if (prevVal1 == 'A' && prevVal2 == '$')
    {
      //dataPos=0;
      dataIn = false;
      Modo = 1;

      if (Serial)
        Serial.println("MODO SETUP");
    }

    /*********************************************
       Modo 2: TEST / PRUEBA
    **********************************************/
    else if (prevVal1 == 'B' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = true;
      Modo = 2;

      if (Serial)
        Serial.println("MODO TEST");
    }

    /*********************************************
       Modo 3: Normal Mode / Modo Normal
    **********************************************/
    else if (prevVal1 == 'C' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = true;
      Modo = 3;

      if (Serial)
        Serial.println("Modo Normal");
    }

    /*************************************************************************
      //  Modo 4: Bloqueo
    **************************************************************************/
    else if (prevVal1 == 'D' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = true;
      Modo = 4;
#ifdef DEBUG
      if (Serial)
        Serial.println("Bloqueado");
#endif DEBUG
    }

    /*************************************************************************
      //  Modo 44: DesBloqueo
    **************************************************************************/
    else if (prevVal1 == 'Z' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = true;
      Modo = 4;
#ifdef DEBUG
      if (Serial)
        Serial.println("Desbloqueado");
#endif DEBUG
    }

    /*************************************************************************
      //  Sincronize Time
    **************************************************************************/
    else if (prevVal1 == 'E' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = true;
      Modo = 5;

#ifdef DEBUG
      if (Serial)
        Serial.println("Sincronizando");
#endif
    }

    /*********************************************
      //  Set Humidity setPoint
    **********************************************/
    else if (prevVal1 == 'H' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = true;
      Modo = 6;

#ifdef DEBUG
      if (Serial)
        Serial.println("Humidity setPoint");
#endif
    }

    /*********************************************
      //  Set Water Level setPoint
    **********************************************/
    else if (prevVal1 == 'L' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = true;
      Modo = 7;

#ifdef DEBUG
      if (Serial)
        Serial.println("Water Level setPoint");
#endif
    }

    /*********************************************
      //   Set Rain intensity setPoint
    **********************************************/
    else if (prevVal1 == 'R' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = true;
      Modo = 8;
#ifdef DEBUG
      if (Serial)
        Serial.println("Rain intensity setPoint");
#endif
    }

    /*********************************************
      //   Open valve in Test Mode during time in minutes
    **********************************************/
    else if (prevVal1 == 'V' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = true;
      Modo = 9;
#ifdef DEBUG
      if (Serial)
        Serial.println("Open valve in Test Mode with time");
#endif
    }

    /*********************************************
      //   Open valve in Test Mode
    **********************************************/
    else if (prevVal1 == 'O' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = false;
      Modo = 10;
#ifdef DEBUG
      if (Serial)
        Serial.println("Open valve in Test Mode");
#endif
    }

    /*********************************************
      //   Close valve in Test Mode
    **********************************************/
    else if (prevVal1 == 'W' && prevVal2 == '$')
    {
      dataPos = 0;
      dataIn = false;
      Modo = 11;
#ifdef DEBUG
      if (Serial)
        Serial.println("Close valve in Test Mode");
#endif
    }

    /*********************************************
      //  END of Data Transmission '#' character
    **********************************************/
    else if (val == '#' )
    {
      if (dataIn)
      {
        if (Modo == 1)
        {

        }

        else if (Modo == 2)
        {

        }

        else if (Modo == 3)
        {

        }

        else if (Modo == 4)
        {

        }

        else if (Modo == 5)
        {

        }
        else if (Modo == 6)
        {

        }
        else if (Modo == 7)
        {

        }
        else if (Modo == 8)
        {

        }
        else if (Modo == 9)
        {

        }
        else if (Modo == 10)
        {

        }
        else if (Modo == 44)
        {

        }
        dataIn = false;
        val = ' ';
        //delay(1000);
      }
    }

    if (dataIn)
    {
      dataBuffer[dataPos] = val;
      if (++dataPos > 255)
        dataPos = 0;

#ifdef DEBUG
      if (Serial)
        Serial.print(val);
#endif DEBUG
    }
  }
}