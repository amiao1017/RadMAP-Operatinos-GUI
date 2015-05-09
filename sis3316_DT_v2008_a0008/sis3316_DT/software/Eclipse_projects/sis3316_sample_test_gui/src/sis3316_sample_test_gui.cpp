/***************************************************************************/
/*                                                                         */
/*  Filename: sis3316_sample_test_gui.cpp                                  */
/*                                                                         */
/*  Funktion:                                                              */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 26.07.2012                                       */
/*  last modification:    27.03.2015                                       */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/*  SIS  Struck Innovative Systeme GmbH                                    */
/*                                                                         */
/*  Harksheider Str. 102A                                                  */
/*  22399 Hamburg                                                          */
/*                                                                         */
/*  Tel. +49 (0)40 60 87 305 0                                             */
/*  Fax  +49 (0)40 60 87 305 20                                            */
/*                                                                         */
/*  http://www.struck.de                                                   */
/*                                                                         */
/*  © 2015                                                                 */
/*                                                                         */
/***************************************************************************/

#include "project_system_define.h"		//define LINUX or WINDOWS
#include "project_interface_define.h"   //define Interface (sis1100/sis310x, sis3150usb or Ethnernet UDP)

#define FIRST_MODULE_BASE_ADDR					0x30000000  

#define MAIN_WINDOW_WIDTH						410
#define MAIN_WINDOW_HIGH						1050
#define MAIN_WINDOW_POSTION_X					0
#define MAIN_WINDOW_POSTION_Y					10

#define SIS3316_TEST_WINDOW_WIDTH_RUN			400
#define SIS3316_TEST_WINDOW_WIDTH				500
#define SIS3316_TEST_WINDOW_HIGH				1120
#define SIS3316_TEST_WINDOW_POSTION_X			30
#define SIS3316_TEST_WINDOW_POSTION_Y			10

#define SIS3316_RAW_DATA_WINDOW_WIDTH			800
#define SIS3316_RAW_DATA_WINDOW_HIGH			490
#define SIS3316_RAW_DATA_WINDOW_POSTION_X		395
#define SIS3316_RAW_DATA_WINDOW_POSTION_Y		50


#define SIS3316_HISTOGRAM_WINDOW_WIDTH			700
#define SIS3316_HISTOGRAM_WINDOW_HIGH			1035
#define SIS3316_HISTOGRAM_WINDOW_POSTION_X		1170
//#define SIS3316_HISTOGRAM_WINDOW_POSTION_X		1200
#define SIS3316_HISTOGRAM_WINDOW_POSTION_Y		50

#define SIS3316_MAW_WINDOW_WIDTH				800
#define SIS3316_MAW_DATA_WINDOW_HIGH			490
#define SIS3316_MAW_DATA_WINDOW_POSTION_X		395
#define SIS3316_MAW_DATA_WINDOW_POSTION_Y		565

#define SIS3316_FFT_WINDOW_WIDTH				800
#define SIS3316_FFT_WINDOW_HIGH					490
#define SIS3316_FFT_WINDOW_POSTION_X			395
#define SIS3316_FFT_WINDOW_POSTION_Y			565


//#include "rootIncludes.h"
#include "sis3316_flash_class.h"

#include <math.h>
//#include <complex.h>
#include "fftw3.h"

//#ifdef raus
#include <TROOT.h>
#include <TApplication.h>
#include <TVirtualX.h>
#include <TVirtualPadEditor.h>
#include <TGResourcePool.h>
#include <TGListBox.h>
#include <TGListTree.h>
#include <TGFSContainer.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TGTextEdit.h>
#include <TGShutter.h>
#include <TGProgressBar.h>
#include <TGColorSelect.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TEnv.h>
#include <TFile.h>
#include <TKey.h>
#include <TGDockableFrame.h>
#include <TGFontDialog.h>
#include <TStyle.h>
#include <TPave.h>
#include <TPaveText.h>

#include "TGraph.h"
#include "TMultiGraph.h"
#include "TLatex.h"

#include "TMath.h"
#include "TF1.h"
//#endif

#ifdef WINDOWS

	#pragma comment (lib, "libGui")
	#pragma comment (lib, "libCore")
	#pragma comment (lib, "libCint")
	#pragma comment (lib, "libRIO")
	#pragma comment (lib, "libNet")
	#pragma comment (lib, "libHist")
	#pragma comment (lib, "libGraf")
	#pragma comment (lib, "libGraf3d")
	#pragma comment (lib, "libGpad")
	#pragma comment (lib, "libTree")
	#pragma comment (lib, "libRint")
	#pragma comment (lib, "libPostscript")
	#pragma comment (lib, "libMatrix")
	#pragma comment (lib, "libPhysics")
	#pragma comment (lib, "libMathCore")
	#pragma comment (lib, "libThread")
//	#pragma comment (lib, "liblistDict")


/*****************************************************************************************************/

	#include <iostream>
	#include <iomanip>
	using namespace std;
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>

	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
#endif



// choose Interface
#include "vme_interface_class.h"

#ifdef PCI_VME_INTERFACE
	#include "sis1100w_vme_class.h"
	sis1100 *gl_vme_crate ;
#endif

#ifdef USB_VME_INTERFACE
	#include "sis3150w_vme_class.h"
	sis3150 *gl_vme_crate ;
#endif

#ifdef USB3_VME_INTERFACE
	#include "sis3153w_vme_class.h"
	sis3153 *gl_vme_crate ;
#endif


#ifdef ETHERNET_UDP_INTERFACE
	#include "sis3316_ethernet_access_class.h"
	sis3316_eth *gl_vme_crate ;
	char  gl_sis3316_ip_addr_string[32] ;

	#ifdef LINUX
		#include <sys/types.h>
		#include <sys/socket.h>
	#endif

	#ifdef WINDOWS
	#include <winsock2.h>
	#pragma comment(lib, "ws2_32.lib")
	//#pragma comment(lib, "wsock32.lib")
	typedef int socklen_t;

	long WinsockStartup()
	{
	  long rc;

	  WORD wVersionRequested;
	  WSADATA wsaData;
	  wVersionRequested = MAKEWORD(2, 1);

	  rc = WSAStartup( wVersionRequested, &wsaData );
	  return rc;
	}
	#endif

#endif

#ifdef ETHERNET_VME_INTERFACE
	#include "sis3153ETH_vme_class.h"
	sis3153eth *gl_vme_crate;
//	char  gl_sis3153_ip_addr_string[32] = "212.60.16.206";
	char  gl_sis3153_ip_addr_string[32] = "192.168.1.11";

	#ifdef LINUX
		#include <sys/types.h>
		#include <sys/socket.h>
	#endif

	#ifdef WINDOWS
	#include <winsock2.h>
	#pragma comment(lib, "ws2_32.lib")
	//#pragma comment(lib, "wsock32.lib")
	typedef int socklen_t;

/*	long WinsockStartup()
	{
	  long rc;

	  WORD wVersionRequested;
	  WSADATA wsaData;
	  wVersionRequested = MAKEWORD(2, 1);

	  rc = WSAStartup( wVersionRequested, &wsaData );
	  return rc;
	}
	*/
	#endif
#endif


unsigned int gl_module_base_addr = FIRST_MODULE_BASE_ADDR   ;


#include "sis3316_class.h"
	sis3316_adc *gl_sis3316_adc1 ;

#include "sis3316.h"



//#define MAX_PRETRIGGER_DELAY 2042 // ADC FPGA Version A006 and lower
#define MAX_PRETRIGGER_DELAY 16378 // ADC FPGA Version A007 and higher

//#define MAX_ROOT_PLOT_LENGTH 0x40000 // 262144
//#define MAX_ROOT_PLOT_LENGTH 0x80000 // 524288
//#define MAX_ROOT_PLOT_LENGTH 0x100000 // 1.048.576
//#define MAX_ROOT_PLOT_LENGTH 0x200000 // 2.097.152
#define MAX_ROOT_PLOT_LENGTH 0x100000 //
#define MAX_SAMPLE_LENGTH 0x10000-2 //

//#define ADC_BUFFER_LENGTH 0x1000000 // 16 Msamples / 32MByte
//#define ADC_BUFFER_LENGTH 0x400000 // 4 Msamples / 8MByte
#define ADC_BUFFER_LENGTH                   0x100000 // 1 Msamples / 2MByte

Int_t gl_x[MAX_ROOT_PLOT_LENGTH] ;
Int_t gl_y[MAX_ROOT_PLOT_LENGTH] ;

float gl_float_x[MAX_ROOT_PLOT_LENGTH/2] ;
float gl_float_y[MAX_ROOT_PLOT_LENGTH/2] ;
float gl_float_y1[MAX_ROOT_PLOT_LENGTH/2] ;

#define MAX_ROOT_PLOT_MAW_LENGTH 1024 //
Int_t gl_maw_x[MAX_ROOT_PLOT_MAW_LENGTH] ;
Int_t gl_maw_y[MAX_ROOT_PLOT_MAW_LENGTH] ;



unsigned int gl_uint_adc1_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc2_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc3_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc4_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc5_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc6_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc7_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc8_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc9_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc10_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc11_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc12_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc13_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc14_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc15_buffer[ADC_BUFFER_LENGTH/2]; //
unsigned int gl_uint_adc16_buffer[ADC_BUFFER_LENGTH/2]; //


#define MAX_NUMBER_LWORDS_64MBYTE			0x1000000       /* 64MByte */
unsigned int gl_rblt_data[MAX_NUMBER_LWORDS_64MBYTE] ;


unsigned int i_ch;
//unsigned int max_nof_channels=10;

unsigned short* ushort_adcA_buffer_ptr; //
unsigned short* ushort_adcB_buffer_ptr; //


#define DefineCanvasBackgroundColor   10
#define DefineChannel_1_Color          2
#define DefineChannel_2_Color          3
#define DefineChannel_3_Color          4
#define DefineChannel_4_Color          6
#define DefineChannel_5_Color          7
#define DefineChannel_6_Color          8
#define DefineChannel_7_Color          9
#define DefineChannel_8_Color         28
#define DefineChannel_9_Color         30
#define DefineChannel_10_Color        40
#define DefineChannel_11_Color        41
#define DefineChannel_12_Color        42
#define DefineChannel_13_Color        43
#define DefineChannel_14_Color        44
#define DefineChannel_15_Color        45
#define DefineChannel_16_Color        46

int SIS3316_WriteBankChannelHeaderToDataFile (FILE *file_data_ptr, unsigned int indentifier, unsigned int bank_loop_no, unsigned int channel_no, unsigned int nof_events, unsigned int event_length, unsigned int maw_length, unsigned int reserved);
int SIS3316_WriteBankChannelEventBufferToDataFile (FILE *file_data_ptr, unsigned int* memory_data_array, unsigned int nof_write_length_lwords);


enum ETestCommandIdentifiers {
   M_FILE_EXIT,
   M_SIS3316TEST1_DLG,
   M_FPGA_PROG_MENUE,
   M_HELP_ABOUT,
};


//class TileFrame;

class TestMainFrame : public TGMainFrame {

private:
   TGCompositeFrame    *main_frameh1;
   TGGroupFrame        *main_frameh1_fGrp[4];

   TGHorizontalFrame    *main_framehsub, *main_framehsub0;
   TGLabel *fLabel_main_frameh1[8] ;
   TGLabel *fLabel_main_frameh2[6] ;

   TGDockableFrame    *fMenuDock;
   TGCompositeFrame   *fStatusFrame;
   TGCanvas           *fCanvasWindow;
   TGTextEntry        *fTestText;
   TGTextBuffer        *fTbmsg;

   TGButton           *fTestButton;
   TGColorSelect      *fColorSel;

   TGMenuBar          *fMenuBar;
   TGPopupMenu        *fMenuFile, *fMenuSIS3316Test, *fMenuFpgaProgram, *fMenuHelp;
   TGLayoutHints      *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;

   TGNumberEntry      *fNumericEntriesModuleAddress;
   TGTextEntry       *fTextEntryModuleIpString;

	TGButton           *fValidateIpAddressButton;
	TGButton            *fChk_JumboFrame;
	TGHorizontalFrame   *fTGHorizontalIpSubFrame1, *fTGHorizontalIpSubFrame2;
	TGNumberEntry       *fNumericEntries_MaxNofPacketsPerReadRequest;
	TGLabel             *fLabel_MaxNofPacketsPerReadRequest;

	TGTextBuffer      *fTextBufferModuleIpString;

	sis3316_flash *fFlashWindow;

protected:
	Bool_t fB_openfMenuSIS3316TestWindowFlag; // shows if setup window is open
	Bool_t fB_openProgramMenueWindowFlag; // shows if setup window is open
	
public:
   TestMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~TestMainFrame();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   int ShowModuleInformation();
};

/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/

#define SIS3316Test_MAX_TABLE_TABS   10
enum ETest1CommandIdentifiers {
   TEST1_FILE_EXIT,
   TEST1_HELP_ABOUT,
};

class SIS3316TestDialog : public TGTransientFrame {

private:
	Bool_t         *fBSetup;       // shows if setup is open

   TGMenuBar          *fMenuBar;
   TGPopupMenu        *fMenuFile, *fMenuSIS3316Test, *fMenuHelp;
   TGLayoutHints      *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;

	TGCompositeFrame    *fFrame2   ;
	TGHorizontalFrame   *fFrame1,  *fTGHor_frame1,  *fTGHor_frame2;
	TGCompositeFrame    *fF1, *fF1A, *fF1B, *fF1C, *fF2, *fF3, *fF4, *fF4A, *fF5, *fF5A , *fF5B , *fF5C, *fF5D   ;
   TGCompositeFrame    *fF_tab1, *fF_tab2, *fF_tab2b, *fF_tab3, *fF_tab3a, *fF_tab4, *fF_tab5, *fF_tab6, *fF_tab7, *fF_tab8;
   TGGroupFrame			*fGrp1;
   TGGroupFrame			*fF_tab1_fGrp1, *fF_tab1_fGrp1A, *fF_tab1_fGrp1B, *fF_tab1_fGrp2, *fF_tab1_fGrp3, *fF_tab1_fGrp4;
   TGGroupFrame			*fF_tab2_fGrp1, *fF_tab2_fGrp2, *fF_tab2_fGrp3, *fF_tab2_fGrp4;
   TGGroupFrame			*fF_tab2b_fGrp1 ;
   TGGroupFrame			*fF_tab3_fGrp1, *fF_tab3_fGrp2;
   TGGroupFrame			*fF_tab3a_fGrp1, *fF_tab3a_fGrp2 ;
   TGGroupFrame			*fF_tab4_fGrp1, *fF_tab4_fGrp2, *fF_tab4_fGrp3, *fF_tab4_fGrp4;
   TGGroupFrame			*fF_tab5_fGrp1, *fF_tab5_fGrp1A, *fF_tab5_fGrp2  ;

 	TGTabElement		*tabel_tab[SIS3316Test_MAX_TABLE_TABS] ;
	unsigned int		sis3316Test1_nof_valid_tabel_tabs ;

   TGHorizontalFrame    *fTGHorizontalFrame;
   TGVerticalFrame		*fTGVerticalFrame;

   TGHorizontalFrame    *fF[16];
   TGVerticalFrame		*fVF[16];
//   TGHorizontalFrame    *fF_ch;
   TGGroupFrame        *fF6, *fF7;
   TGButton            *fQuitButton;
   TGButton            *fStartB, *fStopB;
   TGButton            *fBtn1, *fBtn2, *fChk1, *fChk2, *fRad1, *fRad2;


	TGTextEntry			*fTextEntryDataFilePath;
	TGTextButton		*fTextButtonDataFilePath;

   TGButton            *fChkFP_BUS_ClockMaster;
//   TGButton            *fChkChannelInputInvert;
   TGButton            *fChkKeyTrigger, *fChkLemoInTiEnable, *fChkExternalTrigger, *fChkInternalTrigger, *fChkIntertnalSumTrigger;

   TGButton            *fChkDisplayAutoZoom;
   TGButton            *fDisplayEnableCh_Set, *fDisplayEnableCh_Clr;

   TGButton            *fChkDisplayAdc[16];
   TGButton            *fChkHistoSum;
   TGButton            *fChkHistoZoomMean;
   TGButton            *fChkHistoGaussFit;
   TGButton            *fChkFFT_Db, *fChkFFT_AutoScale, *fChkFFT_Sum, *fChkFFTLogY;


   TGButton            *fChkInvertChannel[16];
   TGButton            *fInvertChannel_Set, *fInvertChannel_Clr;

   TGButton            *fChkTriggerCfdEnable;
   TGButton            *fChkTriggerEnableCh[16+4];
   TGButton            *fTriggerEnableCh_Set, *fTriggerEnableCh_Clr;

   TGButton            *fChkInput_50Ohm, *fChkInput_5V_Range, *fChkDacInrementTest;

	TGButton            *fChkStopAfterBanks;
	TGNumberEntry       *fNumericEntriesStopAfterBanks;

	TGButton            *fChkWriteDataToFile, *fChkWriteMultipleFiles;
	TGFileInfo			fileInfoDataFile;

	TGButton            *fChkNofEvents_ProBank;
	TGNumberEntry       *fNumericEntries_SampleControl_NofEvents_ProBank;
	TGNumberEntry       *fNumericEntries_SampleControl_MaxNofEvents_ProBank;


	TGButton            *fChk_EventHitParameter_DataFormatBit0, *fChk_EventHitParameter_DataFormatBit2, *fChk_EventHitParameter_DataFormatBit3;

	
 
   TGPictureButton     *fPicBut1;
   TGCheckButton       *fCheck1;
 //  TGCheckButton       *fCheckMulti;
//   TGListBox           *fListBox;
   TGComboBox          *fCombo;
   TGTab               *fTab;
   TGNumberEntry       *fNumericEntries_EventHitParameter[8];
   TGLabel              *fLabel[8];
//   TGTextEntry         *fTxt1, *fTxt2;
   TGLayoutHints       *fL1, *fL2, *fL3, *fL4, *fL5, *fL7;
//   TRootEmbeddedCanvas *fEc1, *fEc2;
//   Int_t                fFirstEntry;
//   Int_t                fLastEntry;
   Bool_t               fSIS3316_Test1;
   TGraph               *fGraph_ch[17];
   TLatex              	*fGraph_Text_ch[17];
   TGraph               *fGraph_maw;
   TH1I                	*iHistoAdc[16];
   TGraph               *fGraph_fft[17];
   //TF1               *fGraph_fft[17];
 //  TGListBox            *fListBox_Display_Histos;

   TGComboBox          *fCombo_SampleControl_BankModus;
   TGComboBox          *fCombo_FP_BUS_ClockOutMux;

   TGComboBox          *fCombo_SampleClock_source;
   TGComboBox          *fCombo_SetInternalClockFreq;
   TGComboBox          *fCombo_SetClockMultiplierMode;
   TGComboBox          *fCombo_SetSelectMAW_TestBuffer;


   TGComboBox          *fCombo_Display_MAW;
   TGComboBox          *fCombo_Display_Histos_Ch;
   TGComboBox          *fCombo_Display_Histos_Build;
   TGComboBox          *fCombo_Display_FFT_Ch;
   TGComboBox          *fCombo_Display_FFT_Window;

   TGComboBox          *fCombo_Set_ADC_SPI_Input_Voltage;
   TGLabel				*fLabel_fCombo_Set_ADC_SPI_text[4] ;
 

   TCanvas              *fCanvas1;
   TCanvas              *fCanvas2;
   TCanvas              *fCanvas3;
   TCanvas              *fCanvas4;
   TPaveText		*histo_pave_text[16];

   	
	
	TGNumberEntry       *fNumericEntriesHistogramXaxisOffset;
	TGNumberEntry       *fNumericEntriesHistogramXaxisDivider;

   TGNumberEntry       *fNumericEntriesGraph_Yaxis[2];
   TGNumberEntry       *fNumericEntriesAnalogOffset;
   TGNumberEntry       *fNumericEntriesTapDelay;

   TGNumberEntry       *fNumericEntriesTriggerPulse_length;
   TGNumberEntry       *fNumericEntriesTriggerGap;
   TGNumberEntry       *fNumericEntriesTriggerPeaking;
   TGNumberEntry       *fNumericEntriesTriggerThreshold;

   TGNumberEntry       *fNumericEntriesEnergyGap;
   TGNumberEntry       *fNumericEntriesEnergyPeaking;
   TGNumberEntry       *fNumericEntriesEnergyTauTable;
   TGNumberEntry       *fNumericEntriesEnergyTauFactor;
   TGNumberEntry       *fNumericEntriesEnergyAdditionalAverage;

	TGNumberEntry       *fNumericEntriesAccumulatorStartIndex[6];
	TGNumberEntry       *fNumericEntriesAccumulatorLength[6];
	TGLabel				*fLabel_AccumulatorStartIndex_text[6] ;
	TGLabel				*fLabel_AccumulatorLength_text[6] ;


   
    Pixel_t tab_color_not_active  ;
    Pixel_t tab_color_active      ;

   
   
   //   int gl_class_SIS3316_device;
//   unsigned int gl_class_adc_tap_delay;

   static const char *const numlabel[8];
   static const char *const entrySampleControl_BankModus[2];
   static const char *const entryFP_BUS_ClockOutMux[2];
   static const char *const entryClock_freq[16];
   static const char *const entryClock_multiplier_modes[7];
   static const char *const entryClock_source[4];

    static const char *const entryClock_SelectMAW_TestBuffer[2];

   static const Double_t numinit[8];
   static const char *const chkDisAdcLabel[16];
   static const char *const AdcHistogramLabel[16];
   static const char *const entryMawLabel[17];

   
   static const char *const entryHistoDisplayOption[3];

   static const char *const entryHistoLabel[18];
   static const char *const entryDisplayFFTLabel[17];
   static const char *const entryDisplayFFTWindowLabel[6];
   static const char *const entryADC_SPI_InputVoltage[3];

   static const char *const chkTriggerEnableChLabel[16+4];

   static const char *const accuStartIndexlabel[6];
   static const char *const accuLengthlabel[6];
 


   static const char *const SIS3316_devices_txt[6];




	void SIS3316_Test1();
	void SIS3316_Test_running_dim_widgets(bool dim_state) ;
		

protected:
	Bool_t fB_openfCanvas1WindowFlag; // shows if Canvas1 window is open
	Bool_t fB_openfCanvas2WindowFlag; // shows if Canvas2 window is open
	Bool_t fB_openfCanvas3WindowFlag; // shows if Canvas3 window is open
	Bool_t fB_openfCanvas4WindowFlag; // shows if Canvas4 window is open


public:
   SIS3316TestDialog(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, Bool_t *b,
               UInt_t options = kVerticalFrame);
   virtual ~SIS3316TestDialog();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};




/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/
/*************************************************************************************************************************/

TestMainFrame::TestMainFrame(const TGWindow *p, UInt_t w, UInt_t h)
      : TGMainFrame(p, w, h)
{


	sis3316_adc  *sis3316_adc1 ;

	// Create test main frame. A TGMainFrame is a top level window.

   // use hierarchical cleaning
	this->SetCleanup(kDeepCleanup);

    this->SetWindowName("SIS3316 Test Menu");
	this->SetIconPixmap("sis1_sis3316.png");

	// open menueWindows
	fB_openfMenuSIS3316TestWindowFlag = kFALSE; // Setup


   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 10, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   fMenuFile = new TGPopupMenu(fClient->GetRoot());
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);


// add SIS3316
   //fMenuSIS3316Test = new TGPopupMenu(fClient->GetRoot());
   fMenuSIS3316Test = new TGPopupMenu(gClient->GetDefaultRoot());
   fMenuSIS3316Test->AddLabel("SIS3316 Test");
   fMenuSIS3316Test->AddSeparator();
   fMenuSIS3316Test->AddEntry("&Test 1", M_SIS3316TEST1_DLG);
   fMenuSIS3316Test->DisableEntry(M_SIS3316TEST1_DLG);

// add FPGA Programming
	fMenuFpgaProgram = new TGPopupMenu(gClient->GetDefaultRoot());
	fMenuFpgaProgram->AddEntry("FPGA Programming Menue", M_FPGA_PROG_MENUE);
	fMenuFpgaProgram->DisableEntry(M_FPGA_PROG_MENUE);



  // fMenuHelp = new TGPopupMenu(fClient->GetRoot());
   fMenuHelp = new TGPopupMenu(gClient->GetDefaultRoot());
   //fMenuHelp->AddSeparator();
   fMenuHelp->AddEntry("&About", M_HELP_ABOUT);

   // Menu button messages are handled by the main frame (i.e. "this")
   // ProcessMessage() method.
   fMenuFile->Associate(this);
   fMenuSIS3316Test->Associate(this);
	fMenuFpgaProgram->Associate(this);

   fMenuBar = new TGMenuBar(this);
   //fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup("SIS3316 Test", fMenuSIS3316Test, fMenuBarItemLayout);
   fMenuBar->AddPopup("FPGA Programming", fMenuFpgaProgram, fMenuBarItemLayout);
   fMenuBar->AddPopup("Help", fMenuHelp, fMenuBarHelpLayout);

   //fMenuDock->AddFrame(fMenuBar, fMenuBarLayout);
	this->AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));



	main_frameh1 = new TGCompositeFrame(this, kHorizontalFrame);

	//main_frameh1->SetBackgroundColor(0xee0000); //
	this->AddFrame(main_frameh1, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

   //fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft ,  20 ,2, 2, 12); //hints, left, right, top, bottom
   //fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 50, 2); // hints, left, right, top, bottom

	TGIcon *fIcon1341 = new TGIcon(main_frameh1, "sislogo.bmp");
	fIcon1341->SetName("fIcon1341");
	main_frameh1->AddFrame(fIcon1341, new TGLayoutHints(kLHintsLeft | kLHintsTop, 5, 5, 15, 15));



#ifdef ETHERNET_VME_INTERFACE
	main_frameh1_fGrp[4] = new TGGroupFrame(main_frameh1, "SIS3153 IP Address");
	main_frameh1->AddFrame(main_frameh1_fGrp[4], new TGLayoutHints(kLHintsExpandX, 5, 5, 25, 25)); // hints, left, right, top, bottom
	main_framehsub0 = new TGHorizontalFrame(main_frameh1_fGrp[4], 200, 30);
	main_frameh1_fGrp[4]->AddFrame(main_framehsub0, new TGLayoutHints(kLHintsExpandX, 2, 2, 12, 2));

	strcpy(gl_sis3153_ip_addr_string,"212.60.16.200") ; // SIS3153 IP address
//	strcpy(gl_sis3153_ip_addr_string,"192.168.1.11") ; // SIS3153 IP address

	fTextEntryModuleIpString = new TGTextEntry(main_framehsub0, new TGTextBuffer(32), 9);
	fTextEntryModuleIpString->SetMaxLength(32);
	fTextEntryModuleIpString->Resize(200, fTextEntryModuleIpString->GetDefaultHeight());
 	fTextEntryModuleIpString->SetAlignment(kTextLeft);
	fTextEntryModuleIpString->SetText(gl_sis3153_ip_addr_string);
	//fTextEntryModuleIpString->Resize(83,fTextEntryModuleIpString->GetDefaultHeight());
	fTextEntryModuleIpString->Associate(this);
	main_framehsub0->AddFrame(fTextEntryModuleIpString, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));

#endif

	main_frameh1_fGrp[0] = new TGGroupFrame(main_frameh1, "Interface Information");
	main_frameh1->AddFrame(main_frameh1_fGrp[0], new TGLayoutHints(kLHintsExpandX, 5, 5, 25, 25)); // hints, left, right, top, bottom

	fLabel_main_frameh1[0] = new TGLabel(main_frameh1_fGrp[0]," ");
	fLabel_main_frameh1[0]->SetTextJustify(kTextLeft + kTextCenterX );
	fLabel_main_frameh1[0]->SetMargins(0,0,0,0);
	fLabel_main_frameh1[0]->SetWrapLength(-1);
	main_frameh1_fGrp[0]->AddFrame(fLabel_main_frameh1[0], new TGLayoutHints(kLHintsExpandX,2,2,15,2));

	fLabel_main_frameh1[1] = new TGLabel(main_frameh1_fGrp[0]);
	fLabel_main_frameh1[1]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh1[1]->SetMargins(0,0,0,0);
	fLabel_main_frameh1[1]->SetWrapLength(-1);
	main_frameh1_fGrp[0]->AddFrame(fLabel_main_frameh1[1], new TGLayoutHints(kLHintsExpandX,2,2,2,5));

	fLabel_main_frameh1[2] = new TGLabel(main_frameh1_fGrp[0]);
	fLabel_main_frameh1[2]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh1[2]->SetMargins(0,0,0,0);
	fLabel_main_frameh1[2]->SetWrapLength(-1);
	main_frameh1_fGrp[0]->AddFrame(fLabel_main_frameh1[2], new TGLayoutHints(kLHintsExpandX,2,2,5,2));

	fLabel_main_frameh1[3] = new TGLabel(main_frameh1_fGrp[0]);
	fLabel_main_frameh1[3]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh1[3]->SetMargins(0,0,0,0);
	fLabel_main_frameh1[3]->SetWrapLength(-1);
	main_frameh1_fGrp[0]->AddFrame(fLabel_main_frameh1[3], new TGLayoutHints(kLHintsExpandX,2,2,2,2));

	fLabel_main_frameh1[4] = new TGLabel(main_frameh1_fGrp[0]);
	fLabel_main_frameh1[4]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh1[4]->SetMargins(0,0,0,0);
	fLabel_main_frameh1[4]->SetWrapLength(-1);
	main_frameh1_fGrp[0]->AddFrame(fLabel_main_frameh1[4], new TGLayoutHints(kLHintsExpandX,2,2,2,2));

	fLabel_main_frameh1[5] = new TGLabel(main_frameh1_fGrp[0]);
	fLabel_main_frameh1[5]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh1[5]->SetMargins(0,0,0,0);
	fLabel_main_frameh1[5]->SetWrapLength(-1);
	main_frameh1_fGrp[0]->AddFrame(fLabel_main_frameh1[5], new TGLayoutHints(kLHintsExpandX,2,2,2,2));

	fLabel_main_frameh1[6] = new TGLabel(main_frameh1_fGrp[0]);
	fLabel_main_frameh1[6]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh1[6]->SetMargins(0,0,0,0);
	fLabel_main_frameh1[6]->SetWrapLength(-1);
	main_frameh1_fGrp[0]->AddFrame(fLabel_main_frameh1[6], new TGLayoutHints(kLHintsExpandX,2,2,2,2));

	fLabel_main_frameh1[7] = new TGLabel(main_frameh1_fGrp[0]);
	fLabel_main_frameh1[7]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh1[7]->SetMargins(0,0,0,0);
	fLabel_main_frameh1[7]->SetWrapLength(-1);
	main_frameh1_fGrp[0]->AddFrame(fLabel_main_frameh1[7], new TGLayoutHints(kLHintsExpandX,2,2,2,15)); // hints, left, right, top, bottom


#ifdef ETHERNET_UDP_INTERFACE

	main_frameh1_fGrp[2] = new TGGroupFrame(main_frameh1, "SIS3316 IP Address");
	main_frameh1->AddFrame(main_frameh1_fGrp[2], new TGLayoutHints(kLHintsExpandX, 5, 5, 25, 25)); // hints, left, right, top, bottom
	main_framehsub = new TGHorizontalFrame(main_frameh1_fGrp[2], 200, 30);
	main_frameh1_fGrp[2]->AddFrame(main_framehsub, new TGLayoutHints(kLHintsExpandX, 2, 2, 12, 2));

	//char  gl_sis3316_ip_addr_string[32] ;
	//strcpy(gl_sis3316_ip_addr_string,"212.60.16.200") ; // SIS3316 IP address
	strcpy(gl_sis3316_ip_addr_string,"192.168.1.100") ; // SIS3316 IP address
	strcpy(gl_sis3316_ip_addr_string,"sis3316-0161") ; // SIS3316 IP address



	//fTextEntryModuleIpString = new TGTextEntry(main_framehsub, 10);
	fTextEntryModuleIpString = new TGTextEntry(main_framehsub, new TGTextBuffer(32), 10);
	fTextEntryModuleIpString->SetMaxLength(32);
	fTextEntryModuleIpString->Resize(150, fTextEntryModuleIpString->GetDefaultHeight());
 	fTextEntryModuleIpString->SetAlignment(kTextLeft);
	fTextEntryModuleIpString->SetText(gl_sis3316_ip_addr_string);
	//fTextEntryModuleIpString->Resize(83,fTextEntryModuleIpString->GetDefaultHeight());
	fTextEntryModuleIpString->Associate(this);
	main_framehsub->AddFrame(fTextEntryModuleIpString, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	fTextEntryModuleIpString->ChangeBackground(GetDefaultFrameBackground()); //

	fValidateIpAddressButton = new TGTextButton(main_framehsub, "  Validate IP address  ", 10);
	fValidateIpAddressButton->ChangeBackground(GetDefaultFrameBackground()); //  
	fValidateIpAddressButton->Associate(this);
	main_framehsub->AddFrame(fValidateIpAddressButton, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 12, 2, 2, 2));



	gl_module_base_addr =  0x0;
 
#else

	main_frameh1_fGrp[2] = new TGGroupFrame(main_frameh1, "SIS3316 VME Base Address");
	main_frameh1->AddFrame(main_frameh1_fGrp[2], new TGLayoutHints(kLHintsExpandX, 5, 5, 25, 25)); // hints, left, right, top, bottom
	main_framehsub = new TGHorizontalFrame(main_frameh1_fGrp[2], 200, 30);
	main_frameh1_fGrp[2]->AddFrame(main_framehsub, new TGLayoutHints(kLHintsExpandX, 2, 2, 12, 2));
	//fNumericEntriesModuleAddress = new TGNumberEntry(main_framehsub, 0x30000000 /* value */, 18 /* width */, 10 /* irq */ , (TGNumberFormat::kNESHex) ) ; //kNESHex
	fNumericEntriesModuleAddress = new TGNumberEntry(main_framehsub, gl_module_base_addr /* value */, 18 /* width */, 10 /* irq */ , (TGNumberFormat::kNESHex) ) ; //kNESHex
	fNumericEntriesModuleAddress->Associate(this);
	main_framehsub->AddFrame(fNumericEntriesModuleAddress, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));

