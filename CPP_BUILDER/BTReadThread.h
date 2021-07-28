//---------------------------------------------------------------------------

#ifndef BTReadThreadH
#define BTReadThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Bluetooth.Components.hpp>
#include <System.Bluetooth.hpp>
#include <System.SysUtils.hpp>
//---------------------------------------------------------------------------
class TBluetoothReadTH : public TThread
{
private:
	TBluetoothSocket* Socket;
	TBytes dataFromArduino; // A dynamic array of bytes
	String StrReceived;


protected:
	void __fastcall Execute();
public:
	Boolean Terminated;
    int cmd;
	__fastcall TBluetoothReadTH(bool CreateSuspended,TBluetoothSocket* BTSocket);
};
//---------------------------------------------------------------------------
#endif
