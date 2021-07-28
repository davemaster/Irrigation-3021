//---------------------------------------------------------------------------

#ifndef TabbedTemplateH
#define TabbedTemplateH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Gestures.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.TabControl.hpp>
#include <FMX.Types.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.ActnList.hpp>
#include <System.Actions.hpp>
#include <FMX.ListBox.hpp>
#include <FMX.DateTimeCtrls.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Edit.hpp>
#include <FMX.EditBox.hpp>
#include <FMX.NumberBox.hpp>
#include <System.Bluetooth.Components.hpp>
#include <System.Bluetooth.hpp>
#include <FMX.Memo.hpp>
#include <FMX.ScrollBox.hpp>
#include <FMX.Media.hpp>
#include <System.iniFiles.hpp>
//---------------------------------------------------------------------------
class TTabbedForm : public TForm
{
__published:	// IDE-managed Components
	TToolBar *HeaderToolBar;
	TLabel *ToolBarLabel;
	TTimer *TimerClock;
	TBluetooth *BluetoothLink;
	TLayout *LayoutMain;
	TTabControl *TabControlMain;
	TTabItem *TabItemSetup;
	TGroupBox *GroupBoxConexion;
	TButton *ButtonConnect;
	TComboBox *ComboBoxDevices;
	TComboBox *ComboBoxService;
	TGroupBox *GroupBoxValve;
	TLayout *LayoutValveTime;
	TLabel *LabelValveTime;
	TTimeEdit *TimeEditValve;
	TButton *ButtonValveSync;
	TLayout *LayoutValveHumidity;
	TLabel *LabelValveHumidity;
	TButton *ButtonValveSetHumidity;
	TNumberBox *NumberBoxSetHumidity;
	TLayout *LayoutSetupLevel;
	TLabel *LabelValveLevel;
	TButton *ButtonValveSetLevel;
	TNumberBox *NumberBoxSetLevel;
	TLayout *LayoutSetupRain;
	TLabel *LabelValveRain;
	TButton *ButtonValveSetRain;
	TNumberBox *NumberBoxSetRain;
	TGroupBox *GroupBoxSetupRain;
	TNumberBox *NumberBoxSetupRainData;
	TGroupBox *GroupBoxSetupLevel;
	TNumberBox *NumberBoxSetupLevelData;
	TGroupBox *GroupBoxSetupHumidity;
	TNumberBox *NumberBoxSetupHumidityData;
	TTabItem *TabItemTest;
	TMemo *Memo1;
	TTabItem *TabItemTareas;
	TTabItem *TabItemAbout;
	TLayout *LayoutTestValve;
	TGroupBox *GroupBoxTestValve;
	TLabel *LabelValveOpen;
	TSwitch *SwitchTestValve;
	TScaledLayout *ScaledLayoutAbout;
	TImageControl *ImageControlDaveWare;
	TCheckBox *CheckBoxTestValveTimed;
	TLabel *LabelValveMinutes;
	TNumberBox *NumberBoxValveTime;
	TImageControl *ImageControlPoweredCppBuilder;
	TButton *ButtonGetPaired;
	TButton *ButtonGetServices;
	TLabel *Label1;
	TLabel *LabelDebug;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall GetPairedDevices(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall TimerClockTimer(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall BluetoothLinkDiscoverableEnd(TObject * const Sender);
	void __fastcall ButtonConnectClick(TObject *Sender);
	void __fastcall ComboBoxDevicesChange(TObject *Sender);
	void __fastcall ButtonValveSyncClick(TObject *Sender);
	void __fastcall ButtonValveSetHumidityClick(TObject *Sender);
	void __fastcall ButtonValveSetLevelClick(TObject *Sender);
	void __fastcall ButtonValveSetRainClick(TObject *Sender);
	void __fastcall SwitchTestValveSwitch(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ComboBoxServiceChange(TObject *Sender);
	void __fastcall ButtonGetPairedClick(TObject *Sender);
	void __fastcall ButtonGetServicesClick(TObject *Sender);


private:	// User declarations
	//TStringList *FPairedDevices;       // keep the list of pairing devices
	TBluetoothManager *BlueManager;
	TBluetoothDeviceList *FPairedDevices;
	TBluetoothAdapter *BlueAdapter;
	TBluetoothSocket *BlueSocket;
	//TBluetoothReadTH *BlueReadThread;

	int setPointHumidity;
	int setPointLevel;
	int setPointRain;

public:		// User declarations
	__fastcall TTabbedForm(TComponent* Owner);
	void __fastcall sendMessage(String MessageToSend);
	void __fastcall LoadIniValues();
	void __fastcall SaveIniValues();
	void __fastcall WriteMemo();
    void __fastcall UpdateValueSensors();
};
//---------------------------------------------------------------------------
extern PACKAGE TTabbedForm *TabbedForm;

//extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
