//---------------------------------------------------------------------------

#include <fmx.h>
#include <System.UITypes.hpp>
#include <System.SysUtils.hpp>

#pragma hdrstop

#include "TabbedTemplate.h"
#include "BTReadThread.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
#pragma resource ("*.NmXhdpiPh.fmx", _PLAT_ANDROID)
#pragma resource ("*.LgXhdpiPh.fmx", _PLAT_ANDROID)
#pragma resource ("*.SmXhdpiPh.fmx", _PLAT_ANDROID)
#pragma resource ("*.iPhone47in.fmx", _PLAT_IOS)
#pragma resource ("*.iPhone55in.fmx", _PLAT_IOS)
#pragma resource ("*.Windows.fmx", _PLAT_MSWINDOWS)
#pragma resource ("*.Moto360.fmx", _PLAT_ANDROID)

TTabbedForm *TabbedForm;
TBluetoothReadTH *BlueReadThread;
TBluetoothDevice * LDevice;
TBluetoothServiceList *LServices;

AnsiString msg;
AnsiString sensors;

//---------------------------------------------------------------------------
__fastcall TTabbedForm::TTabbedForm(TComponent* Owner)
	: TForm(Owner)
{
}


//--------------------------------------------------------------------------
void __fastcall TTabbedForm::FormCreate(TObject *Sender)
{
	// This defines the default active tab at runtime
	TabControlMain->ActiveTab = TabItemSetup;
	BluetoothLink->SocketTimeout=10000;



}
//---------------------------------------------------------------------------


void __fastcall TTabbedForm::Button1Click(TObject *Sender)
{
	//Form1->Show();
}
//---------------------------------------------------------------------------


void __fastcall TTabbedForm::TimerClockTimer(TObject *Sender)
{
	TimeEditValve->Time=System::Sysutils::Now();
}
//---------------------------------------------------------------------------


void __fastcall TTabbedForm::FormShow(TObject *Sender)
{
	TabbedForm->Caption="DaveWare® Design";

	LoadIniValues();

	//GetPairedDevices(Sender);


}
//---------------------------------------------------------------------------

void __fastcall TTabbedForm::BluetoothLinkDiscoverableEnd(TObject * const Sender)

{
		//BlueManager=new TBluetoothManager;
	try
	{
		//BlueManager=BluetoothLink->CurrentManager;
		//BlueManager->StartDiscovery(10000);
		//BlueDevList=BluetoothLink->PairedDevices();// ->GetPairedDevices(BluetoothLink->CurrentAdapter);
	}

	catch(Exception *ex)
	{
		ShowMessage("DaveWare says: "+ex->Message);
	}

}


//---------------------------------------------------------------------------
void __fastcall TTabbedForm::GetPairedDevices(TObject *Sender)
{
    try
        {
				BlueManager = TBluetoothManager::Current; // Obtén el administrador de Bluetooth nativo
				BlueAdapter = BlueManager->CurrentAdapter; //Obtén el dispositivo Bluetooth nativo

				if(BlueManager->ConnectionState == TBluetoothConnectionState::Connected) //Si lo hay y el dispositivo Bluetooth está encendido
				{
						Memo1->Lines->Add(L"Obtén el dispositivo Bluetooth nativo: '" + BlueManager->CurrentAdapter->AdapterName + L"'");

						ComboBoxDevices->Clear();
						FPairedDevices = BlueManager->GetPairedDevices(); //Obtenga una lista de dispositivos emparejados
						if(FPairedDevices->Count > 0)
                        {
								for(int i = 0; i < FPairedDevices->Count; i++)
								{
										ComboBoxDevices->Items->Add(FPairedDevices->Items[i]->DeviceName);
								}
								if(ComboBoxDevices->Count > 0)
										ComboBoxDevices->ItemIndex = 0;

						}
						else
						{
								Memo1->Lines->Add(L"Ningún dispositivo emparejado");


						}
				}
				else //No hay dispositivo Bluetooth o no está encendido
				{
						Memo1->Lines->Add(L"La máquina no tiene o no está enciendido el dispositivo Bluetooth");
				}

		}
		catch (Exception &ex)
        {
                ShowMessage(ex.Message);
        }
}

//---------------------------------------------------------------------------


