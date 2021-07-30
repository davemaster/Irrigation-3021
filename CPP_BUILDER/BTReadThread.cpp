//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "BTReadThread.h"
#include "TabbedTemplate.h"
#pragma package(smart_init)

extern AnsiString msg;
extern AnsiString sensors;
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TBluetoothReadTH::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TBluetoothReadTH::TBluetoothReadTH(bool CreateSuspended,TBluetoothSocket* BTSocket)
	: TThread(CreateSuspended)
{
	Socket=BTSocket;
	dataFromArduino.Length = 64;
	Terminated=false;
	cmd=3;
}
//---------------------------------------------------------------------------
void __fastcall TBluetoothReadTH::Execute()
{
	//---- Place thread code here ----

	AnsiString AllReceivedData;

	char *LastChar;

	while (!Terminated)
	{
		if(cmd==1)
		{
			try
			{
				Socket->Connect();
			}
			catch(...)
			{

            }

		}

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
	}


}
//---------------------------------------------------------------------------
