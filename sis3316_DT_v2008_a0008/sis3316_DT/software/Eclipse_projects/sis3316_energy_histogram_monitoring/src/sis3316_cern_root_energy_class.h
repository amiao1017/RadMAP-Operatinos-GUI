

/***************************************************************************/
/*                                                                         */
/*  Filename: sis3316_cern_root_energy_class.h                               */
/*                                                                         */
/*  Funktion:                                                              */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 17.02.2014                                       */
/*  last modification:    03.03.2014                                       */
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
/*  � 2014                                                                 */
/*                                                                         */
/***************************************************************************/

#ifndef _SIS_CERN_ROOT_ENERGY_CLASS_
#define _SIS_CERN_ROOT_ENERGY_CLASS_

#include "TApplication.h"
#include <TGLabel.h>

#include "TObject.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TMultiGraph.h"
#include "TMath.h"
#include "TCanvas.h"
//#include "TRandom.h"
//#include "TThread.h"
#include <TSystem.h>
#include "TLatex.h"
#include <TGFrame.h>
#include "TPaveLabel.h"
#include "TFrame.h"
#include "TGNumberEntry.h"
#include <TGComboBox.h>
#include "TRootEmbeddedCanvas.h"
#include <TGProgressBar.h>
#include <TStyle.h>
#include <TGGC.h>

/***************************************************************************************************************************************************************************************************************************************/

class sis_root_canvas_with_Energy_spectrums{
private:
	TCanvas *c1 ;
	TH1I   	*h_ch_histogram[2];
	//char temp_histogram_text[128] ;
	char histogram_text[128] ;

public:
	int CanvasBackgroundColor ; // white
	int nof_TH1I_histograms ; //
	int TH1I_histogram_bins ; //

public:

	sis_root_canvas_with_Energy_spectrums(char* canvas_text, int canvas_x_pos, int canvas_y_pos, int canvas_x_size, int canvas_y_size) ;

~sis_root_canvas_with_Energy_spectrums(void);

void sis3316_write_histogram (int histogram_index, unsigned int length, unsigned int* uint_histogram_array );
void sis3316_draw_histogram (void );


};

/***************************************************************************************************************************************************************************************************************************************/







/***************************************************************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************************************************************/



#define ENTRY_DRAW_OPTION_LENGTH  11

class sis_root_spectrum_display_control_panel{
private:
	TGMainFrame *fMain;

	TRootEmbeddedCanvas *c1;
	TGHorizontalFrame		*fHor1, *fHor2;
	TGVerticalFrame		*fVert1, *fVert2;

	TGGroupFrame *fV1_Grp0;
	TGCompositeFrame *fV1_Grp0_vert ;

	TGComboBox *fCombo_ChannelSelect ;


	TGGroupFrame *fV1_Grp1;
	TGCompositeFrame *fV1_Grp1_vert ;

	TGCheckButton *fChk_display_BeamOnOff ;

	TGGroupFrame *fV1_Grp2;
	TGCompositeFrame *fV1_Grp2_vert ;

	TGComboBox *fCombo_Shape_DrawOption ;
	TGCheckButton *fChk_Shape_display_Update ;

	TGGroupFrame *fV1_Grp3;
	TGCompositeFrame *fV1_Grp3_vert ;
	TGCheckButton *fChk_Tof_display_Update ;

	TGGroupFrame *fV1_Grp4;
	TGCompositeFrame *fV1_Grp4_vert ;
	TGCheckButton *fChk_Charge_display_Update ;

	char histogram_text[128] ;

public:
	static const char *const entryDrawOption[ENTRY_DRAW_OPTION_LENGTH] ;
	static const char *const entryChannelSelect[16] ;

public:
	int CanvasBackgroundColor ; // white
	unsigned int channelSelect_index ; //
	unsigned int display_BeamOnOFF_flag ; //

	unsigned int shape_draw_option_index ; //
	unsigned int shape_display_update_flag ; //
	unsigned int tof_display_update_flag ; //
	unsigned int charge_display_update_flag ; //



public:

sis_root_spectrum_display_control_panel(char* canvas_text, int canvas_x_pos, int canvas_y_pos, int canvas_x_size, int canvas_y_size);
~sis_root_spectrum_display_control_panel(void);

void update_parameters(void ) ;



};


#endif


/***************************************************************************************************************************************************************************************************************************************/

class sis_root_energy_spectrum_display_control_panel{
private:
	TGMainFrame *fMain;
	TRootEmbeddedCanvas *cv ;

	TRootEmbeddedCanvas *c1;
	TGHorizontalFrame		*fHor1, *fHor2;
	TGVerticalFrame		*fVert1, *fVert2;

	TGGroupFrame *fV1_Grp0;
	TGCompositeFrame *fV1_Grp0_vert ;

	TGComboBox *fCombo_ChannelSelect ;


	TGGroupFrame *fV1_Grp1;
	TGCompositeFrame *fV1_Grp1_vert ;

	TGCheckButton *fChk_display_BeamOnOff ;

	TGGroupFrame *fV1_Grp2;
	TGCompositeFrame *fV1_Grp2_vert ;

	TGComboBox *fCombo_Shape_DrawOption ;
	TGCheckButton *fChk_Shape_display_Update ;

	TGGroupFrame *fV1_Grp3;
	TGCompositeFrame *fV1_Grp3_vert ;
	TGCheckButton *fChk_Tof_display_Update ;

	TGGroupFrame *fV1_Grp4;
	TGCompositeFrame *fV1_Grp4_vert ;
	TGCheckButton *fChk_Charge_display_Update ;

	TH1I   	*h_ch_histogram[2];
	//char temp_histogram_text[128] ;
	char histogram_text[128] ;




public:
	static const char *const entryDrawOption[ENTRY_DRAW_OPTION_LENGTH] ;
	static const char *const entryChannelSelect[16] ;

public:
	int CanvasBackgroundColor ; // white
	int nof_TH1I_histograms ; //
	int TH1I_histogram_bins ; //
	unsigned int channelSelect_index ; //


public:

sis_root_energy_spectrum_display_control_panel(char* canvas_text, int canvas_x_pos, int canvas_y_pos, int canvas_x_size, int canvas_y_size);
~sis_root_energy_spectrum_display_control_panel(void);

void update_parameters(void ) ;
void sis3316_write_histogram (int histogram_index, unsigned int length, unsigned int* uint_histogram_array );
void sis3316_draw_histogram (void );



};




/***************************************************************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************************************************************/

