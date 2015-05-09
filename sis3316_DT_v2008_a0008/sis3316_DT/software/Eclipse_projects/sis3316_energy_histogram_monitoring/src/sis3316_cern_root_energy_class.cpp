/***************************************************************************/
/*                                                                         */
/*  Filename: sis3316_cern_root_energy_class.cpp                             */
/*                                                                         */
/*  Funktion:                                                              */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 17.02.2014                                       */
/*  last modification:    17.02.2014                                       */
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

//#include "stdafx.h"
#include <iostream>
#include <cstdio>
#include "sis3316_cern_root_energy_class.h"

using namespace std;


/***************************************************************************************************************************************************************************************************************************************/

sis_root_canvas_with_Energy_spectrums::sis_root_canvas_with_Energy_spectrums(char* canvas_text, int canvas_x_pos, int canvas_y_pos, int canvas_x_size, int canvas_y_size)
{
	unsigned int beam_on_off_seperation_flag ;
	beam_on_off_seperation_flag = 0 ,
	CanvasBackgroundColor = 10 ; // white

	//c1 = new TCanvas("c1", graph_text, graph_x, graph_y, graph_x_size, graph_y_size);
	this->c1 = new TCanvas(canvas_text, canvas_text, canvas_x_pos, canvas_y_pos, canvas_x_size, canvas_y_size);
	this->c1->SetGrid();
	this->c1->SetFillColor(CanvasBackgroundColor);

	TH1I_histogram_bins = 0x10000 ;

	int i;
	if (beam_on_off_seperation_flag == 0) {
		this->nof_TH1I_histograms = 1 ;
	}
	else {
		this->nof_TH1I_histograms = 2 ;
	}


	if (this->nof_TH1I_histograms == 2) {
		this->c1->Divide(2,1);
	}
	else {
		this->c1->Divide(1);
	}
	for (i=0;i<nof_TH1I_histograms;i++) {
		this->c1->cd(1+i);
		gPad->SetGrid();
		gPad->SetFillColor(CanvasBackgroundColor);
		if (beam_on_off_seperation_flag == 0) {
			snprintf(histogram_text, 128, "TOF spectrum") ;
		}
		else {
			if (i==0) {
				snprintf(histogram_text, 128, "TOF spectrum Neutron") ;
			}
			else {
				snprintf(histogram_text, 128, "TOF spectrum Gamma") ;
			}
		}

		this->h_ch_histogram[i] = new TH1I(histogram_text, histogram_text, TH1I_histogram_bins, 0, TH1I_histogram_bins-1); //
		h_ch_histogram[i]->SetStats(kFALSE); //
		h_ch_histogram[i]->Reset(); //
		h_ch_histogram[i]->BufferEmpty(1); // action =  1 histogram is filled and buffer is deleted
		h_ch_histogram[i]->Draw();
		h_ch_histogram[i]->SetFillColor(kRed);
	}


	this->c1->Modified();
	this->c1->Update();
}


void sis_root_canvas_with_Energy_spectrums::sis3316_write_histogram (int histogram_index, unsigned int length, unsigned int* uint_histogram_array )
{
	int i ;
	int i_length ;
	i_length = length ;
	if (i_length > TH1I_histogram_bins) {
		i_length = TH1I_histogram_bins ;
	}
	h_ch_histogram[histogram_index]->Reset(); //
	h_ch_histogram[histogram_index]->BufferEmpty(1); // action =  1 histogram is filled and buffer is deleted
	for (i=0; i<i_length; i++) {
		if (uint_histogram_array[i] != 0) {
			h_ch_histogram[histogram_index]->SetBinContent(i, uint_histogram_array[i] );
		}
	}
	return  ;
}

void sis_root_canvas_with_Energy_spectrums::sis3316_draw_histogram (void )
{
int i;
	for (i=0;i<nof_TH1I_histograms;i++) {
		this->c1->cd(1+i);
		h_ch_histogram[i]->Draw();
	}
	//this->c1->Modified();
	this->c1->Update();

	return  ;
}

sis_root_canvas_with_Energy_spectrums::~sis_root_canvas_with_Energy_spectrums(void)
{
	delete this->c1 ;
}

/***************************************************************************************************************************************************************************************************************************************/





/***************************************************************************************************************************************************************************************************************************************/




/***************************************************************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************************************************************/





const char *const sis_root_spectrum_display_control_panel::entryChannelSelect[16] = {
   "channel 1",
   "channel 2",
   "channel 3",
   "channel 4",
   "channel 5",
   "channel 6",
   "channel 7",
   "channel 8",
   "channel 9",
   "channel 10",
   "channel 11",
   "channel 12",
   "channel 13",
   "channel 14",
   "channel 15",
   "channel 16"
};





