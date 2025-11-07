/////////////////////////////////////////////////////////////////////////////
// Name:        main_gui.cpp
// Purpose:     GUI to control solenoids and pressure regulators
// Author:      Mark Volosov
// Modified by:
// Created:
// Copyright:   (c) AltVivo
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#ifndef wxHAS_IMAGES_IN_RESOURCES

#endif

#include <vector>
#include "SolenoidState.hpp"        
#include "global-constants.hpp"
#include "SolenoidTimingDialog.hpp"
#include "SolenoidControl.hpp"
#include "SerialComm.hpp"



// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title);
    ~MyFrame();

    // event handlers
    void OnSerialPortSelected(wxCommandEvent &event);
    void OnQuit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);

private:
    // Backend controller - manages all solenoid/pressure operations
    SolenoidControl controller;

    wxMenu *serialMenu;

    // UI elements
    std::vector<wxButton *> sol_buttons;
    wxSlider *positive_pressure_slider;
    wxSlider *vacuum_slider;

    // Event handlers
    wxDECLARE_EVENT_TABLE();
    void Sol_Button_Clicked(wxCommandEvent &event);
    void Sol_Button_Program(wxMouseEvent &event);
    void OnPositivePressureSlider(wxCommandEvent &event);
    void OnVacuumSlider(wxCommandEvent &event);

    // Helper methods
    void UpdateButtonAppearance(size_t index);
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    Serial_Menu_ID = 100,
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
        EVT_MENU(Minimal_About, MyFrame::OnAbout)
            wxEND_EVENT_TABLE()

                wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    MyFrame *frame = new MyFrame("Solenoid Control System");
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