#endif
 






	main_frameh1_fGrp[1] = new TGGroupFrame(main_frameh1, "SIS3316 Information");
	main_frameh1->AddFrame(main_frameh1_fGrp[1], new TGLayoutHints(kLHintsExpandX, 5, 5, 25, 25)); // hints, left, right, top, bottom

	fLabel_main_frameh2[0] = new TGLabel(main_frameh1_fGrp[1]," ");
	fLabel_main_frameh2[0]->SetTextJustify(kTextLeft + kTextCenterX );
	fLabel_main_frameh2[0]->SetMargins(0,0,0,0);
	fLabel_main_frameh2[0]->SetWrapLength(-1);
	main_frameh1_fGrp[1]->AddFrame(fLabel_main_frameh2[0], new TGLayoutHints(kLHintsExpandX,2,2,15,2));

	fLabel_main_frameh2[1] = new TGLabel(main_frameh1_fGrp[1]);
	fLabel_main_frameh2[1]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh2[1]->SetMargins(0,0,0,0);
	fLabel_main_frameh2[1]->SetWrapLength(-1);
	main_frameh1_fGrp[1]->AddFrame(fLabel_main_frameh2[1], new TGLayoutHints(kLHintsExpandX,2,2,2,2));

	fLabel_main_frameh2[2] = new TGLabel(main_frameh1_fGrp[1]);
	fLabel_main_frameh2[2]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh2[2]->SetMargins(0,0,0,0);
	fLabel_main_frameh2[2]->SetWrapLength(-1);
	main_frameh1_fGrp[1]->AddFrame(fLabel_main_frameh2[2], new TGLayoutHints(kLHintsExpandX,2,2,2,2));

	fLabel_main_frameh2[3] = new TGLabel(main_frameh1_fGrp[1]);
	fLabel_main_frameh2[3]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh2[3]->SetMargins(0,0,0,0);
	fLabel_main_frameh2[3]->SetWrapLength(-1);
	main_frameh1_fGrp[1]->AddFrame(fLabel_main_frameh2[3], new TGLayoutHints(kLHintsExpandX,2,2,2,2));

	fLabel_main_frameh2[4] = new TGLabel(main_frameh1_fGrp[1]);
	fLabel_main_frameh2[4]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh2[4]->SetMargins(0,0,0,0);
	fLabel_main_frameh2[4]->SetWrapLength(-1);
	main_frameh1_fGrp[1]->AddFrame(fLabel_main_frameh2[4], new TGLayoutHints(kLHintsExpandX,2,2,2,15));

	fLabel_main_frameh2[5] = new TGLabel(main_frameh1_fGrp[1]);
	fLabel_main_frameh2[5]->SetTextJustify(kTextLeft + kTextCenterX);
	fLabel_main_frameh2[5]->SetMargins(0,0,0,0);
	fLabel_main_frameh2[5]->SetWrapLength(-1);
	main_frameh1_fGrp[1]->AddFrame(fLabel_main_frameh2[5], new TGLayoutHints(kLHintsExpandX,2,2,2,15));




#ifdef not_used
   // Create status frame containing a button and a text entry widget
   fStatusFrame = new TGCompositeFrame(this, 160, 220, kHorizontalFrame | kSunkenFrame);

   fTestText = new TGTextEntry(fStatusFrame, fTbmsg = new TGTextBuffer(200));
   //fTestText = new TGTextEntry(fStatusFrame, new TGTextBuffer(200));
//    fTestText->SetToolTipText("This is a text entry widget");
   fTestText->Resize(200, fTestText->GetDefaultHeight());
   fStatusFrame->AddFrame(fTestText, new TGLayoutHints(kLHintsTop | kLHintsLeft, 10, 2, 2, 2));
   AddFrame(fStatusFrame, new TGLayoutHints(kLHintsBottom | kLHintsExpandX,  5, 5, 5, 5));

   fTbmsg->Clear();
   fTbmsg->AddText(0, "message box");
#endif
/******************************************************************************************************************************/
/* VME Master Create, Open and Setup                                                                                          */
/******************************************************************************************************************************/


#ifdef PCI_VME_INTERFACE
	// create SIS1100/SIS310x vme interface device
	sis1100 *vme_crate = new sis1100(0);
#endif

#ifdef USB_VME_INTERFACE
USHORT idVendor;
USHORT idProduct;
USHORT idSerNo;
USHORT idFirmwareVersion;
USHORT idDriverVersion;
	// create SIS3150USB vme interface device
	sis3150 *vme_crate = new sis3150(0);
#endif

#ifdef USB3_VME_INTERFACE
USHORT idVendor;
USHORT idProduct;
USHORT idSerNo;
USHORT idDriverVersion;
USHORT idFxFirmwareVersion;
USHORT idFpgaFirmwareVersion;
	// create SIS3153USB vme interface device
	sis3153 *vme_crate = new sis3153(0);
#endif



CHAR char_messages[128];
UINT nof_found_devices ;
unsigned int return_code ;

char s[64];

#ifdef ETHERNET_UDP_INTERFACE

	char  pc_ip_addr_string[32] ;
	char  sis3316_ip_addr_string[32] ;
	strcpy(sis3316_ip_addr_string, gl_sis3316_ip_addr_string) ; // SIS3316 IP address
	//strcpy(sis3316_ip_addr_string,"212.60.16.200") ; // SIS3316 IP address
	//int return_code ;
	#ifdef WINDOWS
    //return_code = WSAStartup();
    return_code = WinsockStartup();
	#endif
	sis3316_eth *vme_crate = new sis3316_eth;
	// increase read_buffer size
	// SUSE needs following command as su: >sysctl -w net.core.rmem_max=33554432
	int	sockbufsize = 335544432 ; // 0x2000000
	return_code = vme_crate->set_UdpSocketOptionBufSize(sockbufsize) ;

	//strcpy(pc_ip_addr_string,"212.60.16.49") ; // Window example: secocnd Lan interface IP address is 212.60.16.49
	strcpy(pc_ip_addr_string,"") ; // empty if default Lan interface (Window: use IP address to bind in case of 2. 3. 4. .. LAN Interface)
	return_code = vme_crate->set_UdpSocketBindMyOwnPort( pc_ip_addr_string);

	vme_crate->set_UdpSocketSIS3316_IpAddress( sis3316_ip_addr_string);
	vme_crate->udp_reset_cmd();
#endif

#ifdef ETHERNET_VME_INTERFACE
	sis3153eth *vme_crate;
	sis3153eth(&vme_crate, gl_sis3153_ip_addr_string);
#endif 
// open Vme Interface device
	return_code = vme_crate->vmeopen ();  // open Vme interface
	vme_crate->get_vmeopen_messages (char_messages, &nof_found_devices);  // open Vme interface

	fLabel_main_frameh1[0]->SetText(char_messages);
	sprintf(s,"-- found %d vme interface device(s)",nof_found_devices);
	fLabel_main_frameh1[1]->SetText(s);


	//printf("\n%s    (found %d vme interface device[s])\n\n",char_messages, nof_found_devices);


/******************************************************************************************/
// additional Vme interface device informations
#ifdef USB_VME_INTERFACE
	vme_crate->get_device_informations (&idVendor, &idProduct, &idSerNo, &idFirmwareVersion, &idDriverVersion);  //
	//printf("idVendor:           %04X\n",idVendor);
	//printf("idProduct:          %04X\n",idProduct);
	//printf("idSerNo:            %d\n",idSerNo);
	//printf("idFirmwareVersion:  %04X\n",idFirmwareVersion);
	//printf("idDriverVersion:    %04X\n",idDriverVersion);
	//printf("\n\n");

	sprintf(s,"idVendor:           %04X",idVendor);
	fLabel_main_frameh1[2]->SetText(s);
	sprintf(s,"idProduct:          %04X",idProduct);
	fLabel_main_frameh1[3]->SetText(s);
	sprintf(s,"idSerNo:            %d",idSerNo);
	fLabel_main_frameh1[4]->SetText(s);
	sprintf(s,"idFirmwareVersion:  %04X",idFirmwareVersion);
	fLabel_main_frameh1[5]->SetText(s);
	//sprintf((s,"idDriverVersion:    %04X",idDriverVersion);
	//fLabel_main_frameh1[6]->SetText(s);


#endif

#ifdef USB3_VME_INTERFACE
	vme_crate->get_device_informations (&idVendor, &idProduct, &idSerNo, &idDriverVersion, &idFxFirmwareVersion, &idFpgaFirmwareVersion);  //

	sprintf(s,"idVendor:               0x%04X",idVendor);
	fLabel_main_frameh1[2]->SetText(s);
	sprintf(s,"idProduct:              0x%04X",idProduct);
	fLabel_main_frameh1[3]->SetText(s);
	sprintf(s,"idSerNo:                %d",idSerNo);
	fLabel_main_frameh1[4]->SetText(s);
	sprintf(s,"idDriverVersion:        0x%04X",idDriverVersion);
	fLabel_main_frameh1[5]->SetText(s);
	sprintf(s,"idFxFirmwareVersion:    0x%04X",idFxFirmwareVersion);
	fLabel_main_frameh1[6]->SetText(s);
	sprintf(s,"idFpgaFirmwareVersion:  0x%04X",idFpgaFirmwareVersion);
	fLabel_main_frameh1[7]->SetText(s);

#endif



/******************************************************************************************/

	gl_vme_crate = vme_crate ;
	return_code = this->ShowModuleInformation();

	if (return_code == 0) {

		// kill request and grant from vme interface
		gl_vme_crate->vme_A32D32_write(gl_module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x80000000);
		// arbitrate
		gl_vme_crate->vme_A32D32_write(gl_module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 1);

		sis3316_adc1 = new sis3316_adc( gl_vme_crate, gl_module_base_addr);
		gl_sis3316_adc1 = sis3316_adc1 ;
		if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
			sprintf(s,"SIS3316-125MHz-16bit");
		}
		else {
			sprintf(s,"SIS3316-250MHz-14bit");
		}
		fLabel_main_frameh2[5]->SetText(s);
		fMenuSIS3316Test->EnableEntry(M_SIS3316TEST1_DLG);
		fMenuFpgaProgram->EnableEntry(M_FPGA_PROG_MENUE);

	}
	else {
		sis3316_adc1 = NULL  ;
		gl_sis3316_adc1 = sis3316_adc1 ;
		fMenuSIS3316Test->DisableEntry(M_SIS3316TEST1_DLG);
		fMenuFpgaProgram->DisableEntry(M_FPGA_PROG_MENUE);

	}


/******************************************************************************************/

	this->SetWindowName("SIS3316 Test (27-03-2015)");
	this->SetWMPosition(MAIN_WINDOW_POSTION_X, MAIN_WINDOW_POSTION_Y);
	this->MapSubwindows();
	this->Resize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HIGH);   // resize to default size
	this-> MapWindow();

}

TestMainFrame::~TestMainFrame()
{
   // Delete all created widgets.
   delete fMenuFile;
   delete fMenuSIS3316Test;
   delete fMenuHelp;
   delete gl_vme_crate;
}

void TestMainFrame::CloseWindow()
{
   // Got close message for this MainFrame. Terminate the application
   // or returns from the TApplication event loop (depending on the
   // argument specified in TApplication::Run()).

   gApplication->Terminate(0);
}

/*******************************************************************************************************************************/

int TestMainFrame::ShowModuleInformation()
{
unsigned int return_code, data ;
float modTemp;
char s[64];

	return_code = gl_vme_crate->vme_A32D32_read ( gl_module_base_addr + SIS3316_MODID, &data);
	//printf("vme_A32D32_read: data = 0x%08x     return_code = 0x%08x\n", data, return_code);

	if (return_code == 0) {
		if ((data &0xffff0000) == 0x33160000) {
			sprintf(s,"VME base address = 0x%08x",gl_module_base_addr);
			fLabel_main_frameh2[0]->SetText(s);
			fLabel_main_frameh2[0]->SetBackgroundColor(0x99FF99); // light green
			sprintf(s,"VME FPGA firmware version  = 0x%08x", data);
			fLabel_main_frameh2[1]->SetText(s);

			// kill request and grant from vme interface
			gl_vme_crate->vme_A32D32_write(gl_module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x80000000);
			// arbitrate
			gl_vme_crate->vme_A32D32_write(gl_module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 1);


			gl_vme_crate->vme_A32D32_read ( gl_module_base_addr + SIS3316_ADC_CH1_4_FIRMWARE_REG, &data);
			sprintf(s,"ADC FPGA firmware version = 0x%08x", data);
			fLabel_main_frameh2[2]->SetText(s);

			gl_vme_crate->vme_A32D32_read ( gl_module_base_addr + SIS3316_INTERNAL_TEMPERATURE_REG, &data);
			modTemp = (float)(data & 0x3FF);
			modTemp /= 4;
 			sprintf(s,"Temperature = %.1f C", modTemp);
			fLabel_main_frameh2[3]->SetText(s);

			gl_vme_crate->vme_A32D32_read ( gl_module_base_addr + SIS3316_SERIAL_NUMBER_REG, &data);
			sprintf(s,"Serial Numberr = %d", data);
			fLabel_main_frameh2[4]->SetText(s);

		}
		else {
			sprintf(s,"No SIS3316");
			fLabel_main_frameh2[0]->SetText(s);
			fLabel_main_frameh2[0]->SetBackgroundColor(0xFF9999); // light red
			sprintf(s,"address = 0x%08x",gl_module_base_addr);
			fLabel_main_frameh2[1]->SetText(s);
			sprintf(s,"version register = 0x%08x", data);
			fLabel_main_frameh2[2]->SetText(s);
			sprintf(s," ");
			fLabel_main_frameh2[3]->SetText(s);
			fLabel_main_frameh2[4]->SetText(s);
 			fLabel_main_frameh2[5]->SetText(s);
 			return_code = 0x900 ;
		}

	}
	else {
		sprintf(s,"No SIS3316");
		fLabel_main_frameh2[0]->SetText(s);
		fLabel_main_frameh2[0]->SetBackgroundColor(0xFF9999); // light red
		sprintf(s,"address = 0x%08x",gl_module_base_addr);
		fLabel_main_frameh2[1]->SetText(s);
		sprintf(s,"return_code = 0x%08x", return_code);
		fLabel_main_frameh2[2]->SetText(s);
		sprintf(s," ");
		fLabel_main_frameh2[3]->SetText(s);
		fLabel_main_frameh2[4]->SetText(s);
		fLabel_main_frameh2[5]->SetText(s);
	}
	return return_code ;
}

/*******************************************************************************************************************************/

Bool_t TestMainFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle messages send to the TestMainFrame object. E.g. all menu button
   // messages.
	CHAR char_messages[128];
	UINT nof_found_devices ;	
	int return_code;
	char s[64];
	TString tstring_temp = "                                 ";
	sis3316_adc  *sis3316_adc1 ;

   switch (GET_MSG(msg)) {
       //printf("switch (GET_MSG(msg) %d\n", GET_MSG(msg));


      case kC_COMMAND:
         //printf("kC_COMMAND\n");
         switch (GET_SUBMSG(msg)) {

            case kCM_MENUSELECT:
               //printf("kCM_MENUSELECT id=%ld\n", parm1);
               break;

            case kCM_MENU:
               //printf("kCM_MENU id=%ld\n", parm1);
               switch (parm1) {

                  case M_FILE_EXIT:
                     CloseWindow();   // this also terminates theApp
                     break;

				  case M_SIS3316TEST1_DLG:
						if(fB_openfMenuSIS3316TestWindowFlag == kFALSE){
							 //printf("M_SIS3316TEST1_DLG\n");
							new SIS3316TestDialog(fClient->GetRoot(), this, 800, 200, &fB_openfMenuSIS3316TestWindowFlag);
							fB_openfMenuSIS3316TestWindowFlag = kTRUE;
						}
						break;

				  case M_FPGA_PROG_MENUE:
						 //printf("M_FPGA_PROG_MENUE\n");
						if(fB_openProgramMenueWindowFlag == kFALSE){
							fB_openProgramMenueWindowFlag = kTRUE;
							fFlashWindow = new sis3316_flash(gClient->GetRoot(), 360, 700, &fB_openProgramMenueWindowFlag);
						}
						else{
							fFlashWindow->RequestFocus();
						}
						break;
						


//yyyyyyy
                  default:
                     break;
               } // kCM_MENU switch (parm1)
               break;

/********************************************/
		#ifdef ETHERNET_UDP_INTERFACE
			case kCM_BUTTON:
				switch(parm1) {
					case 10:

						fValidateIpAddressButton->ChangeBackground(GetDefaultFrameBackground()); // 
						fTextEntryModuleIpString->ChangeBackground(GetDefaultFrameBackground());  
						tstring_temp = fTextEntryModuleIpString->GetText();
						strcpy(gl_sis3316_ip_addr_string,tstring_temp) ; // SIS3316 IP address

						return_code = gl_vme_crate->set_UdpSocketSIS3316_IpAddress( gl_sis3316_ip_addr_string);
						gl_vme_crate->udp_reset_cmd();
						gl_module_base_addr = 0x0 ;

						// Set  Receive Timeout
						//gl_vme_crate->recv_timeout_sec  = 0 ;
						//gl_vme_crate->recv_timeout_usec = 10000 ; // default 10ms
						//gl_vme_crate->set_UdpSocketOptionTimeout() ;

						fMenuSIS3316Test->DisableEntry(M_SIS3316TEST1_DLG);
						fMenuFpgaProgram->DisableEntry(M_FPGA_PROG_MENUE);
	
						return_code = this->ShowModuleInformation();
						if (return_code == 0) {
							// kill request and grant from vme interface
							gl_vme_crate->vme_A32D32_write(gl_module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x80000000);
							// arbitrate
							gl_vme_crate->vme_A32D32_write(gl_module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 1);
							if (gl_sis3316_adc1 == NULL) {
								delete gl_sis3316_adc1;
							}
							sis3316_adc1 = new sis3316_adc( gl_vme_crate, gl_module_base_addr);
							gl_sis3316_adc1 = sis3316_adc1 ;
							if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
								sprintf(s,"SIS3316-125MHz-16bit");
							}
							else {
								sprintf(s,"SIS3316-250MHz-14bit");
							}
							fLabel_main_frameh2[5]->SetText(s);
							fMenuSIS3316Test->EnableEntry(M_SIS3316TEST1_DLG);
							fMenuFpgaProgram->EnableEntry(M_FPGA_PROG_MENUE);

						};
						// Set  Receive Timeout
						//gl_vme_crate->recv_timeout_sec  = 0 ;
						//gl_vme_crate->recv_timeout_usec = 50000 ; // default 50ms
						//printf("recv_timeout_sec = %d   recv_timeout_usec = %d\n",this->recv_timeout_sec, this->recv_timeout_usec);
						//gl_vme_crate->set_UdpSocketOptionTimeout() ;

						break;


					default:
						break;

				} //switch(parm1)
			break; //case kCM_BUTTON
		#endif
 /********************************************/

            default:
               break;
         } // kC_COMMAND switch (GET_SUBMSG(msg))
         break;

	  case kC_TEXTENTRY:
			//printf("kC_TEXTENTRY item %ld activated\n", parm1);
              switch (parm1) {
				 case 9:
					#ifdef ETHERNET_VME_INTERFACE
						tstring_temp = fTextEntryModuleIpString->GetText();
						strcpy(gl_sis3153_ip_addr_string,tstring_temp) ; // SIS3316 IP address

						sis3153eth(&gl_vme_crate, gl_sis3153_ip_addr_string);

						// open Vme Interface device
						return_code = gl_vme_crate->vmeopen ();  // open Vme interface
						gl_vme_crate->get_vmeopen_messages (char_messages, &nof_found_devices);  // open Vme interface

						fLabel_main_frameh1[0]->SetText(char_messages);
						sprintf(s,"-- found %d vme interface device(s)",nof_found_devices);
						fLabel_main_frameh1[1]->SetText(s);

						fMenuSIS3316Test->DisableEntry(M_SIS3316TEST1_DLG);

						return_code = this->ShowModuleInformation();
						if (return_code == 0) {
							// kill request and grant from vme interface
							gl_vme_crate->vme_A32D32_write(gl_module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x80000000);
							// arbitrate
							gl_vme_crate->vme_A32D32_write(gl_module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 1);
							if (gl_sis3316_adc1 == NULL) {
								delete gl_sis3316_adc1;
							}
							sis3316_adc1 = new sis3316_adc( gl_vme_crate, gl_module_base_addr);
							gl_sis3316_adc1 = sis3316_adc1 ;
							if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
								sprintf(s,"SIS3316-125MHz-16bit");
							}
							else {
								sprintf(s,"SIS3316-250MHz-14bit");
							}
							fLabel_main_frameh2[5]->SetText(s);
							fMenuSIS3316Test->EnableEntry(M_SIS3316TEST1_DLG);

						};

					#endif


					 break;

	//		fLabel_main_frameh2[0]->SetBackgroundColor(0x99FF99); // light green
	//	fLabel_main_frameh2[0]->SetBackgroundColor(0xFF9999); // light red
	//fTextEntryModuleIpString->ChangeBackground(0x99FF99); //light green


	//fStartB = new TGTextButton(fTGHor_frame1, "\n      &Start Sampling      \n", SIS3316TestDialog_kCM_BUTTON_IRQ_NO_40);
	//fValidateIpAddressButton = new TGTextButton(main_framehsub, "  Validate IP address  ", 10);
	//fValidateIpAddressButton->ChangeBackground(0x99FF99); // light green
	//fValidateIpAddressButton->Associate(this);


				 case 10:
					#ifdef ETHERNET_UDP_INTERFACE
						fTextEntryModuleIpString->ChangeBackground(0xffff00); //light yellow
						fValidateIpAddressButton->ChangeBackground(0x99FF99); // light green
					#else
						gl_module_base_addr =  fNumericEntriesModuleAddress->GetIntNumber() ;
						fMenuSIS3316Test->DisableEntry(M_SIS3316TEST1_DLG);

						return_code = this->ShowModuleInformation();
						if (return_code == 0) {
							// kill request and grant from vme interface
							gl_vme_crate->vme_A32D32_write(gl_module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x80000000);
							// arbitrate
							gl_vme_crate->vme_A32D32_write(gl_module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 1);
							if (gl_sis3316_adc1 == NULL) {
								delete gl_sis3316_adc1;
							}
							sis3316_adc1 = new sis3316_adc( gl_vme_crate, gl_module_base_addr);
							gl_sis3316_adc1 = sis3316_adc1 ;
							if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
								sprintf(s,"SIS3316-125MHz-16bit");
							}
							else {
								sprintf(s,"SIS3316-250MHz-14bit");
							}
							fLabel_main_frameh2[5]->SetText(s);
							fMenuSIS3316Test->EnableEntry(M_SIS3316TEST1_DLG);

						};
					#ifdef ETHERNET_UDP_INTERFACE
						// Set  Receive Timeout
						gl_vme_crate->recv_timeout_sec  = 0 ;
						gl_vme_crate->recv_timeout_usec = 100000 ; // default 100ms
						//printf("recv_timeout_sec = %d   recv_timeout_usec = %d\n",this->recv_timeout_sec, this->recv_timeout_usec);
						gl_vme_crate->set_UdpSocketOptionTimeout() ;
					#endif
					#endif

						break;


				 default:
                     break;
               } // kC_TEXTENTRY switch (parm1)
         break;

 

	  default:
         break;
   } // switch (GET_MSG(msg)
   return kTRUE;
}

#ifdef not_used
	case kC_TEXTENTRY:
      printf("kC_TEXTENTRY item %ld activated\n", parm1);
	  switch (GET_SUBMSG(msg)) {
	      case kTE_TEXTCHANGED:
				switch(parm1) {
					  case 20:
						printf("\n pressed 20\n");
						break;
					  case 21:
						printf("\n pressed 21\n");
						break;
				}
			break;
	  }
	  break;
#endif














/*******************************************************************************************************************************/
/*******************************************************************************************************************************/
/*******************************************************************************************************************************/
/*******************************************************************************************************************************/
/*******************************************************************************************************************************/
/*******************************************************************************************************************************/
/***********************       SIS3316TestDialog                                       *****************************************/
	  
#define SIS3316TestDialog_kCM_BUTTON_IRQ_NO_40			40
#define SIS3316TestDialog_kCM_BUTTON_IRQ_NO_41			41
#define SIS3316TestDialog_kCM_BUTTON_IRQ_NO_42			42

#define SIS3316TestDialog_kCM_CHECKBUTTON_IRQ_NO_10		10
#define SIS3316TestDialog_kCM_CHECKBUTTON_IRQ_NO_11		11
#define SIS3316TestDialog_kCM_CHECKBUTTON_IRQ_NO_12		12

#define SIS3316TestDialog_kCM_COMBOBOX_IRQ_NO_30		30   
#define SIS3316TestDialog_kCM_COMBOBOX_IRQ_NO_90		90   
	  
	  
const char *const SIS3316TestDialog::entrySampleControl_BankModus[2] = {
   "Single Event/Hit - Single Bank Mode",
   "Multi  Event/Hit - Double Bank Mode "
};

const char *const SIS3316TestDialog::entryFP_BUS_ClockOutMux[2] = {
   "Use Internal Programmable Clock Oscillator",
   "Use External NIM Clock (via Clock Multiplier) "
};

const char *const SIS3316TestDialog::entryClock_source[4] = {
   "Use Internal Programmable Clock Oscillator",
   "Use VXS Sample Clock",
   "Use FP-Bus Sample Clock",
   "Use External NIM Clock (via Clock Multiplier) "
};


const char *const SIS3316TestDialog::entryClock_freq[16] = {
   "Set Internal Sample Clock to 250.000 MHz",
   "Set Internal Sample Clock to 227.273 MHz",
   "Set Internal Sample Clock to 208,333 MHz",
   "Set Internal Sample Clock to 178,571 MHz",
   "Set Internal Sample Clock to 166.667 MHz",
   "Set Internal Sample Clock to 138.889 MHz",
   "Set Internal Sample Clock to 125.000 MHz",
   "Set Internal Sample Clock to 119.048 MHz",
   "Set Internal Sample Clock to 113.636 MHz",
   "Set Internal Sample Clock to 104.167 MHz",
   "Set Internal Sample Clock to 100.000 MHz",
   "Set Internal Sample Clock to  83.333 MHz",
   "Set Internal Sample Clock to  71.429 MHz",
   "Set Internal Sample Clock to  62.500 MHz",
   "Set Internal Sample Clock to  50.000 MHz",
   "Set Internal Sample Clock to  25.000 MHz"
};

const char *const SIS3316TestDialog::entryClock_multiplier_modes[7] = {
   "Set Clock Multiplier: Bypass",  //
   "Set Clock Multiplier: 10 MHz to 250 MHz",  // bw=0    n1_hs=5   n1_clk = 4   n2=500  n3=1   range 10,00 to 11,34 -> 250 to 283
   "Set Clock Multiplier: 20 MHz to 250 MHz",  // bw=0/1  n1_hs=5   n1_clk = 4   n2=250  n3=1   range 19,40 to 22,68 -> 242 to 283
   "Set Clock Multiplier: 50 MHz to 250 MHz",  // bw=1/2  n1_hs=11  n1_clk = 1   n2=110  n3=1   range 44,00 to 51,54 -> 220 to 257
   "Set Clock Multiplier: 10 MHz to 125 MHz",  // bw=0    n1_hs=4   n1_clk = 10  n2=500  n3=1   range 10,00 to 11,34 -> 125 to 141
   "Set Clock Multiplier: 20 MHz to 125 MHz",  // bw=0/1  n1_hs=5   n1_clk = 8   n2=250  n3=1   range 19,40 to 22,68 -> 121 to 141
   "Set Clock Multiplier: 50 MHz to 125 MHz"   // bw=1/2  n1_hs=5   n1_clk = 8   n2=100  n3=1   range 48,50 to 56,70 -> 121 to 141
};

const char *const SIS3316TestDialog::entryClock_SelectMAW_TestBuffer[2] = {
   "Trigger MAW",  //
   "Energy  MAW"   //
};



const char *const SIS3316TestDialog::numlabel[8] = {
   "Pre Trigger Delay",
   "Raw Data Sample Start Index",
   "Raw Data Sample Length",
   "MAW Test Buffer Length",
   "MAW Test Buffer Pre Trigger Delay",
    "free",
   "free",
   "free"
};

const Double_t SIS3316TestDialog::numinit[8] = {
   1, 2, 3, 4, 5, 6,
   7,
   8
};


// graph
const char *const SIS3316TestDialog::chkDisAdcLabel[16] = {
   "Ch 1",
   "Ch 2",
   "Ch 3",
   "Ch 4",
   "Ch 5",
   "Ch 6",
   "Ch 7",
   "Ch 8",
   "Ch 9",
   "Ch 10",
   "Ch 11",
   "Ch 12",
   "Ch 13",
   "Ch 14",
   "Ch 15",
   "Ch 16"
};



// MAW (Moving Average Window)
const char *const SIS3316TestDialog::entryMawLabel[17] = {
   "No MAW",
   "Display MAW Ch 1",
   "Display MAW Ch 2",
   "Display MAW Ch 3",
   "Display MAW Ch 4",
   "Display MAW Ch 5",
   "Display MAW Ch 6",
   "Display MAW Ch 7",
   "Display MAW Ch 8",
   "Display MAW Ch 9",
   "Display MAW Ch 10",
   "Display MAW Ch 11",
   "Display MAW Ch 12",
   "Display MAW Ch 13",
   "Display MAW Ch 14",
   "Display MAW Ch 15",
   "Display MAW Ch 16"
};

// graph
const char *const SIS3316TestDialog::AdcHistogramLabel[16] = {
   "Histogram Ch 1",
   "Histogram Ch 2",
   "Histogram Ch 3",
   "Histogram Ch 4",
   "Histogram Ch 5",
   "Histogram Ch 6",
   "Histogram Ch 7",
   "Histogram Ch 8",
   "Histogram Ch 9",
   "Histogram Ch 10",
   "Histogram Ch 11",
   "Histogram Ch 12",
   "Histogram Ch 13",
   "Histogram Ch 14",
   "Histogram Ch 15",
   "Histogram Ch 16"
};




// histogram build option
const char *const SIS3316TestDialog::entryHistoDisplayOption[3] = {
   "Display ADC Output Code Histogram",
    "reserved for Display FIR Energy Histogram",
	"reserved"
//   "Read internal FIR Energy Histogram"
};


// histogram channel
const char *const SIS3316TestDialog::entryHistoLabel[18] = {
   "No Histogramming",
   "Display Histogram Ch 1",
   "Display Histogram Ch 2",
   "Display Histogram Ch 3",
   "Display Histogram Ch 4",
   "Display Histogram Ch 5",
   "Display Histogram Ch 6",
   "Display Histogram Ch 7",
   "Display Histogram Ch 8",
   "Display Histogram Ch 9",
   "Display Histogram Ch 10",
   "Display Histogram Ch 11",
   "Display Histogram Ch 12",
   "Display Histogram Ch 13",
   "Display Histogram Ch 14",
   "Display Histogram Ch 15",
   "Display Histogram Ch 16",
   "Display Histogram Ch 1-16"
};


const char *const SIS3316TestDialog::entryDisplayFFTLabel[17] = {
   "No FFT",
   "Display FFT Ch 1",
   "Display FFT Ch 2",
   "Display FFT Ch 3",
   "Display FFT Ch 4",
   "Display FFT Ch 5",
   "Display FFT Ch 6",
   "Display FFT Ch 7",
   "Display FFT Ch 8",
   "Display FFT Ch 9",
   "Display FFT Ch 10",
   "Display FFT Ch 11",
   "Display FFT Ch 12",
   "Display FFT Ch 13",
   "Display FFT Ch 14",
   "Display FFT Ch 15",
   "Display FFT Ch 16"
};


const char *const SIS3316TestDialog::entryDisplayFFTWindowLabel[6] = {
   "Rectangular window (no window)",
   "Hamming window",
   "Hann window",
   "Blackmann window",
   "Blackmann-Harris window",
   "No"
};


const char *const SIS3316TestDialog::accuStartIndexlabel[6] = {
   "Start Index of Accumulator 1",
   "Start Index of Accumulator 2",
   "Start Index of Accumulator 3",
   "Start Index of Accumulator 4",
   "Start Index of Accumulator 5",
   "Start Index of Accumulator 6"
};

const char *const SIS3316TestDialog::accuLengthlabel[6] = {
   "Length of Accumulator 1",
   "Length of Accumulator 2",
   "Length of Accumulator 3",
   "Length of Accumulator 4",
   "Length of Accumulator 5",
   "Length of Accumulator 6"
};


const char *const SIS3316TestDialog::entryADC_SPI_InputVoltage[3] = {
   "ADC chip full scale 1.50V input range",
   "ADC chip full scale 1.75V input range",
   "ADC chip full scale 2.00V input range"
};



// graph
const char *const SIS3316TestDialog::chkTriggerEnableChLabel[16+4] = {
   "Ch 1",
   "Ch 2",
   "Ch 3",
   "Ch 4",
   "Ch 5",
   "Ch 6",
   "Ch 7",
   "Ch 8",
   "Ch 9",
   "Ch 10",
   "Ch 11",
   "Ch 12",
   "Ch 13",
   "Ch 14",
   "Ch 15",
   "Ch 16",
   "Ch_Sum 1 to 4",
   "Ch_Sum 5 to 8",
   "Ch_Sum 9 to 12",
   "Ch_Sum 13 to 16"
};

const char *const SIS3316TestDialog::SIS3316_devices_txt[6] = {
   "/dev/SIS3316-0",
   "/dev/SIS3316-1",
   "/dev/SIS3316-2",
   "/dev/SIS3316-3",
   "/dev/SIS3316-4",
   "/dev/SIS3316-5",
};