void __fastcall TTabbedForm::ButtonConnectClick(TObject *Sender)
{
	UnicodeString strServiceGUID = ComboBoxService->Items->Strings[ComboBoxService->ItemIndex]; //GUID de servicio

	/*TBluetoothDevice */ LDevice = FPairedDevices->Items[ComboBoxService->ItemIndex];
	Memo1->Lines->Add(L"get ready to connect to service: " + strServiceGUID);
	Memo1->GoToTextEnd();

	BlueSocket = LDevice->CreateClientSocket(StringToGUID(strServiceGUID), false); //Establecer comunicación de socket con el dispositivo Bluetooth


	if(BlueSocket != NULL)
	{
		try
		{

			BlueSocket->Connect();  //conexión
			//Sleep(500);

			BlueReadThread = new TBluetoothReadTH(false,BlueSocket); //Crear un hilo de datos de lectura

			//msg="Conectado con éxito a Socket！";
			//Memo1->Lines->Add(msg);
			//Memo1->GoToTextEnd();
			//Synchronize(WriteMemo);
			GroupBoxValve->Enabled=true;
			GroupBoxTestValve->Enabled=true;
			ButtonConnect->Enabled=false;
			ButtonGetPaired->Enabled=false;
			ButtonGetServices->Enabled=false;
			BlueReadThread->cmd=2;
		}
		catch(...)
		{
			delete BlueSocket;
			BlueSocket = NULL;
			GroupBoxValve->Enabled=false;
			GroupBoxTestValve->Enabled=false;
			ButtonConnect->Enabled=true;
			ButtonGetPaired->Enabled=true;
			ButtonGetServices->Enabled=true;

			//msg="Falló la conexión de socket！";
			//Memo1->Lines->Add(msg);
			//Memo1->GoToTextEnd();
			//Synchronize(WriteMemo);

		}

	}
	else
	{
		ShowMessage(L"Tiempo de espera de envío (15 segundos)");
	}
}
//---------------------------------------------------------------------------

void __fastcall TTabbedForm::ComboBoxDevicesChange(TObject *Sender)
{
	ComboBoxService->Clear();
	if(ComboBoxDevices->ItemIndex > -1)
	{
		/*TBluetoothDevice */ LDevice = FPairedDevices->Items[ComboBoxDevices->ItemIndex];
		/*TBluetoothServiceList */ LServices = LDevice->GetServices(); //Obtenga los servicios proporcionados por el dispositivo emparejado
		for(int i=0; i<LServices->Count; i++)
		{
			ComboBoxService->Items->Add(GUIDToString(LServices->Items[i].UUID));
		}
		if(ComboBoxService->Count > 0)
			ComboBoxService->ItemIndex = 0;
	}
	else
	{
		ShowMessage(L"Seleccione un dispositivo en la lista emparejada");
	}
}
//---------------------------------------------------------------------------

void __fastcall TTabbedForm::ButtonValveSyncClick(TObject *Sender)
{
	TDateTime dt=Now();


	// Enviar contenido impreso   en formato YYMMDDwHHMMSS, with an 'x' at the end.
	String cmd="E$";
	TBytes ToSend = TEncoding::ANSI->GetBytes(cmd);
	BlueSocket->SendData(ToSend);

	// Enviar contenido impreso
	String Msg=dt.FormatString("yymmddhhnnss");

	ToSend = TEncoding::ANSI->GetBytes(Msg);
	BlueSocket->SendData(ToSend);

	// Enviar fin de Trama
	String endCmd="#";
	ToSend = TEncoding::ANSI->GetBytes(endCmd);
	BlueSocket->SendData(ToSend);

	Memo1->Lines->Add(L"data sent:" +cmd+ Msg+endCmd);
	Memo1->GoToTextEnd();

}
//---------------------------------------------------------------------------

//------------------------MY FUNCTIONS -------------------------
void __fastcall TTabbedForm::sendMessage(String MessageToSend)
{
	// Enviar contenido impreso
	String cmd="|";
	TBytes ToSend = TEncoding::ANSI->GetBytes(cmd);
	BlueSocket->SendData(ToSend);

	// MessageToSend
	ToSend = TEncoding::ANSI->GetBytes(MessageToSend);
	BlueSocket->SendData(ToSend);

	String endCmd="#";
	ToSend = TEncoding::ANSI->GetBytes(endCmd);
	BlueSocket->SendData(ToSend);

	Memo1->Lines->Add(L"ddata sent:" +cmd+ MessageToSend + endCmd);
	Memo1->GoToTextEnd();

}
//---------------------------------------------------------------------------
void __fastcall TTabbedForm::ButtonValveSetHumidityClick(TObject *Sender)
{

	AnsiString Data=FormatFloat("0000.####",NumberBoxSetHumidity->Value);
	AnsiString cmd="H$";
	sendMessage(cmd+Data);
}
//---------------------------------------------------------------------------

void __fastcall TTabbedForm::ButtonValveSetLevelClick(TObject *Sender)
{
	AnsiString Data=FormatFloat("0000.####",NumberBoxSetLevel->Value);
	AnsiString cmd="L$";
	sendMessage(cmd+Data);

}
//---------------------------------------------------------------------------

