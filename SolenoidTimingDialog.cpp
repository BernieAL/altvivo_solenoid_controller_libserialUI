#include "SolenoidTimingDialog.hpp"
#include "global-constants.hpp"

SolenoidTimingDialog::SolenoidTimingDialog(wxWindow* parent)
  : wxDialog(parent, wxID_ANY, "Solenoid Timing Settings", 
	     wxDefaultPosition, wxSize(300, 200))
{
  Create_Controls();
  Setup_Layout();
}



SolenoidTiming SolenoidTimingDialog::Get_Timing()
{
  // Get values from spinners
  timing.openTime = open_time_spinner->GetValue();
  timing.closeTime = close_time_spinner->GetValue();
        
  return timing;
}

void SolenoidTimingDialog::Create_Controls()
{
  // Create labels
  open_label = new wxStaticText(this, wxID_ANY, "Open Time (ms):");
  close_label = new wxStaticText(this, wxID_ANY, "Close Time (ms):");

  // clear_label = new wxStaticText(this, wxID_ANY,"");

  // Create spin controls (number inputs)
  open_time_spinner = new wxSpinCtrl(this, wxID_ANY, "100", 
				     wxDefaultPosition, wxDefaultSize,
				     wxSP_ARROW_KEYS, 1, 10000, 100);
        
  close_time_spinner = new wxSpinCtrl(this, wxID_ANY, "100",
				      wxDefaultPosition, wxDefaultSize,
				      wxSP_ARROW_KEYS, 1, 10000, 100);


}

void SolenoidTimingDialog::Setup_Layout()
{
  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 2, 4, 4);

  // Add controls to grid
  gridSizer->Add(open_label, 
		 0, wxALIGN_CENTER_VERTICAL);
  gridSizer->Add(open_time_spinner, 1, wxEXPAND);
        
  gridSizer->Add(close_label, 
		 0, wxALIGN_CENTER_VERTICAL);
  gridSizer->Add(close_time_spinner, 1, wxEXPAND);

  
  // gridSizer->Add(clear_label, 
	// 	 0, wxALIGN_CENTER_VERTICAL);
  // gridSizer->Add(clear_time_spinner, 1, wxEXPAND);

  gridSizer->AddGrowableCol(1);

  // Button sizer
  wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  buttonSizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxRIGHT, 5);
  buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"));
  // buttonSizer->Add(new wxButton(this, wxID_CLEAR, "Clear"));

  // Main layout
  mainSizer->Add(gridSizer, 1, wxEXPAND | wxALL, 15);
  mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxALL, 15);

  SetSizer(mainSizer);
}

void SolenoidTimingDialog::OnOK(wxCommandEvent& event)
{
  // Get values from spinners
  timing.openTime = open_time_spinner->GetValue();
  timing.closeTime = close_time_spinner->GetValue();
        
  EndModal(wxID_OK);  // Close dialog with OK result
}

void SolenoidTimingDialog::OnCancel(wxCommandEvent& event)
{
  EndModal(wxID_CANCEL);  // Close dialog with Cancel result
}


// void SolenoinTimingDialog::onClear(wxCommandEvent& event)
// {
//   timing.openTime = open_time_spinner->GetValue();
//   timing.closeTime = close_time_spinner->GetValue();
// }