/*******************************************************************************************************************************/
/*******************************************************************************************************************************/
// open Test dialog
SIS3316TestDialog::SIS3316TestDialog(const TGWindow *p, const TGWindow *main, UInt_t w,  UInt_t h, Bool_t *b,UInt_t options)
					   : TGTransientFrame(p, main, w, h, options)
{
  int i;
  int SIS3316_device ;
  unsigned int return_code ;
  unsigned int data;
  unsigned int module_base_addr   ;
  char s[64];



   SetCleanup(kDeepCleanup);
	fBSetup = b;
	*fBSetup = kTRUE;

	fB_openfCanvas1WindowFlag = kFALSE; // Setup
	fB_openfCanvas2WindowFlag = kFALSE; // Setup
	fB_openfCanvas3WindowFlag = kFALSE; // Setup
	fB_openfCanvas4WindowFlag = kFALSE; // Setup


	module_base_addr = gl_module_base_addr ;
	return_code = gl_sis3316_adc1->register_read( SIS3316_MODID, &data);
	if (return_code == 0) {
		printf("SIS3316_MODID                    = 0x%08x\n\n", data);

		gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits
		gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits
		gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits
		gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits


		gl_sis3316_adc1->register_read( SIS3316_ADC_CH1_4_FIRMWARE_REG, &data);
		printf("SIS3316_ADC_CH1_4_FIRMWARE_REG   = 0x%08x \n", data);
		gl_sis3316_adc1->register_read( SIS3316_ADC_CH5_8_FIRMWARE_REG, &data);
		printf("SIS3316_ADC_CH5_8_FIRMWARE_REG   = 0x%08x \n", data);
		gl_sis3316_adc1->register_read( SIS3316_ADC_CH9_12_FIRMWARE_REG, &data);
		printf("SIS3316_ADC_CH9_12_FIRMWARE_REG  = 0x%08x \n", data);
		gl_sis3316_adc1->register_read( SIS3316_ADC_CH13_16_FIRMWARE_REG, &data);
		printf("SIS3316_ADC_CH13_16_FIRMWARE_REG = 0x%08x \n\n", data);

		gl_sis3316_adc1->register_read( SIS3316_ADC_CH1_4_STATUS_REG, &data);
		printf("SIS3316_ADC_CH1_4_STATUS_REG     = 0x%08x \n", data);
		gl_sis3316_adc1->register_read( SIS3316_ADC_CH5_8_STATUS_REG, &data);
		printf("SIS3316_ADC_CH5_8_STATUS_REG     = 0x%08x \n", data);
		gl_sis3316_adc1->register_read( SIS3316_ADC_CH9_12_STATUS_REG, &data);
		printf("SIS3316_ADC_CH9_12_STATUS_REG    = 0x%08x \n", data);
		gl_sis3316_adc1->register_read( SIS3316_ADC_CH13_16_STATUS_REG, &data);
		printf("SIS3316_ADC_CH13_16_STATUS_REG   = 0x%08x \n\n", data);

	}
	else {
		printf("SIS3316_MODID                  = 0x%08x     return_code = 0x%08x\n", data, return_code);
	}


/************************************************************************************************************************************************************/



   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   fMenuFile = new TGPopupMenu(fClient->GetRoot());
   fMenuFile->AddEntry("E&xit/Close/Quit", TEST1_FILE_EXIT);
   fMenuFile->AddSeparator(); 


// add SIS3316
   // fMenuSIS3316Test = new TGPopupMenu(gClient->GetDefaultRoot());
   //fMenuSIS3316Test->AddLabel("SIS3316 Test");
   //fMenuSIS3316Test->AddSeparator();
   //fMenuSIS3316Test->AddEntry("&Test 1", M_SIS3316TEST1_DLG);
   //fMenuSIS3316Test->DisableEntry(M_SIS3316TEST1_DLG);

  // fMenuHelp = new TGPopupMenu(fClient->GetRoot());
   fMenuHelp = new TGPopupMenu(gClient->GetDefaultRoot());
   //fMenuHelp->AddSeparator();
   fMenuHelp->AddEntry("&About", TEST1_HELP_ABOUT);

   // Menu button messages are handled by the main frame (i.e. "this")
   // ProcessMessage() method.
   fMenuFile->Associate(this);
 
   fMenuBar = new TGMenuBar(this);
   //fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   //fMenuDock->AddFrame(fMenuBar, fMenuBarLayout);
	this->AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
/************************************************************************************************************************************************************/



#ifdef not_used
  i_device = 0;
  do {
    SIS3316_device = open(SIS3316_devices_txt[i_device], O_RDWR);
    gl_class_SIS3316_device = SIS3316_device ;
    i_device++;
    printf("i_device %d \n",i_device);
  } while (gl_class_SIS3316_device >= 0) ;
  printf("\nnumber of SIS3316 device(s) %d \n",i_device-1);

  // open 1. SIS3316
  SIS3316_device = open(SIS3316_devices_txt[0], O_RDWR);
  gl_class_SIS3316_device = SIS3316_device ;
#endif

  // Create a dialog window. A dialog window pops up with respect to its
   // "main" window.
   Pixel_t yellow;
   fClient->GetColorByName("yellow", yellow);
   Pixel_t green;
   fClient->GetColorByName("green", green);
   Pixel_t red;
   fClient->GetColorByName("red", red);

   // use hierarchical cleani
   TGGC myGC = *fClient->GetResourcePool()->GetFrameGC();
   TGFont *myfont = fClient->GetFont("-adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
   if (myfont) myGC.SetFont(myfont->GetFontHandle());
//printf("\nSIS3316TestDialog: SetCleanup \n");

  // fFrame1 = new TGHorizontalFrame(this, 160, 20, kFixedWidth);
   fFrame1 = new TGHorizontalFrame(this, 600, 20, kFixedWidth);


	fGrp1 = new TGGroupFrame(fFrame1, "Control");
	fFrame1->AddFrame(fGrp1, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));

	fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
	fTGHor_frame1 = new TGHorizontalFrame(fGrp1, 260, 220, kHorizontalFrame);
	fGrp1->AddFrame(fTGHor_frame1, fL3);


	this->AddFrame(fFrame1, new TGLayoutHints(kLHintsTop | kLHintsLeft ,  2, 2, 2, 2));


   fFrame2 = new TGHorizontalFrame(this, 160, 20, kFixedWidth);
   fQuitButton = new TGTextButton(fFrame2, "&Quit",1);
   fQuitButton->Associate(this);
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY,  2, 2, 2, 2);
   fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);
   fL7 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 22, 2); // hints, left, right, top, bottom

   fFrame2->AddFrame(fQuitButton, fL1);
   fFrame2->Resize(150, fQuitButton->GetDefaultHeight()); // quit button breite
   AddFrame(fFrame2, fL2);



	fStartB = new TGTextButton(fTGHor_frame1, "\n      &Start Sampling      \n", SIS3316TestDialog_kCM_BUTTON_IRQ_NO_40);
	fStopB  = new TGTextButton(fTGHor_frame1, "\n      &Stop Sampling       \n", SIS3316TestDialog_kCM_BUTTON_IRQ_NO_41);
	fStartB->ChangeBackground(green);
	fStopB->ChangeBackground(red);
	//fStopB->ChangeBackground(0xFF9999);
	//fStartB->Resize(200, 50);
	fStartB->Associate(this);
	fStopB->Associate(this);
	fTGHor_frame1->AddFrame(fStartB, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY,  2, 5, 5, 5));// hints, left, right, top, bottom
	fTGHor_frame1->AddFrame(fStopB, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY,  5, 2, 5, 5));



   fSIS3316_Test1 = kFALSE;
   for (i=0;i<17;i++) {
      fGraph_ch[i]   = 0;
   }
   for (i=0;i<16;i++) {
      iHistoAdc[i]   = 0;
   }
   for (i=0;i<16;i++) {
      histo_pave_text[i]   = 0;
   }
   fGraph_fft[0]   = 0;


   //--------- create Tab widget and some composite frames for Tab testing



#define scroll
#ifdef scroll
	TGCanvas *fCanvasScroll = new TGCanvas(this);
	TGViewPort *fViewPortScroll =  fCanvasScroll->GetViewPort();
	fTab = new TGTab(fViewPortScroll, 300, 300);

	tab_color_not_active = 0xFFFFFF ;
    tab_color_active     = 0xE0E0E0 ;

#endif

#ifndef scroll
   fTab = new TGTab(this, 300, 300);
#endif
   fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
//   fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 6, 6, 6, 6);


/**********************************************************************************************************/
/**********************************************************************************************************/

// Tab1

	fTab->SetEnabled(0, kTRUE);
	fTab->Associate(this);

  
	TGCompositeFrame *tf = fTab->AddTab("Sample Control");
	tabel_tab[0] = fTab->GetTabTab("Sample Control");
	tabel_tab[0]->ChangeBackground(tab_color_active);
	this->sis3316Test1_nof_valid_tabel_tabs = 1 ;	
	fF_tab1 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);

	fF_tab1_fGrp1 = new TGGroupFrame(fF_tab1, "Run Control");
	fF_tab1->AddFrame(fF_tab1_fGrp1, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 3));

	fF1 = new TGCompositeFrame(fF_tab1_fGrp1, 260, 220, kHorizontalFrame);
	fF_tab1_fGrp1->AddFrame(fF1, fL3);

	fF1A = new TGCompositeFrame(fF_tab1_fGrp1, 260, 220, kHorizontalFrame);
	fF_tab1_fGrp1->AddFrame(fF1A, fL3);

	fChkStopAfterBanks = new TGCheckButton(fF1A, "Stop after Bank Loops", SIS3316TestDialog_kCM_CHECKBUTTON_IRQ_NO_10);
	fChkStopAfterBanks->Associate(this);
	fF1A->AddFrame(fChkStopAfterBanks, new TGLayoutHints(kLHintsTop | kLHintsLeft ,  2, 15, 3, 1));// hints, left, right, top, bottom

	fNumericEntriesStopAfterBanks = new TGNumberEntry(fF1A, 20 /* value */, 8 /* width */, 90 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
	//fNumericEntriesStopAfterBanks->Associate(this);
	fF1A->AddFrame(fNumericEntriesStopAfterBanks, new TGLayoutHints(kLHintsTop | kLHintsRight, 21, 2, 2, 2));
	//fLabel[1] = new TGLabel(fF[1], "Energy Gap", myGC(), myfont->GetFontStruct());
   //fF[1]->AddFrame(fLabel[1], fL5);

	fChkStopAfterBanks->SetState(kButtonUp)   ; // is OFF !
	fNumericEntriesStopAfterBanks->SetState(kFALSE); //

	fNumericEntriesStopAfterBanks->SetLimits((TGNumberFormat::kNELLimitMinMax), 1, 1000000);
	fNumericEntriesStopAfterBanks->SetIntNumber(1); // 

	//fChkChannelInputInvert->SetEnabled(dim_state); //


	// Trigger FIR Filter 


 	//			fLabel_main_frameh2[0]->SetBackgroundColor(0xFF9999); // light red
 
/**********************/
	fF_tab1_fGrp1B = new TGGroupFrame(fF_tab1, "Write Data to File");
	fF_tab1->AddFrame(fF_tab1_fGrp1B, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 3));

	fF1B = new TGCompositeFrame(fF_tab1_fGrp1B, 260, 220, kHorizontalFrame);
	fF_tab1_fGrp1B->AddFrame(fF1B, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));

	fChkWriteDataToFile = new TGCheckButton(fF1B, "Write Data to File enable", SIS3316TestDialog_kCM_CHECKBUTTON_IRQ_NO_12);
	fChkWriteDataToFile->Associate(this);
	fF1B->AddFrame(fChkWriteDataToFile, new TGLayoutHints(kLHintsTop | kLHintsLeft ,  2, 2, 5, 2));// hints, left, right, top, bottom

	fChkWriteMultipleFiles = new TGCheckButton(fF1B, "Write multiple Files", 0);
	//fChkWriteMultipleFiles->Associate(this);
	fF1B->AddFrame(fChkWriteMultipleFiles, new TGLayoutHints(kLHintsTop | kLHintsLeft ,  5, 2, 5, 2));// hints, left, right, top, bottom



	fF1C = new TGCompositeFrame(fF_tab1_fGrp1B, 260, 220, kHorizontalFrame);
	fF_tab1_fGrp1B->AddFrame(fF1C, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));

	fTextEntryDataFilePath = new TGTextEntry(fF1C, "sis3316_data_file.dat");
	fF1C->AddFrame(fTextEntryDataFilePath, new TGLayoutHints(kLHintsNormal, 5, 5, 5, 5));
	fTextEntryDataFilePath->SetWidth(250);
	
	fTextButtonDataFilePath = new TGTextButton(fF1C, "...", SIS3316TestDialog_kCM_BUTTON_IRQ_NO_42);
	fTextButtonDataFilePath->ChangeOptions(fTextButtonDataFilePath->GetOptions() | kFixedWidth);
	fTextButtonDataFilePath->SetWidth(50);
	fTextButtonDataFilePath->Associate(this);
	fF1C->AddFrame(fTextButtonDataFilePath, new TGLayoutHints(kLHintsNormal, 5, 5, 5, 5));

	fTextEntryDataFilePath->SetText("sis3316_test_data.dat");





// -----

	fF_tab1_fGrp1A = new TGGroupFrame(fF_tab1, "Sample Parameter");
	fF_tab1->AddFrame(fF_tab1_fGrp1A, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 3));

	fF4A = new TGCompositeFrame(fF_tab1_fGrp1A, 60, 20, kHorizontalFrame);
	//fF4A = new TGCompositeFrame(fF_tab1_fGrp1A, 60, 20, kVerticalFrame);
	fF_tab1_fGrp1A->AddFrame(fF4A, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));



	fCombo_SampleControl_BankModus = new TGComboBox(fF4A, SIS3316TestDialog_kCM_COMBOBOX_IRQ_NO_30);
	fCombo_SampleControl_BankModus->Associate(this); // Event (IRQ) anmelden
	fF4A->AddFrame(fCombo_SampleControl_BankModus, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5));

	for (i = 0; i < 2; i++) {
		fCombo_SampleControl_BankModus->AddEntry(entrySampleControl_BankModus[i], i);
	}
	fCombo_SampleControl_BankModus->Resize(270, 25);

	//   TGHorizontalFrame    *fTGHorizontalFrame;
   //TGVerticalFrame		*fTGVerticalFrame;


	fChkNofEvents_ProBank = new TGCheckButton(fF_tab1_fGrp1A, "use max number of Events", SIS3316TestDialog_kCM_CHECKBUTTON_IRQ_NO_11);
	fChkNofEvents_ProBank->Associate(this);
	fF_tab1_fGrp1A->AddFrame(fChkNofEvents_ProBank, new TGLayoutHints(kLHintsTop | kLHintsLeft ,  5, 15, 4, 2));// hints, left, right, top, bottom
	
    fTGHorizontalFrame = new TGHorizontalFrame(fF_tab1_fGrp1A, 200, 30);
	fF_tab1_fGrp1A->AddFrame(fTGHorizontalFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));

	fNumericEntries_SampleControl_MaxNofEvents_ProBank = new TGNumberEntry(fTGHorizontalFrame, 1 /* value */, 12 /* width */,0, (TGNumberFormat::kNESInteger) ) ; //kNESHex
    //fNumericEntries_SampleControl_NofEvents_ProBank[i]->Associate(this);
    fTGHorizontalFrame->AddFrame(fNumericEntries_SampleControl_MaxNofEvents_ProBank, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
    fLabel[0] = new TGLabel(fTGHorizontalFrame, "max Number of Events per Bank", myGC(), myfont->GetFontStruct());
    fTGHorizontalFrame->AddFrame(fLabel[0], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5));
	fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetLimits((TGNumberFormat::kNELLimitMinMax), 1, 1000000);
 
	//delete(fTGHorizontalFrame) ;
    fTGHorizontalFrame = new TGHorizontalFrame(fF_tab1_fGrp1A, 200, 30);
	fF_tab1_fGrp1A->AddFrame(fTGHorizontalFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));

	fNumericEntries_SampleControl_NofEvents_ProBank = new TGNumberEntry(fTGHorizontalFrame, 1 /* value */, 12 /* width */,0, (TGNumberFormat::kNESInteger) ) ; //kNESHex
    //fNumericEntries_SampleControl_NofEvents_ProBank[i]->Associate(this);
    fTGHorizontalFrame->AddFrame(fNumericEntries_SampleControl_NofEvents_ProBank, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
    fLabel[0] = new TGLabel(fTGHorizontalFrame, "Number of Events pro Bank", myGC(), myfont->GetFontStruct());
    fTGHorizontalFrame->AddFrame(fLabel[0], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5));

	fCombo_SampleControl_BankModus->Select(0, kTRUE); //  Single

	fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetIntNumber(1); // 
	fNumericEntries_SampleControl_NofEvents_ProBank->SetIntNumber(1); // 
	fChkNofEvents_ProBank->SetState(kButtonUp)   ; // is Off!
	fNumericEntries_SampleControl_NofEvents_ProBank->SetState(kFALSE); //

	fChkWriteDataToFile->SetState(kButtonUp)   ; // is OFF !
	fChkWriteMultipleFiles->SetState(kButtonDown)   ; // is On !

	fChkWriteMultipleFiles->SetEnabled(kFALSE);
	fTextEntryDataFilePath->SetEnabled(kFALSE);
	fTextButtonDataFilePath->SetEnabled(kFALSE);

	if (fCombo_SampleControl_BankModus->GetSelected() == 0) {
		fChkWriteDataToFile->SetEnabled(kFALSE);
		fChkNofEvents_ProBank->SetEnabled(kFALSE)   ; //
		fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetState(kFALSE); //
		fNumericEntries_SampleControl_NofEvents_ProBank->SetIntNumber(1); // 
	}
	else {
		fChkWriteDataToFile->SetEnabled(kTRUE);
		fChkNofEvents_ProBank->SetEnabled(kTRUE)   ; //
		if (fChkNofEvents_ProBank->IsOn() == kTRUE)  {
			fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetState(kTRUE); //
		}
		else {
			fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetState(kFALSE); //
		}
	}






/************************************************************************************************************************************/
	fF_tab1_fGrp3 = new TGGroupFrame(fF_tab1, "Event/Hit Parameter");
	fF_tab1->AddFrame(fF_tab1_fGrp3, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 3));
	fF5A = new TGCompositeFrame(fF_tab1_fGrp3, 60, 20, kVerticalFrame);
	fF_tab1_fGrp3->AddFrame(fF5A, fL3);

	fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2);

	i = 0 ; // Pretrigger Delay
    fF[i] = new TGHorizontalFrame(fF5A, 200, 30);
    fF5A->AddFrame(fF[i], fL5);
    fNumericEntries_EventHitParameter[i] = new TGNumberEntry(fF[i], numinit[i], 12, i + 20, (TGNumberFormat::kNESInteger) ) ; //kNESHex
    //fNumericEntries_EventHitParameter[i]->Associate(this);
    fF[i]->AddFrame(fNumericEntries_EventHitParameter[i], fL5);
    fLabel[i] = new TGLabel(fF[i], numlabel[i], myGC(), myfont->GetFontStruct());
    fF[i]->AddFrame(fLabel[i], fL5);

	i = 1 ; // Sample Start Index
    fF[i] = new TGHorizontalFrame(fF5A, 200, 30);
    fF5A->AddFrame(fF[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 15));// hints, left, right, top, bottom
    fNumericEntries_EventHitParameter[i] = new TGNumberEntry(fF[i], numinit[i], 12, i + 20, (TGNumberFormat::kNESInteger) ) ; //kNESHex
    //fNumericEntries_EventHitParameter[i]->Associate(this);
    fF[i]->AddFrame(fNumericEntries_EventHitParameter[i], fL5);
    fLabel[i] = new TGLabel(fF[i], numlabel[i], myGC(), myfont->GetFontStruct());
    fF[i]->AddFrame(fLabel[i],  fL5);

	i = 2 ; //  Sample Length
    fF[i] = new TGHorizontalFrame(fF5A, 200, 30);
    fF5A->AddFrame(fF[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 15));// hints, left, right, top, bottom
    fNumericEntries_EventHitParameter[i] = new TGNumberEntry(fF[i], numinit[i], 12, i + 20, (TGNumberFormat::kNESInteger) ) ; //kNESHex
    //fNumericEntries_EventHitParameter[i]->Associate(this);
    fF[i]->AddFrame(fNumericEntries_EventHitParameter[i], fL5);
    fLabel[i] = new TGLabel(fF[i], numlabel[i], myGC(), myfont->GetFontStruct());
    fF[i]->AddFrame(fLabel[i],  fL5);



// MAW Test Buffer

	fCombo_SetSelectMAW_TestBuffer = new TGComboBox(fF5A, 28);
	//fCombo_SetClockMultiplierMode->Associate(this); // Event (IRQ) anmelden
	fF5A->AddFrame(fCombo_SetSelectMAW_TestBuffer, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));// hints, left, right, top, bottom

	for (i = 0; i < 2; i++) {
		fCombo_SetSelectMAW_TestBuffer->AddEntry(entryClock_SelectMAW_TestBuffer[i], i);
	}
	fCombo_SetSelectMAW_TestBuffer->Select(0, kTRUE); //  Trigger
	fCombo_SetSelectMAW_TestBuffer->Resize(270, 25);


	i = 3 ; // MAW TestBuffer Length
    fF[i] = new TGHorizontalFrame(fF5A, 200, 30);
    fF5A->AddFrame(fF[i], fL5);
    fNumericEntries_EventHitParameter[i] = new TGNumberEntry(fF[i], numinit[i], 12, i + 20, (TGNumberFormat::kNESInteger) ) ; //kNESHex
    //fNumericEntries_EventHitParameter[i]->Associate(this);
    fF[i]->AddFrame(fNumericEntries_EventHitParameter[i], fL5);
    fLabel[i] = new TGLabel(fF[i], numlabel[i], myGC(), myfont->GetFontStruct());
    fF[i]->AddFrame(fLabel[i], fL5);

	i = 4 ; //  MAW TestBuffer Pretrigger Delay
    fF[i] = new TGHorizontalFrame(fF5A, 200, 30);
    fF5A->AddFrame(fF[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 15));// hints, left, right, top, bottom
    fNumericEntries_EventHitParameter[i] = new TGNumberEntry(fF[i], numinit[i], 12, i + 20, (TGNumberFormat::kNESInteger) ) ; //kNESHex
    //fNumericEntries_EventHitParameter[i]->Associate(this);
    fF[i]->AddFrame(fNumericEntries_EventHitParameter[i], fL5);
    fLabel[i] = new TGLabel(fF[i], numlabel[i], myGC(), myfont->GetFontStruct());
    fF[i]->AddFrame(fLabel[i],  fL5);


   fChk_EventHitParameter_DataFormatBit0 = new TGCheckButton(fF5A, "DataFormat: Peak-Height / 6xAccumulators enable", 0);
   fChk_EventHitParameter_DataFormatBit0->SetState(kButtonDown)   ; // is ON !
   fF5A->AddFrame(fChk_EventHitParameter_DataFormatBit0, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 10, 2));// hints, left, right, top, bottom

   fChk_EventHitParameter_DataFormatBit2 = new TGCheckButton(fF5A, "DataFormat: 3xTrigger MAW values enable", 0);
   fChk_EventHitParameter_DataFormatBit2->SetState(kButtonUp)   ; // is Off!
   fF5A->AddFrame(fChk_EventHitParameter_DataFormatBit2, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 2, 2));// hints, left, right, top, bottom

   fChk_EventHitParameter_DataFormatBit3 = new TGCheckButton(fF5A, "DataFormat: Energy FIR Filter values enable", 0);
   fChk_EventHitParameter_DataFormatBit3->SetState(kButtonDown)   ; // is ON !
   fF5A->AddFrame(fChk_EventHitParameter_DataFormatBit3, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 2, 5));// hints, left, right, top, bottom



   // start values

	i = 0 ; // Pretrigger Delay
	fNumericEntries_EventHitParameter[i]->SetLimits((TGNumberFormat::kNELLimitMinMax), 0, MAX_PRETRIGGER_DELAY);
	fNumericEntries_EventHitParameter[i]->SetIntNumber(64); // pretrigger delay

	i = 1 ; // Sample Start Index
	fNumericEntries_EventHitParameter[i]->SetLimits((TGNumberFormat::kNELLimitMinMax), 0, 65534);
	fNumericEntries_EventHitParameter[i]->SetIntNumber(0); // Sample Start Index


 	i = 2 ; //  Sample Length
	fNumericEntries_EventHitParameter[i]->SetLimits((TGNumberFormat::kNELLimitMinMax), 0, 65534);
	fNumericEntries_EventHitParameter[i]->SetIntNumber(1024); // sample_length



	i = 3 ; // MAW TestBuffer Length
	fNumericEntries_EventHitParameter[i]->SetLimits((TGNumberFormat::kNELLimitMinMax), 0, 1022);
	fNumericEntries_EventHitParameter[i]->SetIntNumber(1000); // Test MAW Length

	i = 4 ; //  MAW TestBuffer Pretrigger Delay
	fNumericEntries_EventHitParameter[i]->SetLimits((TGNumberFormat::kNELLimitMinMax), 0, 1022);
	fNumericEntries_EventHitParameter[i]->SetIntNumber(100); // Test MAW Pretrigger Delay



/**********************/

	fF_tab1_fGrp4 = new TGGroupFrame(fF_tab1, "Sample Trigger Condition");
	fF_tab1->AddFrame(fF_tab1_fGrp4, new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 3));
	fF5B = new TGCompositeFrame(fF_tab1_fGrp4, 60, 20, kVerticalFrame);
	fF_tab1_fGrp4->AddFrame(fF5B, fL3);


   //fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 2, 2);
   fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 10, 2); // hints, left, right, top, bottom

   fChkKeyTrigger = new TGCheckButton(fF5B, "Software: Key-Trigger enable", 0);
   fChkKeyTrigger->SetState(kButtonDown)   ; // is ON !
   fF5B->AddFrame(fChkKeyTrigger, fL5);

   fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 2, 2); // hints, left, right, top, bottom

   fChkLemoInTiEnable = new TGCheckButton(fF5B, "VME FPGA: Lemo Input TI use as Trigger", 0);
   fChkLemoInTiEnable->SetState(kButtonUp)   ; // is OFF !
   fF5B->AddFrame(fChkLemoInTiEnable, fL5);



   fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 10, 2); // hints, left, right, top, bottom

   fChkExternalTrigger = new TGCheckButton(fF5B, "ADC FPGAs: External Trigger", 0);
   fChkExternalTrigger->SetState(kButtonDown)   ; // is ON !
   fF5B->AddFrame(fChkExternalTrigger, fL5);

   fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 2, 2); // hints, left, right, top, bottom

   fChkInternalTrigger = new TGCheckButton(fF5B, "ADC FPGAs: Internal Trigger", 0);
   fChkInternalTrigger->SetState(kButtonUp)   ; // is OFF !
   fF5B->AddFrame(fChkInternalTrigger, fL5);

   fChkIntertnalSumTrigger = new TGCheckButton(fF5B, "ADC FPGAs: Internal Sum Trigger", 0);
   fChkIntertnalSumTrigger->SetState(kButtonUp)   ; // is OFF !
   fF5B->AddFrame(fChkIntertnalSumTrigger, fL5);

   fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 10, 2); // hints, left, right, top, bottom

   //fChkChannelInputInvert = new TGCheckButton(fF5B, "ADC FPGAs: Channel Input Invert", 0);
   //fChkChannelInputInvert->SetState(kButtonUp)   ; // is OFF !
   //fF5B->AddFrame(fChkChannelInputInvert, fL5);


	tf->AddFrame(fF_tab1, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));

 /**********************************************************************************************************/
 /**********************************************************************************************************/
 /**********************************************************************************************************/
// Tab 2  : Display Control
	//fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 50, 2); // hints, left, right, top, bottom



	tf = fTab->AddTab("Display Control");
 	tabel_tab[1] = fTab->GetTabTab("Display Control");
	tabel_tab[1]->ChangeBackground(tab_color_not_active);
	this->sis3316Test1_nof_valid_tabel_tabs++ ;	

	fF_tab2 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
	fF_tab2_fGrp1 = new TGGroupFrame(fF_tab2, "Raw data");
	fF_tab2->AddFrame(fF_tab2_fGrp1, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

	fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 2, 2);

//   kLHintsNoHints = 0,
//   kLHintsLeft    = BIT(0),
//   kLHintsCenterX = BIT(1),
//   kLHintsRight   = BIT(2),
//   kLHintsTop     = BIT(3),
//   kLHintsCenterY = BIT(4),
//   kLHintsBottom  = BIT(5),
//   kLHintsExpandX = BIT(6),
//   kLHintsExpandY = BIT(7),
//   kLHintsNormal  = (kLHintsLeft | kLHintsTop)

	fL2 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 2, 2); // hints, left, right, top, bottom
	fF[0] = new TGHorizontalFrame(fF_tab2_fGrp1, 200, 30);
	fF_tab2_fGrp1->AddFrame(fF[0], new TGLayoutHints(kLHintsExpandX, 2, 2, 12, 2));

	fChkDisplayAutoZoom = new TGCheckButton(fF[0], "Y-AutoZoom", 0);
	fChkDisplayAutoZoom->SetState(kButtonDown)   ; // is ON !
	fF[0]->AddFrame(fChkDisplayAutoZoom, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2)); //hints, left, right, top, bottom


	fF[1] = new TGHorizontalFrame(fF_tab2_fGrp1, 200, 30);
	fF_tab2_fGrp1->AddFrame(fF[1], new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 2, 2));
	fNumericEntriesGraph_Yaxis[0] = new TGNumberEntry(fF[1], 0 /* value */, 8 /* width */, 120 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
	//fNumericEntries[i]->Associate(this); // Event (IRQ) anmelden
	fF[1]->AddFrame(fNumericEntriesGraph_Yaxis[0],  new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	fLabel[0] = new TGLabel(fF[1], "Y-max", myGC(), myfont->GetFontStruct());
	fF[1]->AddFrame(fLabel[0],  new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));

	fF[2] = new TGHorizontalFrame(fF_tab2_fGrp1, 200, 30);
	fF_tab2_fGrp1->AddFrame(fF[2], new TGLayoutHints(kLHintsTop | kLHintsLeft ,  2, 2, 2, 10));
	fNumericEntriesGraph_Yaxis[1] = new TGNumberEntry(fF[2], 0 /* value */, 8 /* width */, 120 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
	//fNumericEntries[i]->Associate(this); // Event (IRQ) anmelden
	fF[2]->AddFrame(fNumericEntriesGraph_Yaxis[1],  new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	fLabel[1] = new TGLabel(fF[2], "Y-min", myGC(), myfont->GetFontStruct());
	fF[2]->AddFrame(fLabel[1],  new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));

	if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
		fNumericEntriesGraph_Yaxis[0]->SetIntNumber(65536); // Y-max
	}
	else {
		fNumericEntriesGraph_Yaxis[0]->SetIntNumber(16384); // Y-max
	}
	//fNumericEntriesGraph_Yaxis[0]->SetIntNumber(16384); // Y-max
	fNumericEntriesGraph_Yaxis[1]->SetIntNumber(0); // Y-min

	fVF[0] = new TGVerticalFrame(fF_tab2_fGrp1, 200, 30);
	fF_tab2_fGrp1->AddFrame(fVF[0], new TGLayoutHints(kLHintsTop | kLHintsLeft ,  2, 2, 2, 10));

 	fF[3] = new TGHorizontalFrame(fF_tab2_fGrp1, 200, 30);
 	fF_tab2_fGrp1->AddFrame(fF[3], new TGLayoutHints(kLHintsTop | kLHintsLeft ,  2, 2, 2, 10));

 	fVF[3] = new TGVerticalFrame(fF[3], 200, 30);
 	fF[3]->AddFrame(fVF[3], new TGLayoutHints(kLHintsTop | kLHintsLeft ,  2, 2, 2, 10));

 	fVF[4] = new TGVerticalFrame(fF[3], 200, 30);
 	fF[3]->AddFrame(fVF[4], new TGLayoutHints(kLHintsTop | kLHintsLeft ,  2, 2, 2, 10));


   fDisplayEnableCh_Set = new TGTextButton(fVF[3], "&Enable all Channels ", 50);
   fDisplayEnableCh_Clr = new TGTextButton(fVF[4], "&Disable all Channels", 51);
   fDisplayEnableCh_Set->ChangeBackground(yellow);
   fDisplayEnableCh_Clr->ChangeBackground(yellow);
   fDisplayEnableCh_Set->Resize(500, 70);
   fDisplayEnableCh_Clr->Resize(500, 70);

   fDisplayEnableCh_Set->Associate(this);
   fDisplayEnableCh_Clr->Associate(this);

   fVF[3]->AddFrame(fDisplayEnableCh_Set, new TGLayoutHints(kLHintsTop | kLHintsLeft ,  2, 2, 2, 10));
   fVF[4]->AddFrame(fDisplayEnableCh_Clr, new TGLayoutHints(kLHintsTop | kLHintsLeft ,  2, 2, 2, 10));


	for (i = 0; i < 8; i++) {
		fChkDisplayAdc[7-i] = new TGCheckButton(fVF[3], chkDisAdcLabel[7-i], 8-i);
		fChkDisplayAdc[7-i]->SetState(kButtonDown)   ; // is ON !
		fVF[3]->AddFrame(fChkDisplayAdc[7-i], new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY , 2, 2, 1, 1)); // hints, left, right, top, bottom
   }

	for (i = 0; i < 8; i++) {
		fChkDisplayAdc[15-i] = new TGCheckButton(fVF[4], chkDisAdcLabel[15-i], 16-i);
		fChkDisplayAdc[15-i]->SetState(kButtonDown)   ; // is ON !
		fVF[4]->AddFrame(fChkDisplayAdc[15-i], new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY , 2, 2, 1, 1)); // hints, left, right, top, bottom
   }


// ************************************************************************************************************************************************************
// Maw
	fF_tab2_fGrp4 = new TGGroupFrame(fF_tab2, "Display Moving Average Window (Test)");
	fF_tab2->AddFrame(fF_tab2_fGrp4, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

	fCombo_Display_MAW = new TGComboBox(fF_tab2_fGrp4, 90);
	//fCombo_Display_MAW->Associate(this); // Event (IRQ) anmelden
	fF_tab2_fGrp4->AddFrame(fCombo_Display_MAW, new TGLayoutHints(kLHintsLeft ,  5, 2, 15, 2));
	for (i = 0; i < 17; i++) {
		fCombo_Display_MAW->AddEntry(entryMawLabel[i], i);
	}
	fCombo_Display_MAW->Select(1, kTRUE); // display ch1
	fCombo_Display_MAW->Resize(250, 22);


// ************************************************************************************************************************************************************



// fL7 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 22, 2); // hints, left, right, top, bottom

// histograms
	fF_tab2_fGrp2 = new TGGroupFrame(fF_tab2, "Build and Display Histograms");
	fF_tab2->AddFrame(fF_tab2_fGrp2, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

/******************/	

	fCombo_Display_Histos_Build = new TGComboBox(fF_tab2_fGrp2, 88);
	//fCombo_Display_Histos_Ch->Associate(this); // Event (IRQ) anmelden
	fF_tab2_fGrp2->AddFrame(fCombo_Display_Histos_Build, new TGLayoutHints(kLHintsLeft ,  2, 2, 15, 2));

	for (i = 0; i < 3; i++) {
		fCombo_Display_Histos_Build->AddEntry(entryHistoDisplayOption[i], i);
	}
	fCombo_Display_Histos_Build->Select(0, kTRUE); // build ADC output code
	fCombo_Display_Histos_Build->Resize(250, 22);
/******************/
	fCombo_Display_Histos_Ch = new TGComboBox(fF_tab2_fGrp2, 88);
	//fCombo_Display_Histos_Ch->Associate(this); // Event (IRQ) anmelden
	fF_tab2_fGrp2->AddFrame(fCombo_Display_Histos_Ch, new TGLayoutHints(kLHintsLeft ,  2, 2, 2, 2));

	for (i = 0; i < 18; i++) {
		fCombo_Display_Histos_Ch->AddEntry(entryHistoLabel[i], i);
	}
	fCombo_Display_Histos_Ch->Select(1, kTRUE); // display ch1
	fCombo_Display_Histos_Ch->Resize(250, 22);

	fChkHistoSum = new TGCheckButton(fF_tab2_fGrp2, "Histogram Sum", 0);
	fChkHistoSum->SetState(kButtonUp)   ; // is Off !
	fF_tab2_fGrp2->AddFrame(fChkHistoSum, new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));

	fChkHistoZoomMean = new TGCheckButton(fF_tab2_fGrp2, "Histogram Zoom to Mean", 0);
	fChkHistoZoomMean->SetState(kButtonUp)   ; // is Off !
	fF_tab2_fGrp2->AddFrame(fChkHistoZoomMean, fL2);

	fChkHistoGaussFit = new TGCheckButton(fF_tab2_fGrp2, "Histogram Gauss Fit", 0);
	fChkHistoGaussFit->SetState(kButtonUp)   ; // is Off !
	fF_tab2_fGrp2->AddFrame(fChkHistoGaussFit, fL1);




	fF[10] = new TGHorizontalFrame(fF_tab2_fGrp2, 200, 30);
	fF_tab2_fGrp2->AddFrame(fF[10], new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 2, 2));
	fNumericEntriesHistogramXaxisOffset = new TGNumberEntry(fF[10], 0 /* value */, 8 /* width */, 120 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
	//fNumericEntries[i]->Associate(this); // Event (IRQ) anmelden
	fF[10]->AddFrame(fNumericEntriesHistogramXaxisOffset,  new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	fLabel[0] = new TGLabel(fF[10], "X-axis Offset", myGC(), myfont->GetFontStruct());
	fF[10]->AddFrame(fLabel[0],  new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));

	fF[11] = new TGHorizontalFrame(fF_tab2_fGrp2, 200, 30);
	fF_tab2_fGrp2->AddFrame(fF[11], new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 2, 2));
	fNumericEntriesHistogramXaxisDivider = new TGNumberEntry(fF[11], 0 /* value */, 8 /* width */, 120 /* irq */ , (TGNumberFormat::kNESRealThree) ) ; //kNESHex
	//fNumericEntries[i]->Associate(this); // Event (IRQ) anmelden
	fF[11]->AddFrame(fNumericEntriesHistogramXaxisDivider,  new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	fLabel[0] = new TGLabel(fF[11], "X-axis Divider (1/Gain)", myGC(), myfont->GetFontStruct());
	fF[11]->AddFrame(fLabel[0],  new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));

	fNumericEntriesHistogramXaxisOffset->SetIntNumber(0);
	//fNumericEntriesHistogramXaxisDivider->SetIntNumber(1.0);
	fNumericEntriesHistogramXaxisDivider->SetNumber(1.000);
	

