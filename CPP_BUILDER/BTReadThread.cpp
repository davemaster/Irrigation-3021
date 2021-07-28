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

		//		dataFromArduino=Socket->ReadData(15000);             //15000
		dataFromArduino=Socket->ReceiveData(15000);             //15000

		if(dataFromArduino.Length>0)
		{
			StrReceived = TEncoding::UTF8->GetString(dataFromArduino);

		msg=StrReceived;

			Synchronize(TabbedForm->WriteMemo);

			if(msg.Length()>3)
			{

				sensors=StrReceived;
				Synchronize(TabbedForm->UpdateValueSensors);

			}
		}

		Sleep(100);
		}
	}


}
//---------------------------------------------------------------------------