sis_root_spectrum_display_control_panel::sis_root_spectrum_display_control_panel(char* canvas_text, int canvas_x_pos, int canvas_y_pos, int canvas_x_size, int canvas_y_size)
{
	unsigned int i;

	fMain = new TGMainFrame(gClient->GetRoot(),canvas_x_size,canvas_y_size);
	fMain->SetWindowName(canvas_text);
	fMain->SetIconPixmap("sis1_sis3316.png");



	fHor1 = new TGHorizontalFrame(fMain);
	fHor1->SetBackgroundColor(0xFFFFFF); // white
/***/
	fVert1 = new TGVerticalFrame(fHor1);
	fVert1->SetBackgroundColor(0xFFFF00); // yellow


	fV1_Grp0 = new TGGroupFrame(fVert1, "Channel select");
	fVert1->AddFrame(fV1_Grp0, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));


	fV1_Grp0_vert = new TGCompositeFrame(fV1_Grp0, 60, 20, kVerticalFrame);
	fV1_Grp0->AddFrame(fV1_Grp0_vert, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5) );

	fCombo_ChannelSelect = new TGComboBox(fV1_Grp0_vert, 46);
	//fCombo_SetInternalClockFreq->Associate(this); // Event (IRQ) anmelden
	fV1_Grp0_vert->AddFrame(fCombo_ChannelSelect, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5));

	for (i = 0; i < 16; i++) {
		fCombo_ChannelSelect->AddEntry(entryChannelSelect[i], i);
	}
	fCombo_ChannelSelect->Select(0, kTRUE); //
	fCombo_ChannelSelect->Resize(120, 25);

// ******************





// update
	channelSelect_index = fCombo_ChannelSelect->GetSelected(); //


	TRootEmbeddedCanvas *cv ;

	fVert2 = new TGVerticalFrame(fHor1);
	fVert2->SetBackgroundColor(0xFFF000); // yellow
	cv = new TRootEmbeddedCanvas(0,fVert2);
	fVert2->AddFrame(cv, new TGLayoutHints((kLHintsNormal |  kLHintsExpandY |  kLHintsExpandX), 3, 3, 3, 3) );







	fHor1->AddFrame(fVert1, new TGLayoutHints((kLHintsNormal |  kLHintsExpandY ), 0, 3, 0, 0) );
	fHor1->AddFrame(fVert2, new TGLayoutHints((kLHintsNormal |  kLHintsExpandY |  kLHintsExpandX), 0, 0, 0, 3) );
	fMain->AddFrame(fHor1, new TGLayoutHints((kLHintsNormal |  kLHintsExpandY |  kLHintsExpandX), 0, 0, 0, 0)  );



	   fMain->MapSubwindows();
		fMain->SetWMSizeHints(200, 100, 1600, 1200, 1, 1);

		fMain->Resize(fMain->GetDefaultSize());
		fMain->SetWMPosition(canvas_x_pos, canvas_y_pos);

		fMain->Resize(canvas_x_size, canvas_y_size);
		fMain->MapWindow();



}



sis_root_spectrum_display_control_panel::~sis_root_spectrum_display_control_panel(void)
{
	delete this->c1 ;
}



void sis_root_spectrum_display_control_panel::update_parameters(void )
{
	// update
	channelSelect_index = fCombo_ChannelSelect->GetSelected(); //

	return  ;
}





/***************************************************************************************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************************************************************************************/
const char *const sis_root_energy_spectrum_display_control_panel::entryChannelSelect[16] = {
   "channel 1",
   "channel 2",
   "channel 3",
   "channel 4",
   "channel 5",
   "channel 6",
   "channel 7",
   "channel 8",
   "channel 9",
   "channel 10",
   "channel 11",
   "channel 12",
   "channel 13",
   "channel 14",
   "channel 15",
   "channel 16"
};


sis_root_energy_spectrum_display_control_panel::sis_root_energy_spectrum_display_control_panel(char* canvas_text, int canvas_x_pos, int canvas_y_pos, int canvas_x_size, int canvas_y_size)
{
	int i;

	fMain = new TGMainFrame(gClient->GetRoot(),canvas_x_size,canvas_y_size);
	fMain->SetWindowName(canvas_text);
	fMain->SetIconPixmap("sis1_sis3316.png");



	fHor1 = new TGHorizontalFrame(fMain);
	//fHor1->SetBackgroundColor(0xFFFFFF); // white
/***/
	fVert1 = new TGVerticalFrame(fHor1);
	//fVert1->SetBackgroundColor(0xFFFF00); // yellow


	fV1_Grp0 = new TGGroupFrame(fVert1, "Channel select");
	fVert1->AddFrame(fV1_Grp0, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));


	fV1_Grp0_vert = new TGCompositeFrame(fV1_Grp0, 60, 20, kVerticalFrame);
	fV1_Grp0->AddFrame(fV1_Grp0_vert, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5) );

	fCombo_ChannelSelect = new TGComboBox(fV1_Grp0_vert, 46);
	//fCombo_SetInternalClockFreq->Associate(this); // Event (IRQ) anmelden
	fV1_Grp0_vert->AddFrame(fCombo_ChannelSelect, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 5, 5));

	for (i = 0; i < 16; i++) {
		fCombo_ChannelSelect->AddEntry(entryChannelSelect[i], i);
	}
	fCombo_ChannelSelect->Select(0, kTRUE); //
	fCombo_ChannelSelect->Resize(120, 25);