/******************************************************************/











// FFT
	fF_tab2_fGrp3 = new TGGroupFrame(fF_tab2, "Display FFT ");
	fF_tab2->AddFrame(fF_tab2_fGrp3, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

	fCombo_Display_FFT_Ch = new TGComboBox(fF_tab2_fGrp3, 89);
	//fCombo_Display_FFT_Ch->Associate(this); // Event (IRQ) anmelden

   //fL7 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 22, 2); // hints, left, right, top, bottom
   fF_tab2_fGrp3->AddFrame(fCombo_Display_FFT_Ch, new TGLayoutHints(kLHintsLeft, 2, 2, 15, 2));

   for (i = 0; i < 17; i++) {
      fCombo_Display_FFT_Ch->AddEntry(entryDisplayFFTLabel[i], i);
   }
   fCombo_Display_FFT_Ch->Select(0, kTRUE); // no display ch
   fCombo_Display_FFT_Ch->Resize(250, 22);


   fCombo_Display_FFT_Window = new TGComboBox(fF_tab2_fGrp3, 90);
   //fCombo_Display_FFT_Window->Associate(this); // Event (IRQ) anmelden

   //fL7 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 22, 2); // hints, left, right, top, bottom
   fF_tab2_fGrp3->AddFrame(fCombo_Display_FFT_Window, new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
   for (i = 0; i < 6; i++) {
      fCombo_Display_FFT_Window->AddEntry(entryDisplayFFTWindowLabel[i], i);
   }
   fCombo_Display_FFT_Window->Select(0, kTRUE); // d
   fCombo_Display_FFT_Window->Resize(250, 22);


   fChkFFT_Sum = new TGCheckButton(fF_tab2_fGrp3, "FFT Spectrum Sum", 100);
   fChkFFT_Sum->Associate(this); // Event (IRQ) anmelden
   fChkFFT_Sum->SetState(kButtonUp)   ; // is Off !
   fChkFFT_Sum->SetEnabled(kFALSE)   ; //
   fF_tab2_fGrp3->AddFrame(fChkFFT_Sum, fL2);

   fChkFFTLogY = new TGCheckButton(fF_tab2_fGrp3, "FFT Display LogY", 101);
   fChkFFTLogY->Associate(this); // Event (IRQ) anmelden
   fChkFFTLogY->SetState(kButtonUp)   ; // is Off !
   fChkFFTLogY->SetEnabled(kFALSE)   ; //
   fF_tab2_fGrp3->AddFrame(fChkFFTLogY, fL2);

   fChkFFT_Db = new TGCheckButton(fF_tab2_fGrp3, "FFT in dB", 102);
   fChkFFT_Db->Associate(this); // Event (IRQ) anmelden
   fChkFFT_Db->SetEnabled(kTRUE)   ; //
   fChkFFT_Db->SetState(kButtonDown)   ; // is On !
   fF_tab2_fGrp3->AddFrame(fChkFFT_Db, fL2);

   fChkFFT_AutoScale = new TGCheckButton(fF_tab2_fGrp3, "FFT Autoscale", 0);
   fChkFFT_AutoScale->SetState(kButtonUp)   ; // is Off !
   fF_tab2_fGrp3->AddFrame(fChkFFT_AutoScale, fL2);


/**********/

 	tf->AddFrame(fF_tab2, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));




/*****************************************************************************************************************************************************/

/**********************************************************************************************************/

	
	
//#ifdef CONSTRUCTION_ZONE	
	// Tab 2b  (Polarity)

   tf = fTab->AddTab("Polarity");
 	tabel_tab[2] = fTab->GetTabTab("Polarity");
	tabel_tab[2]->ChangeBackground(tab_color_not_active);
	this->sis3316Test1_nof_valid_tabel_tabs++ ;	
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft ,  20 ,2, 2, 12); //hints, left, right, top, bottom
   fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 50, 2); // hints, left, right, top, bottom
   fF_tab2b = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);


   fF_tab2b_fGrp1 = new TGGroupFrame(fF_tab2b, "Channel Input Invert ");
   fF_tab2b->AddFrame(fF_tab2b_fGrp1, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

   fL2 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 2, 2); // hints, left, right, top, bottom

   fInvertChannel_Set = new TGTextButton(fF_tab2b_fGrp1, "&Set Invert all Channels", 70);
   fInvertChannel_Clr = new TGTextButton(fF_tab2b_fGrp1, "&Clear Invert all Channels", 71);
   fInvertChannel_Set->ChangeBackground(yellow);
   fInvertChannel_Clr->ChangeBackground(yellow);
   fInvertChannel_Set->Resize(500, 60);
   fInvertChannel_Clr->Resize(500, 60);

   fInvertChannel_Set->Associate(this);
   fInvertChannel_Clr->Associate(this);

   //fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY,  2, 2, 2, 2);
   //fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
   fF_tab2b_fGrp1->AddFrame(fInvertChannel_Set, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY,  2, 2, 12, 5));
   fF_tab2b_fGrp1->AddFrame(fInvertChannel_Clr, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY,  2, 2, 5, 12));


   for (i = 0; i < 16; i++) {
      fChkInvertChannel[15-i] = new TGCheckButton(fF_tab2b_fGrp1, chkTriggerEnableChLabel[15-i], 16-i);
      fChkInvertChannel[15-i]->SetState(kButtonUp)   ; // is OFF !
      fF_tab2b_fGrp1->AddFrame(fChkInvertChannel[15-i], fL2);
   }

   //tf->AddFrame(fF_tab2b_fGrp1, fL3);
//#endif

	tf->AddFrame(fF_tab2b, fL3);


 
   
/*****************************************************************************************************************************************************/
/**********************************************************************************************************/
// Tab 3  (FIR Trigger)

   tf = fTab->AddTab("Trigger");
 	tabel_tab[3] = fTab->GetTabTab("Trigger");
	tabel_tab[3]->ChangeBackground(tab_color_not_active);
	this->sis3316Test1_nof_valid_tabel_tabs++ ;	
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft ,  20 ,2, 2, 12); //hints, left, right, top, bottom
   fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 50, 2); // hints, left, right, top, bottom
   fF_tab3 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);


   fF_tab3_fGrp1 = new TGGroupFrame(fF_tab3, "FIR Filter Trigger Settings for all channels");
   fF_tab3->AddFrame(fF_tab3_fGrp1, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

   fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 2, 2);

   fF[0] = new TGHorizontalFrame(fF_tab3_fGrp1, 200, 30);

   fF_tab3_fGrp1->AddFrame(fF[0], new TGLayoutHints(kLHintsExpandX, 2, 2, 12, 2));
   fNumericEntriesTriggerPulse_length = new TGNumberEntry(fF[0], 4 /* value */, 8 /* width */, 80 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
   //fNumericEntriesTriggerPulse_length->Associate(this);
   fF[0]->AddFrame(fNumericEntriesTriggerPulse_length, fL5);
   fLabel[0] = new TGLabel(fF[0], "Trigger Out Pulse Length", myGC(), myfont->GetFontStruct());
   fF[0]->AddFrame(fLabel[0], fL5);


   fF[1] = new TGHorizontalFrame(fF_tab3_fGrp1, 200, 30);
   fF_tab3_fGrp1->AddFrame(fF[1], fL5);
   fNumericEntriesTriggerGap = new TGNumberEntry(fF[1], 5 /* value */, 8 /* width */, 80 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
   //fNumericEntriesTriggerGap->Associate(this);
   fF[1]->AddFrame(fNumericEntriesTriggerGap, fL5);
   fLabel[1] = new TGLabel(fF[1], "Trigger Gap", myGC(), myfont->GetFontStruct());
   fF[1]->AddFrame(fLabel[1], fL5);


   fF[2] = new TGHorizontalFrame(fF_tab3_fGrp1, 200, 30);
   fF_tab3_fGrp1->AddFrame(fF[2], fL5);
   fNumericEntriesTriggerPeaking = new TGNumberEntry(fF[2], 8 /* value */, 8 /* width */, 80 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
   //fNumericEntriesTriggerPeaking->Associate(this);
   fF[2]->AddFrame(fNumericEntriesTriggerPeaking, fL5);
   fLabel[2] = new TGLabel(fF[2], "Trigger Peaking", myGC(), myfont->GetFontStruct());
   fF[2]->AddFrame(fLabel[2], fL5);


   fF[3] = new TGHorizontalFrame(fF_tab3_fGrp1, 200, 30);
   fF_tab3_fGrp1->AddFrame(fF[3], fL5);
   fNumericEntriesTriggerThreshold = new TGNumberEntry(fF[3], 100 /* value */, 8 /* width */, 80 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
   //fNumericEntriesTriggerThreshold->Associate(this);
   fF[3]->AddFrame(fNumericEntriesTriggerThreshold, fL5);
   fLabel[3] = new TGLabel(fF[3], "Trigger Threshold (adc value !)", myGC(), myfont->GetFontStruct());
   fF[3]->AddFrame(fLabel[3], fL5);

 
   fChkTriggerCfdEnable = new TGCheckButton(fF_tab3_fGrp1, "CFD Enable ", 80);
   fChkTriggerCfdEnable->SetState(kButtonDown)   ; // is ON !
   fF_tab3_fGrp1->AddFrame(fChkTriggerCfdEnable,  new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2,5, 2));// hints, left, right, top, bottom




   fF_tab3_fGrp2 = new TGGroupFrame(fF_tab3, "Trigger Enable ");
	//this->AddFrame(fF_tab3_fGrp1, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
   fF_tab3->AddFrame(fF_tab3_fGrp2, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
   fL2 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 2, 2); // hints, left, right, top, bottom

   fTriggerEnableCh_Set = new TGTextButton(fF_tab3_fGrp2, "&Enable all Channels", 60);
   fTriggerEnableCh_Clr = new TGTextButton(fF_tab3_fGrp2, "&Disable all Channels", 61);
   fTriggerEnableCh_Set->ChangeBackground(yellow);
   fTriggerEnableCh_Clr->ChangeBackground(yellow);
   fTriggerEnableCh_Set->Resize(500, 60);
   fTriggerEnableCh_Clr->Resize(500, 60);

   fTriggerEnableCh_Set->Associate(this);
   fTriggerEnableCh_Clr->Associate(this);

   //fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY,  2, 2, 2, 2);
   //fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
   fF_tab3_fGrp2->AddFrame(fTriggerEnableCh_Set, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY,  2, 2, 12, 5));
   fF_tab3_fGrp2->AddFrame(fTriggerEnableCh_Clr, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY,  2, 2, 5, 12));

   //tf->AddFrame(fF1, fL3);


   for (i = 0; i < 16; i++) {
      fChkTriggerEnableCh[15-i] = new TGCheckButton(fF_tab3_fGrp2, chkTriggerEnableChLabel[15-i], 16-i);
      fChkTriggerEnableCh[15-i]->SetState(kButtonUp)   ; // is OFF !
      fF_tab3_fGrp2->AddFrame(fChkTriggerEnableCh[15-i], fL2);
   }
   for (i = 16; i < 20; i++) {
      fChkTriggerEnableCh[16+(19-i)] = new TGCheckButton(fF_tab3_fGrp2, chkTriggerEnableChLabel[16+(19-i)], 16+(19-i));
      fChkTriggerEnableCh[16+(19-i)]->SetState(kButtonUp)   ; // is OFF !
	  if (i==16) {
		fF_tab3_fGrp2->AddFrame(fChkTriggerEnableCh[16+(19-i)], new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 17, 2));
	  }
	  else {
		fF_tab3_fGrp2->AddFrame(fChkTriggerEnableCh[16+(19-i)], fL2);
	  }
   }

   tf->AddFrame(fF_tab3, fL3);

   
   
 /**********************************************************************************************************/

   
   
   // Tab 3a  (FIR Energy)
   tf = fTab->AddTab("Energy");
  	tabel_tab[4] = fTab->GetTabTab("Energy");
	tabel_tab[4]->ChangeBackground(tab_color_not_active);
	this->sis3316Test1_nof_valid_tabel_tabs++ ;	
  fF_tab3a = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);

   fF_tab3a_fGrp1 = new TGGroupFrame(fF_tab3a, "FIR Filter Energy Settings for all channels");
   fF_tab3a->AddFrame(fF_tab3a_fGrp1, new TGLayoutHints(kLHintsExpandX, 5, 5, 10, 5));

   fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 2, 2);

   fF[0] = new TGHorizontalFrame(fF_tab3a_fGrp1, 200, 30);

   fF_tab3a_fGrp1->AddFrame(fF[0], new TGLayoutHints(kLHintsExpandX, 2, 2, 12, 2));
   fNumericEntriesEnergyPeaking = new TGNumberEntry(fF[0], 50 /* value */, 8 /* width */, 90 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
   //fNumericEntriesEnergyPeaking->Associate(this);
   fF[0]->AddFrame(fNumericEntriesEnergyPeaking, fL5);
   fLabel[0] = new TGLabel(fF[0], "Energy Peaking", myGC(), myfont->GetFontStruct());
   fF[0]->AddFrame(fLabel[0], fL5);

   fF[1] = new TGHorizontalFrame(fF_tab3a_fGrp1, 200, 30);
   fF_tab3a_fGrp1->AddFrame(fF[1], fL5);
   fNumericEntriesEnergyGap = new TGNumberEntry(fF[1], 20 /* value */, 8 /* width */, 90 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
   //fNumericEntriesEnergyGap->Associate(this);
   fF[1]->AddFrame(fNumericEntriesEnergyGap, fL5);
   fLabel[1] = new TGLabel(fF[1], "Energy Gap", myGC(), myfont->GetFontStruct());
   fF[1]->AddFrame(fLabel[1], fL5);

   fF[2] = new TGHorizontalFrame(fF_tab3a_fGrp1, 200, 30);
   fF_tab3a_fGrp1->AddFrame(fF[2], fL5);
   fNumericEntriesEnergyTauTable = new TGNumberEntry(fF[2], 0 /* value */, 8 /* width */, 90 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
   //fNumericEntriesEnergyTauTable->Associate(this);
   fF[2]->AddFrame(fNumericEntriesEnergyTauTable, fL5);
   fLabel[2] = new TGLabel(fF[2], "Energy Dekay Tau table", myGC(), myfont->GetFontStruct());
   fF[2]->AddFrame(fLabel[2], fL5);

   fF[3] = new TGHorizontalFrame(fF_tab3a_fGrp1, 200, 30);
   fF_tab3a_fGrp1->AddFrame(fF[3], fL5);
   fNumericEntriesEnergyTauFactor = new TGNumberEntry(fF[3], 0 /* value */, 8 /* width */, 90 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
   //fNumericEntriesEnergyTauFactor->Associate(this);
   fF[3]->AddFrame(fNumericEntriesEnergyTauFactor, fL5);
   fLabel[3] = new TGLabel(fF[3], "Energy Dekay Tau factor", myGC(), myfont->GetFontStruct());
   fF[3]->AddFrame(fLabel[3], fL5);


   fF[4] = new TGHorizontalFrame(fF_tab3a_fGrp1, 200, 30);
   fF_tab3a_fGrp1->AddFrame(fF[4], fL5);
   fNumericEntriesEnergyAdditionalAverage = new TGNumberEntry(fF[4], 0 /* value */, 8 /* width */, 90 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
   //fNumericEntriesEnergyAdditionalAverage->Associate(this);
   fF[4]->AddFrame(fNumericEntriesEnergyAdditionalAverage, fL5);
   fLabel[4] = new TGLabel(fF[4], "Energy Additional Average factor", myGC(), myfont->GetFontStruct());
   fF[4]->AddFrame(fLabel[4], fL5);


//*********************************

   fF_tab3a_fGrp2 = new TGGroupFrame(fF_tab3a, "Accumulator Settings for all channels");
   fF_tab3a->AddFrame(fF_tab3a_fGrp2, new TGLayoutHints(kLHintsExpandX, 5, 5, 10, 5)); // hints, left, right, top, bottom


   fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 2, 2);
   
	for (i=0;i<6;i++) {
		fTGHorizontalFrame = new TGHorizontalFrame(fF_tab3a_fGrp2, 200, 30);
		fF_tab3a_fGrp2->AddFrame(fTGHorizontalFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 12, 2));
		fNumericEntriesAccumulatorStartIndex[i] = new TGNumberEntry(fTGHorizontalFrame, 0 /* value */, 8 /* width */, 0 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
		//fNumericEntriesAccumulatorStartIndex[i]->Associate(this);
		fTGHorizontalFrame->AddFrame(fNumericEntriesAccumulatorStartIndex[i], fL5);
		fLabel_AccumulatorStartIndex_text[i] = new TGLabel(fTGHorizontalFrame, accuStartIndexlabel[i], myGC(), myfont->GetFontStruct());
		fTGHorizontalFrame->AddFrame(fLabel_AccumulatorStartIndex_text[i], fL5);
	 	fNumericEntriesAccumulatorStartIndex[i]->SetLimits((TGNumberFormat::kNELLimitMinMax), 0, 65535);


		fTGHorizontalFrame = new TGHorizontalFrame(fF_tab3a_fGrp2, 200, 30);
		fF_tab3a_fGrp2->AddFrame(fTGHorizontalFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));
		fNumericEntriesAccumulatorLength[i] = new TGNumberEntry(fTGHorizontalFrame, 10 /* value */, 8 /* width */, 0 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
		//fNumericEntriesAccumulatorLength[i]->Associate(this);
		fTGHorizontalFrame->AddFrame(fNumericEntriesAccumulatorLength[i], fL5);
		fLabel_AccumulatorLength_text[i] = new TGLabel(fTGHorizontalFrame, accuLengthlabel[i], myGC(), myfont->GetFontStruct());
		fTGHorizontalFrame->AddFrame(fLabel_AccumulatorLength_text[i], fL5);
	 	fNumericEntriesAccumulatorLength[i]->SetLimits((TGNumberFormat::kNELLimitMinMax), 1, 512);
	}



   tf->AddFrame(fF_tab3a, fL3);


/**********************************************************************************************************/



/**********************************************************************************************************/


// Tab 4  (Gain / Offset)
 
   tf = fTab->AddTab("Gain/Offset");
 	tabel_tab[5] = fTab->GetTabTab("Gain/Offset");
	tabel_tab[5]->ChangeBackground(tab_color_not_active);
	this->sis3316Test1_nof_valid_tabel_tabs++ ;	
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft ,  20 ,2, 2, 12); //hints, left, right, top, bottom

   fF_tab4 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   fF_tab4_fGrp1 = new TGGroupFrame(fF_tab4, "SIS3316 Gain Settings for all channels");
   fF_tab4->AddFrame(fF_tab4_fGrp1, new TGLayoutHints(kLHintsExpandX, 5, 5, 15, 5));

   fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 20, 2); // hints, left, right, top, bottom

   fChkInput_50Ohm = new TGCheckButton(fF_tab4_fGrp1, "Input 50 Ohm", 0);
   fChkInput_50Ohm->SetState(kButtonDown)   ; // is ON !
   fF_tab4_fGrp1->AddFrame(fChkInput_50Ohm, new TGLayoutHints(kLHintsExpandX, 5, 5, 20, 5));

   fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 2, 2); // hints, left, right, top, bottom
   fChkInput_5V_Range = new TGCheckButton(fF_tab4_fGrp1, "Input Range 5V", 0);
   fChkInput_5V_Range->SetState(kButtonDown)   ; // is ON !
   fF_tab4_fGrp1->AddFrame(fChkInput_5V_Range, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 20));



   fF_tab4_fGrp2 = new TGGroupFrame(fF_tab4, "SIS3316 Offset Settings for all channels");
   fF_tab4->AddFrame(fF_tab4_fGrp2, new TGLayoutHints(kLHintsExpandX, 5, 5, 15, 5));

   fF[0] = new TGHorizontalFrame(fF_tab4_fGrp2, 200, 30);
   fF_tab4_fGrp2->AddFrame(fF[0],  new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 0, 0, 15, 10));

   fNumericEntriesAnalogOffset = new TGNumberEntry(fF[0], 0 /* value */, 8 /* width */, 120 /* irq */ , (TGNumberFormat::kNESInteger) ) ; //kNESHex
   fF[0]->AddFrame(fNumericEntriesAnalogOffset, new TGLayoutHints(kLHintsCenterY | kLHintsLeft , 5, 5, 5, 5 ));   // left, right, top, bottom
   fLabel[0] = new TGLabel(fF[0], "Analog DAC Offset", myGC(), myfont->GetFontStruct());
   fF[0]->AddFrame(fLabel[0], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 5, 5, 5, 5));
   fNumericEntriesAnalogOffset->SetIntNumber(0x8000); //
   fNumericEntriesAnalogOffset->SetLimits((TGNumberFormat::kNELLimitMinMax), 0, 0xffff);


// CheckButton to enable DAC-test (automatical DAC value increment)
   fChkDacInrementTest = new TGCheckButton(fF_tab4_fGrp2, "automatical Increment Dac value Test enable", 0);
   fChkDacInrementTest->SetState(kButtonUp)   ; // is Off !
   fF_tab4_fGrp2->AddFrame(fChkDacInrementTest, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 20));


   fF_tab4_fGrp3 = new TGGroupFrame(fF_tab4, "ADC SPI Settings for all channels");
   fF_tab4->AddFrame(fF_tab4_fGrp3, new TGLayoutHints(kLHintsExpandX, 5, 5, 15, 5));

   fCombo_Set_ADC_SPI_Input_Voltage = new TGComboBox(fF_tab4_fGrp3, 90);
   fCombo_Set_ADC_SPI_Input_Voltage->Associate(this); // Event (IRQ) anmelden
   fF_tab4_fGrp3->AddFrame(fCombo_Set_ADC_SPI_Input_Voltage, new TGLayoutHints(kLHintsExpandX ,  5, 2, 15, 15));

	for (i = 0; i < 3; i++) {
		fCombo_Set_ADC_SPI_Input_Voltage->AddEntry(entryADC_SPI_InputVoltage[i], i);
	}
	if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
		fCombo_Set_ADC_SPI_Input_Voltage->Select(2, kTRUE); // ADC SPI 2.0V
	}
	else {
		fCombo_Set_ADC_SPI_Input_Voltage->Select(1, kTRUE); // ADC SPI 1.75V
	}
	fCombo_Set_ADC_SPI_Input_Voltage->Resize(200, 20);


	for (i = 0; i < 3; i++) {
		fLabel_fCombo_Set_ADC_SPI_text[i] = new TGLabel(fF_tab4_fGrp3," ");
		fLabel_fCombo_Set_ADC_SPI_text[i]->SetTextJustify(kTextLeft + kTextCenterX );
		fLabel_fCombo_Set_ADC_SPI_text[i]->SetMargins(0,0,0,0);
		fLabel_fCombo_Set_ADC_SPI_text[i]->SetWrapLength(-1);
		fF_tab4_fGrp3->AddFrame(fLabel_fCombo_Set_ADC_SPI_text[i], new TGLayoutHints(kLHintsExpandX,2,2,10,2));
	}

	if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
		sprintf(s,"chip full scale 2.00V: input range 5V->5V / 2V->2V (default)");
		fLabel_fCombo_Set_ADC_SPI_text[0]->SetText(s);
		sprintf(s,"chip full scale 1.75V: input range 5V->4.375V / 2V->1.75V  ");
		fLabel_fCombo_Set_ADC_SPI_text[1]->SetText(s);
		sprintf(s,"chip full scale 1.50V:  input range 5V->3.75V / 2V->1.5V");
		fLabel_fCombo_Set_ADC_SPI_text[2]->SetText(s);
	}
	else {
		sprintf(s,"chip full scale 1.75V: input range 5V->5V / 2V->2V (default)");
		fLabel_fCombo_Set_ADC_SPI_text[0]->SetText(s);
		sprintf(s,"chip full scale 2.00V: input range 5V->5.7V / 2V->2.3V  ");
		fLabel_fCombo_Set_ADC_SPI_text[1]->SetText(s);
		sprintf(s,"chip full scale 1.50V: input range 5V->4.3V / 2V->1.7V");
		fLabel_fCombo_Set_ADC_SPI_text[2]->SetText(s);
	}



   fF_tab4_fGrp4 = new TGGroupFrame(fF_tab4, "SIS3316 Tap Delay Settings for all channels");
   fF_tab4->AddFrame(fF_tab4_fGrp4, new TGLayoutHints(kLHintsExpandX, 5, 5, 15, 5));

   fF[0] = new TGHorizontalFrame(fF_tab4_fGrp4, 200, 30);
   fF_tab4_fGrp4->AddFrame(fF[0],  new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 0, 0, 15, 10));

   fNumericEntriesTapDelay = new TGNumberEntry(fF[0], 0 /* value */, 8 /* width */, 120 /* irq */ , (TGNumberFormat::kNESHex) ) ; //kNESHex
   fNumericEntriesTapDelay->Associate(this); // Event (IRQ) anmelden
    fF[0]->AddFrame(fNumericEntriesTapDelay, new TGLayoutHints(kLHintsCenterY | kLHintsLeft , 5, 5, 5, 5 ));   // left, right, top, bottom
   fLabel[0] = new TGLabel(fF[0], "Tap Delay (hex)", myGC(), myfont->GetFontStruct());
   fF[0]->AddFrame(fLabel[0], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 5, 5, 5, 5));
   fNumericEntriesTapDelay->SetIntNumber(0x0000); //
   fNumericEntriesTapDelay->SetLimits((TGNumberFormat::kNELLimitMinMax), 0, 0xffff);


   tf->AddFrame(fF_tab4, fL3);
 


/**********************************************************************************************************/
// Tab 5  (Clock)
 
   tf = fTab->AddTab("Sample Clock");
 	tabel_tab[6] = fTab->GetTabTab("Sample Clock");
	tabel_tab[6]->ChangeBackground(tab_color_not_active);
	this->sis3316Test1_nof_valid_tabel_tabs++ ;	
    fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft ,  20 ,2, 2, 12); //hints, left, right, top, bottom

   fF_tab5 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);

   fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 20, 2); // hints, left, right, top, bottom


 
///////////////////////////////
   	fF_tab5_fGrp1A = new TGGroupFrame(fF_tab5, "FP-Bus Control");
	fF_tab5->AddFrame(fF_tab5_fGrp1A, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

	fF4A = new TGCompositeFrame(fF_tab5_fGrp1A, 60, 20, kVerticalFrame);
	fF_tab5_fGrp1A->AddFrame(fF4A, fL3);

	fChkFP_BUS_ClockMaster = new TGCheckButton(fF4A, "Enable Internal Sample Clock to FP-BUS", 0);
	fChkFP_BUS_ClockMaster->SetState(kButtonUp)   ; // is Off !
	fF4A->AddFrame(fChkFP_BUS_ClockMaster,  new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 5, 2));// hints, left, right, top, bottom


//***

	fCombo_FP_BUS_ClockOutMux = new TGComboBox(fF4A, 48);
	//fCombo_FP_BUS_ClockOutMux->Associate(this); // Event (IRQ) anmelden
	fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5);
	fF4A->AddFrame(fCombo_FP_BUS_ClockOutMux, fL5);

	for (i = 0; i < 2; i++) {
		fCombo_FP_BUS_ClockOutMux->AddEntry(entryFP_BUS_ClockOutMux[i], i);
	}
	fCombo_FP_BUS_ClockOutMux->Select(0, kTRUE); //  internal
	fCombo_FP_BUS_ClockOutMux->Resize(270, 25);

/**********************/

	fF_tab5_fGrp2 = new TGGroupFrame(fF_tab5, "Sample Clock");
	fF_tab5->AddFrame(fF_tab5_fGrp2, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

	fF5 = new TGCompositeFrame(fF_tab5_fGrp2, 60, 20, kVerticalFrame);
	fF_tab5_fGrp2->AddFrame(fF5, fL3);

//***
	fCombo_SampleClock_source = new TGComboBox(fF5, 45);
	//fCombo_Clock_source->Associate(this); // Event (IRQ) anmelden
	fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 15); // hints, left, right, top, bottom
	fF5->AddFrame(fCombo_SampleClock_source, fL5);

	for (i = 0; i < 4; i++) {
		fCombo_SampleClock_source->AddEntry(entryClock_source[i], i);
	}
	fCombo_SampleClock_source->Select(0, kTRUE); //
	fCombo_SampleClock_source->Resize(270, 25);

//***

	fCombo_SetInternalClockFreq = new TGComboBox(fF5, 46);
	//fCombo_SetInternalClockFreq->Associate(this); // Event (IRQ) anmelden
	fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5);
	fF5->AddFrame(fCombo_SetInternalClockFreq, fL5);

	for (i = 0; i < 16; i++) {
		fCombo_SetInternalClockFreq->AddEntry(entryClock_freq[i], i);
	}

	if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
		fCombo_SetInternalClockFreq->Select(6, kTRUE); //
	}
	else {
		fCombo_SetInternalClockFreq->Select(0, kTRUE); //
	}
	fCombo_SetInternalClockFreq->Resize(270, 25);

 //***

	fCombo_SetClockMultiplierMode = new TGComboBox(fF5, 47);
	//fCombo_SetClockMultiplierMode->Associate(this); // Event (IRQ) anmelden
	fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5);
	fF5->AddFrame(fCombo_SetClockMultiplierMode, fL5);

	for (i = 0; i < 7; i++) {
		fCombo_SetClockMultiplierMode->AddEntry(entryClock_multiplier_modes[i], i);
	}
	fCombo_SetClockMultiplierMode->Select(0, kTRUE); //  Bypass
	fCombo_SetClockMultiplierMode->Resize(270, 25);


   tf->AddFrame(fF_tab5, fL3);

 
/**********************************************************************************************************/

//#ifdef CONSTRUCTION_ZONE	   
   // Tab 6  (outputs )
     tf = fTab->AddTab("NIM Outputs");
 	tabel_tab[7] = fTab->GetTabTab("NIM Outputs");
	tabel_tab[7]->ChangeBackground(tab_color_not_active);
	this->sis3316Test1_nof_valid_tabel_tabs++ ;	
    fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft ,  20 ,2, 2, 12); //hints, left, right, top, bottom

	fF_tab6 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
	fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 20, 2); // hints, left, right, top, bottom

   tf->AddFrame(fF_tab6, fL3);
//#endif
/**********************************************************************************************************/


#ifdef ETHERNET_UDP_INTERFACE

// Tab 7  (UDP parameters)
 
   tf = fTab->AddTab("UDP parameter");
 	tabel_tab[8] = fTab->GetTabTab("UDP parameter");
	tabel_tab[8]->ChangeBackground(tab_color_not_active);
	this->sis3316Test1_nof_valid_tabel_tabs++ ;	
    fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft ,  20 ,2, 2, 12); //hints, left, right, top, bottom

	fF_tab7 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
	fL5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  20, 2, 20, 2); // hints, left, right, top, bottom

   
   
#ifdef raus  
   
   if (fChk_JumboFrame->IsOn() == kTRUE) {
		vme_crate->set_UdpSocketEnableJumboFrame();
	}
	else {
		vme_crate->set_UdpSocketDisableJumboFrame();
	}
	unsigned int uint_MaxNofPacketsPerReadRequest  ;
	uint_MaxNofPacketsPerReadRequest =  fNumericEntries_MaxNofPacketsPerReadRequest->GetIntNumber() ;

	if (fChk_JumboFrame->IsOn() == kTRUE) {
		vme_crate->set_UdpSocketEnableJumboFrame();
	}
	else {
		vme_crate->set_UdpSocketDisableJumboFrame();
	}

//		fChk_JumboFrame
//fNumericEntries_MaxNofPacketsPerReadRequest
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


//*****************
//#ifdef raus

	fTGHorizontalIpSubFrame1 = new TGHorizontalFrame(main_frameh1_fGrp[2], 200, 30);
	main_frameh1_fGrp[2]->AddFrame(fTGHorizontalIpSubFrame1, new TGLayoutHints(kLHintsExpandX, 2, 2, 12, 2));

	fChk_JumboFrame = new TGCheckButton(fTGHorizontalIpSubFrame1, "Jumbo Frame enable (1440 / 8192)", 0);
	//fChk_JumboFrame->Associate(this);
	fTGHorizontalIpSubFrame1->AddFrame(fChk_JumboFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft ,  5, 15, 4, 2));// hints, left, right, top, bottom
	
//*****************
	fTGHorizontalIpSubFrame2 = new TGHorizontalFrame(main_frameh1_fGrp[2], 200, 30);
 	main_frameh1_fGrp[2]->AddFrame(fTGHorizontalIpSubFrame2, new TGLayoutHints(kLHintsExpandX, 2, 2, 12, 2));

	fNumericEntries_MaxNofPacketsPerReadRequest = new TGNumberEntry(fTGHorizontalIpSubFrame2, 1 /* value */, 4 /* width */,0, (TGNumberFormat::kNESInteger) ) ; //kNESHex
    //fNumericEntries_SampleControl_NofEvents_ProBank[i]->Associate(this);
    fTGHorizontalIpSubFrame2->AddFrame(fNumericEntries_MaxNofPacketsPerReadRequest, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
    fLabel_MaxNofPacketsPerReadRequest = new TGLabel(fTGHorizontalIpSubFrame2, "max Number of Packets per Read Request");
    //fLabel_MaxNofPacketsPerReadRequest = new TGLabel(fTGHorizontalIpSubFrame, "max Number of Packets per Read Request", myGC(), myfont->GetFontStruct());
    fTGHorizontalIpSubFrame2->AddFrame(fLabel_MaxNofPacketsPerReadRequest, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5));

	fNumericEntries_MaxNofPacketsPerReadRequest->SetLimits((TGNumberFormat::kNELLimitMinMax), 1, 100);
//*****************
#endif
#ifdef raus 
///////////////////////////////
   	fF_tab5_fGrp1A = new TGGroupFrame(fF_tab5, "FP-Bus Control");
	fF_tab5->AddFrame(fF_tab5_fGrp1A, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

	fF4A = new TGCompositeFrame(fF_tab5_fGrp1A, 60, 20, kVerticalFrame);
	fF_tab5_fGrp1A->AddFrame(fF4A, fL3);

	fChkFP_BUS_ClockMaster = new TGCheckButton(fF4A, "Enable Internal Sample Clock to FP-BUS", 0);
	fChkFP_BUS_ClockMaster->SetState(kButtonUp)   ; // is Off !
	fF4A->AddFrame(fChkFP_BUS_ClockMaster,  new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,  2, 2, 5, 2));// hints, left, right, top, bottom


//***

	fCombo_FP_BUS_ClockOutMux = new TGComboBox(fF4A, 48);
	//fCombo_FP_BUS_ClockOutMux->Associate(this); // Event (IRQ) anmelden
	fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5);
	fF4A->AddFrame(fCombo_FP_BUS_ClockOutMux, fL5);

	for (i = 0; i < 2; i++) {
		fCombo_FP_BUS_ClockOutMux->AddEntry(entryFP_BUS_ClockOutMux[i], i);
	}
	fCombo_FP_BUS_ClockOutMux->Select(0, kTRUE); //  internal
	fCombo_FP_BUS_ClockOutMux->Resize(270, 25);