MyFrame::MyFrame(const wxString &title)
    : wxFrame(nullptr, wxID_ANY, title)
{
    // Set the frame icon
    // SetIcon(wxICON(sample));

    // Set theme colors
    SetBackgroundColour(BACKGROUND_BLACK);
    SetForegroundColour(FOREGROUND_WHITE);

    // ========================================================================
    // Create menu bar
    // ========================================================================

    serialMenu = new wxMenu;
    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;

    // Populate serial port menu
    struct serial_ports_list *ports_list = get_ports();

    if (ports_list != NULL)
    {
        for (ssize_t i = 0; i < ports_list->length; i++)
        {
            int menu_id = Serial_Menu_ID + i;
            serialMenu->Append(menu_id, ports_list->list[i], "Open This Port");
            Bind(wxEVT_MENU, &MyFrame::OnSerialPortSelected, this, menu_id);
        }

        // Free port list memory
        for (ssize_t i = 0; i < ports_list->length; i++)
        {
            free(ports_list->list[i]);
        }
        free(ports_list->list);
        free(ports_list);
    }

    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");
    fileMenu->Append(Minimal_Quit, "&Exit\tAlt-X", "Quit this program");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(serialMenu, "&Open Port");
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    wxStatusBar *status_bar = CreateStatusBar(2);
    status_bar->SetBackgroundColour(BACKGROUND_BLACK);
    status_bar->SetForegroundColour(FOREGROUND_WHITE);
    SetStatusText("Welcome! Please connect a serial port.");
#endif

    // ========================================================================
    // Create solenoid button panel
    // ========================================================================

    wxPanel *sol_panel = new wxPanel(this);
    wxSize sol_button_size = FromDIP(wxSize(64, 64));

    sol_buttons.reserve(NUM_OF_SOLENOIDS);

    wxFlexGridSizer *sol_gridSizer = new wxFlexGridSizer(NUM_OF_ROWS,
                                                         NUM_OF_COLS,
                                                         HORIZONTAL_MARGIN,
                                                         VERTICAL_MARGIN);

    // Make grid stretchable
    for (int col = 0; col < NUM_OF_COLS; col++)
    {
        sol_gridSizer->AddGrowableCol(col, 1);
    }

    for (int row = 0; row < NUM_OF_ROWS; row++)
    {
        sol_gridSizer->AddGrowableRow(row, 1);
    }

    // Create all solenoid buttons
    for (size_t i = 0; i < NUM_OF_SOLENOIDS; i++)
    {
        wxButton *button = new wxButton(sol_panel, SOL_ID_BASE + i, "");
        button->SetBackgroundColour(BUTTON_RED);
        button->SetMinSize(sol_button_size);
        sol_buttons.push_back(button);
        sol_gridSizer->Add(button, 1, wxEXPAND | wxALL, 2);

        button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MyFrame::Sol_Button_Clicked, this);
        button->Bind(wxEVT_RIGHT_UP, &MyFrame::Sol_Button_Program, this);
    }

    sol_panel->SetSizer(sol_gridSizer);

    // ========================================================================
    // Create sliders panel
    // ========================================================================

    wxPanel *sliders_panel = new wxPanel(this);
    sliders_panel->SetBackgroundColour(BACKGROUND_BLACK);
    sliders_panel->SetForegroundColour(FOREGROUND_WHITE);

    positive_pressure_slider = new wxSlider(sliders_panel, wxID_ANY, 50, 0, 100,
                                            wxDefaultPosition, wxDefaultSize,
                                            wxSL_HORIZONTAL | wxSL_LABELS | wxSL_VALUE_LABEL);

    vacuum_slider = new wxSlider(sliders_panel, wxID_ANY, 75, 0, 100,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxSL_HORIZONTAL | wxSL_LABELS | wxSL_VALUE_LABEL);

    positive_pressure_slider->SetBackgroundColour(BACKGROUND_BLACK);
    positive_pressure_slider->SetForegroundColour(FOREGROUND_WHITE);
    vacuum_slider->SetBackgroundColour(BACKGROUND_BLACK);
    vacuum_slider->SetForegroundColour(FOREGROUND_WHITE);

    positive_pressure_slider->Bind(wxEVT_SLIDER, &MyFrame::OnPositivePressureSlider, this);
    vacuum_slider->Bind(wxEVT_SLIDER, &MyFrame::OnVacuumSlider, this);

    wxStaticText *label1 = new wxStaticText(sliders_panel, wxID_ANY, "Positive Pressure:");
    wxStaticText *label2 = new wxStaticText(sliders_panel, wxID_ANY, "Vacuum:");
    label1->SetForegroundColour(FOREGROUND_WHITE);
    label2->SetForegroundColour(FOREGROUND_WHITE);

    wxFlexGridSizer *sliders_sizer = new wxFlexGridSizer(2, 2, 10, 10);
    sliders_sizer->Add(label1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sliders_sizer->Add(positive_pressure_slider, 1, wxEXPAND | wxALL, 5);
    sliders_sizer->Add(label2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sliders_sizer->Add(vacuum_slider, 1, wxEXPAND | wxALL, 5);
    sliders_sizer->AddGrowableCol(1, 1);

    sliders_panel->SetSizer(sliders_sizer);

    // ========================================================================
    // Main layout
    // ========================================================================

    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(sol_panel, 3, wxEXPAND | wxALL, 10);
    main_sizer->Add(sliders_panel, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    SetMinClientSize(wxSize(600, 500));
    SetClientSize(wxSize(800, 700));
    SetSizer(main_sizer);
}

MyFrame::~MyFrame()
{
    // Clean up serial port on exit
    if (serial_is_open())
    {
        close_port();
    }
}

// ============================================================================
// Event handlers
// ============================================================================

void MyFrame::Sol_Button_Clicked(wxCommandEvent &event)
{
    int button_id = event.GetId();
    size_t index = button_id - SOL_ID_BASE;

    // Use controller to toggle solenoid - it handles all the state and serial logic
    controller.toggleSolenoid(index);

    // Update button appearance based on new state
    UpdateButtonAppearance(index);
}

void MyFrame::Sol_Button_Program(wxMouseEvent &event)
{
    wxButton *clickedButton = (wxButton *)event.GetEventObject();
    int button_id = clickedButton->GetId();
    size_t index = button_id - SOL_ID_BASE;

    // Create and show timing dialog
    SolenoidTimingDialog dialog(this);

    if (dialog.ShowModal() == wxID_OK)
    {
        SolenoidTiming timing = dialog.Get_Timing();

        // Use controller to program the solenoid
        controller.programSolenoid(index, timing.openTime, timing.closeTime);

        // Update button label based on programming
        const Solenoid &sol = controller.getState().get(index);

        if (sol.programmed)
        {
            clickedButton->SetLabel(wxString::Format("Open %d ms\nClose %d ms",
                                                     sol.timing.openTime,
                                                     sol.timing.closeTime));
            SetStatusText(wxString::Format("Solenoid %zu programmed: Open=%dms, Close=%dms",
                                           index,
                                           sol.timing.openTime,
                                           sol.timing.closeTime));
        }
        else
        {
            // Cleared
            clickedButton->SetLabel("");
            SetStatusText(wxString::Format("Solenoid %zu programming cleared", index));
        }

        clickedButton->Refresh();
    }
}

void MyFrame::OnSerialPortSelected(wxCommandEvent &event)
{
    wxMenuItem *item = serialMenu->FindItem(event.GetId());
    if (!item)
        return;

    // Get port name from menu
    wxString portName = item->GetItemLabelText();
    const char *port = portName.mb_str();

    // Close existing connection if any
    if (serial_is_open())
    {
        close_port();
    }

    // Open the serial port (flags parameter not used in libserialport version)
    int result = init_serial(port, 0);

    if (result < 0)
    {
        wxMessageBox(wxString::Format("Failed to open port: %s", port),
                     "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Configure the serial port
    struct serial_config config;
    config.baud = B115200; // Now using enum value
    config.parity = false;
    config.one_stop_bit = true;
    config.data_size = DATA_8B;
    config.hw_flow = false;

    if (config_serial(&config) < 0)
    {
        wxMessageBox("Failed to configure serial port", "Error", wxOK | wxICON_ERROR);
        close_port();
        return;
    }

    wxMessageBox(wxString::Format("Successfully opened port: %s", port),
                 "Success", wxOK | wxICON_INFORMATION);

    SetStatusText(wxString::Format("Connected: %s", port));
}

void MyFrame::OnPositivePressureSlider(wxCommandEvent &event)
{
    int value = positive_pressure_slider->GetValue();

    // Use controller to set pressure
    controller.setPositivePressure(value);

    SetStatusText(wxString::Format("Positive Pressure: %d%%", value), 0);
}

void MyFrame::OnVacuumSlider(wxCommandEvent &event)
{
    int value = vacuum_slider->GetValue();

    // Use controller to set vacuum
    controller.setVacuum(value);

    SetStatusText(wxString::Format("Vacuum: %d%%", value), 1);
}

void MyFrame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
                     "Solenoid Control System\n"
                     "\n"
                     "Built with %s\n"
                     "Running under %s.",
                     wxVERSION_STRING,
                     wxGetOsDescription()),
                 "About Solenoid Control",
                 wxOK | wxICON_INFORMATION,
                 this);
}

// ============================================================================
// Helper methods
// ============================================================================

void MyFrame::UpdateButtonAppearance(size_t index)
{
    if (index >= sol_buttons.size())
        return;

    wxButton *button = sol_buttons[index];
    const Solenoid &sol = controller.getState().get(index);

    // Set color based on active state
    if (sol.active)
    {
        button->SetBackgroundColour(BUTTON_GREEN);
    }
    else
    {
        button->SetBackgroundColour(BUTTON_RED);
    }

    button->Refresh();
}