// ******************





// update
	channelSelect_index = fCombo_ChannelSelect->GetSelected(); //





// ****************************************************************************************************************************
	this->TH1I_histogram_bins = 0x10000 ;

	fVert2 = new TGVerticalFrame(fHor1);
	//fVert2->SetBackgroundColor(0xFFF000); // yellow
	cv = new TRootEmbeddedCanvas(0,fVert2);
	fVert2->AddFrame(cv, new TGLayoutHints((kLHintsNormal |  kLHintsExpandY |  kLHintsExpandX), 3, 3, 3, 3) );

	cv->GetCanvas()->cd();
	cv->GetCanvas()->SetGrid();
	cv->GetCanvas()->SetFillColor(CanvasBackgroundColor);

	this->nof_TH1I_histograms = 1 ;


	if (this->nof_TH1I_histograms == 1) {
		cv->GetCanvas()->Divide(1);
	}
	else {
		cv->GetCanvas()->Divide(2,1);
	}
	for (i=0;i<nof_TH1I_histograms;i++) {
		cv->GetCanvas()->cd(1+i);
		gPad->SetGrid();
		gPad->SetFillColor(CanvasBackgroundColor);
		snprintf(histogram_text, 128, "Energy spectrum") ;

		this->h_ch_histogram[i] = new TH1I(histogram_text, histogram_text, TH1I_histogram_bins, 0, TH1I_histogram_bins-1); //
		//h_ch_histogram[i]->SetStats(kFALSE); //
		h_ch_histogram[i]->SetStats(kTRUE); //
		h_ch_histogram[i]->Reset(); //
		h_ch_histogram[i]->BufferEmpty(1); // action =  1 histogram is filled and buffer is deleted
		h_ch_histogram[i]->Draw();
		h_ch_histogram[i]->SetFillColor(kRed);
	}


	cv->GetCanvas()->Modified();
	cv->GetCanvas()->Update();





// *****************************************************************************************************************************
	fHor1->AddFrame(fVert1, new TGLayoutHints((kLHintsNormal |  kLHintsExpandY ), 0, 3, 0, 0) );
	fHor1->AddFrame(fVert2, new TGLayoutHints((kLHintsNormal |  kLHintsExpandY |  kLHintsExpandX), 0, 0, 0, 3) );
	fMain->AddFrame(fHor1, new TGLayoutHints((kLHintsNormal |  kLHintsExpandY |  kLHintsExpandX), 0, 0, 0, 0)  );



	fMain->MapSubwindows();
	fMain->SetWMSizeHints(200, 100, 1600, 1200, 1, 1);

	fMain->Resize(fMain->GetDefaultSize());
	fMain->SetWMPosition(canvas_x_pos, canvas_y_pos);
	//fMain->SetWMPosition(100, 200);

	fMain->Resize(canvas_x_size, canvas_y_size);
	fMain->MapWindow();

}



sis_root_energy_spectrum_display_control_panel::~sis_root_energy_spectrum_display_control_panel(void)
{
	 delete this->c1 ;
}



void sis_root_energy_spectrum_display_control_panel::update_parameters(void )
{
	// update
	channelSelect_index = fCombo_ChannelSelect->GetSelected(); //

	return  ;
}


void sis_root_energy_spectrum_display_control_panel::sis3316_write_histogram (int histogram_index, unsigned int length, unsigned int* uint_histogram_array )
{
	int i ;
	int i_length ;
	i_length = length ;
	if (i_length > TH1I_histogram_bins) {
		i_length = TH1I_histogram_bins ;
	}
	h_ch_histogram[histogram_index]->Reset(); //
	h_ch_histogram[histogram_index]->BufferEmpty(1); // action =  1 histogram is filled and buffer is deleted
	for (i=0; i<i_length; i++) {
		if (uint_histogram_array[i] != 0) {
			h_ch_histogram[histogram_index]->SetBinContent(i, uint_histogram_array[i] );
		}
	}
	return  ;
}

void sis_root_energy_spectrum_display_control_panel::sis3316_draw_histogram (void )
{
int i;
	for (i=0;i<nof_TH1I_histograms;i++) {
		cv->GetCanvas()->cd(1+i);
		h_ch_histogram[i]->Draw();
	}
	//this->c1->Modified();
	cv->GetCanvas()->Update();

	return  ;
}




/***************************************************************************************************************************************************************************************************************************************/