/**********************/

	fF_tab5_fGrp2 = new TGGroupFrame(fF_tab5, "Sample Clock");
	fF_tab5->AddFrame(fF_tab5_fGrp2, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

	fF5 = new TGCompositeFrame(fF_tab5_fGrp2, 60, 20, kVerticalFrame);
	fF_tab5_fGrp2->AddFrame(fF5, fL3);

//***
	fCombo_SampleClock_source = new TGComboBox(fF5, 45);
	//fCombo_Clock_source->Associate(this); // Event (IRQ) anmelden
	fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 15); // hints, left, right, top, bottom
	fF5->AddFrame(fCombo_SampleClock_source, fL5);

	for (i = 0; i < 4; i++) {
		fCombo_SampleClock_source->AddEntry(entryClock_source[i], i);
	}
	fCombo_SampleClock_source->Select(0, kTRUE); //
	fCombo_SampleClock_source->Resize(270, 25);

//***

	fCombo_SetInternalClockFreq = new TGComboBox(fF5, 46);
	//fCombo_SetInternalClockFreq->Associate(this); // Event (IRQ) anmelden
	fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5);
	fF5->AddFrame(fCombo_SetInternalClockFreq, fL5);

	for (i = 0; i < 16; i++) {
		fCombo_SetInternalClockFreq->AddEntry(entryClock_freq[i], i);
	}

	if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
		fCombo_SetInternalClockFreq->Select(6, kTRUE); //
	}
	else {
		fCombo_SetInternalClockFreq->Select(0, kTRUE); //
	}
	fCombo_SetInternalClockFreq->Resize(270, 25);

 //***

	fCombo_SetClockMultiplierMode = new TGComboBox(fF5, 47);
	//fCombo_SetClockMultiplierMode->Associate(this); // Event (IRQ) anmelden
	fL5 = new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5);
	fF5->AddFrame(fCombo_SetClockMultiplierMode, fL5);

	for (i = 0; i < 7; i++) {
		fCombo_SetClockMultiplierMode->AddEntry(entryClock_multiplier_modes[i], i);
	}
	fCombo_SetClockMultiplierMode->Select(0, kTRUE); //  Bypass
	fCombo_SetClockMultiplierMode->Resize(270, 25);
#endif

   tf->AddFrame(fF_tab7, fL3);


   

#endif

/**********************************************************************************************************/

   TGLayoutHints *fL5 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
                                          kLHintsExpandY, 2, 2, 5, 1);
#ifdef scroll
	fViewPortScroll->AddFrame(fTab, new TGLayoutHints(kLHintsNormal | kLHintsExpandX | kLHintsExpandY));

   //fVert1->SetLayoutManager(new TGTileLayout(fVert1,8));
   fTab->MapSubwindows();
   fCanvasScroll->SetContainer(fTab);
   fCanvasScroll->MapSubwindows();
   this->AddFrame(fCanvasScroll, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY ,2,2,2,2));

#endif

#ifndef scroll
   AddFrame(fTab, fL5);
#endif

	MapSubwindows();
	SetWMPosition(SIS3316_TEST_WINDOW_POSTION_X, SIS3316_TEST_WINDOW_POSTION_Y);
	Resize(SIS3316_TEST_WINDOW_WIDTH, SIS3316_TEST_WINDOW_HIGH);   // resize to default size


   // position relative to the parent's window
   //CenterOnParent();

   SetWindowName("SIS3316 Test Dialog");

   MapWindow();
   //fClient->WaitFor(this);    // otherwise canvas contextmenu does not work
}

/********************************************************************************************************************************/











SIS3316TestDialog::~SIS3316TestDialog()
{
	*fBSetup = kFALSE;
	// Delete test dialog widgets.

	//delete gl_sis3316_adc1;
	if (fB_openfCanvas1WindowFlag == kTRUE) {
		delete fCanvas1 ; //
	}
	if (fB_openfCanvas2WindowFlag == kTRUE) {
		delete fCanvas2 ; //
	}
	if (fB_openfCanvas3WindowFlag == kTRUE) {
		delete fCanvas3 ; //
	}
	if (fB_openfCanvas4WindowFlag == kTRUE) {
		delete fCanvas4 ; //
	}
	fB_openfCanvas1WindowFlag = kFALSE; // Setup
	fB_openfCanvas2WindowFlag = kFALSE; // Setup
	fB_openfCanvas3WindowFlag = kFALSE; // Setup
	fB_openfCanvas4WindowFlag = kFALSE; // Setup
//yyyyy
}


/**********************************************************************************************************************************/

void SIS3316TestDialog::SIS3316_Test_running_dim_widgets(bool dim_state)
{
	unsigned int i ;
	unsigned int uint_SampleControl_BankModus ;
#ifdef not_used
	fChkStopAfterBanks->SetEnabled(dim_state); //
	if (dim_state == kTRUE) {
		gSystem->ProcessEvents();  // handle GUI events
		if (fChkStopAfterBanks->IsOn() == kTRUE)  {
			fNumericEntriesStopAfterBanks->SetState(kTRUE); //
		}
		else {
			fNumericEntriesStopAfterBanks->SetState(kFALSE); //
		}
	}
	else {
		fNumericEntriesStopAfterBanks->SetState(kFALSE); //
	}
#endif




	uint_SampleControl_BankModus = fCombo_SampleControl_BankModus->GetSelected();
	fCombo_SampleControl_BankModus->SetEnabled(dim_state); //


	fChkFP_BUS_ClockMaster->SetEnabled(dim_state); //
	fCombo_FP_BUS_ClockOutMux->SetEnabled(dim_state); //

	fCombo_SampleClock_source->SetEnabled(dim_state); //
	fCombo_SetInternalClockFreq->SetEnabled(dim_state); //
	fCombo_SetClockMultiplierMode->SetEnabled(dim_state); //

	fCombo_SetSelectMAW_TestBuffer->SetEnabled(dim_state); //



	fNumericEntries_EventHitParameter[1]->SetState(dim_state); //
	fNumericEntries_EventHitParameter[2]->SetState(dim_state); //
	fNumericEntries_EventHitParameter[3]->SetState(dim_state); //
	//fNumericEntries_EventHitParameter[5]->SetState(dim_state); //
	//fNumericEntries_EventHitParameter[6]->SetState(dim_state); //
	//fNumericEntries_EventHitParameter[7]->SetState(dim_state); //

	fChk_EventHitParameter_DataFormatBit0->SetEnabled(dim_state); //
	fChk_EventHitParameter_DataFormatBit2->SetEnabled(dim_state); //
	fChk_EventHitParameter_DataFormatBit3->SetEnabled(dim_state); //


	fChkKeyTrigger->SetEnabled(dim_state); //
	fChkLemoInTiEnable->SetEnabled(dim_state); //
	fChkExternalTrigger->SetEnabled(dim_state); //
	fChkInternalTrigger->SetEnabled(dim_state); //
	fChkIntertnalSumTrigger->SetEnabled(dim_state); //
	//fChkChannelInputInvert->SetEnabled(dim_state); //



	// Polarity 
	fInvertChannel_Set->SetEnabled(dim_state); //
	fInvertChannel_Clr->SetEnabled(dim_state); //
	for (i=0;i<16;i++) {
		fChkInvertChannel[i]->SetEnabled(dim_state); //
	}




	// Trigger FIR Filter 
	fNumericEntriesTriggerPulse_length->SetState(dim_state); //
	fNumericEntriesTriggerGap->SetState(dim_state); //
	fNumericEntriesTriggerPeaking->SetState(dim_state); //
	fNumericEntriesTriggerThreshold->SetState(dim_state); //
	fChkTriggerCfdEnable->SetEnabled(dim_state); //
	fTriggerEnableCh_Set->SetEnabled(dim_state); //
	fTriggerEnableCh_Clr->SetEnabled(dim_state); //
	for (i=0;i<20;i++) {
		fChkTriggerEnableCh[i]->SetEnabled(dim_state); //
	}

	// Energy FIR Filter 
	fNumericEntriesEnergyGap->SetState(dim_state); //
	fNumericEntriesEnergyPeaking->SetState(dim_state); //
	fNumericEntriesEnergyTauTable->SetState(dim_state); //
	fNumericEntriesEnergyTauFactor->SetState(dim_state); //
	fNumericEntriesEnergyAdditionalAverage->SetState(dim_state); //

	gSystem->ProcessEvents();  // handle GUI events
	if (uint_SampleControl_BankModus == 0) {
		fChkNofEvents_ProBank->SetEnabled(kFALSE)   ; //
		fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetState(kFALSE); //

		fChkWriteDataToFile->SetEnabled(kFALSE); //
		fChkWriteMultipleFiles->SetEnabled(kFALSE); //
		fTextEntryDataFilePath->SetEnabled(kFALSE); //
		fTextButtonDataFilePath->SetEnabled(kFALSE); //
	}
	else {
		fChkNofEvents_ProBank->SetEnabled(dim_state)   ; //
		fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetState(dim_state); //
	}

 
	if (uint_SampleControl_BankModus != 0) {

		if (dim_state == kFALSE) {
			fChkWriteDataToFile->SetEnabled(kFALSE); //
			fChkWriteMultipleFiles->SetEnabled(kFALSE); //
			fTextEntryDataFilePath->SetEnabled(kFALSE); //
			fTextButtonDataFilePath->SetEnabled(kFALSE); //
		}
		else {
			fChkWriteDataToFile->SetEnabled(kTRUE); //
			gSystem->ProcessEvents();  // handle GUI events
			if (fChkWriteDataToFile->IsOn() == kTRUE)  {
				fChkWriteMultipleFiles->SetEnabled(kTRUE); //
				fTextEntryDataFilePath->SetEnabled(kTRUE); //
				fTextButtonDataFilePath->SetEnabled(kTRUE); //
			}
			else {
				fChkWriteMultipleFiles->SetEnabled(kFALSE); //
				fTextEntryDataFilePath->SetEnabled(kFALSE); //
				fTextButtonDataFilePath->SetEnabled(kFALSE); //
			}
		}


		fNumericEntries_EventHitParameter[0]->SetState(dim_state); //
		fNumericEntries_EventHitParameter[4]->SetState(dim_state); //

		fCombo_Display_FFT_Ch->Select(0, kTRUE); // no display ch
		fCombo_Display_FFT_Ch->SetEnabled(dim_state);


		// Gain/Offset
		fChkInput_50Ohm->SetEnabled(dim_state); //
		fChkInput_5V_Range->SetEnabled(dim_state); //
		fNumericEntriesAnalogOffset->SetState(dim_state); //
		fChkDacInrementTest->SetEnabled(dim_state); //
		fCombo_Set_ADC_SPI_Input_Voltage->SetEnabled(dim_state);
		fNumericEntriesTapDelay->SetState(dim_state); //

	}

}



  

/**********************************************************************************************************************************/
#define MAW_GRAPH

#define HISTOGRAM
#define FFT_GRAPH

void SIS3316TestDialog::SIS3316_Test1()
{
	volatile unsigned int return_code ;
	//unsigned int addr;
	unsigned int module_base_addr   ;

	unsigned int req_nof_32bit_words, got_nof_32bit_words;
	unsigned int  ch_data_valid[16];

	unsigned int ui, plot_length;
	int ymin, ymax ;
	int ywidth, y_delta ;
	unsigned int i, i_adc_fpga;
	unsigned int i_accu;

	unsigned int uint_soft_trigger_flag ;
	//static int cnt;
	unsigned int sample_length, sample_start_index ;
	unsigned int pre_trigger_delay = 1, old_pre_trigger_delay = 1  ;
	unsigned int dac_offset, old_dac_offset  ;
	unsigned int trigger_gate_window_length ;

	unsigned int old_gain_termination_val, gain_termination_reg_val  ;


	unsigned int nof_events_pro_bank;
	unsigned int event_length;
	unsigned int header_length;
	//unsigned int maw_length;

	unsigned int maw_test_buffer_length ;
	unsigned int maw_test_buffer_delay, old_maw_test_buffer_delay ;


	unsigned int header_v0100_accu_4_values_enable_flag ;
	unsigned int header_accu_6_values_enable_flag ;
	unsigned int header_accu_2_values_enable_flag ;
	unsigned int header_maw_3_values_enable_flag ;
	unsigned int header_energy_filter_values_enable_flag ;

	unsigned int maw_test_buffer_enable_flag ;
	unsigned int maw_test_buffer_energy_mux_flag ;
	unsigned int uint_trigger_maw_offset ;
	unsigned int* uint_maw_ptr;

	unsigned int header_maw_3_values_offset ;
	unsigned int header_accu_6_values_offset ;
	unsigned int header_accu_2_values_offset ;
	unsigned int header_energy_values_offset ;


	unsigned int header_offset_ushort_ptr ;
	unsigned int address_threshold ;

	unsigned int trigger_pulse_length_val ;
	unsigned int trigger_gap_val ;
	unsigned int trigger_peaking_val ;
	unsigned int trigger_threshold_adc_val ;
	unsigned int trigger_threshold_val ;
	unsigned int trigger_CFD_enable_flag ;
	unsigned int trigger_enable_flags[20] ;

	unsigned int energy_peaking_val ;
	unsigned int energy_gap_val ;
	unsigned int energy_decay_tau_table_val ;
	unsigned int energy_decay_tau_factor_val ;
	unsigned int energy_additional_average_val ;


	unsigned int accumulator_start_index[6] ;
	unsigned int accumulator_length[6] ;



	unsigned int fp_lvds_bus_control_value  ;
	unsigned int clock_source_choice;
	unsigned int clock_freq_choice;
	unsigned int clock_multiplier_choice;
	unsigned int clock_N1div_val ;
	unsigned int clock_HSdiv_val ;
	unsigned int iob_delay_value ;

	unsigned int clock_N1div_array[16]    ;
	unsigned int clock_HSdiv_array[16]    ;
	unsigned int iob_delay_14bit_array[16];
	unsigned int iob_delay_16bit_array[16]  ;
	double double_fft_frequency_array[16]  ;

	unsigned int display_histo_counter, display_histo_ch_no ;
	unsigned int data ;
	unsigned int fpga_data[4] ;
	unsigned short* ushort_adc_buffer_array_ptr[16]; //
	unsigned short* ushort_adc_buffer_ptr; //

	unsigned int display_histogram_choice, new_display_histogram_choice, changed_display_histogram_choice_flag;
	unsigned int histogram_gausfit_enable_flag, histogram_gausfit_clear_flag;

	unsigned int display_MAW_choice;
	unsigned int display_FFT_choice;
	unsigned int display_FFT_Window_choice;


	unsigned int amplitude = 30000 ;
	unsigned int fft_plot_length  ;
	double double_fft_frequency  ;

	double double_histo_min_x ;
	double double_histo_max_x;
	double double_histo_mean ;
	double double_histo_min_mean[16] ;
	double double_histo_max_mean[16] ;
	double double_histo_fwhm ;
	char char_temp[80] ;

	//unsigned int frequency= 10000;

	unsigned int adc_fpga_firmware_version ;

	unsigned int plot_counter;
	unsigned int bank_buffer_counter;
	unsigned int bank1_armed_flag;
	unsigned int poll_counter;


	#define WRITE_DTATA_TO_FILE_MAX_32BIT_WORDS	 0x10000000   // 256M 32-bit words -> 1GByte	
	unsigned int uint_WritenData_to_File_32bit_words = 0 ;
	unsigned int uint_WriteData_to_File_EnableFlag = 0 ;
	unsigned int uint_WriteData_to_MultipleFiles_Flag = 0 ;
	unsigned int uint_WriteData_to_File_OpenFlag = 0 ;
	unsigned int uint_WriteData_to_File_counter = 0 ;
	char char_WriteData_to_File_initialize_filename[512]  ;
	char char_WriteData_to_File_filename[512]  ;

	FILE *file_WriteData_to_File_Pointer           ;
	unsigned file_header_indentifier;

  	unsigned int uint_SampleControl_BankModus ;
	unsigned int half_possibe_events_pro_bank ;
	unsigned int gui_max_events_pro_bank ;


	uint_SampleControl_BankModus = fCombo_SampleControl_BankModus->GetSelected();


  ushort_adc_buffer_array_ptr[0] = (unsigned short*) gl_uint_adc1_buffer;
  ushort_adc_buffer_array_ptr[1] = (unsigned short*) gl_uint_adc2_buffer;
  ushort_adc_buffer_array_ptr[2] = (unsigned short*) gl_uint_adc3_buffer;
  ushort_adc_buffer_array_ptr[3] = (unsigned short*) gl_uint_adc4_buffer;
  ushort_adc_buffer_array_ptr[4] = (unsigned short*) gl_uint_adc5_buffer;
  ushort_adc_buffer_array_ptr[5] = (unsigned short*) gl_uint_adc6_buffer;
  ushort_adc_buffer_array_ptr[6] = (unsigned short*) gl_uint_adc7_buffer;
  ushort_adc_buffer_array_ptr[7] = (unsigned short*) gl_uint_adc8_buffer;
  ushort_adc_buffer_array_ptr[8] = (unsigned short*) gl_uint_adc9_buffer;
  ushort_adc_buffer_array_ptr[9] = (unsigned short*) gl_uint_adc10_buffer;
  ushort_adc_buffer_array_ptr[10] = (unsigned short*) gl_uint_adc11_buffer;
  ushort_adc_buffer_array_ptr[11] = (unsigned short*) gl_uint_adc12_buffer;
  ushort_adc_buffer_array_ptr[12] = (unsigned short*) gl_uint_adc13_buffer;
  ushort_adc_buffer_array_ptr[13] = (unsigned short*) gl_uint_adc14_buffer;
  ushort_adc_buffer_array_ptr[14] =(unsigned short*) gl_uint_adc15_buffer;
  ushort_adc_buffer_array_ptr[15] = (unsigned short*) gl_uint_adc16_buffer;


// Parameter Setup

  pre_trigger_delay = fNumericEntries_EventHitParameter[0]->GetIntNumber();
  if (pre_trigger_delay > MAX_PRETRIGGER_DELAY) { pre_trigger_delay = MAX_PRETRIGGER_DELAY ; }
  fNumericEntries_EventHitParameter[0]->SetIntNumber(pre_trigger_delay );
  old_pre_trigger_delay = pre_trigger_delay ;

  sample_start_index = fNumericEntries_EventHitParameter[1]->GetIntNumber();
  if (sample_start_index > MAX_SAMPLE_LENGTH) { sample_start_index = MAX_SAMPLE_LENGTH - 2; }
  fNumericEntries_EventHitParameter[1]->SetIntNumber(sample_start_index );

  sample_length = fNumericEntries_EventHitParameter[2]->GetIntNumber();
  if (sample_length > MAX_SAMPLE_LENGTH) { sample_length = MAX_SAMPLE_LENGTH - 2; }
  fNumericEntries_EventHitParameter[2]->SetIntNumber(sample_length );



  maw_test_buffer_length = fNumericEntries_EventHitParameter[3]->GetIntNumber();
  maw_test_buffer_length = maw_test_buffer_length & 0xfffe; // only even number
  if (maw_test_buffer_length > 1022) { maw_test_buffer_length = 1024 - 2; }
  fNumericEntries_EventHitParameter[3]->SetIntNumber(maw_test_buffer_length );


	maw_test_buffer_energy_mux_flag = 0 ;
	uint_trigger_maw_offset         = 0x08000000 ;
	if (fCombo_SetSelectMAW_TestBuffer->GetSelected() == 1) {
		maw_test_buffer_energy_mux_flag = 1  ;
		uint_trigger_maw_offset         = 0x0 ;
	}


	maw_test_buffer_delay = fNumericEntries_EventHitParameter[4]->GetIntNumber();
	maw_test_buffer_delay = maw_test_buffer_delay & 0xfffe; // only even number
	if (maw_test_buffer_delay > 1022) { maw_test_buffer_delay = 1024 - 2; }
	fNumericEntries_EventHitParameter[4]->SetIntNumber(maw_test_buffer_delay );
	old_maw_test_buffer_delay = maw_test_buffer_delay ;



	dac_offset = fNumericEntriesAnalogOffset->GetIntNumber();
	if (dac_offset > 0xffff) { dac_offset = 0xffff ; }
	fNumericEntriesAnalogOffset->SetIntNumber(dac_offset );
	old_dac_offset = dac_offset ;

	trigger_gap_val =  fNumericEntriesTriggerGap->GetIntNumber(); ;
	if (trigger_gap_val > 510) { trigger_gap_val = 510; }
	if (trigger_gap_val < 2)   { trigger_gap_val = 2; }   // lt 2 ?
	trigger_gap_val = trigger_gap_val & 0x3fe ;
	fNumericEntriesTriggerGap->SetIntNumber(trigger_gap_val );


	trigger_pulse_length_val =  fNumericEntriesTriggerPulse_length->GetIntNumber(); ;
	if (trigger_pulse_length_val > 255) { trigger_pulse_length_val = 255; }
	fNumericEntriesTriggerPulse_length->SetIntNumber(trigger_pulse_length_val );

	trigger_gap_val =  fNumericEntriesTriggerGap->GetIntNumber(); ;
	if (trigger_gap_val > 510) { trigger_gap_val = 510; }
	if (trigger_gap_val < 2)   { trigger_gap_val = 2; }   // lt 2 ?
	trigger_gap_val = trigger_gap_val & 0x3fe ;
	fNumericEntriesTriggerGap->SetIntNumber(trigger_gap_val );

	trigger_peaking_val =  fNumericEntriesTriggerPeaking->GetIntNumber(); ;
	if (trigger_peaking_val > 510) { trigger_peaking_val = 510; } // gt 510 ?
	if (trigger_peaking_val < 2)   { trigger_peaking_val = 2; }   // lt 2 ?
	trigger_peaking_val = trigger_peaking_val & 0x3fe ;
	fNumericEntriesTriggerPeaking->SetIntNumber(trigger_peaking_val );


	trigger_threshold_adc_val =  fNumericEntriesTriggerThreshold->GetIntNumber(); ;
	if (trigger_threshold_adc_val > 0x3fff) { trigger_threshold_adc_val = 0x3fff; }
	fNumericEntriesTriggerThreshold->SetIntNumber(trigger_threshold_adc_val );

  	if (fChkTriggerCfdEnable->IsOn() == kTRUE) {
		trigger_CFD_enable_flag = 1 ;
	}


	for (i=0;i<20;i++) {
		trigger_enable_flags[i] = 0 ;
		if (fChkTriggerEnableCh[i]->IsOn() == kTRUE) {
			trigger_enable_flags[i] = 1 ;
		}
	}


/*** FIR Filter Energy ************/

    energy_peaking_val =  fNumericEntriesEnergyPeaking->GetIntNumber(); ;
	if (energy_peaking_val > FIR_ENERGY_MAX_PEAKING) { energy_peaking_val = FIR_ENERGY_MAX_PEAKING; } //
	if (energy_peaking_val < FIR_ENERGY_MIN_PEAKING)   { energy_peaking_val = FIR_ENERGY_MIN_PEAKING; }   //
	fNumericEntriesEnergyPeaking->SetIntNumber(energy_peaking_val );

    energy_gap_val =  fNumericEntriesEnergyGap->GetIntNumber(); ;
	if (energy_gap_val > FIR_ENERGY_MAX_GAP) { energy_gap_val = FIR_ENERGY_MAX_GAP; } //
	if (energy_gap_val < FIR_ENERGY_MIN_GAP)   { energy_gap_val = FIR_ENERGY_MIN_GAP; }   //
	fNumericEntriesEnergyGap->SetIntNumber(energy_gap_val );

    energy_decay_tau_table_val =  fNumericEntriesEnergyTauTable->GetIntNumber(); ;
	if (energy_decay_tau_table_val > FIR_ENERGY_MAX_TAU_TABLE) { energy_decay_tau_table_val = FIR_ENERGY_MAX_TAU_TABLE; } //
	fNumericEntriesEnergyTauTable->SetIntNumber(energy_decay_tau_table_val );

    energy_decay_tau_factor_val =  fNumericEntriesEnergyTauFactor->GetIntNumber(); ;
	if (energy_decay_tau_factor_val > FIR_ENERGY_MAX_TAU_FACTOR) { energy_decay_tau_factor_val = FIR_ENERGY_MAX_TAU_FACTOR; } //
	fNumericEntriesEnergyTauFactor->SetIntNumber(energy_decay_tau_factor_val );

    energy_additional_average_val =  fNumericEntriesEnergyAdditionalAverage->GetIntNumber(); ;
	if (energy_additional_average_val > FIR_ENERGY_MAX_ADD_AVERAGE) { energy_additional_average_val = FIR_ENERGY_MAX_ADD_AVERAGE; } //
	fNumericEntriesEnergyAdditionalAverage->SetIntNumber(energy_additional_average_val );


/*** Accumulator parameters ************/

	for (i=0;i<6;i++) {
	    accumulator_start_index[i] =  fNumericEntriesAccumulatorStartIndex[i]->GetIntNumber(); ;
		if (accumulator_start_index[i] > ACCUMULATOR_MAX_START_INDEX) { accumulator_start_index[i] = ACCUMULATOR_MAX_START_INDEX; } //
		fNumericEntriesAccumulatorStartIndex[i]->SetIntNumber(accumulator_start_index[i] );

		accumulator_length[i] =  fNumericEntriesAccumulatorLength[i]->GetIntNumber(); ;
		if (accumulator_length[i] > ACCUMULATOR_MAX_LENGTH+1) { accumulator_length[i] = ACCUMULATOR_MAX_START_INDEX+1; } //
		if (accumulator_length[i] == 0) { accumulator_length[i] = 1; } //
		fNumericEntriesAccumulatorLength[i]->SetIntNumber(accumulator_length[i] );
	}



	//	TGNumberEntry       *fNumericEntriesHistogramXaxisOffset;
	// TGNumberEntry       *fNumericEntriesHistogramXaxisDivider;



  //printf("sample_length = %d \n", sample_length);

  plot_length = sample_length ;

  display_histo_counter = 0 ;
  display_histo_ch_no   = 0 ;


/**************************************************************************************************************/


  // Raw Data Graph Setup
  if (fB_openfCanvas1WindowFlag == kFALSE) {
      fCanvas1 = new TCanvas("fCanvas1","ADC Raw data ", SIS3316_RAW_DATA_WINDOW_POSTION_X, SIS3316_RAW_DATA_WINDOW_POSTION_Y, SIS3316_RAW_DATA_WINDOW_WIDTH, SIS3316_RAW_DATA_WINDOW_HIGH); // X, y, WITDH, High
 		fB_openfCanvas1WindowFlag = kTRUE; //
  }
      fCanvas1->Clear();
      fCanvas1->cd(1);
      fCanvas1->SetGrid();
      fCanvas1->SetFillColor(DefineCanvasBackgroundColor);
    //TCanvas *fCanvas1 = new TCanvas("fCanvas1","A Simple Graph Example",200,10,700,500);

  for (ui=0;ui<plot_length;ui++){
    gl_x[ui] = ui ;
  }



  if (!fGraph_ch[0]) {
      for (i=0;i<17;i++) {
	  fGraph_ch[i]   = new TGraph(MAX_ROOT_PLOT_LENGTH,gl_x,gl_y);
      }
      for (i=0;i<16;i++) {
	  fGraph_Text_ch[i]   = new TLatex(plot_length,10,"Ch ");;
      }
  }

  fGraph_ch[16]->SetLineColor(DefineCanvasBackgroundColor);
  fGraph_ch[0]->SetLineColor(DefineChannel_1_Color);
  fGraph_ch[1]->SetLineColor(DefineChannel_2_Color);
  fGraph_ch[2]->SetLineColor(DefineChannel_3_Color);
  fGraph_ch[3]->SetLineColor(DefineChannel_4_Color);
  fGraph_ch[4]->SetLineColor(DefineChannel_5_Color);
  fGraph_ch[5]->SetLineColor(DefineChannel_6_Color);
  fGraph_ch[6]->SetLineColor(DefineChannel_7_Color);
  fGraph_ch[7]->SetLineColor(DefineChannel_8_Color);
  fGraph_ch[8]->SetLineColor(DefineChannel_9_Color);
  fGraph_ch[9]->SetLineColor(DefineChannel_10_Color);
  fGraph_ch[10]->SetLineColor(DefineChannel_11_Color);
  fGraph_ch[11]->SetLineColor(DefineChannel_12_Color);
  fGraph_ch[12]->SetLineColor(DefineChannel_13_Color);
  fGraph_ch[13]->SetLineColor(DefineChannel_14_Color);
  fGraph_ch[14]->SetLineColor(DefineChannel_15_Color);
  fGraph_ch[15]->SetLineColor(DefineChannel_16_Color);

  fGraph_Text_ch[0]->SetTextColor(DefineChannel_1_Color);
  fGraph_Text_ch[1]->SetTextColor(DefineChannel_2_Color);
  fGraph_Text_ch[2]->SetTextColor(DefineChannel_3_Color);
  fGraph_Text_ch[3]->SetTextColor(DefineChannel_4_Color);
  fGraph_Text_ch[4]->SetTextColor(DefineChannel_5_Color);
  fGraph_Text_ch[5]->SetTextColor(DefineChannel_6_Color);
  fGraph_Text_ch[6]->SetTextColor(DefineChannel_7_Color);
  fGraph_Text_ch[7]->SetTextColor(DefineChannel_8_Color);
  fGraph_Text_ch[8]->SetTextColor(DefineChannel_9_Color);
  fGraph_Text_ch[9]->SetTextColor(DefineChannel_10_Color);
  fGraph_Text_ch[10]->SetTextColor(DefineChannel_11_Color);
  fGraph_Text_ch[11]->SetTextColor(DefineChannel_12_Color);
  fGraph_Text_ch[12]->SetTextColor(DefineChannel_13_Color);
  fGraph_Text_ch[13]->SetTextColor(DefineChannel_14_Color);
  fGraph_Text_ch[14]->SetTextColor(DefineChannel_15_Color);
  fGraph_Text_ch[15]->SetTextColor(DefineChannel_16_Color);


#ifdef HISTOGRAM

	new_display_histogram_choice = fCombo_Display_Histos_Ch->GetSelected();
	display_histogram_choice = new_display_histogram_choice ;
	changed_display_histogram_choice_flag = 0 ;

	if (display_histogram_choice > 0)  {

		if (fB_openfCanvas2WindowFlag == kFALSE) {
			fCanvas2 = new TCanvas("fCanvas2","Histogram ", SIS3316_HISTOGRAM_WINDOW_POSTION_X, SIS3316_HISTOGRAM_WINDOW_POSTION_Y, SIS3316_HISTOGRAM_WINDOW_WIDTH, SIS3316_HISTOGRAM_WINDOW_HIGH);
 			fB_openfCanvas2WindowFlag = kTRUE; // Setup
		}
		if (display_histogram_choice == 17) {
			fCanvas2->Clear();
			fCanvas2->Divide(2,8);
		}
		else {
			fCanvas2->Clear();
			fCanvas2->Divide(1);
		}

		fCanvas2->SetGrid();
		fCanvas2->SetFillColor(DefineCanvasBackgroundColor);
		for (i=0;i<16;i++) {
			  fCanvas2->cd(1+i);
			  gPad->SetGrid();
			  gPad->SetFillColor(DefineCanvasBackgroundColor);
		}
	}
	else {
		if (fB_openfCanvas2WindowFlag == kTRUE) {
			delete fCanvas2 ;
			fB_openfCanvas2WindowFlag = kFALSE; //
		}
	}





  histogram_gausfit_clear_flag  = 0 ;
  histogram_gausfit_enable_flag = 0 ;
  if (fChkHistoGaussFit->IsOn() == kTRUE)  {
    histogram_gausfit_enable_flag = 1 ;
    histogram_gausfit_clear_flag  = 0 ;
  }
  else {
    if (histogram_gausfit_enable_flag == 1) {
      histogram_gausfit_clear_flag  = 1 ;
    }
    histogram_gausfit_enable_flag = 0 ;
  }

  unsigned i_histo_length ;
  if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
	   i_histo_length = 0x10000 ;
   }
  else {
	   i_histo_length = 0x4000 ;
   }


  if (!iHistoAdc[0]) {
      for (i=0;i<16;i++) {
		 // iHistoAdc[i]   = new TH1I(AdcHistogramLabel[i],AdcHistogramLabel[i],0x10000,0,0xffff); // 16 bit ADC
		  iHistoAdc[i]   = new TH1I(AdcHistogramLabel[i],AdcHistogramLabel[i], i_histo_length,0, i_histo_length-1);
		 //iHistoAdc[i]->SetName("b");
			gStyle->SetStatFormat("6.6g");
		  gStyle->SetStatH((Float_t)0.36);
		  gStyle->SetStatW((Float_t)0.3);
		  gStyle->SetStatX((Float_t)0.99);
		  gStyle->SetStatY((Float_t)1.00);
      }
  }
  for (i=0;i<16;i++) {
       gPad->SetFillColor(DefineCanvasBackgroundColor);

      iHistoAdc[i]->Reset(); //
      iHistoAdc[i]->BufferEmpty(1); // action =  1 histogram is filled and buffer is deleted
      //iHistoAdc[i]->BufferEmpty(0); // action =  0 histogram is filled from the buffer
      //iHistoAdc[i]->BufferEmpty(-1); // action =  -1 histogram is reset and refilled from the buffer (called by THistPainter::Paint)
      iHistoAdc[i]->Draw();
      iHistoAdc[i]->SetFillColor(kRed);
  }

  for (i=0;i<16;i++) {
    double_histo_min_mean[i] = (double) i_histo_length ;
    double_histo_max_mean[i] = 0.0 ;
  }

//TPaveText *pt;
  //pt = histo_pave_text[]
//	if (uint_SampleControl_BankModus == 0) {

	  if (!histo_pave_text[0]) {
		  for (i=0;i<16;i++) {
			//histo_pave_text[i] = new TPaveText(0.1, 0.5, 0.38, 0.9,"brNDC"); //x1,y1,x2,y2
			//histo_pave_text[i]->SetFillColor(DefineCanvasBackgroundColor);
			//histo_pave_text[i]->SetTextAlign(12);
			histo_pave_text[i] = new TPaveText(0.1, 0.75, 0.40, 0.92,"brNDC"); //x1,y1,x2,y2
			histo_pave_text[i]->SetFillColor(DefineCanvasBackgroundColor);
			histo_pave_text[i]->SetTextAlign(12);
			histo_pave_text[i]->SetBorderSize(1);
		  }
	  }	 	
//	}




#ifdef not_used
   TPaveText *histo_pave_text = new TPaveText(0.1, 0.5, 0.33, 0.9,"brNDC"); //x1,y1,x2,y2
   //pt->SetFillColor(18);
   histo_pave_text[i]->SetFillColor(DefineCanvasBackgroundColor);
   pt->SetTextAlign(12);
   pt->SetTextSize(0.06);
#endif
	if (fB_openfCanvas2WindowFlag == kTRUE) {

		fCanvas2->Modified();
		fCanvas2->Update();
	}
#endif



// FFT Graph Setup