void __fastcall TTabbedForm::ButtonValveSetRainClick(TObject *Sender)
{
    AnsiString Data=FormatFloat("0000.####",NumberBoxSetRain->Value);
	AnsiString cmd="R$";
	sendMessage(cmd+Data);
}
//---------------------------------------------------------------------------

void __fastcall TTabbedForm::SwitchTestValveSwitch(TObject *Sender)
{
	if(SwitchTestValve->IsChecked)
	{
		if(CheckBoxTestValveTimed->IsChecked)
		{
			//Open Valve with a time of operation
			AnsiString Data=FormatFloat("0000.####",NumberBoxValveTime->Value);
			sendMessage("V$"+Data);
		}
		else
			sendMessage("O$");      //just open Valve
		NumberBoxValveTime->Enabled=false;
		CheckBoxTestValveTimed->Enabled=false;
	}
	else
	{
	   sendMessage("W$");           //Close Valve unconditionally
	   NumberBoxValveTime->Enabled=true;
	   CheckBoxTestValveTimed->Enabled=true;
	}

}
//---------------------------------------------------------------------------


void __fastcall TTabbedForm::SaveIniValues()
{
	TIniFile *ini;

	try
	{

		ini = new TIniFile(/*this->Caption*/+"Config.INI" );
		ini->WriteInteger( "SetPoints", "Humidity", setPointHumidity );
		ini->WriteInteger( "SetPoints", "Level", setPointLevel );
		ini->WriteString ( "SetPoints", "Rain", setPointRain );
		//ini->WriteBool   ( "Form", "InitMax", WindowState == wsMaximized );



	}
	catch(Exception *ex)
	{
		ShowMessage(ex->Message);
	}

    delete ini;

}

//---------------------------------------------------------------------------

void __fastcall TTabbedForm::LoadIniValues()
{
	TIniFile *ini;
	ini = new TIniFile (/*this->Caption+*/"Config.INI" );

	try
	{

		setPointHumidity	=  ini->ReadInteger( "SetPoints", "Humidity", 400 );
		setPointLevel   	=  ini->ReadInteger( "SetPoints", "Level", 600 );
		setPointRain   		=  ini->ReadInteger( "SetPoints", "Rain", 400 );

		/*ini->ReadBool( "Form", "initMax", false ) ?
			  WindowState = wsMaximized :
			  WindowState = wsNormal;*/

		delete ini;

		NumberBoxSetHumidity->Value=setPointHumidity;
		NumberBoxSetLevel->Value=setPointLevel;
		NumberBoxSetRain->Value=setPointRain;

	}
	catch(Exception *ex)
	{
        ShowMessage(ex->Message);
    }

}

//---------------------------------------------------------------------------

void __fastcall TTabbedForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	if(BlueSocket != NULL)
	{
		try
		{
            BlueReadThread->Terminated=true;
			BlueReadThread->cmd=3;

			BlueSocket->Close();
			BlueSocket->Free();
		}
		catch(...)
		{

		}

	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TTabbedForm::ComboBoxServiceChange(TObject *Sender)
{
	ButtonConnect->Enabled=true;
}

//---------------------------------------------------------------------------

void __fastcall TTabbedForm::ButtonGetPairedClick(TObject *Sender)
{
	GetPairedDevices(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TTabbedForm::ButtonGetServicesClick(TObject *Sender)
{
	ComboBoxService->Clear();
	if(ComboBoxDevices->ItemIndex > -1)
	{
		/*TBluetoothDevice */LDevice = FPairedDevices->Items[ComboBoxDevices->ItemIndex];
		/*TBluetoothServiceList */LServices = LDevice->GetServices(); //Obtenga los servicios proporcionados por el dispositivo emparejado
		for(int i=0; i<LServices->Count; i++)
		{
			ComboBoxService->Items->Add(GUIDToString(LServices->Items[i].UUID));
		}
		if(ComboBoxService->Count > 0)
			ComboBoxService->ItemIndex = 0;
	}
	else
	{
		ShowMessage(L"Select a device from paired list");
	}
}
//---------------------------------------------------------------------------
void __fastcall TTabbedForm::WriteMemo()
{
	Memo1->Lines->Add(msg);
    Memo1->GoToTextEnd();
}

void __fastcall TTabbedForm::UpdateValueSensors()
{
	String temp=sensors.SubString(2,4);
	NumberBoxSetupHumidityData->Text=temp;
	//Memo1->Lines->Add(sensors.SubString(2,4));
	temp=sensors.SubString(6,4);
	NumberBoxSetupLevelData->Text=temp;
	//Memo1->Lines->Add(sensors.SubString(6,4));
	temp=sensors.SubString(10,4);
	NumberBoxSetupRainData->Text=temp;
	//Memo1->Lines->Add(sensors.SubString(10,4));
	//Memo1->GoToTextEnd();
}

