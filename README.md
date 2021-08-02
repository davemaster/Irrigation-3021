# Irrigation-3021
https://youtube.com/embed/tNYx-wSfjVc

![imagen](https://user-images.githubusercontent.com/1075807/127100624-5069074b-b088-48a9-8c29-cb0491887942.png)

How Bluetooth Technology can improve the use of water resources worldwide. Programming using [C++ Builder 10.3 Community Edition](https://www.embarcadero.com/es/products/cbuilder/starter) was surprisly very faster than the turtle of Android Studio and its eternal indexing-nightmares; So, its time to move forward using this IDE, in this project, using the multi-device features. 

The goal, more control over the water use in irrigation issues, first in my mom's garden, then in the city parks, then Farmlands, then in any water distribution activity worldwide. Thanks to the [Building Good with C++Builder Contest](https://blogs.embarcadero.com/building-good-with-cbuilder-contest/) for giving this oportunity.

# Project Resume
The system is going to use a bluetooth capable device to control a valve to irrigate a plant. The control-device can be a laptop/pc or a smartphone (Android or iOS). These devices control through bluetooth wireless link an Arduino UNO board connected to a HC-05 bluetooth module; so this last can open/close the solenoid valve by a relay; according the following modes: 1. Based on sensors (humidity, water tank level, and rain intensitivity) the Arduino will decide when to open/close the valve to properly water the plant. The sensors values will be compared with setpoints to make decisions. Mode 2. Test mode, we can open/close valve no matter the sensors values. Mode 3, using irrigation schedules, ie, time defined periods when the valve will be open.

# Needed Parts
1. Mobile Phone: Iphone, Android or laptop/pc Bluetooth capable. C++ Builder 10.3 can program them all with ONE project at the same time; pretty GOOD!!!.

![imagen](https://user-images.githubusercontent.com/1075807/123554941-37e05d00-d748-11eb-9f1d-36520b245556.png)


2. Arduino Board: UNO, NANO, MEGA, etc. https://www.arduino.cc/en/Main/Products

![imagen](https://user-images.githubusercontent.com/1075807/123558850-1ab68900-d75e-11eb-842d-da82d6587bb2.png)

3. HC-05 Bluetooth Module https://www.siarperu.com/store/#view=00011&busq=hc-05

![imagen](https://user-images.githubusercontent.com/1075807/123558943-ceb81400-d75e-11eb-96f8-b45b90d5b03f.png)

4. A 12V solenoid Valve https://www.sparkfun.com/products/10456

![imagen](https://user-images.githubusercontent.com/1075807/123558813-e216af80-d75d-11eb-8267-b26cd7df049b.png)
 
5. Some Protoboards cabling, female-male, male-male or female-female https://naylampmechatronics.com/accesorios-y-prototipado/26-cable-para-protoboard.html

![imagen](https://user-images.githubusercontent.com/1075807/123559024-438b4e00-d75f-11eb-8493-d7573617cdf3.png)

6. Sensors for Humidity moisture, rain intensitivity, and water level:

![imagen](https://user-images.githubusercontent.com/1075807/123560329-265a7d80-d767-11eb-9698-25c5bc9d9758.png)  ![imagen](https://user-images.githubusercontent.com/1075807/123560371-543fc200-d767-11eb-845c-b915b612602a.png) ![imagen](https://user-images.githubusercontent.com/1075807/123560462-e21bad00-d767-11eb-8c31-cb67b74aa5d3.png)

# Needed Software
1. Embarcadero C++ Builder 10.3 Community Edition https://www.embarcadero.com/products/cbuilder/starter or lastest https://embarcadero.com/products/cbuilder

![imagen](https://user-images.githubusercontent.com/1075807/123524911-da3e0900-d692-11eb-8301-566e07b4aca2.png)

2. Arduino IDE 1.8.15 https://www.arduino.cc/en/software

![imagen](https://user-images.githubusercontent.com/1075807/123525458-71f12680-d696-11eb-87fa-3764e6561bc7.png)


3. Any Serial Terminal (for PC Windows or Mac where the C++ Builder is installed)

![imagen](https://user-images.githubusercontent.com/1075807/123525397-f1cac100-d695-11eb-8ec3-e5776882f7fc.png)

4. Proteus 8.6 with Arduino libraries for simulate Arduino valve control and the sensors attached (humidity, water leve, and rain) lectures and response of the valve
   control system (circuit and firmware) to commands from Irrigation 3021 app programmed in C++ Builder. 
   
   ![imagen](https://user-images.githubusercontent.com/1075807/127801912-9e7191ca-199f-403c-8354-2ab2d7e00fdf.png)


   ![imagen](https://user-images.githubusercontent.com/1075807/123555394-1b91ef80-d74b-11eb-83d5-8d26bdd6a079.png)


5. In my particular case, I used the scrcpy for see my project (Irrigation 3021) in its Android versión. https://github.com/Genymobile/scrcpy

![imagen](https://user-images.githubusercontent.com/1075807/123525105-2473ba00-d694-11eb-94cc-f0e34bdb33e6.png)

![imagen](https://user-images.githubusercontent.com/1075807/123525220-aa900080-d694-11eb-8241-3cd5df7e1c26.png)

SOURCE CODE UPDATED

In file BTReadThread.cpp, the following changes has been made:

.................................

  else if(cmd==2)
		{
			dataFromArduino=Socket->ReceiveData();

			if(dataFromArduino.Length>0)
			{
				StrReceived = TEncoding::ASCII->GetString(dataFromArduino);

				/*************************************/
				AllReceivedData+=StrReceived;

				msg=AllReceivedData;
				Synchronize(TabbedForm->WriteMemo);

				if(AllReceivedData.Length()>12)
				{
					sensors=AllReceivedData;
					Synchronize(TabbedForm->UpdateValueSensors);

					AllReceivedData="";
					Sleep(100);
				}

				#ifndef __ANDROID__
				else
					AllReceivedData="";
				#endif

				/*****************************************/

				/*msg=StrReceived;
				Synchronize(TabbedForm->WriteMemo);

				if(msg.Length()>3)
				{

					sensors=StrReceived;
					Synchronize(TabbedForm->UpdateValueSensors);

				} */
			}

			Sleep(100);
		}

..................................

![imagen](https://user-images.githubusercontent.com/1075807/127631900-2e92a8fd-23ad-4516-b5bd-4063e20b5038.png)