#ifdef FFT_GRAPH


  if (!fGraph_fft[0]) {
      //for (i=0;i<11;i++) {
	//  fGraph_fft[i]   = new TGraph(MAX_ROOT_PLOT_LENGTH/2, gl_float_x,gl_float_y);
      //}
    //fGraph_fft[0]   = new TF1();
    fGraph_fft[0]   = new TGraph(MAX_ROOT_PLOT_LENGTH/2, gl_float_x,gl_float_y);
    fGraph_fft[1]   = new TGraph(MAX_ROOT_PLOT_LENGTH/2, gl_float_x,gl_float_y1);
  }
  fGraph_fft[0]->SetLineColor(DefineChannel_1_Color);
  fGraph_fft[1]->SetLineColor(DefineCanvasBackgroundColor);



  for (ui=0;ui<plot_length/2;ui++){
    gl_float_y[ui] = 0.0 ;
  }
  for (ui=0;ui<plot_length/2;ui++){
    gl_float_y1[ui] = 10.0 ;
  }
  gl_float_y1[40] = -140.0 ;


  fftw_plan p;
  fftw_complex *fftw_complex_in, *fftw_complex_out ;

  fftw_complex_in  =  (fftw_complex*) fftw_malloc ( sizeof (fftw_complex) * plot_length );
  fftw_complex_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*plot_length);

  int *int_save_adc_buffer;
  int_save_adc_buffer = (int*) malloc(sizeof(int)*plot_length);

  double *double_fft_spectrum ;
  double_fft_spectrum = (double*) malloc(sizeof(double)*plot_length / 2);

  double *double_window_weight ;
  double_window_weight = (double*) malloc(sizeof(double)*plot_length);

  double_fft_frequency = 125000000.0 ;

/**************************************************************************************************************/

#endif

	if (gl_sis3316_adc1->adc_125MHz_flag == 0) {
		// 250  MHz
		file_header_indentifier      =  0  ;
	}
	else {
		// 125 MHz
		file_header_indentifier      =  1  ;
	}



	module_base_addr = gl_module_base_addr ;
	return_code = gl_sis3316_adc1->register_read( SIS3316_MODID, &data);
	//printf("register_read: data = 0x%08x     return_code = 0x%08x\n", data, return_code);

	return_code = gl_sis3316_adc1->register_write( SIS3316_KEY_RESET, 0);
	return_code = gl_sis3316_adc1->register_write( SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS, 0xE0E0E0E0);  // clear error Latch bits

	return_code = gl_sis3316_adc1->register_read( SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS, &data);

	if ((return_code != 0) || (data != 0x18181818)) {
		printf("Error: SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS: data = 0x%08x     return_code = 0x%08x\n", data, return_code);
	}


/**************************************************************************************************************/


// enable FP-Bus Clock Master
	fp_lvds_bus_control_value = 0 ;
	if (fChkFP_BUS_ClockMaster->IsOn() == kTRUE) {
		fp_lvds_bus_control_value = fp_lvds_bus_control_value + 0x10  ;
	}
	if (fCombo_FP_BUS_ClockOutMux->GetSelected() == 1) {
		fp_lvds_bus_control_value = fp_lvds_bus_control_value + 0x20  ;
	}
	return_code = gl_sis3316_adc1->register_write( SIS3316_FP_LVDS_BUS_CONTROL, fp_lvds_bus_control_value);  //


// set Clock Multiplier
	clock_multiplier_choice = fCombo_SetClockMultiplierMode->GetSelected();
	switch (clock_multiplier_choice) {
	    case 0: // Bypass
			return_code = gl_sis3316_adc1->bypass_external_clock_multiplier() ;
			break;
	    case 1: //
			return_code = gl_sis3316_adc1->set_external_clock_multiplier(0, 5, 4, 4, 500, 1, 10 ) ; // bw=0    n1_hs=5   n1_clk1 = 4   n1_clk2 = 4   n2=500  n3=1   range 10,00 to 11,34 -> 250 to 283
			break;
	    case 2: //
			return_code = gl_sis3316_adc1->set_external_clock_multiplier(1, 5, 4, 4, 250, 1, 20 ) ; // bw=0/1  n1_hs=5   n1_clk1 = 4   n1_clk2 = 4   n2=250  n3=1   range 19,40 to 22,68 -> 242 to 283
			break;
	    case 3: //
			return_code = gl_sis3316_adc1->set_external_clock_multiplier(2, 11, 1, 1, 110, 1, 50 ) ; // bw=1/2  n1_hs=11  n1_clk1 = 1   n1_clk2 = 1   n2=110  n3=1   range 44,00 to 51,54 -> 220 to 257
			break;
	    case 4: //
			return_code = gl_sis3316_adc1->set_external_clock_multiplier(0, 4, 10, 10, 500, 1, 10 ) ; // bw=0    n1_hs=4   n1_clk1 = 10  n1_clk2 = 10  n2=500  n3=1   range 10,00 to 11,34 -> 125 to 141
			break;
	    case 5: //
			return_code = gl_sis3316_adc1->set_external_clock_multiplier(1, 5, 8, 8, 250, 1, 20 ) ; // bw=0/1  n1_hs=5   n1_clk1 = 8   n1_clk2 = 8   n2=250  n3=1   range 19,40 to 22,68 -> 121 to 141
			break;
	    case 6: //
			return_code = gl_sis3316_adc1->set_external_clock_multiplier(2, 5, 8, 8, 100, 1, 50 ) ; // bw=1/2  n1_hs=5   n1_clk1 = 8   n1_clk2 = 8   n2=100  n3=1   range 48,50 to 56,70 -> 121 to 141
			break;
	}

	if (return_code != 0) {
		printf("set_external_clock_multiplier: return_code = 0x%08x     \n", return_code);
	}

	//return_code = gl_sis3316_adc1->get_status_external_clock_multiplier(&data) ;
	//printf("get_status_external_clock_multiplier: status = 0x%08x   return_code = 0x%08x     \n", data, return_code);


// define Sample Clock
	clock_source_choice = fCombo_SampleClock_source->GetSelected();
	data = 0 ;
	switch (clock_source_choice) {
	    case 0: // Internal Clock
			data = 0 ;
			break;
	    case 1: // VXS Clock
			data = 1 ;
			break;
	    case 2: // FP-Bus Clock
			data = 2 ;
			break;
	    case 3: //External NIM Clock
			data = 3 ;
			break;
	}
	return_code = gl_sis3316_adc1->register_write( SIS3316_SAMPLE_CLOCK_DISTRIBUTION_CONTROL, data);  //



	return_code = gl_sis3316_adc1->register_read( SIS3316_ADC_CH1_4_FIRMWARE_REG, &data);  //
	adc_fpga_firmware_version = data & 0xffff;

	if (adc_fpga_firmware_version < 4) {
		// 250.000 MHz
		iob_delay_14bit_array[0]  =  0x48  ;
		iob_delay_16bit_array[0]  =  0x00  ;
		// 125.000 MHz
		iob_delay_14bit_array[6]  =  0x50  ;
		iob_delay_16bit_array[6]  =  0x7F  ;
		// 119.048 MHz
		iob_delay_14bit_array[7]  =  0x60  ;
		iob_delay_16bit_array[7]  =  0x7F  ;

		// 113.636 MHz
		iob_delay_14bit_array[8]  =  0x1010  ;
		iob_delay_16bit_array[8]  =  0x7F ;
		// 71.429 MHz
		iob_delay_14bit_array[12] =  0x1060  ;
		iob_delay_16bit_array[12] =  0x0000 ;
	}
	else {
		// 250.000 MHz
		iob_delay_14bit_array[0]  =  0x1002;
		iob_delay_16bit_array[0]  =  0x00  ;
		// 125.000 MHz
		iob_delay_14bit_array[6]  =  0x50  ;
		iob_delay_16bit_array[6]  =  0x1020  ;
		// 119.048 MHz
		iob_delay_14bit_array[7]  =  0x60  ;
		iob_delay_16bit_array[7]  =  0x1020  ;

		// 113.636 MHz
		iob_delay_14bit_array[8]  =  0x1010  ;
		iob_delay_16bit_array[8]  =  0x1020  ;
		// 71.429 MHz
		iob_delay_14bit_array[12] =  0x1060  ;
		iob_delay_16bit_array[12] =  0x1060 ;
	}


	// 227.273
	iob_delay_14bit_array[1]  =  0x101f ;
	iob_delay_16bit_array[1]  =  0x000  ;
	// 208,333 MHz
	iob_delay_14bit_array[2]  =  0x1035  ;
	iob_delay_16bit_array[2]  =  0x000  ;
	// 178,571 MHz
	iob_delay_14bit_array[3]  =  0x12  ;
	iob_delay_16bit_array[3]  =  0x000  ;

	// 166.667 MHz
	iob_delay_14bit_array[4]  =  0x20  ;
	iob_delay_16bit_array[4]  =  0x000  ;
	// 138.889 MHz
	iob_delay_14bit_array[5]  =  0x35  ;
	iob_delay_16bit_array[5]  =  0x000  ;

	// 104.167 MHz
	iob_delay_14bit_array[9]  =  0x1020  ;
	iob_delay_16bit_array[9]  =  0x1030  ;
	// 100.000 MHz
	iob_delay_14bit_array[10] =  0x1020  ;
	iob_delay_16bit_array[10] =  0x1030  ;
	// 83.333 MHz
	iob_delay_14bit_array[11] =  0x1030  ;
	iob_delay_16bit_array[11] =  0x1040  ;

	// 62.500 MHz
	iob_delay_14bit_array[13] =  0x1060  ;
	iob_delay_16bit_array[13] =  0x20  ;
	// 50.000 MHz
	iob_delay_14bit_array[14] =  0x20  ;
	iob_delay_16bit_array[14] =  0x30  ;
	// 25.000 MHz
	iob_delay_14bit_array[15] =  0x20  ;
	iob_delay_16bit_array[15] =  0x30  ;




	// 250.000 MHz
	clock_N1div_array[0]      =  4  ;
	clock_HSdiv_array[0]      =  5  ;
	double_fft_frequency_array[0]  =  250000000.0  ;

	// 227.273
	clock_N1div_array[1]      =  2  ;
	clock_HSdiv_array[1]      =  11  ;
	double_fft_frequency_array[1]  =  227273000.0  ;

	// 208,333 MHz
	clock_N1div_array[2]      =  4  ;
	clock_HSdiv_array[2]      =  6  ;
	double_fft_frequency_array[2]  =  208333000.0  ;

	// 178,571 MHz
	clock_N1div_array[3]      =  4  ;
	clock_HSdiv_array[3]      =  7  ;
	double_fft_frequency_array[3]  =  178571000.0  ;

	// 166.667 MHz
	clock_N1div_array[4]      =  6  ;
	clock_HSdiv_array[4]      =  5  ;
	double_fft_frequency_array[4]  =  166667000.0  ;

	// 138.889 MHz
	clock_N1div_array[5]      =  6  ;
	clock_HSdiv_array[5]      =  6  ;
	double_fft_frequency_array[5]  =  138889000.0  ;

	// 125.000 MHz
	clock_N1div_array[6]      =  8  ;
	clock_HSdiv_array[6]      =  5  ;
	double_fft_frequency_array[6]  =  125000000.0  ;

	// 119.048 MHz
	clock_N1div_array[7]      =  6  ;
	clock_HSdiv_array[7]      =  7  ;
	double_fft_frequency_array[7]  =  119048000.0  ;

	// 113.636 MHz
	clock_N1div_array[8]      =  4  ;
	clock_HSdiv_array[8]      =  11  ;
	double_fft_frequency_array[8]  =  113636000.0  ;

	// 104.167 MHz
	clock_N1div_array[9]      =  8  ;
	clock_HSdiv_array[9]      =  6  ;
	double_fft_frequency_array[9]  =  104167000.0  ;

	// 100.000 MHz
	clock_N1div_array[10]     =  10  ;
	clock_HSdiv_array[10]     =  5  ;
	double_fft_frequency_array[10]  =  100000000.0  ;

	// 83.333 MHz
	clock_N1div_array[11]     =  12  ;
	clock_HSdiv_array[11]     =  5  ;
	double_fft_frequency_array[11]  =  83333000.0  ;


	// 71.429 MHz
	clock_N1div_array[12]     =  14  ;
	clock_HSdiv_array[12]     =  5  ;
	double_fft_frequency_array[12]  =  71429000.0  ;


	// 62.500 MHz
	clock_N1div_array[13]     =  16  ;
	clock_HSdiv_array[13]     =  5  ;
	double_fft_frequency_array[13]  =  62500000.0  ;


	// 50.000 MHz
	clock_N1div_array[14]     =  20  ;
	clock_HSdiv_array[14]     =  5  ;
	double_fft_frequency_array[14]  =  50000000.0  ;

	// 25.000 MHz
	clock_N1div_array[15]     =  40  ;
	clock_HSdiv_array[15]     =  5  ;
	double_fft_frequency_array[15]  =  25000000.0  ;




// set internal Frequency
	clock_freq_choice = fCombo_SetInternalClockFreq->GetSelected();
	if (clock_freq_choice >= 16) {
		clock_freq_choice = 0 ;
		fCombo_SetInternalClockFreq->Select(clock_freq_choice, kTRUE); //  set frequency to 250 MHz
	}
	if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
		if (clock_freq_choice < 6) {
			clock_freq_choice = 6 ;
			fCombo_SetInternalClockFreq->Select(clock_freq_choice, kTRUE); //  set frequency to 125 MHz
		}
	}
 	double_fft_frequency = double_fft_frequency_array[clock_freq_choice] ;
 	clock_N1div_val = clock_N1div_array[clock_freq_choice] ;
	clock_HSdiv_val = clock_HSdiv_array[clock_freq_choice] ;

	if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
		iob_delay_value = iob_delay_16bit_array[clock_freq_choice]  ;
	}
	else {
		iob_delay_value = iob_delay_14bit_array[clock_freq_choice]  ;
	}

	// reprogram internal Osc. and reset DCM/PLL of the ADC-FPGAs
	gl_sis3316_adc1->change_frequency_HSdiv_N1div(0, clock_HSdiv_val, clock_N1div_val) ;
	fNumericEntriesTapDelay->SetIntNumber(iob_delay_value); //

	// Calibrate and configure IOB _delay Logic
	gl_sis3316_adc1->configure_adc_fpga_iob_delays(iob_delay_value) ; 

	//enable ADC outputs (bit was cleared with Key-reset !)
	gl_sis3316_adc1->adc_spi_reg_enable_adc_outputs() ; 

	// Channel Header ID register (is already done with  "new gl_sis3316_adc1" )
	data = module_base_addr & 0xFF000000 ;
	gl_sis3316_adc1->write_channel_header_ID( data) ;


	// adc DAC offset setup
	for (i_ch=0;i_ch<16; i_ch++) {
		gl_sis3316_adc1->adc_dac_offset_ch_array[i_ch] = dac_offset ; //  
	}
	gl_sis3316_adc1->write_all_adc_dac_offsets() ; 


	// Gain/Termination
	data = 0 ;
	if (fChkInput_50Ohm->IsOn() == kFALSE) {
		data = 0x04 ;
	}

	if (fChkInput_5V_Range->IsOn() == kFALSE) {
		data = data + 0x01 ;
	}
	gain_termination_reg_val = data ;

	for (i_ch=0;i_ch<16; i_ch++) {
		gl_sis3316_adc1->adc_gain_termination_ch_array[i_ch] = gain_termination_reg_val ; //  
	}
	gl_sis3316_adc1->write_all_gain_termination_values() ; 
	old_gain_termination_val = gain_termination_reg_val ;



	// pre_trigger_delay
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //


	// sample_length and sample_start_index
	sample_start_index = 0;
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length




	unsigned int trigger_gate_window_length_conditions[9] ;

	// trigger_gate_window_length
	trigger_gate_window_length_conditions[0] = pre_trigger_delay + (energy_gap_val & 0xfff) + (2 * (energy_peaking_val & 0xfff)) ; // should be longer than the ENERGY FIR Filter (could be 1 x energy_peaking_val)
	if (sample_length == 0) {
		trigger_gate_window_length_conditions[1] = 0 ;  
	}
	else {
		trigger_gate_window_length_conditions[1] = sample_start_index + sample_length + 10 ;  
	}
	trigger_gate_window_length_conditions[2] = maw_test_buffer_length + 10 ;  


	for (i_accu=0; i_accu<6; i_accu++) {
			trigger_gate_window_length_conditions[3+i_accu] =    (accumulator_length[i_accu]  & 0x1ff) + (accumulator_start_index[i_accu] & 0xffff);
	}

	// find the highest condition
	trigger_gate_window_length = trigger_gate_window_length_conditions[0] ;
	for (i=1; i<9; i++) {
		if (trigger_gate_window_length_conditions[i] >   trigger_gate_window_length) {
			trigger_gate_window_length = trigger_gate_window_length_conditions[i] ;
		}
	}

	trigger_gate_window_length = trigger_gate_window_length + 4 ; 


	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_TRIGGER_GATE_WINDOW_LENGTH_REG, (trigger_gate_window_length & 0xffff) ); // trigger_gate_window_length
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_TRIGGER_GATE_WINDOW_LENGTH_REG, (trigger_gate_window_length & 0xffff) ); // trigger_gate_window_length
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_TRIGGER_GATE_WINDOW_LENGTH_REG, (trigger_gate_window_length & 0xffff) ); // trigger_gate_window_length
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_TRIGGER_GATE_WINDOW_LENGTH_REG, (trigger_gate_window_length & 0xffff) ); // trigger_gate_window_length








	//maw_test_buffer_delay  = trigger_peaking_val + (trigger_peaking_val>>1) + trigger_gap_val + 16 + 20 ;
	if (maw_test_buffer_delay > 1022) {
		maw_test_buffer_delay  = 1022 ;
	}

	maw_test_buffer_enable_flag      = 0 ;
	if (maw_test_buffer_length > 0) {
		maw_test_buffer_enable_flag      = 1 ;
	}


	if ((adc_fpga_firmware_version & 0x0f00) == 0x0100) { //
		header_v0100_accu_4_values_enable_flag = 1 ;
		header_accu_6_values_enable_flag = 0 ;
		header_accu_2_values_enable_flag = 0 ;
		header_maw_3_values_enable_flag  = 0;
		header_energy_filter_values_enable_flag  = 0 ;

	// data format
		header_length = 2;
		header_accu_6_values_offset = 2 ;
		header_accu_2_values_offset = 2 ;
		header_maw_3_values_offset  = 2 ;
		header_energy_values_offset = 2 ;		//  
	}
	else { // standard version
		header_v0100_accu_4_values_enable_flag = 0 ;
		header_accu_6_values_enable_flag = 0 ;
		header_accu_2_values_enable_flag = 0 ;
		header_maw_3_values_enable_flag  = 0;
		header_energy_filter_values_enable_flag  = 0 ;

		if (fChk_EventHitParameter_DataFormatBit0->IsOn() == kTRUE) {
			header_accu_6_values_enable_flag = 1;
		}

		if (fChk_EventHitParameter_DataFormatBit2->IsOn() == kTRUE) {
			header_maw_3_values_enable_flag = 1;
		}

		if (fChk_EventHitParameter_DataFormatBit3->IsOn() == kTRUE) {
			header_energy_filter_values_enable_flag = 1;
		}


	// data format
		header_length = 3;
		header_accu_6_values_offset = 2 ;
		header_accu_2_values_offset = 2 ;
		header_maw_3_values_offset  = 2 ;
		header_energy_values_offset = 2 ;		//  
	}






	data = 0 ;

	if (header_v0100_accu_4_values_enable_flag == 1) {
		header_length = header_length + 4 ;
		header_maw_3_values_offset  = header_maw_3_values_offset + 4 ;
		header_accu_2_values_offset  = header_accu_2_values_offset + 4 ;
		data = data + 0x1 ; // set bit 0
	}


	if (header_accu_6_values_enable_flag == 1) {
		header_length = header_length + 7 ;
		header_maw_3_values_offset  = header_maw_3_values_offset + 7 ;
		header_accu_2_values_offset  = header_accu_2_values_offset + 7 ;
		header_energy_values_offset  = header_energy_values_offset + 7 ;
		data = data + 0x1 ; // set bit 0
	}
	if (header_accu_2_values_enable_flag == 1) {
		header_length = header_length + 2 ;
		header_maw_3_values_offset  = header_maw_3_values_offset + 2 ;
		header_energy_values_offset  = header_energy_values_offset + 2 ;
		data = data + 0x2 ; // set bit 1
	}
	if (header_maw_3_values_enable_flag == 1) {
		header_length = header_length + 3 ;
		header_energy_values_offset  = header_energy_values_offset + 3 ;
		data = data + 0x4 ; // set bit 2
	}

	if (header_energy_filter_values_enable_flag == 1) {
		header_length = header_length + 2 ;
		data = data + 0x8 ; // set bit 2
	}

	if (maw_test_buffer_enable_flag == 1) {
		data = data + 0x10 ; // set bit 4
	}
	if (maw_test_buffer_energy_mux_flag == 1) {
		data = data + 0x20 ; // set bit 5
	}



	event_length = (header_length + (sample_length / 2)  + maw_test_buffer_length);

	data = data + (data << 8) + (data << 16) + (data << 24);
	// data Format
	header_offset_ushort_ptr = 2 * header_length ; //
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_DATAFORMAT_CONFIG_REG, data );
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_DATAFORMAT_CONFIG_REG, data );
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_DATAFORMAT_CONFIG_REG, data );
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_DATAFORMAT_CONFIG_REG, data );

	//printf("event_length = 0x%08x   header_length = 0x%08x   data_format  = 0x%08x \n", event_length, header_length, data);



	// MAW Test Buffer configuration
	data = maw_test_buffer_length + (maw_test_buffer_delay << 16) ;
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_MAW_TEST_BUFFER_CONFIG_REG, data );
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_MAW_TEST_BUFFER_CONFIG_REG, data );
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_MAW_TEST_BUFFER_CONFIG_REG, data );
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_MAW_TEST_BUFFER_CONFIG_REG, data );



	// Lemo out UO
	return_code = gl_sis3316_adc1->register_write( SIS3316_LEMO_OUT_UO_SELECT_REG, 0x4); //


// FIR Energy
	// set FIR Energy Setup
	for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
		data =    ((energy_decay_tau_table_val & FIR_ENERGY_MAX_TAU_TABLE) << 30)
			   +  ((energy_decay_tau_factor_val & FIR_ENERGY_MAX_TAU_FACTOR) << 24)
			   +  ((energy_additional_average_val  & 0x3) << 22)
			   +  ((energy_gap_val  & 0x3ff) << 12)
			   +  (energy_peaking_val & 0xfff) ; //
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_ENERGY_SETUP_REG, data) ;
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_ENERGY_SETUP_REG, data) ;
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_ENERGY_SETUP_REG, data) ;
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_ENERGY_SETUP_REG, data) ;
	}


// Accumulatoren
	for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
		for (i_accu=0; i_accu<6; i_accu++) {
			data =    (((accumulator_length[i_accu]  & 0x1ff) - 1) << 16)
			       +  (accumulator_start_index[i_accu] & 0xffff) ; //
			return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + (SIS3316_ADC_CH1_4_ACCUMULATOR_GATE1_CONFIG_REG + (i_accu*4)), data) ;
		}
	}

	

	// FIR Trigger
// disable all FIR Triggers
	data = 0x00000000 ;
	for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SUM_FIR_TRIGGER_THRESHOLD_REG, data );  // disable all ch_sum
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_TRIGGER_THRESHOLD_REG, data );  // disable ch1, 5, 9, 13
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_TRIGGER_THRESHOLD_REG, data );  // disable ch2, ..
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_TRIGGER_THRESHOLD_REG, data );  // disable ch3, ..
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_TRIGGER_THRESHOLD_REG, data );  // disable ch4, ..
	}

	// set HighEnergy Threshold
	data =  0x08000000 + (trigger_peaking_val * 0x1000) ; // gt 4096
	for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_HIGH_ENERGY_THRESHOLD_REG, data);
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_HIGH_ENERGY_THRESHOLD_REG, data);
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_HIGH_ENERGY_THRESHOLD_REG, data);
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_HIGH_ENERGY_THRESHOLD_REG, data);
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SUM_FIR_HIGH_ENERGY_THRESHOLD_REG, data);
	}

	// set FIR Trigger Setup
	for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SUM_FIR_TRIGGER_SETUP_REG, 0) ; // clear FIR Trigger Setup -> a following Setup will reset the logic !
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_TRIGGER_SETUP_REG, 0) ;
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_TRIGGER_SETUP_REG, 0) ;
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_TRIGGER_SETUP_REG, 0) ;
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_TRIGGER_SETUP_REG, 0) ;
	}

	// set FIR Trigger Setup
	for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
		data =  ((trigger_pulse_length_val & 0xff) << 24)   + ((trigger_gap_val  & 0x3ff) << 12)  + (trigger_peaking_val & 0x3ff) ; //
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SUM_FIR_TRIGGER_SETUP_REG, data) ;
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_TRIGGER_SETUP_REG, data) ;
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_TRIGGER_SETUP_REG, data) ;
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_TRIGGER_SETUP_REG, data) ;
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_TRIGGER_SETUP_REG, data) ;
	}

	// set Trigger Threshold
	trigger_threshold_val =  (trigger_peaking_val * trigger_threshold_adc_val) & 0x7ffffff ;   // bits 26:0
	trigger_threshold_val =  trigger_threshold_val + 0x08000000;   // bit is to 1 (zero line of MAW)
	if(trigger_CFD_enable_flag == 1) {
		trigger_threshold_val =  trigger_threshold_val + 0x30000000;   // CFD 50% enable 29:28
	}
	i_ch=0 ;
	for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
		data =  trigger_threshold_val ; //
		if (trigger_enable_flags[i_ch] == 1) {
			data =  data + 0x80000000 ; // Trigger Enable
		}
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_TRIGGER_THRESHOLD_REG, data) ;
		i_ch++ ;

		data =  trigger_threshold_val ; //
		if (trigger_enable_flags[i_ch] == 1) {
			data =  data + 0x80000000 ; // Trigger Enable
		}
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_TRIGGER_THRESHOLD_REG, data) ;
		i_ch++ ;

		data =  trigger_threshold_val ; //
		if (trigger_enable_flags[i_ch] == 1) {
			data =  data + 0x80000000 ; // Trigger Enable
		}
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_TRIGGER_THRESHOLD_REG, data) ;
		i_ch++ ;

		data =  trigger_threshold_val ; //
		if (trigger_enable_flags[i_ch] == 1) {
			data =  data + 0x80000000 ; // Trigger Enable
		}
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_TRIGGER_THRESHOLD_REG, data) ;
		i_ch++ ;
	}


	i_ch=16 ;
	for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
		data =  trigger_threshold_val ; //
		if (trigger_enable_flags[i_ch] == 1) {
			data =  data + 0x80000000 ; // Trigger Enable
		}
		return_code = gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SUM_FIR_TRIGGER_THRESHOLD_REG, data) ;
		i_ch++ ;
	}


	uint_soft_trigger_flag = 0;  //
	if (fChkKeyTrigger->IsOn() == kTRUE) {
		uint_soft_trigger_flag = 1;  //
	}

 
	
	// 	Event Configuration
	fpga_data[0] = 0 ;
	if (fChkInvertChannel[0]->IsOn() == kTRUE) { fpga_data[0] = fpga_data[0] + 0x1 ;   }
	if (fChkInvertChannel[1]->IsOn() == kTRUE) { fpga_data[0] = fpga_data[0] + 0x100 ; }
	if (fChkInvertChannel[2]->IsOn() == kTRUE) { fpga_data[0] = fpga_data[0] + 0x10000 ; }
	if (fChkInvertChannel[3]->IsOn() == kTRUE) { fpga_data[0] = fpga_data[0] + 0x1000000 ; }
	fpga_data[1] = 0 ;
	if (fChkInvertChannel[4]->IsOn() == kTRUE) { fpga_data[1] = fpga_data[1] + 0x1 ; }
	if (fChkInvertChannel[5]->IsOn() == kTRUE) { fpga_data[1] = fpga_data[1] + 0x100 ; }
	if (fChkInvertChannel[6]->IsOn() == kTRUE) { fpga_data[1] = fpga_data[1] + 0x10000 ; }
	if (fChkInvertChannel[7]->IsOn() == kTRUE) { fpga_data[1] = fpga_data[1] + 0x1000000 ; }
	fpga_data[2] = 0 ;
	if (fChkInvertChannel[8]->IsOn() == kTRUE) { fpga_data[2] = fpga_data[2] + 0x1 ; }
	if (fChkInvertChannel[9]->IsOn() == kTRUE) { fpga_data[2] = fpga_data[2] + 0x100 ; }
	if (fChkInvertChannel[10]->IsOn() == kTRUE) { fpga_data[2] = fpga_data[2] + 0x10000 ; }
	if (fChkInvertChannel[11]->IsOn() == kTRUE) { fpga_data[2] = fpga_data[2] + 0x1000000 ; }
	fpga_data[3] = 0 ;
	if (fChkInvertChannel[12]->IsOn() == kTRUE) { fpga_data[3] = fpga_data[3] + 0x1 ; }
	if (fChkInvertChannel[13]->IsOn() == kTRUE) { fpga_data[3] = fpga_data[3] + 0x100 ; }
	if (fChkInvertChannel[14]->IsOn() == kTRUE) { fpga_data[3] = fpga_data[3] + 0x10000 ; }
	if (fChkInvertChannel[15]->IsOn() == kTRUE) { fpga_data[3] = fpga_data[3] + 0x1000000 ; }

	data = 0 ;
	if (fChkIntertnalSumTrigger->IsOn() == kTRUE) {
		data = data + 0x02020202 ;
	}
	if (fChkInternalTrigger->IsOn() == kTRUE) {
		data = data + 0x04040404 ;
	}
	if (fChkExternalTrigger->IsOn() == kTRUE) {
		data = data + 0x08080808 ;
	}
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_EVENT_CONFIG_REG, data + fpga_data[0] ); //  Event Configuration
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_EVENT_CONFIG_REG, data + fpga_data[1] ); //
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_EVENT_CONFIG_REG, data  + fpga_data[2]); //
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_EVENT_CONFIG_REG, data  + fpga_data[3]); //




// Lemo Input "TI" configuration
	data = 0;
	if (fChkLemoInTiEnable->IsOn() == kTRUE) {
		data = data + 0x10 ;
	}
	return_code = gl_sis3316_adc1->register_write( SIS3316_NIM_INPUT_CONTROL_REG, data ); //



// Select LEMO Output "Co"
	data = 0x1 ; // Select Sample Clock
	return_code = gl_sis3316_adc1->register_write( SIS3316_LEMO_OUT_CO_SELECT_REG, data ); //

	// Enable LEMO Output "TO"
	data = 0xffff ; // Select all triggers
	return_code = gl_sis3316_adc1->register_write( SIS3316_LEMO_OUT_TO_SELECT_REG, data ); //

	// enbale external (global) functions
	data = 0x100 ; // enable "external Trigger function" (NIM In, if enabled and VME key write)
	data = data + 0x400 ; // enable "external Timestamp clear function" (NIM In, if enabled and VME key write)
	return_code = gl_sis3316_adc1->register_write( SIS3316_ACQUISITION_CONTROL_STATUS, data );

