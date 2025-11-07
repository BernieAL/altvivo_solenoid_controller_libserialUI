#ifndef __SOLENOID_TIMING_HPP__
#define __SOLENOID_TIMING_HPP__

#include <wx/spinctrl.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include "SolenoidState.hpp"  // Changed from global-constants.hpp - this has SolenoidTiming struct

class SolenoidTimingDialog : public wxDialog
{
private:
    wxStaticText* open_label;
    wxStaticText* close_label;
    wxSpinCtrl* open_time_spinner;
    wxSpinCtrl* close_time_spinner;
    
    SolenoidTiming timing;

public:
    SolenoidTimingDialog(wxWindow* parent);
    SolenoidTimingDialog(wxWindow* parent, SolenoidTiming* ref);
    
    SolenoidTiming Get_Timing();

private:
    void Create_Controls();
    void Setup_Layout();
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);
};

#endif // __SOLENOID_TIMING_HPP__