/**************************************************************************************************************/

 
 
	//nof_events_pro_bank	= fNumericEntries_SampleControl_NofEvents_ProBank->GetIntNumber(); // 
	if (uint_SampleControl_BankModus == 0) { // SINGLE_EVENT_SINGLE_BANK 
		nof_events_pro_bank	= 1; // 
	}
	else { // MULTI_EVENT_DOUBLE_BANK
		half_possibe_events_pro_bank = (SIS3316_ADC_MEMORY_BANK_32BIT_SIZE / event_length) / 2 ; // halfffull

		if (fChkNofEvents_ProBank->IsOn() == kTRUE)  {
		//if (fChkNofEvents_ProBank->IsOn() == kTRUE)  { // 

			gui_max_events_pro_bank = fNumericEntries_SampleControl_MaxNofEvents_ProBank->GetIntNumber() ; //

			if (gui_max_events_pro_bank >= half_possibe_events_pro_bank) {
				nof_events_pro_bank	= half_possibe_events_pro_bank; // 
			}
			else {
				nof_events_pro_bank	= gui_max_events_pro_bank; // 
			}
		}
		else {
				nof_events_pro_bank	= half_possibe_events_pro_bank; // 
		}
	}

	fNumericEntries_SampleControl_NofEvents_ProBank->SetIntNumber(nof_events_pro_bank); // 

	//address_threshold
	//address_threshold = (sample_length / 2) + header_length  ;  // one Hit/Event !
	address_threshold = (nof_events_pro_bank * event_length) - 1 ;  //    ;  // one Hit/Event !
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_ADDRESS_THRESHOLD_REG, address_threshold ); //
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_ADDRESS_THRESHOLD_REG, address_threshold); //
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_ADDRESS_THRESHOLD_REG, address_threshold ); //
	return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_ADDRESS_THRESHOLD_REG, address_threshold ); //

	printf("INFO:  trigger_gate_window_length = %08d    \n", trigger_gate_window_length);
	printf("INFO:  nof_events_pro_bank = %08d    event_length =%08d  address_threshold = 0x%08x\n", nof_events_pro_bank, event_length,  address_threshold, return_code);
	

	uint_WriteData_to_File_OpenFlag = 0 ;
	uint_WriteData_to_File_counter  = 0 ;
	if (uint_SampleControl_BankModus == 0) {
		uint_WriteData_to_File_EnableFlag    = 0 ;
		uint_WriteData_to_MultipleFiles_Flag = 0 ;
	}
	else {
		if (fChkWriteDataToFile->IsOn() == kTRUE)  {
			uint_WriteData_to_File_EnableFlag    = 1 ;
			if (fChkWriteMultipleFiles->IsOn() == kTRUE)  {
				uint_WriteData_to_MultipleFiles_Flag = 1 ;
			}
			else {
				uint_WriteData_to_MultipleFiles_Flag = 0 ;
			}
		}
		else {
			uint_WriteData_to_File_EnableFlag    = 0 ;	
			uint_WriteData_to_MultipleFiles_Flag = 0 ;
		}
	}

 
	char char_str_temp[512] ;
	if (uint_WriteData_to_File_EnableFlag == 1) {
		strcpy(char_WriteData_to_File_initialize_filename, fTextEntryDataFilePath->GetText()) ; //  
		for (i=0;i<strlen(char_WriteData_to_File_initialize_filename);i++) {
			if (char_WriteData_to_File_initialize_filename[i] == '.')  {
				break ;
			}
		}
		char_WriteData_to_File_initialize_filename[i] = '\0';   /* null character manually added */
	}



	SIS3316_Test_running_dim_widgets(kFALSE);

	/***********************   Start of SINGLE_EVENT_SINGLE_BANK Loop   **********************************************/

	if (uint_SampleControl_BankModus == 0) {

		#define SINGLE_EVENT_SINGLE_BANK
		#ifdef SINGLE_EVENT_SINGLE_BANK

		bank_buffer_counter=0;

		while (fSIS3316_Test1) {

			// Clear Timestamp  */
			return_code = gl_sis3316_adc1->register_write( SIS3316_KEY_TIMESTAMP_CLEAR , 0);  //

		// refresh PreTriggerDelay
			pre_trigger_delay = fNumericEntries_EventHitParameter[0]->GetIntNumber();
			if (old_pre_trigger_delay != pre_trigger_delay) {
				if (pre_trigger_delay > MAX_PRETRIGGER_DELAY) {
					pre_trigger_delay = MAX_PRETRIGGER_DELAY ;
					fNumericEntries_EventHitParameter[0]->SetIntNumber(pre_trigger_delay );
				}
				old_pre_trigger_delay = pre_trigger_delay ;
 				return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //
				return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //
				return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //
				return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //
			}


		// refresh MAW PreTriggerDelay
			maw_test_buffer_delay = fNumericEntries_EventHitParameter[4]->GetIntNumber();
			if (old_maw_test_buffer_delay != maw_test_buffer_delay) {
				if (maw_test_buffer_delay > 1022) {
					maw_test_buffer_delay = 1022 ;
					fNumericEntries_EventHitParameter[4]->SetIntNumber(maw_test_buffer_delay );
				}
				old_maw_test_buffer_delay = maw_test_buffer_delay ;
				data = maw_test_buffer_length + (maw_test_buffer_delay << 16) ;
				return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_MAW_TEST_BUFFER_CONFIG_REG, data );
				return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_MAW_TEST_BUFFER_CONFIG_REG, data );
				return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_MAW_TEST_BUFFER_CONFIG_REG, data );
				return_code = gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_MAW_TEST_BUFFER_CONFIG_REG, data );
			}


		// refresh DAC
			if (fChkDacInrementTest->IsOn() == kTRUE) {
				dac_offset = (dac_offset + 0x100) & 0xffff;
				fNumericEntriesAnalogOffset->SetIntNumber(dac_offset);
			}
			dac_offset = fNumericEntriesAnalogOffset->GetIntNumber();
			if (old_dac_offset != dac_offset) {
				old_dac_offset = dac_offset ;
				if (dac_offset > 0xffff) {
					dac_offset = 0xffff ;
					fNumericEntriesAnalogOffset->SetIntNumber(dac_offset );
				}
				// write adc DAC offset  
				for (i_ch=0;i_ch<16; i_ch++) {
					gl_sis3316_adc1->adc_dac_offset_ch_array[i_ch] = dac_offset ; //  
				}
				gl_sis3316_adc1->write_all_adc_dac_offsets() ; 
			}


			// refresh Gain/Termination
			// Gain/Termination
			data = 0 ;
			if (fChkInput_50Ohm->IsOn() == kFALSE) {
				data = 0x04 ;
			}

			if (fChkInput_5V_Range->IsOn() == kFALSE) {
				data = data + 0x01 ;
			}
			gain_termination_reg_val = data ;
		
			if (gain_termination_reg_val != gain_termination_reg_val) {
				for (i_ch=0;i_ch<16; i_ch++) {
					gl_sis3316_adc1->adc_gain_termination_ch_array[i_ch] = data ; //  
				}
				gl_sis3316_adc1->write_all_gain_termination_values() ; 
				old_gain_termination_val = gain_termination_reg_val ;
 				usleep(2000); // 2 ms
			}



		// start sampling
			return_code = gl_sis3316_adc1->register_write( SIS3316_KEY_DISARM_AND_ARM_BANK1, 0 ); // //  Arm

			if (uint_soft_trigger_flag == 1) {
				return_code = gl_sis3316_adc1->register_write( SIS3316_KEY_TRIGGER , 0);  //  Trigger
			}

		// wait for address threshold flag
			//printf("wait for address threshold flag\n");
			do {
				gSystem->ProcessEvents();  // handle GUI events
				return_code = gl_sis3316_adc1->register_read( SIS3316_ACQUISITION_CONTROL_STATUS, &data);
			} while (((data & 0x80000) == 0x0) && (fSIS3316_Test1 == kTRUE) && (return_code == 0)) ; // address Threshold ?
			//	Sleep(50); // 50ms
			if (return_code != 0) {
				fSIS3316_Test1 = kFALSE ;
			}

			return_code = gl_sis3316_adc1->register_write( SIS3316_KEY_DISARM, 0 );  //  Disarm
			if (fSIS3316_Test1 == kTRUE) {

				do {
					//usleep(50000); // 50ms
					gl_sis3316_adc1->register_read( SIS3316_ACQUISITION_CONTROL_STATUS, &data);
					//printf("SIS3316_KEY_DISARM   0x%08x     \n", data);
				} while ((data & 0x50000) != 0x0) ; // wait until logic is not Busy
				gSystem->ProcessEvents();  // handle GUI events
			

			// read all ADC channels which have data
				//req_nof_32bit_words =  (sample_length/2) +  header_length ;
				req_nof_32bit_words =  event_length ;
				for (i_ch=0; i_ch<16; i_ch++) {
					return_code = gl_sis3316_adc1->read_DMA_Channel_PreviousBankDataBuffer( 0 /*bank2_read_flag*/, i_ch /* 0 to 15 */, req_nof_32bit_words/*max_read_nof_words */, &got_nof_32bit_words, (unsigned int *) ushort_adc_buffer_array_ptr[i_ch] ) ;
					if (return_code != 0) {
						printf("Error: read_DMA_Channel_PreviousBankDataBuffer: i_ch %d  got_nof_32bit_words = 0x%08x  return_code = 0x%08x\n",i_ch,  got_nof_32bit_words, return_code);
						ch_data_valid[i_ch] = 0;
						fSIS3316_Test1 = kFALSE ;
					}
					else {
						if (got_nof_32bit_words >= req_nof_32bit_words) {
							ch_data_valid[i_ch] = 1;
						}
						else {
							ch_data_valid[i_ch] = 0;
						}
					}
				}





		/*************************************************************************************************************************/
			/*  Display Graph */
			//printf("Display Graph\n");

			  // X-axis
			  for (ui=0;ui<plot_length;ui++){
				gl_x[ui] = ui ;
			  }
			  // check min and max for y-cordiante
				ymin = 0 ;
				ymax = 0xffff ;
				if (fChkDisplayAutoZoom->IsOn() == kTRUE)  {
					ymax = 0 ;
					ymin = 0xffff ;
					for(i = 0; i < 16; i++){
						ushort_adc_buffer_ptr =  ushort_adc_buffer_array_ptr[i];
						if (fChkDisplayAdc[i]->IsOn() == kTRUE)  {
							if(ch_data_valid[i] == 1) {
								for(ui = header_offset_ushort_ptr; ui < plot_length+header_offset_ushort_ptr;ui++){
								  if(ushort_adc_buffer_ptr[ui] < ymin) ymin = ushort_adc_buffer_ptr[ui];
								  if(ushort_adc_buffer_ptr[ui] > ymax) ymax = ushort_adc_buffer_ptr[ui];
								}
							}
						}
					} // i
				}
				else {
				  ymax = fNumericEntriesGraph_Yaxis[0]->GetIntNumber();
				  ymin = fNumericEntriesGraph_Yaxis[1]->GetIntNumber();
				  if (ymin > ymax) {
					  fNumericEntriesGraph_Yaxis[0]->SetIntNumber(ymin); // Y-max
					  fNumericEntriesGraph_Yaxis[1]->SetIntNumber(ymax); // Y-min      }
					  ymax = fNumericEntriesGraph_Yaxis[0]->GetIntNumber();
					  ymin = fNumericEntriesGraph_Yaxis[1]->GetIntNumber();
					  ymax = ymax+1;
				  }
				}

				//printf("ymin = %d  ymax = %d    \n", ymin, ymax);
				ywidth = ymax - ymin ;
				y_delta = ywidth/17 ;

				fCanvas1->Clear();
				fCanvas1->cd(1);

				// Display axis
				for(i = 0; i < plot_length; i++){
					gl_y[i]  = ymin+((ymax-ymin)/2)  ;
				}
				gl_y[(plot_length/2)] = ymin ;
				gl_y[((plot_length/2))+1] = ymax ;
				fGraph_ch[16]->DrawGraph(plot_length,gl_x,gl_y,"AL");

			// Display channels
				for(i_ch = 0; i_ch < 16; i_ch++){
					if(ch_data_valid[i_ch] == 1) {
						ushort_adc_buffer_ptr =  ushort_adc_buffer_array_ptr[i_ch] + header_offset_ushort_ptr ;
						if (fChkDisplayAdc[i_ch]->IsOn() == kTRUE) {
							for (ui=0; ui<plot_length; ui++){
							  gl_y[ui] = (ushort_adc_buffer_ptr[ui] )  ;
							}
							fGraph_ch[i_ch]-> DrawGraph(plot_length, gl_x,gl_y,"L");
							fGraph_Text_ch[i_ch]->DrawLatex(plot_length+10,ymax-((16-i_ch)*y_delta),chkDisAdcLabel[i_ch]);
						}
					}

				}
				fCanvas1->Update();
				//fCanvas1->Modified();

		#ifdef TEST_PRINT
				for(i_ch = 0; i_ch < 16; i_ch++){
					if(ch_data_valid[i_ch] == 1) {
						ushort_adc_buffer_ptr =  ushort_adc_buffer_array_ptr[i_ch]  ;
						printf("ch%2d Channel ID = 0x%03x     \n", i_ch+1, ( ((unsigned int ) ushort_adc_buffer_ptr[0]) & 0xFFF0 ) >> 4);
					}
				}
				printf("\n");
		#endif




	/***************************************************************************************************/

		#ifdef MAW_GRAPH

				display_MAW_choice = fCombo_Display_MAW->GetSelected();
				if (display_MAW_choice > 0)  {
					if (fB_openfCanvas4WindowFlag == kFALSE) {
						for(i=0;i<MAX_ROOT_PLOT_MAW_LENGTH;i++) {
							gl_maw_x[i] = i ;
							gl_maw_y[i] = 0 ;
						}
						fCanvas4 = new TCanvas("fCanvas4","SIS3316 Filter: Moving Average Window", SIS3316_MAW_DATA_WINDOW_POSTION_X, SIS3316_MAW_DATA_WINDOW_POSTION_Y, SIS3316_MAW_WINDOW_WIDTH, SIS3316_MAW_DATA_WINDOW_HIGH);
						fB_openfCanvas4WindowFlag = kTRUE; //
					}
					fCanvas4->Clear();
					fCanvas4->cd(1);
					fCanvas4->SetGrid();
					fCanvas4->SetFillColor(DefineCanvasBackgroundColor);

					fGraph_maw = new TGraph( MAX_ROOT_PLOT_MAW_LENGTH, gl_maw_x, gl_maw_y);
					//this->gr1->SetTitle(graph_text);
					fGraph_maw->SetLineColor(DefineChannel_1_Color);

					fCanvas4->Update();
				}
				else {
					if (fB_openfCanvas4WindowFlag == kTRUE) {
						delete fCanvas4 ;
						fB_openfCanvas4WindowFlag = kFALSE; //
					}
				}

				ushort_adc_buffer_ptr =  ushort_adc_buffer_array_ptr[display_MAW_choice-1];
				uint_maw_ptr = (unsigned int*) ushort_adc_buffer_ptr ;

				if ((display_MAW_choice > 0) && (ch_data_valid[display_MAW_choice-1] == 1)) {
					for (ui=0; ui<maw_test_buffer_length; ui++){
					  gl_maw_y[ui] = uint_maw_ptr[ui + header_length + (sample_length/2)] - uint_trigger_maw_offset ; //  0x08000000 or 0x0
					}
					fGraph_maw->DrawGraph(maw_test_buffer_length, gl_maw_x,gl_maw_y,"AL");
					fCanvas4->Update();
				}
		#endif

	/***************************************************************************************************/
		#ifdef FFT_GRAPH

				display_FFT_choice = fCombo_Display_FFT_Ch->GetSelected();
				if (display_FFT_choice > 0)  {

					if (fB_openfCanvas3WindowFlag == kFALSE) {
						fCanvas3 = new TCanvas("fCanvas3","Fast Fourier Transform ", SIS3316_FFT_WINDOW_POSTION_X, SIS3316_FFT_WINDOW_POSTION_Y, SIS3316_FFT_WINDOW_WIDTH, SIS3316_FFT_WINDOW_HIGH);
						fB_openfCanvas3WindowFlag = kTRUE; //
						fChkFFT_Sum->SetState(kButtonUp)   ; // is Off !
						fChkFFTLogY->SetState(kButtonUp)   ; // is Off !
						fChkFFT_Sum->SetEnabled(kFALSE)   ; //
						fChkFFTLogY->SetEnabled(kFALSE)   ; //
						fChkFFT_Db->SetEnabled(kTRUE)   ; //
						fChkFFT_Db->SetState(kButtonDown)   ; // is On !
					}
					fCanvas3->Clear();
					fCanvas3->cd(1);
					fCanvas3->SetGrid();
					fCanvas3->SetFillColor(DefineCanvasBackgroundColor);

					fCanvas3->SetFrameBorderMode(1);
					fCanvas3->SetFrameBorderSize(6);
					//fCanvas3->SetFrameFillColor(21);
					//fCanvas3->SetFrameFillColor(TColor::GetColor(240,240,240));
					fCanvas3->SetFrameFillColor(DefineCanvasBackgroundColor);
					//gPad->SetGrid();
					//  gPad->SetFrameFillColor(19);
					   //TGVerticalFrame *controls = new TGVerticalFrame(this);
					fCanvas3->Update();
				}
				else {
					if (fB_openfCanvas3WindowFlag == kTRUE) {
						delete fCanvas3 ;
						fB_openfCanvas3WindowFlag = kFALSE; //
						fChkFFT_Db->SetEnabled(kTRUE)   ; //
						if (fChkFFT_Db->IsOn() == kTRUE)  {
							fChkFFT_Sum->SetEnabled(kFALSE)   ; //
							fChkFFTLogY->SetEnabled(kFALSE)   ; //
						}
						else {
							fChkFFT_Sum->SetEnabled(kTRUE)   ; //
							fChkFFTLogY->SetEnabled(kTRUE)   ; //
						}
					}
				}

				fft_plot_length = sample_length / 2 ;
				// printf("fft_plot_length = %d\n", fft_plot_length);


				//if (display_FFT_choice > 0) {
				if ((display_FFT_choice > 0) && (ch_data_valid[display_FFT_choice-1] == 1)) {
					ushort_adc_buffer_ptr =  ushort_adc_buffer_array_ptr[display_FFT_choice-1] + header_offset_ushort_ptr;

					int int_sum_value ;
					int_sum_value = 0 ;
					for(ui = 0;ui < sample_length;ui++){
						int_sum_value =  int_sum_value + (unsigned int)(ushort_adc_buffer_ptr[ui] & 0xFFFF) ;
					}
					int_sum_value =  ((int_sum_value   /  sample_length) );

					unsigned int uint_value ;
					int int_value ;
					for(ui = 0;ui < sample_length;ui++){
						uint_value =  (unsigned int)(ushort_adc_buffer_ptr[ui] & 0xFFFF) ;
						int_value =  uint_value ;
						int_save_adc_buffer[ui] =   int_value - int_sum_value  ;
					}

				//#ifdef Window_FUNCTION
					#define M_PI       3.14159265358979323846
					double double_a0, double_a1, double_a2, double_a3 ;
					double double_window_correctur_factor ;
					double_window_correctur_factor = 1.0 ;

					for(ui = 0;ui < sample_length;ui++){
						double_window_weight[ui] =  1.015 ; // no window
					}
					display_FFT_Window_choice = fCombo_Display_FFT_Window->GetSelected();
					switch(display_FFT_Window_choice) {
						case 1: //Hamming window
							double_window_correctur_factor = 1.368 * 1.368 ;
							for(ui = 0;ui < sample_length;ui++){
								double_window_weight[ui] =  0.54 - (0.46 * (cos((2 * M_PI * ui) / (sample_length-1) )))  ; // Hamming Window
								//printf ( "1  %3d  %12f \n", ui, double_window_weight[ui] );
							}
							break;
						case 2: //Hann window
							double_window_correctur_factor = 1.419  * 1.419 ;
							for(ui = 0;ui < sample_length;ui++){
								double_window_weight[ui] =  0.5 * (1 - cos( (2 * M_PI * ui) / (sample_length-1) ))  ; // HAnn Window
								//printf ( "2  %3d  %12f \n", ui, double_window_weight[ui] );
							}
							break;
						case 3: //Blackmann window
							double_window_correctur_factor = 1.5365 * 1.5365 ;
							double_a0 = 7938.0/18608.0 ;
							double_a1 = 9240.0/18608.0 ;
							double_a2 = 1430.0/18608.0 ;
							for(ui = 0;ui < sample_length;ui++){
								double_window_weight[ui] =  double_a0 - (double_a1 * cos( (2 * M_PI * ui) / (sample_length-1) ))  + (double_a2 * cos( (4 * M_PI * ui) / (sample_length-1) )); // Blackmann Window
								//printf ( "2  %3d  %12f \n", ui, double_window_weight[ui] );
							}
							break;
						case 4: //Blackmann-Harris window
							double_window_correctur_factor = 1.6697 * 1.6697 ;
							double_a0 = 0.35875 ;
							double_a1 = 0.48829 ;
							double_a2 = 0.14128 ;
							double_a3 = 0.01168 ;
							for(ui = 0;ui < sample_length;ui++){
								double_window_weight[ui] =  double_a0 - (double_a1 * cos( (2 * M_PI * ui) / (sample_length-1) ))  + (double_a2 * cos( (4 * M_PI * ui) / (sample_length-1) )) - (double_a3 * cos( (6 * M_PI * ui) / (sample_length-1) )) ; // Blackmann-Harris Window
								//printf ( "2  %3d  %12f \n", ui, double_window_weight[ui] );
							}
							break;
					}

				//#endif
				// FFT
					for(ui = 0;ui < sample_length;ui++){
						fftw_complex_in[ui][0] =  (double) int_save_adc_buffer[ui]  * double_window_weight[ui] ; // real.
						fftw_complex_in[ui][1] =  (double) 0.0 ; // imag..
					}

					p = fftw_plan_dft_1d (sample_length, fftw_complex_in, fftw_complex_out, FFTW_FORWARD, FFTW_ESTIMATE);
					fftw_execute(p);

					for(ui = 1;ui < fft_plot_length;ui++){
						//double_fft_spectrum[ui] = sqrtf ( (fftw_complex_out[ui][0] * fftw_complex_out[ui][0]) + (fftw_complex_out[ui][1]  * fftw_complex_out[ui][1] ))  / (plot_length/2) ;
						//double_fft_spectrum[ui] = double_window_correctur_factor * 2 *  (sqrtf  ( (fftw_complex_out[ui][0] * fftw_complex_out[ui][0]) + (fftw_complex_out[ui][1]  * fftw_complex_out[ui][1]) )  / fft_plot_length) ;
						double_fft_spectrum[ui] =  double_window_correctur_factor * (double)((sqrtf  ( (fftw_complex_out[ui][0] * fftw_complex_out[ui][0]) + (fftw_complex_out[ui][1]  * fftw_complex_out[ui][1]) )  / (fft_plot_length/2))) ;
						//printf ( "  %3d  %12f  %12f  %12f\n", ui, fftw_complex_out[ui][0], fftw_complex_out[ui][1], spectrum[ui] );
					}
					fftw_destroy_plan(p) ;

					fCanvas3->Clear();
					fCanvas3->cd(1);
				//
					double log10_value ;
					double noise_floor;
					log10_value = log10 ((float)(sample_length/2));

					if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
						noise_floor = ((6.02*16) + 1.76 + (10 * log10_value)) ; // 16-bit ADC
					}
					else {
						noise_floor = ((6.02*14) + 1.76 + (10 * log10_value)) ; // 14-bit ADC
					}
					printf("noise_floor = %f\n", noise_floor);

					for (ui=0;ui<fft_plot_length;ui++){
						  gl_float_x[ui] = (float) ((ui * double_fft_frequency) / fft_plot_length / 2 ) ;
						  if (fChkFFT_Db->IsOn() == kTRUE)  {
							   if (gl_sis3316_adc1->adc_125MHz_flag == 1) {
									gl_float_y[ui] = (float)(20.0 * ( log10 (double_fft_spectrum[ui] / 65535.0) ))  ; // Amplitude Spectrum 16 bit
									//	gl_float_y[ui] = 10.0 * ( log10 ((double_fft_spectrum[ui] * double_fft_spectrum[ui]  ) / (65535.0 * 65535.0)) ) ; // Power Spectrum
							   }
							   else {
									gl_float_y[ui] = (float)(20.0 * ( log10 (double_fft_spectrum[ui] / 16383.0) )) ; // Amplitude Spectrum 14 bit
							   }
						  }
						  else {
								if (fChkFFT_Sum->IsOn() == kTRUE)  {
									gl_float_y[ui] = gl_float_y[ui] + (Float_t)double_fft_spectrum[ui] ;
								}
								else {
									gl_float_y[ui] = (Float_t)double_fft_spectrum[ui]  ;
								}
							}
					}

					if (fChkFFT_AutoScale->IsOn() == kFALSE)  {
						fGraph_fft[1]-> DrawGraph(fft_plot_length-1, &gl_float_x[1], &gl_float_y1[1],"AL");
						fGraph_fft[0]-> DrawGraph(fft_plot_length-1, &gl_float_x[1], &gl_float_y[1],"L");
					}
					else {
						fGraph_fft[0]-> DrawGraph(fft_plot_length-1, &gl_float_x[1], &gl_float_y[1],"AL");
					}
					//fGraph_fft[0]->GetXaxis()->SetLimits(0.0, gl_float_y[fft_plot_length-1]);
					//fGraph_fft[0]->GetHistogram()->SetMaximum(0.0);
					//fGraph_fft[0]->GetHistogram()->SetMinimum(-140.0);
					//fGraph_fft[0]-> Draw("AL");
					//fGraph_fft[0]-> DrawGraph(fft_plot_length-1, &gl_float_x[1], &gl_float_y[1],"L");
					//fGraph_fft[0]-> DrawF1(fft_plot_length-1, &gl_float_x[1], &gl_float_y[1],"AL");
					fCanvas3->Update();
				}
		#endif

	
				gSystem->ProcessEvents();  // handle GUI events

		#ifdef HISTOGRAM
				new_display_histogram_choice = fCombo_Display_Histos_Ch->GetSelected();

				if (new_display_histogram_choice > 0)  {

					if (fB_openfCanvas2WindowFlag == kFALSE) {
						fCanvas2 = new TCanvas("fCanvas2","Histogram ", SIS3316_HISTOGRAM_WINDOW_POSTION_X, SIS3316_HISTOGRAM_WINDOW_POSTION_Y, SIS3316_HISTOGRAM_WINDOW_WIDTH, SIS3316_HISTOGRAM_WINDOW_HIGH);
						fB_openfCanvas2WindowFlag = kTRUE; // Setup
					}
					if (display_histogram_choice == 17) {
						fCanvas2->Clear();
						fCanvas2->Divide(2,8);
					}
					else {
						fCanvas2->Clear();
						fCanvas2->Divide(1);
					}

					fCanvas2->SetGrid();
					fCanvas2->SetFillColor(DefineCanvasBackgroundColor);
					for (i=0;i<16;i++) {
						  fCanvas2->cd(1+i);
						  gPad->SetGrid();
						  gPad->SetFillColor(DefineCanvasBackgroundColor);
					}


					for(i = 0; i < 16; i++){
						if(fChkHistoSum->IsOn() == kFALSE)  {
							iHistoAdc[i]->Reset(); //
							iHistoAdc[i]->BufferEmpty(1); // action =  1 histogram is filled and buffer is deleted
						}

						if(ch_data_valid[i] == 1) {
							ushort_adc_buffer_ptr =  ushort_adc_buffer_array_ptr[i] + header_offset_ushort_ptr;
							for (ui=0;ui<plot_length;ui++){
								iHistoAdc[i]->Fill((int)ushort_adc_buffer_ptr[ui]);
							}

							double_histo_mean = iHistoAdc[i]->GetMean(1); //
							if (double_histo_mean < double_histo_min_mean[i]) {
								double_histo_min_mean[i] = double_histo_mean ;
							}
							if (double_histo_mean > double_histo_max_mean[i]) {
								double_histo_max_mean[i] = double_histo_mean ;
							}



							histo_pave_text[i]->Clear();
							histo_pave_text[i]->SetTextSize(0.04);
							sprintf(char_temp,"current mean %5.1f",double_histo_mean);
							histo_pave_text[i]->AddText(char_temp);
							sprintf(char_temp,"minimal mean %5.1f",double_histo_min_mean[i]);
							histo_pave_text[i]->AddText(char_temp);
							sprintf(char_temp,"maximal mean %5.1f",double_histo_max_mean[i]);
							histo_pave_text[i]->AddText(char_temp);

							if(fChkHistoZoomMean->IsOn() == kTRUE)  {
								double_histo_mean = iHistoAdc[i]->GetMean(1); //
								//printf("double_histo_mean = %f\n", double_histo_mean);
								double_histo_min_x = 0.0;
								// double_histo_max_x = 65535.0;
								// double_histo_max_x = 16384;
								double_histo_max_x = (double) (i_histo_length-1) ; //  65535.0;
								if (double_histo_mean > 30.0) { double_histo_min_x = double_histo_mean - 30.0;}
								//if (double_histo_mean < 65515.0) { double_histo_max_x = double_histo_mean + 20.0;}
								if (double_histo_mean < (double) (i_histo_length-30)) { double_histo_max_x = double_histo_mean + 30.0;}
								iHistoAdc[i]->SetAxisRange(double_histo_min_x,double_histo_max_x,"X"); //
							}
							else {
								double_histo_min_x = 0.0;
								//double_histo_max_x = 16384;
								double_histo_max_x = (double) (i_histo_length-1);
								iHistoAdc[i]->SetAxisRange(double_histo_min_x,double_histo_max_x,"X"); //
							}
						}

					}

				// display histograms
					display_histo_counter++;
					//if (display_histo_counter > 20) {
					if (display_histo_counter >= 0) {
						//printf("\ndisplay_histo_counter  %d \n",display_histo_counter);
						display_histo_counter=0;

						new_display_histogram_choice = fCombo_Display_Histos_Ch->GetSelected();
						if (new_display_histogram_choice != display_histogram_choice) {
							display_histogram_choice = new_display_histogram_choice ;
							changed_display_histogram_choice_flag = 1 ;
						}
						else {
							changed_display_histogram_choice_flag = 0 ;
						}

						if (changed_display_histogram_choice_flag == 1) {
							if (display_histogram_choice == 17) {
								fCanvas2->Clear();
								fCanvas2->Divide(2,8);
							}
							else {
								fCanvas2->Clear();
								fCanvas2->Divide(1);
							}
						}

						if (fChkHistoGaussFit->IsOn() == kTRUE)  {
						  histogram_gausfit_enable_flag = 1 ;
						  histogram_gausfit_clear_flag  = 0 ;
						}
						else {
						  if (histogram_gausfit_enable_flag == 1) {
							histogram_gausfit_clear_flag  = 1 ;
						  }
						  histogram_gausfit_enable_flag = 0 ;
						}

						//printf("display_histogram_choice = %d\n", display_histogram_choice);

						if (display_histogram_choice == 17) {
							for (i=0;i<16;i++) {
								fCanvas2->cd(1+display_histo_ch_no);
								if (histogram_gausfit_enable_flag == 1)  {
								  iHistoAdc[display_histo_ch_no]->Fit("gaus","Q");
								}
								if (histogram_gausfit_clear_flag == 1)  {
								  iHistoAdc[display_histo_ch_no]->Fit("gaus","0","Q");
								  histogram_gausfit_clear_flag = 0 ;
								}

								iHistoAdc[display_histo_ch_no]->Draw();
								histo_pave_text[display_histo_ch_no]->Draw();
								//printf("display_histo_ch_no = %d\n",display_histo_ch_no);
								display_histo_ch_no++ ;
								if (display_histo_ch_no > 15) {
									display_histo_ch_no = 0 ;
								}
							}
						}
						else {
							display_histo_ch_no = display_histogram_choice-1;
							//fCanvas2->Clear();
							//fCanvas2->Divide(1);
							fCanvas2->cd(1);
							if (histogram_gausfit_enable_flag == 1)  {
							  iHistoAdc[display_histo_ch_no]->Fit("gaus","Q");
							}
							if (histogram_gausfit_clear_flag == 1)  {
							  iHistoAdc[display_histo_ch_no]->Fit("gaus","0","Q");
							  histogram_gausfit_clear_flag = 0 ;
							}
							iHistoAdc[display_histo_ch_no]->Draw();
							histo_pave_text[display_histo_ch_no]->Draw();
						}
						  fCanvas2->Update();
						//fCanvas2->Modified();
					}
				}
				else {
					if (fB_openfCanvas2WindowFlag == kTRUE) {
						delete fCanvas2 ;
						fB_openfCanvas2WindowFlag = kFALSE; //
					}
				}

		#endif
			}
			gSystem->ProcessEvents();  // handle GUI events

			bank_buffer_counter++;
			if (fChkStopAfterBanks->IsOn() == kTRUE)  {
				 if (bank_buffer_counter > fNumericEntriesStopAfterBanks->GetIntNumber() ) {
					fSIS3316_Test1 = kFALSE ;
				 }
			}

		} //while (fSIS3316_Test1)
		#endif //#ifdef SINGLE_EVENT_SINGLE_BANK

	} //if (uint_SampleControl_BankModus == 0)

	/***********************   End of SINGLE_EVENT_SINGLE_BANK Loop   **********************************************/







	/***********************   Start of MULTI_EVENT_DOUBLE_BANK Loop   **********************************************/


	plot_counter=0;
	bank_buffer_counter=0;


	if (uint_SampleControl_BankModus == 1) {
		#define MULTI_EVENT_DOUBLE_BANK
		#ifdef MULTI_EVENT_DOUBLE_BANK


 	//printf("uint_WriteData_to_File_EnableFlag = %08d   \n", uint_WriteData_to_File_EnableFlag);
 	//printf("uint_WriteData_to_MultipleFiles_Flag = %08d   \n", uint_WriteData_to_MultipleFiles_Flag);
 	//printf("uint_WriteData_to_File_OpenFlag = %08d   \n", uint_WriteData_to_File_OpenFlag);
 	//printf("uint_WriteData_to_File_counter = %08d   \n", uint_WriteData_to_File_counter);


		// Clear Timestamp  */
		return_code = gl_sis3316_adc1->register_write( SIS3316_KEY_TIMESTAMP_CLEAR , 0);  //  

		// Start Readout Loop  */
		return_code = gl_sis3316_adc1->register_write( SIS3316_KEY_DISARM_AND_ARM_BANK1 , 0);  //  Arm Bank1
		bank1_armed_flag = 1; // start condition
		//printf("SIS3316_KEY_DISARM_AND_ARM_BANK1 \n");



//		Raw data
		// X-axis
		for (ui=0;ui<plot_length;ui++){
			gl_x[ui] = ui ;
		}
		fChkDisplayAutoZoom->SetState(kButtonUp)   ; // is off !


	unsigned int  *uint_firFilter_energy_histogram_start_ptr;
	//unsigned int  *uint_accumulator_energy_histogram_start_ptr;
	//unsigned int  *uint_peakHeight_energy_histogram_start_ptr;
	#define HISTOGRAM_ENERGY_MAX_LENGTH				65536    //

		if (header_energy_filter_values_enable_flag == 1) {
			uint_firFilter_energy_histogram_start_ptr = (unsigned int*) malloc(sizeof(unsigned int)*HISTOGRAM_ENERGY_MAX_LENGTH*16); // all 16 channels
			for (i=0;i<HISTOGRAM_ENERGY_MAX_LENGTH*16;i++) {
				uint_firFilter_energy_histogram_start_ptr[i] = 0 ;
			}
		}


		while (fSIS3316_Test1) {

			poll_counter = 0 ;
			do {

				// generates triggers if it is enabled
				if (uint_soft_trigger_flag == 1) {
					return_code = gl_sis3316_adc1->register_write( SIS3316_KEY_TRIGGER , 0);  //  Trigger
					gSystem->ProcessEvents();  // handle GUI events
					//usleep(1000) ;
				}
				poll_counter=100;
				do {
					gSystem->ProcessEvents();  // handle GUI events
					poll_counter--;
				} while (poll_counter != 0) ;
				//poll_counter++;
				//if (poll_counter == 100) {
					gSystem->ProcessEvents();  // handle GUI events
				//	poll_counter = 0 ;
    			//}
				return_code = gl_sis3316_adc1->register_read (SIS3316_ACQUISITION_CONTROL_STATUS, &data);  
			} while (((data & 0x80000) == 0x0) && (fSIS3316_Test1 == kTRUE) && (return_code == 0)) ; // address Threshold ?

			if (return_code != 0) {
				fSIS3316_Test1 = kFALSE ;
			}

			if (fSIS3316_Test1 == kTRUE) {

				// bank swap command
				if (bank1_armed_flag == 1) {
					return_code = gl_sis3316_adc1->register_write(SIS3316_KEY_DISARM_AND_ARM_BANK2 , 0);  //  Arm Bank2
					bank1_armed_flag = 0; // bank 2 is armed
					printf("SIS3316_KEY_DISARM_AND_ARM_BANK2 \n");
				}
				else {
					return_code = gl_sis3316_adc1->register_write(SIS3316_KEY_DISARM_AND_ARM_BANK1 , 0);  //  Arm Bank1
					bank1_armed_flag = 1; // bank 1 is armed
					printf("SIS3316_KEY_DISARM_AND_ARM_BANK1 \n");
				}
 
				// prepare file write
				if (uint_WriteData_to_File_EnableFlag == 1) {   ; //  
					if (uint_WriteData_to_File_OpenFlag == 0) {   // first time
						if (uint_WriteData_to_MultipleFiles_Flag == 0) {   //  
							sprintf(char_WriteData_to_File_filename,"%s.dat",char_WriteData_to_File_initialize_filename ) ;
						}
						else {
							sprintf(char_WriteData_to_File_filename,"%s_%d.dat",char_WriteData_to_File_initialize_filename ,  uint_WriteData_to_File_counter) ;
						}
						file_WriteData_to_File_Pointer = fopen(char_WriteData_to_File_filename,"wb") ;
						uint_WriteData_to_File_OpenFlag = 1 ;
					}
					else { // file is open
						if (uint_WriteData_to_MultipleFiles_Flag == 1) {   //  
							if (uint_WritenData_to_File_32bit_words > WRITE_DTATA_TO_FILE_MAX_32BIT_WORDS) {
								uint_WritenData_to_File_32bit_words = 0 ;
								fclose(file_WriteData_to_File_Pointer);
								uint_WriteData_to_File_counter++;
								sprintf(char_WriteData_to_File_filename,"%s_%d.dat",char_WriteData_to_File_initialize_filename ,  uint_WriteData_to_File_counter) ;
								file_WriteData_to_File_Pointer = fopen(char_WriteData_to_File_filename,"wb") ;
							}
						}
					}
				}



			// prepare Raw data graph
				ymax = fNumericEntriesGraph_Yaxis[0]->GetIntNumber();
				ymin = fNumericEntriesGraph_Yaxis[1]->GetIntNumber();
				if (ymin > ymax) {
					fNumericEntriesGraph_Yaxis[0]->SetIntNumber(ymin); // Y-max
					fNumericEntriesGraph_Yaxis[1]->SetIntNumber(ymax); // Y-min      }
					ymax = fNumericEntriesGraph_Yaxis[0]->GetIntNumber();
					ymin = fNumericEntriesGraph_Yaxis[1]->GetIntNumber();
					ymax = ymax+1;
				}

				ywidth = ymax - ymin ;
				y_delta = ywidth/17 ;
				fCanvas1->Clear();
				fCanvas1->cd(1);

				// Display axis
				for(i = 0; i < plot_length; i++){
					gl_y[i]  = ymin+((ymax-ymin)/2)  ;
				}
				gl_y[(plot_length/2)] = ymin ;
				gl_y[((plot_length/2))+1] = ymax ;
				fGraph_ch[16]->DrawGraph(plot_length,gl_x,gl_y,"AL");

				unsigned int max_req_nof_32bit_words ;
				unsigned int ch_event_counter ;
				unsigned int uint_channel_event_counter[16] ;
				for (i_ch=0; i_ch<16; i_ch++) {
					uint_channel_event_counter[i_ch]  = 0; // channel event/hit counter
				} // loop over n channels

				//max_req_nof_32bit_words = nof_events_pro_bank * event_length ; // max_request is limited by request nof_events
				max_req_nof_32bit_words = SIS3316_ADC_MEMORY_BANK_32BIT_SIZE ; // max_request is limited by Memory Banks_Size

				// loop over n channels
				for (i_ch=0; i_ch<16; i_ch++) {
					gSystem->ProcessEvents();  // handle GUI events
					// read channel events
					return_code = gl_sis3316_adc1->read_DMA_Channel_PreviousBankDataBuffer(bank1_armed_flag /*bank2_read_flag*/, i_ch /* 0 to 15 */, max_req_nof_32bit_words, &got_nof_32bit_words, gl_rblt_data ) ; // read maximun (all) events
					if (return_code != 0) {
						printf("read_DMA_Channel_PreviousBankDataBuffer: return_code = 0x%08x\n", return_code);
						fSIS3316_Test1 = kFALSE ;
					}
					ch_event_counter = (got_nof_32bit_words  / event_length) ;


					if (ch_event_counter > 0) {
						//printf("read_Channel_PreviousBankDataBuffer: i_ch %d  got_nof_32bit_words = 0x%08x  return_code = 0x%08x\n",i_ch+1,  got_nof_32bit_words, return_code);
						uint_channel_event_counter[i_ch]  = uint_channel_event_counter[i_ch] + ch_event_counter; // channel event/hit counter


						 printf("ch %d   event_length = 0x%08x\n", i_ch+1,   event_length);
					// file write
						if (uint_WriteData_to_File_OpenFlag == 1) {   ; //  
							uint_WritenData_to_File_32bit_words += SIS3316_WriteBankChannelHeaderToDataFile (file_WriteData_to_File_Pointer, file_header_indentifier, bank_buffer_counter, i_ch, ch_event_counter , event_length, maw_test_buffer_length, 0 /* reserved */) ;
							uint_WritenData_to_File_32bit_words += SIS3316_WriteBankChannelEventBufferToDataFile (file_WriteData_to_File_Pointer, gl_rblt_data, got_nof_32bit_words)  ;
						}
 						//printf("uint_WritenData_to_File_32bit_words = 0x%08x\n", uint_WritenData_to_File_32bit_words);

					// save  Timestamp of first Event

						//unsigned int bank_channel_firstTimestamp_upper[16] ;
						//unsigned int bank_channel_firstTimestamp_lower[16] ;
						//double double_bank_channel_firstTimestamp_timestamp_sec[16] ;

						//bank_channel_firstTimestamp_upper[i_ch]   = (gl_rblt_data[0] & 0xffff0000)  >> 16 ;
						//bank_channel_firstTimestamp_lower[i_ch]   =  gl_rblt_data[1]  ;
						//double_bank_channel_firstTimestamp_timestamp_sec[i_ch] = ( (4294967296.0 * bank_channel_firstTimestamp_upper[i_ch] ) + (bank_channel_firstTimestamp_lower[i_ch]) ) / (double_fft_frequency)  ; //  
						//printf("ch %d   double_bank_channel_firstTimestamp_timestamp_sec = %10.9f    \n", i_ch+1, double_bank_channel_firstTimestamp_timestamp_sec[i_ch]);
						//printf("ch %d   bank_channel_firstTimestamp_upper = 0x%04x  bank_channel_firstTimestamp_lower[i_ch] = 0x%08x  \n", i_ch+1, bank_channel_firstTimestamp_upper[i_ch], bank_channel_firstTimestamp_lower[i_ch]  );
						//printf("\n");

					// Raw data graph
						if (fChkDisplayAdc[i_ch]->IsOn() == kTRUE) {
							for (i=0; i<ch_event_counter; i++ ) {
								if (i>10) {break ; } // plot ony 10 events
								ushort_adc_buffer_ptr =  (unsigned short*) (&gl_rblt_data[i*(event_length) + header_length]) ;
								for (ui=0; ui<plot_length; ui++){
								  gl_y[ui] = (ushort_adc_buffer_ptr[ui] )  ;
								}
								fGraph_ch[i_ch]-> DrawGraph(plot_length, gl_x,gl_y,"L");
							}
							fGraph_Text_ch[i_ch]->DrawLatex(plot_length+10,ymax-((16-i_ch)*y_delta),chkDisAdcLabel[i_ch]);
							fCanvas1->Update();
						}



						//volatile unsigned int uint_save_energy_max ;
						unsigned int uint_energy_max ;

						// Fir Filter Energy histogram
						if (header_energy_filter_values_enable_flag == 1) {
							for (i=0; i<ch_event_counter; i++) {
								uint_energy_max = gl_rblt_data[(i*event_length) + header_energy_values_offset+1]  ; 
								//uint_save_energy_max = uint_energy_max;
								//uint_energy_max = uint_energy_max / energy_peaking_val ;
								uint_energy_max = (uint_energy_max / fNumericEntriesHistogramXaxisDivider->GetNumber() ) +  fNumericEntriesHistogramXaxisOffset->GetIntNumber();
								//if (uint_energy_max < 200) {
					//printf("uint_energy_max  %d \n",uint_energy_max);
								//}
								if ((uint_energy_max > 0) && (uint_energy_max<HISTOGRAM_ENERGY_MAX_LENGTH)) {
									uint_firFilter_energy_histogram_start_ptr[(i_ch*HISTOGRAM_ENERGY_MAX_LENGTH)+uint_energy_max]++;
								}
							}
						}

						// PeakHeigh Energy abd accumulator (integral) histogram
						if (header_accu_6_values_enable_flag == 1) {
						}




					} //if (ch_event_counter > 0) {

				} // loop over n channels

			} // if (fSIS3316_Test1 == kTRUE) {


// display histograms
				new_display_histogram_choice = fCombo_Display_Histos_Ch->GetSelected();

				if (new_display_histogram_choice > 0)  {

					if (fB_openfCanvas2WindowFlag == kFALSE) {
						fCanvas2 = new TCanvas("fCanvas2","Histogram ", SIS3316_HISTOGRAM_WINDOW_POSTION_X, SIS3316_HISTOGRAM_WINDOW_POSTION_Y, SIS3316_HISTOGRAM_WINDOW_WIDTH, SIS3316_HISTOGRAM_WINDOW_HIGH);
						fB_openfCanvas2WindowFlag = kTRUE; // Setup
					}
					if (display_histogram_choice == 17) {
						fCanvas2->Clear();
						fCanvas2->Divide(2,8);
					}
					else {
						fCanvas2->Clear();
						fCanvas2->Divide(1);
					}

					fCanvas2->SetGrid();
					fCanvas2->SetFillColor(DefineCanvasBackgroundColor);
					for (i=0;i<16;i++) {
						  fCanvas2->cd(1+i);
						  gPad->SetGrid();
						  gPad->SetFillColor(DefineCanvasBackgroundColor);
					}


					for (i_ch=0; i_ch<16; i_ch++) {
						iHistoAdc[i_ch]->Reset(); //
						iHistoAdc[i_ch]->BufferEmpty(1); // action =  1 histogram is filled and buffer is deleted

						for (i=0; i<HISTOGRAM_ENERGY_MAX_LENGTH; i++) {
							if (uint_firFilter_energy_histogram_start_ptr[(i_ch*HISTOGRAM_ENERGY_MAX_LENGTH)+i] != 0) {
								iHistoAdc[i_ch]->SetBinContent(i, uint_firFilter_energy_histogram_start_ptr[(i_ch*HISTOGRAM_ENERGY_MAX_LENGTH)+i]);  
							}
						}
 
							double_histo_fwhm = iHistoAdc[i_ch]->GetRMS(1) * 2.354; //
	
						histo_pave_text[i_ch]->Clear();
							histo_pave_text[i_ch]->SetTextSize(0.04);
							sprintf(char_temp,"FWHM %5.1f",double_histo_fwhm);
							histo_pave_text[i_ch]->AddText(char_temp);


							if(fChkHistoZoomMean->IsOn() == kTRUE)  {
								//double_histo_mean = iHistoAdc[i]->GetMean(1); //
								//printf("double_histo_mean = %f\n", double_histo_mean);
								double_histo_min_x = 0.0;
								double_histo_max_x = 8000.0;
								iHistoAdc[i_ch]->SetAxisRange(double_histo_min_x,double_histo_max_x,"X"); //
							}
							else {
								double_histo_min_x = 0.0;
								//double_histo_max_x = 16384;
								double_histo_max_x = (double) (i_histo_length-1);
								iHistoAdc[i_ch]->SetAxisRange(double_histo_min_x,double_histo_max_x,"X"); //
							}



					} //for (i_ch=0; i_ch<16; i_ch++)

				// display histograms
					//display_histo_counter++;
					//if (display_histo_counter > 20) {
					//if (display_histo_counter >= 0) {
					//printf("\ndisplay_histo_counter  %d \n",display_histo_counter);
					//display_histo_counter=0;
















					new_display_histogram_choice = fCombo_Display_Histos_Ch->GetSelected();
					if (new_display_histogram_choice != display_histogram_choice) {
						display_histogram_choice = new_display_histogram_choice ;
						changed_display_histogram_choice_flag = 1 ;
					}
					else {
						changed_display_histogram_choice_flag = 0 ;
					}

					if (changed_display_histogram_choice_flag == 1) {
					  if (display_histogram_choice == 17) {
						fCanvas2->Clear();
						fCanvas2->Divide(2,8);
					  }
					  else {
						fCanvas2->Clear();
						fCanvas2->Divide(1);
					  }
					}

					if (fChkHistoGaussFit->IsOn() == kTRUE)  {
					  histogram_gausfit_enable_flag = 1 ;
					  histogram_gausfit_clear_flag  = 0 ;
					}
					else {
					  if (histogram_gausfit_enable_flag == 1) {
						histogram_gausfit_clear_flag  = 1 ;
					  }
					  histogram_gausfit_enable_flag = 0 ;
					}

					//printf("display_histogram_choice = %d\n", display_histogram_choice);

					if (display_histogram_choice == 17) {
						for (i=0;i<16;i++) {
							fCanvas2->cd(1+display_histo_ch_no);
							if (histogram_gausfit_enable_flag == 1)  {
							  iHistoAdc[display_histo_ch_no]->Fit("gaus","Q");
							}
							if (histogram_gausfit_clear_flag == 1)  {
							  iHistoAdc[display_histo_ch_no]->Fit("gaus","0","Q");
							  histogram_gausfit_clear_flag = 0 ;
							}

							iHistoAdc[display_histo_ch_no]->Draw();
	
							histo_pave_text[display_histo_ch_no]->Draw();
							//printf("display_histo_ch_no = %d\n",display_histo_ch_no);
							display_histo_ch_no++ ;
							if (display_histo_ch_no > 15) {
								display_histo_ch_no = 0 ;
							}
						}
					}
					else {
						display_histo_ch_no = display_histogram_choice-1;
						//fCanvas2->Clear();
						//fCanvas2->Divide(1);
						fCanvas2->cd(1);
						if (histogram_gausfit_enable_flag == 1)  {
						  iHistoAdc[display_histo_ch_no]->Fit("gaus","Q");
						}
						if (histogram_gausfit_clear_flag == 1)  {
						  iHistoAdc[display_histo_ch_no]->Fit("gaus","0","Q");
						  histogram_gausfit_clear_flag = 0 ;
						}

						iHistoAdc[display_histo_ch_no]->Draw();
						histo_pave_text[display_histo_ch_no]->Draw();
					}
					  fCanvas2->Update();
					  //fCanvas2->Modified();
				}
				else {
					if (fB_openfCanvas2WindowFlag == kTRUE) {
						delete fCanvas2 ;
						fB_openfCanvas2WindowFlag = kFALSE; //
					}
				}

				bank_buffer_counter++;
				if (fChkStopAfterBanks->IsOn() == kTRUE)  {
					 if (bank_buffer_counter > fNumericEntriesStopAfterBanks->GetIntNumber() ) {
						fSIS3316_Test1 = kFALSE ;
					 }
				}


			gSystem->ProcessEvents();  // handle GUI events
		} //while (fSIS3316_Test1)


		return_code = gl_sis3316_adc1->register_write( SIS3316_KEY_DISARM , 0);  //   
 		if (uint_WriteData_to_File_OpenFlag == 1) {   ; //  
			fclose(file_WriteData_to_File_Pointer);
			uint_WriteData_to_File_OpenFlag = 0 ;
		}

	#endif //#ifdef MULTI_EVENT_DOUBLE_BANK
	} //if (uint_SampleControl_BankModus == 1)
		/***********************   End of MULTI_EVENT_DOUBLE_BANK  Loop  **********************************************/

#ifdef raus
					unsigned int build_histogram_choice ;
					build_histogram_choice = fCombo_Display_Histos_Build->GetSelected();
 
					// Histogramming
					if (build_histogram_choice == 0) { // Build ADC Output Code Histogram

						}
#endif




	SIS3316_Test_running_dim_widgets(kTRUE);


/**************************************************************************************************************/

#ifdef FFT_GRAPH
  fftw_free(fftw_complex_in);
  fftw_free(fftw_complex_out);
  free(double_fft_spectrum);
  free(double_window_weight);
  //close(SIS3316_device);
#endif

}



/**********************************************************************************************************************************/

/**********************************************************************************************************************************/




#define FILE_FORMAT_EVENT_HEADER        	0xDEADBEEF  

int SIS3316_WriteBankChannelHeaderToDataFile (FILE *file_data_ptr, unsigned int indentifier, unsigned int bank_loop_no, unsigned int channel_no, unsigned int nof_events, unsigned int event_length, unsigned int maw_length, unsigned int reserved)
{   
int written ;
int data ;
  //header
	data = FILE_FORMAT_EVENT_HEADER ;    
    written=fwrite(&data,0x4,0x1, file_data_ptr); // write one  uint word
    written+=fwrite(&indentifier,0x4,0x1,file_data_ptr); // write one  uint word
    written+=fwrite(&bank_loop_no,0x4,0x1,file_data_ptr); // write one  uint word
    written+=fwrite(&channel_no,0x4,0x1,file_data_ptr); // write one  uint word
    written+=fwrite(&nof_events,0x4,0x1,file_data_ptr); // write one  uint word
    written+=fwrite(&event_length,0x4,0x1,file_data_ptr); // write one  uint word
    written+=fwrite(&maw_length,0x4,0x1,file_data_ptr); // write one  uint word	
    written+=fwrite(&reserved,0x4,0x1,file_data_ptr); // write one  uint word
 	return written;

}


//--------------------------------------------------------------------------- 
int SIS3316_WriteBankChannelEventBufferToDataFile (FILE *file_data_ptr, unsigned int* memory_data_array, unsigned int nof_write_length_lwords)
{   
int nof_write_elements ;
int written ;
		nof_write_elements = nof_write_length_lwords ;
		written=fwrite(memory_data_array,0x4, nof_write_elements, file_data_ptr); //  
		//gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written  ;
		if(nof_write_elements != written) { 
    		printf ("Data File Write Error in  WriteEventToDataFile()  \n");
 		 }

 	return written;

}



















void SIS3316TestDialog::CloseWindow()
{
   // Called when window is closed (via the window manager or not).
   // Let's stop histogram filling...
   fSIS3316_Test1 = kFALSE;
   // Add protection against double-clicks
   fQuitButton->SetState(kButtonDisabled);
   //fOkButton->SetState(kButtonDisabled);
   //fCancelButton->SetState(kButtonDisabled);
   // ... and close the Ged editor if it was activated.
 //  if (TVirtualPadEditor::GetPadEditor(kFALSE) != 0)
  //    TVirtualPadEditor::Terminate();
   DeleteWindow();
}



Bool_t SIS3316TestDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Process messages coming from widgets associated with the dialog.

int i;
unsigned int  adc_input_voltage_select;
unsigned int  data;
unsigned int  i_adc_fpga_group, i_adc_chip;
unsigned int  i_adc_fpga;


unsigned int energy_peaking_val, old_energy_peaking_val ;
unsigned int energy_gap_val, old_energy_gap_val ;
unsigned int energy_decay_tau_factor_val, old_energy_decay_tau_factor_val ;
unsigned int energy_decay_tau_table_val, old_energy_decay_tau_table_val ;
unsigned int energy_additional_average_val, old_energy_additional_average_val ;

static const char *dataFilePath_gDefTypes[] = { "binary files", "*.dat",
									   "text files",    "*.txt",
                                        0,             0 };


//    char tmp[20];
//   static int newtab = 0;
   Pixel_t green;
   fClient->GetColorByName("green", green);
   Pixel_t red;
   fClient->GetColorByName("red", red);

   //printf("\nSIS3316TestDialog::ProcessMessage:case kC_COMMAND;kCM_BUTTON \n");

 
   switch (GET_MSG(msg)) {

		case kC_COMMAND:
          switch (GET_SUBMSG(msg)) {

/********************************************/
            case kCM_MENU:
               //printf("kCM_MENU id=%ld\n", parm1);
               switch (parm1) {

                  case TEST1_FILE_EXIT:
					 fSIS3316_Test1 = kFALSE;
                     CloseWindow();   // this also terminates theApp
                     break;

                  default:
                     break;
               } // kCM_MENU switch (parm1)
               break;

/********************************************/
 

            case kCM_TAB:
				// printf("kCM_TAB id=%ld\n", parm1);

				 if(parm1 < this->sis3316Test1_nof_valid_tabel_tabs) {
					for(i=0;i<this->sis3316Test1_nof_valid_tabel_tabs;i++) {
						tabel_tab[i]->ChangeBackground(tab_color_not_active);
					}
					tabel_tab[parm1]->ChangeBackground(tab_color_active);
				}
#ifdef notused
               switch (parm1) {

                  case 0:
                     break;

                  default:
                     break;
               } // kCM_MENU switch (parm1)
 #endif

			   break;

/********************************************/



			case kCM_BUTTON:
		//printf("\nSIS3316TestDialog::ProcessMessage:case kC_COMMAND;kCM_BUTTON \n");
               switch(parm1) {
                  case 1:
                  //case 2:
                     //printf("\nTerminating dialog: %s pressed\n", (parm1 == 1) ? "Quit" : "Cancel");
                     fSIS3316_Test1 = kFALSE;
                     CloseWindow();
                     break;
				  //case 40:  // start test
				  case SIS3316TestDialog_kCM_BUTTON_IRQ_NO_40:  // start test
					  if(fSIS3316_Test1 != kTRUE) { // start only if not running
						Resize(SIS3316_TEST_WINDOW_WIDTH_RUN, SIS3316_TEST_WINDOW_HIGH);   // resize to default size
						fStartB->ChangeBackground(red);
						fStopB->ChangeBackground(green);
						fSIS3316_Test1 = kTRUE;
	                    printf("\nstart Test1\n");
						SIS3316_Test1();
	                    printf("stop Test1\n");
					    fStartB->ChangeBackground(green);
						fStopB->ChangeBackground(red);
						Resize(SIS3316_TEST_WINDOW_WIDTH, SIS3316_TEST_WINDOW_HIGH);   // resize to default size
					  }
					  break;
                  //case 41:  //  stop test
				  case SIS3316TestDialog_kCM_BUTTON_IRQ_NO_41:  //  
                     fSIS3316_Test1 = kFALSE;
					 // fStartB->ChangeBackground(green);
					  fStopB->ChangeBackground(red);
						Resize(SIS3316_TEST_WINDOW_WIDTH, SIS3316_TEST_WINDOW_HIGH);   // resize to default size
                     break;


				  case SIS3316TestDialog_kCM_BUTTON_IRQ_NO_42:  //  
						fileInfoDataFile.fFileTypes = dataFilePath_gDefTypes;
						new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fileInfoDataFile);
						if(fileInfoDataFile.fFilename){
							fTextEntryDataFilePath->SetText(fileInfoDataFile.fFilename);
						}else{
							fTextEntryDataFilePath->SetText("sis3316_test_data.dat");
						}


						break;





				  case 50:
						for (i = 0; i < 16; i++) {
							fChkDisplayAdc[i]->SetState(kButtonDown)   ; // is ON !
						}
						//printf("\n pressed 50\n");
						break;
				  case 51:
						for (i = 0; i < 16; i++) {
							fChkDisplayAdc[i]->SetState(kButtonUp)   ; // is OFF !
						}
						break;



				  case 60:
						for (i = 0; i < 16; i++) {
							fChkTriggerEnableCh[i]->SetState(kButtonDown)   ; // is ON !
						}
						//printf("\n pressed 60\n");
						break;
				  case 61:
						for (i = 0; i < 16; i++) {
							fChkTriggerEnableCh[i]->SetState(kButtonUp)   ; // is OFF !
						}
						break;

				  case 70:
						for (i = 0; i < 16; i++) {
							fChkInvertChannel[i]->SetState(kButtonDown)   ; // is ON !
						}
						//printf("\n pressed 60\n");
						break;
				  case 71:
						for (i = 0; i < 16; i++) {
							fChkInvertChannel[i]->SetState(kButtonUp)   ; // is OFF !
						}
						break;
				  default:
                     break;

               } //switch(parm1)
               break; //case kCM_BUTTON
#ifdef not_used
            case kCM_RADIOBUTTON:
               switch (parm1) {
                  case 81:
                     break;
                  case 82:
                     break;
                  default:
                     break;
               }
               break;
#endif

	    case kCM_CHECKBUTTON:
	      //printf("\nSIS3316TestDialog::ProcessMessage:case kC_COMMAND;kCM_CHECKBUTTON parm1 = %d \n",parm1);
               switch (parm1) {

					case SIS3316TestDialog_kCM_CHECKBUTTON_IRQ_NO_10:  //  
						if (fChkStopAfterBanks->IsOn() == kTRUE)  {
							fNumericEntriesStopAfterBanks->SetState(kTRUE); //
						}
						else {
							fNumericEntriesStopAfterBanks->SetState(kFALSE); //
						}
                     break;



					case SIS3316TestDialog_kCM_CHECKBUTTON_IRQ_NO_11:  //  
							if (fChkNofEvents_ProBank->IsOn() == kTRUE)  {
								fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetState(kTRUE); //
							}
							else {
								fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetState(kFALSE); //
							}
                     break;


					case SIS3316TestDialog_kCM_CHECKBUTTON_IRQ_NO_12:  //  
							if (fChkWriteDataToFile->IsOn() == kTRUE)  {
								fChkWriteMultipleFiles->SetEnabled(kTRUE); //
								fTextEntryDataFilePath->SetEnabled(kTRUE); //
								fTextButtonDataFilePath->SetEnabled(kTRUE); //
							}
							else {
								fChkWriteMultipleFiles->SetEnabled(kFALSE); //
								fTextEntryDataFilePath->SetEnabled(kFALSE); //
								fTextButtonDataFilePath->SetEnabled(kFALSE); //
							}

                     break;
//




				  case 100: //fChkFFT_Sum
					if (fChkFFT_Sum->IsOn() == kTRUE)  {
						fChkFFT_Db->SetEnabled(kFALSE)   ; //
						//fChkFFT_Db->SetState(kButtonUp)   ; // is Off !
						fChkFFTLogY->SetEnabled(kTRUE)   ; //
						fChkFFTLogY->SetState(kButtonUp)   ; // is Off !
					}
					else {
						//fChkFFT_Db->SetState(kButtonDown)   ; // is On !
						fChkFFTLogY->SetState(kButtonUp)   ; // is Off !
						fChkFFTLogY->SetEnabled(kFALSE)   ; //
						fChkFFT_Db->SetEnabled(kTRUE)   ; //
						if (fChkFFT_Db->IsOn() == kTRUE)  {
							fChkFFT_Sum->SetEnabled(kFALSE)   ; //
						}
						if (fB_openfCanvas3WindowFlag == kTRUE) {
							fCanvas3->SetLogy(fChkFFTLogY->IsOn() == kTRUE);
						}
					}
					  break;

                  case 101: // fChkFFTLogY
			  			if (fB_openfCanvas3WindowFlag == kTRUE) {
							fCanvas3->SetLogy(fChkFFTLogY->IsOn() == kTRUE);
						}
						else {
							fChkFFTLogY->SetState(kButtonUp)   ; // is Off !
						}
					  break;

                  case 102: // fChkFFT_Db
						if (fChkFFT_Db->IsOn() == kTRUE)  {
							fChkFFTLogY->SetState(kButtonUp)   ; // is Off !
							fChkFFTLogY->SetEnabled(kFALSE)   ; //
							fChkFFT_Sum->SetEnabled(kFALSE)   ; //
						}
						else {
							fChkFFT_Sum->SetEnabled(kTRUE)   ; //
							fChkFFTLogY->SetEnabled(kTRUE)   ; //
						}

						if (fB_openfCanvas3WindowFlag == kTRUE) {
							fCanvas3->SetLogy(fChkFFTLogY->IsOn() == kTRUE);
						}
						else {
							//fChkFFTLogY->SetState(kButtonUp)   ; // is Off !
						}
					  break;

                  default:
                     break;
               }
               break;

#ifdef not_used
	    case kCM_TAB:
               //printf("Tab item %ld activated\n", parm1);
               break;
#endif

volatile unsigned int uint_SampleControl_BankModus ;
	
		case kCM_COMBOBOX:
              //printf("kCM_COMBOBOX item %ld activated\n", parm1);
               switch (parm1) {
                  case SIS3316TestDialog_kCM_COMBOBOX_IRQ_NO_30: // Sample Paramter
					  	uint_SampleControl_BankModus = fCombo_SampleControl_BankModus->GetSelected();
#ifdef raus
						if (uint_SampleControl_BankModus == 0) {
							fChkKeyTrigger->SetEnabled(kTRUE); //
						}
						else {
							fChkKeyTrigger->SetState(kButtonUp)   ; // is Off!
							fChkKeyTrigger->SetEnabled(kFALSE); //
						}
#endif
						if (uint_SampleControl_BankModus == 0) {
							fChkNofEvents_ProBank->SetEnabled(kFALSE)   ; //
							fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetState(kFALSE); //
							fNumericEntries_SampleControl_NofEvents_ProBank->SetIntNumber(1); // 

							fChkWriteDataToFile->SetState(kButtonUp)   ; // is OFF !
							fChkWriteDataToFile->SetEnabled(kFALSE); //
							fChkWriteMultipleFiles->SetEnabled(kFALSE); //
							fTextEntryDataFilePath->SetEnabled(kFALSE); //
							fTextButtonDataFilePath->SetEnabled(kFALSE); //

						}
						else {
							fChkNofEvents_ProBank->SetEnabled(kTRUE)   ; //
							if (fChkNofEvents_ProBank->IsOn() == kTRUE)  {
								fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetState(kTRUE); //
							}
							else {
								fNumericEntries_SampleControl_MaxNofEvents_ProBank->SetState(kFALSE); //
							}

							fChkWriteDataToFile->SetEnabled(kTRUE); //
							if (fChkWriteDataToFile->IsOn() == kTRUE)  {
								fChkWriteMultipleFiles->SetEnabled(kTRUE); //
								fTextEntryDataFilePath->SetEnabled(kTRUE); //
								fTextButtonDataFilePath->SetEnabled(kTRUE); //
							}
							else {
								fChkWriteMultipleFiles->SetEnabled(kFALSE); //
								fTextEntryDataFilePath->SetEnabled(kFALSE); //
								fTextButtonDataFilePath->SetEnabled(kFALSE); //
							}

						}
					  break;






				  case SIS3316TestDialog_kCM_COMBOBOX_IRQ_NO_90: //SPI ADC
						adc_input_voltage_select = fCombo_Set_ADC_SPI_Input_Voltage->GetSelected();
					  	if (gl_sis3316_adc1->adc_125MHz_flag == 0) { // 250 MHz chip AD9643
							switch (adc_input_voltage_select) {
								case 0: //SPI ADC
									data = 0x15 ; 	//  1.50V (1.75V - (11 * 0.022V) = 1,508)
									break;
								case 1: //SPI ADC
									data = 0x0 ; 	//  1.75V
									break;
								case 2: //SPI ADC
									data = 0xB ; 	//  2.00V (1.75V + (11 * 0.022V) = 1,992)
									break;
								default:
									data = 0x0 ; 	//  1.75V
									break;
							}
						}
						else { // 125 MHz chip AD9268
							switch (adc_input_voltage_select) {
								case 0: //SPI ADC
									data = 0x40 ; 	//  1.50V
									break;
								case 1: //SPI ADC
									data = 0x80 ; 	//  1.75V
									break;
								case 2: //SPI ADC
									data = 0xC0 ; 	//  2.00V
									break;
								default:
									data = 0x80 ; 	//  1.75V
									break;
							}
						}
						for (i_adc_fpga_group = 0; i_adc_fpga_group < 4; i_adc_fpga_group++) {
							for (i_adc_chip = 0; i_adc_chip < 2; i_adc_chip++) {
								gl_sis3316_adc1->adc_spi_write( i_adc_fpga_group, i_adc_chip, 0x18, data);
								gl_sis3316_adc1->adc_spi_write( i_adc_fpga_group, i_adc_chip, 0xff, 0x1);  // update
							}
						}

						//printf("kCM_COMBOBOX case 90\n");
						break;

				  default:
                     break;
               }


			  break;

	    default:
              //printf("default item %ld activated\n", parm1);
               break;
         }
         break;


 	case kC_TEXTENTRY:
      //printf("kC_TEXTENTRY item %ld activated\n", parm1);
	  switch (GET_SUBMSG(msg)) {
	      case kTE_TEXTCHANGED:
				switch(parm1) {
					  case 20:
						//printf("\n pressed 20\n");
						break;
					  case 21:
						//printf("\n pressed 21\n");
						break;
					  case 90:
						//printf("\n pressed 90\n");
						gl_sis3316_adc1->register_read( SIS3316_ADC_CH1_FIR_ENERGY_SETUP_REG, &data);  //
						old_energy_peaking_val             = data & 0xfff ;
						old_energy_gap_val                 = (data & 0x3ff000) >> 12  ;
						old_energy_additional_average_val  = (data & 0xc000000) >> 22 ;
						old_energy_decay_tau_factor_val    = (data & 0x3f000000) >> 24 ;
						old_energy_decay_tau_table_val     = (data & 0xc0000000) >> 30 ;


						energy_peaking_val =  fNumericEntriesEnergyPeaking->GetIntNumber(); ;
						if (gl_sis3316_adc1->adc_125MHz_flag == 0) { // SIS3316-250MHz-14bit
							if (energy_peaking_val != 0) {
								if (energy_peaking_val > old_energy_peaking_val) {
									energy_peaking_val =  (energy_peaking_val + 1) & 0xffe ; // only even
								}
								if (energy_peaking_val < old_energy_peaking_val) {
									energy_peaking_val =  (energy_peaking_val - 1) & 0xffe ; // only even
								}
							}
						}
						if (energy_peaking_val > FIR_ENERGY_MAX_PEAKING) { energy_peaking_val = FIR_ENERGY_MAX_PEAKING; } //
						if (energy_peaking_val < FIR_ENERGY_MIN_PEAKING)   { energy_peaking_val = FIR_ENERGY_MIN_PEAKING; }   //
						if (gl_sis3316_adc1->adc_125MHz_flag == 0) { // SIS3316-250MHz-14bit
						}
						fNumericEntriesEnergyPeaking->SetIntNumber(energy_peaking_val );

					    energy_gap_val =  fNumericEntriesEnergyGap->GetIntNumber(); ;
						if (gl_sis3316_adc1->adc_125MHz_flag == 0) { // SIS3316-250MHz-14bit
							if (energy_gap_val != 0) {
								if (energy_gap_val > old_energy_gap_val) {
									energy_gap_val =  (energy_gap_val + 1) & 0x3fe ; // only even
								}
								if (energy_gap_val < old_energy_gap_val) {
									energy_gap_val =  (energy_gap_val - 1) & 0x3fe ; // only even
								}
							}
						}
						if (energy_gap_val > FIR_ENERGY_MAX_GAP) { energy_gap_val = FIR_ENERGY_MAX_GAP; } //
						if (energy_gap_val < FIR_ENERGY_MIN_GAP)   { energy_gap_val = FIR_ENERGY_MIN_GAP; }   //
						fNumericEntriesEnergyGap->SetIntNumber(energy_gap_val );


						energy_decay_tau_table_val =  fNumericEntriesEnergyTauTable->GetIntNumber(); ;
						if (energy_decay_tau_table_val > FIR_ENERGY_MAX_TAU_TABLE) { energy_decay_tau_table_val = FIR_ENERGY_MAX_TAU_TABLE; } //
						fNumericEntriesEnergyTauTable->SetIntNumber(energy_decay_tau_table_val );

						energy_decay_tau_factor_val =  fNumericEntriesEnergyTauFactor->GetIntNumber(); ;
						if (energy_decay_tau_factor_val > FIR_ENERGY_MAX_TAU_FACTOR) { energy_decay_tau_factor_val = FIR_ENERGY_MAX_TAU_FACTOR; } //
						fNumericEntriesEnergyTauFactor->SetIntNumber(energy_decay_tau_factor_val );







						energy_additional_average_val =  fNumericEntriesEnergyAdditionalAverage->GetIntNumber(); ;
						if (energy_additional_average_val > FIR_ENERGY_MAX_ADD_AVERAGE) { energy_additional_average_val = FIR_ENERGY_MAX_ADD_AVERAGE; } //
						fNumericEntriesEnergyAdditionalAverage->SetIntNumber(energy_additional_average_val );


						// set FIR Energy Setup
						data =    ((energy_decay_tau_table_val & FIR_ENERGY_MAX_TAU_TABLE) << 30)
							   +  ((energy_decay_tau_factor_val & FIR_ENERGY_MAX_TAU_FACTOR) << 24)
							   +  ((energy_additional_average_val  & 0x3) << 22)
							   +  ((energy_gap_val  & 0x3ff) << 12)
							   +  (energy_peaking_val & 0xfff) ; //
						for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
							gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_ENERGY_SETUP_REG, data) ;
							gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_ENERGY_SETUP_REG, data) ;
							gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_ENERGY_SETUP_REG, data) ;
							gl_sis3316_adc1->register_write( (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_ENERGY_SETUP_REG, data) ;
						}

						break;

					  case 120:
							unsigned int tap_delay_val;
							tap_delay_val =  fNumericEntriesTapDelay->GetIntNumber(); ;
							fNumericEntriesTapDelay->SetIntNumber(tap_delay_val); //
							gl_sis3316_adc1->register_write( SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG, (tap_delay_val & 0x10ff)  + 0x300 ) ;
							gl_sis3316_adc1->register_write( SIS3316_ADC_CH5_8_INPUT_TAP_DELAY_REG, (tap_delay_val & 0x10ff)  + 0x300 ) ;
							gl_sis3316_adc1->register_write( SIS3316_ADC_CH9_12_INPUT_TAP_DELAY_REG, (tap_delay_val & 0x10ff)  + 0x300 ) ;
							gl_sis3316_adc1->register_write( SIS3316_ADC_CH13_16_INPUT_TAP_DELAY_REG, (tap_delay_val & 0x10ff)  + 0x300 ) ;

						   //printf("\n pressed 120\n");
						break;

				}
			break;
	  }
	  break;

      default:
	  printf("\nSIS3316TestDialog::ProcessMessage:case default parm1 = %d\n",parm1);
      break;
   }
   return kTRUE;
}

/*******************************************************/





//---- Main program ------------------------------------------------------------

int main(int argc, char **argv)
{
   TApplication theApp("App", &argc, argv);
//#ifdef raus // warum
   if (gROOT->IsBatch()) {
      fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
      return 1;
   }
//#endif
   //TestMainFrame mainWindow(gClient->GetRoot(), 400, 220);
	new TestMainFrame(gClient->GetRoot(), 400, 220);

   theApp.Run();

   return 0;
}



