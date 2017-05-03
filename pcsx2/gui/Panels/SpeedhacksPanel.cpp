/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2010  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PrecompiledHeader.h"
#include "App.h"
#include "ConfigurationPanels.h"

using namespace pxSizerFlags;

const wxChar* Panels::SpeedHacksPanel::GetEEcycleSliderMsg( int val )
{
	switch( val )
	{
		case -3:
		{
			m_msg_eecycle->SetForegroundColour(wxColour(L"Red"));
			return pxEt(L"-3 - Reduces the EE's cyclerate to about 50%.  Big speedup, but *will* cause stuttering audio on many FMVs.");
		}
		case -2:
		{
			m_msg_eecycle->SetForegroundColour(wxColour(L"Red"));
			return pxEt(L"-2 - Reduces the EE's cyclerate to about 60%.  Moderate speedup, but may cause stuttering audio on many FMVs.");
		}
		case -1:
		{
			m_msg_eecycle->SetForegroundColour(wxColour(L"Red"));
			return pxEt(L"-1 - Reduces the EE's cyclerate to about 75%.  Mild speedup for most games with high compatibility.");
		}
		case 0:
		{
			const wxColour DarkSeaGreen = wxColour(14, 158, 19);
			m_msg_eecycle->SetForegroundColour(DarkSeaGreen);
			return pxEt(L"0 - Default cyclerate (100%). This closely matches the actual speed of a real PS2 EmotionEngine.");
		}
		case 1:
		{
			m_msg_eecycle->SetForegroundColour(wxColour(L"Red"));
			return pxEt(L"1 - Increases the EE's cyclerate to about 130%. Mildly increases hardware requirements, may increase in-game FPS.");
		}
		case 2:
		{
			m_msg_eecycle->SetForegroundColour(wxColour(L"Red"));
			return pxEt(L"2 - Increases the EE's cyclerate to about 180%. Increases hardware requirements, may noticeably increase in-game FPS.");
		}
		case 3:
		{
			m_msg_eecycle->SetForegroundColour(wxColour(L"Red"));
			return pxEt(L"3 - Increases the EE's cyclerate to about 300%. Greatly increases hardware requirements, may noticeably increase in-game FPS.\nThis setting can cause games to FAIL TO BOOT.");
		}
		default:
			break;
	}

	return L"Unreachable Warning Suppressor!!";
}

const wxChar* Panels::SpeedHacksPanel::GetVUcycleSliderMsg( int val )
{
	switch( val )
	{
		case 0:
		{
			const wxColour DarkSeaGreen = wxColour(14, 158, 19);
			m_msg_vustealer->SetForegroundColour(DarkSeaGreen);
			return pxEt(L"0 - Disables VU Cycle Stealing.  Most compatible setting!");
		}
		case 1:
		{
			m_msg_vustealer->SetForegroundColour(wxColour(L"Red"));
			return pxEt(L"1 - Mild VU Cycle Stealing.  Lower compatibility, but some speedup for most games.");
		}
		case 2:
		{
			m_msg_vustealer->SetForegroundColour(wxColour(L"Red"));
			return pxEt(L"2 - Moderate VU Cycle Stealing.  Even lower compatibility, but significant speedups in some games.");
		}
		case 3:
		{
			// TODO: Mention specific games that benefit from this setting here.
			m_msg_vustealer->SetForegroundColour(wxColour(L"Red"));
			return pxEt(L"3 - Maximum VU Cycle Stealing.  Usefulness is limited, as this will cause flickering visuals or slowdown in most games.");
		}
		default:
			break;
	}

	return L"Unreachable Warning Suppressor!!";
}

void Panels::SpeedHacksPanel::SetEEcycleSliderMsg()
{
	m_msg_eecycle->SetLabel( GetEEcycleSliderMsg(m_slider_eecycle->GetValue()) );
}

void Panels::SpeedHacksPanel::SetVUcycleSliderMsg()
{
	m_msg_vustealer->SetLabel( GetVUcycleSliderMsg(m_slider_vustealer->GetValue()) );
}

Panels::SpeedHacksPanel::SpeedHacksPanel( wxWindow* parent )
 : BaseApplicableConfigPanel_SpecificConfig( parent )
{
	const wxSizerFlags sliderFlags( wxSizerFlags().Border( wxLEFT | wxRIGHT, 8 ).Expand() );

	m_check_Enable = new pxCheckBox( this, _("Enable speedhacks"),
		pxE( L"Speedhacks usually improve emulation speed, but can cause glitches, broken audio, and false FPS readings.  When having emulation problems, disable this panel first."
		)
	);
	m_check_Enable->SetToolTip(_("A safe and easy way to make sure that all speedhacks are completely disabled.")).SetSubPadding( 1 );

	wxPanelWithHelpers* left	= new wxPanelWithHelpers( this, wxVERTICAL );
	wxPanelWithHelpers* right	= new wxPanelWithHelpers( this, wxVERTICAL );

	left->SetMinWidth( 350 );
	right->SetMinWidth( 350 );

	m_button_Defaults = new wxButton( right, wxID_DEFAULT, _("Restore Defaults") );

	// ------------------------------------------------------------------------
	// EE Cyclerate Hack Section:

	// Misc help text that I might find a home for later:
	// Cycle stealing works by 'fast-forwarding' the EE by an arbitrary number of cycles whenever VU1 micro-programs
	// are run, which works as a rough-guess skipping of what would normally be idle time spent running on the EE.

	m_eeSliderPanel = new wxPanelWithHelpers( left, wxVERTICAL, _("EE Cyclerate [Not Recommended]") );

	m_slider_eecycle = new wxSlider( m_eeSliderPanel, wxID_ANY, 0, -3, 3,
		wxDefaultPosition, wxDefaultSize, wxHORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS );

	m_msg_eecycle = new pxStaticHeading( m_eeSliderPanel );

	const wxChar* ee_tooltip = pxEt( L"Setting lower values on this slider effectively reduces the clock speed of the EmotionEngine's R5900 core cpu, and typically brings big speedups to games that fail to utilize the full potential of the real PS2 hardware. Conversely, higher values effectively increase the clock speed which may bring about an increase in in-game FPS while also making games more demanding and possibly causing glitches."
	);

	pxSetToolTip( m_slider_eecycle, ee_tooltip );
	pxSetToolTip( m_msg_eecycle, ee_tooltip );

	// ------------------------------------------------------------------------
	// VU Cycle Stealing Hack Section:

	m_vuSliderPanel = new wxPanelWithHelpers( right, wxVERTICAL, _("VU Cycle Stealing [Not Recommended]") );

	m_slider_vustealer = new wxSlider(m_vuSliderPanel, wxID_ANY, 0, 0, 3, wxDefaultPosition, wxDefaultSize,
		wxHORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS );

	m_msg_vustealer = new pxStaticHeading(m_vuSliderPanel);

	const wxChar* vu_tooltip = pxEt( L"This slider controls the amount of cycles the VU unit steals from the EmotionEngine.  Higher values increase the number of cycles stolen from the EE for each VU microprogram the game runs."
	);

	pxSetToolTip( m_slider_vustealer, vu_tooltip );
	pxSetToolTip( m_msg_vustealer, vu_tooltip );

	// ------------------------------------------------------------------------
	// microVU Hacks Section:

	wxPanelWithHelpers* vuHacksPanel = new wxPanelWithHelpers( right, wxVERTICAL, _("microVU Hacks") );

	m_check_vuFlagHack = new pxCheckBox( vuHacksPanel, _("mVU Flag Hack"),
		_("Good Speedup and High Compatibility; may cause bad graphics... [Recommended]" ) );

	m_check_vuThread = new pxCheckBox( vuHacksPanel, _("MTVU (Multi-Threaded microVU1)"),
		_("Good Speedup and High Compatibility; may cause hanging... [Recommended if 3+ cores]") );

	m_check_vuFlagHack->SetToolTip( pxEt( L"Updates Status Flags only on blocks which will read them, instead of all the time. This is safe most of the time, and Super VU does something similar by default."
	) );

	m_check_vuThread->SetToolTip( pxEt( L"Runs VU1 on its own thread (microVU1-only). Generally a speedup on CPUs with 3 or more cores. This is safe for most games, but a few games are incompatible and may hang. In the case of GS limited games, it may be a slowdown (especially on dual core CPUs)."
	) );

	// ------------------------------------------------------------------------
	// All other hacks Section:

	wxPanelWithHelpers* miscHacksPanel = new wxPanelWithHelpers( left, wxVERTICAL, _("Other Hacks") );

	m_check_intc = new pxCheckBox( miscHacksPanel, _("Enable INTC Spin Detection"),
		_("Huge speedup for some games, with almost no compatibility side effects. [Recommended]") );

	m_check_waitloop = new pxCheckBox( miscHacksPanel, _("Enable Wait Loop Detection"),
		_("Moderate speedup for some games, with no known side effects. [Recommended]" ) );

	m_check_fastCDVD = new pxCheckBox( miscHacksPanel, _("Enable fast CDVD"),
		_("Fast disc access, less loading times. [Not Recommended]") );


	m_check_intc->SetToolTip( pxEt( L"This hack works best for games that use the INTC Status register to wait for vsyncs, which includes primarily non-3D RPG titles. Games that do not use this method of vsync will see little or no speedup from this hack."
	) );

	m_check_waitloop->SetToolTip( pxEt( L"Primarily targetting the EE idle loop at address 0x81FC0 in the kernel, this hack attempts to detect loops whose bodies are guaranteed to result in the same machine state for every iteration until a scheduled event triggers emulation of another unit.  After a single iteration of such loops, we advance to the time of the next event or the end of the processor's timeslice, whichever comes first."
	) );

	m_check_fastCDVD->SetToolTip( pxEt( L"Check HDLoader compatibility lists for known games that have issues with this. (Often marked as needing 'mode 1' or 'slow DVD'"
	) );

	// ------------------------------------------------------------------------
	//  Layout and Size ---> (!!)

	//wxFlexGridSizer& DefEnableSizer( *new wxFlexGridSizer( 3, 0, 12 ) );
	//DefEnableSizer.AddGrowableCol( 1, 1 );
	//DefEnableSizer.AddGrowableCol( 2, 10 );
	//DefEnableSizer.AddGrowableCol( 1, 1 );
	//DefEnableSizer	+= m_button_Defaults	| StdSpace().Align( wxALIGN_LEFT );
	//DefEnableSizer	+= pxStretchSpacer(1);
	//DefEnableSizer	+= m_check_Enable		| StdExpand().Align( wxALIGN_RIGHT );

	*m_eeSliderPanel += m_slider_eecycle | sliderFlags;
	*m_eeSliderPanel += m_msg_eecycle | sliderFlags;

	*m_vuSliderPanel += m_slider_vustealer | sliderFlags;
	*m_vuSliderPanel += m_msg_vustealer | sliderFlags;

	*vuHacksPanel += m_check_vuFlagHack | StdExpand();
	*vuHacksPanel += m_check_vuThread | StdExpand();
	//*vuHacksPanel	+= 57; // Aligns left and right boxes in default language and font size

	*miscHacksPanel	+= m_check_intc | StdExpand();
	*miscHacksPanel	+= m_check_waitloop | StdExpand();
	*miscHacksPanel	+= m_check_fastCDVD | StdExpand();

	*left	+= m_eeSliderPanel | StdExpand();
	*left	+= miscHacksPanel	| StdExpand();

	*right	+= m_vuSliderPanel | StdExpand();
	*right	+= vuHacksPanel		| StdExpand();
	*right	+= StdPadding;
	*right	+= m_button_Defaults| StdButton();

	s_table = new wxFlexGridSizer( 2 );
	s_table->AddGrowableCol( 0, 1 );
	s_table->AddGrowableCol( 1, 1 );
	*s_table+= left				| pxExpand;
	*s_table+= right			| pxExpand;

	*this	+= m_check_Enable | StdExpand();
	*this	+= new wxStaticLine( this )	| pxExpand.Border(wxLEFT | wxRIGHT, 20);
	*this	+= StdPadding;
	*this	+= s_table					| pxExpand;

	// ------------------------------------------------------------------------

	Bind(wxEVT_SCROLL_CHANGED, &SpeedHacksPanel::EECycleRate_Scroll, this, m_slider_eecycle->GetId());
	Bind(wxEVT_SCROLL_CHANGED, &SpeedHacksPanel::VUCycleRate_Scroll, this, m_slider_vustealer->GetId());
	Bind(wxEVT_CHECKBOX, &SpeedHacksPanel::OnEnable_Toggled, this, m_check_Enable->GetId());
	Bind(wxEVT_BUTTON, &SpeedHacksPanel::Defaults_Click, this, wxID_DEFAULT);
}

// Doesn't modify values - only locks(gray out)/unlocks as necessary.
void Panels::SpeedHacksPanel::EnableStuff( AppConfig* configToUse )
{
	if (!configToUse) configToUse = g_Conf.get();

	bool hasPreset = configToUse->EnablePresets;
	bool hacksEnabled = configToUse->EnableSpeedHacks;
	bool HacksEnabledAndNoPreset = hacksEnabled && !hasPreset;

	// Main checkbox and Restore-defaults - locked only if presets are enabled
	m_check_Enable->Enable(!hasPreset);
	m_button_Defaults->Enable(!hasPreset);

	// lock/unlock the slider panels rather than the sliders themselves
	// in order to affect both sliders and texts
	m_eeSliderPanel->Enable(HacksEnabledAndNoPreset);
	m_vuSliderPanel->Enable(HacksEnabledAndNoPreset);

	// checkboxes
	m_check_vuFlagHack->Enable(HacksEnabledAndNoPreset);
	m_check_intc->Enable(HacksEnabledAndNoPreset);
	m_check_waitloop->Enable(HacksEnabledAndNoPreset);
	m_check_fastCDVD->Enable(HacksEnabledAndNoPreset);

	m_check_vuThread->Enable(hacksEnabled); // MTVU is unaffected by presets

	// Layout necessary to ensure changed slider text gets re-aligned properly
	// and to properly gray/ungray pxStaticText stuff (I suspect it causes a
	// paint event to be sent on Windows)
	TrigLayout();
}

void Panels::SpeedHacksPanel::AppStatusEvent_OnSettingsApplied()
{
	//Console.WriteLn("SpeedHacksPanel::AppStatusEvent_OnSettingsApplied()");
	ApplyConfigToGui( *g_Conf );
}

void Panels::SpeedHacksPanel::ApplyConfigToGui( AppConfig& configToApply, int flags )
{
	Pcsx2Config::SpeedhackOptions& opts=configToApply.EmuOptions.Speedhacks;

	// First, set the values of the widgets (checked/unchecked etc).
	m_check_Enable->SetValue(configToApply.EnableSpeedHacks);

	m_slider_eecycle	->SetValue( opts.EECycleRate );
	m_slider_vustealer	->SetValue( opts.VUCycleSteal );

	SetEEcycleSliderMsg();
	SetVUcycleSliderMsg();

	m_check_vuFlagHack->SetValue(opts.vuFlagHack);
	if( !(flags & AppConfig::APPLY_FLAG_FROM_PRESET) )
		m_check_vuThread	->SetValue(opts.vuThread);
	m_check_intc->SetValue(opts.IntcStat);
	m_check_waitloop->SetValue(opts.WaitLoop);
	m_check_fastCDVD->SetValue(opts.fastCDVD);

	// Then, lock(gray out)/unlock the widgets as necessary.
	EnableStuff( &configToApply );

	//Console.WriteLn("SpeedHacksPanel::ApplyConfigToGui: EnabledPresets: %s", configToApply.EnablePresets?"true":"false");
}

// Apply the values from the widgets to the config,
// regardless if locked (grayed out) or not.
void Panels::SpeedHacksPanel::Apply()
{
	g_Conf->EnableSpeedHacks = m_check_Enable->GetValue();

	Pcsx2Config::SpeedhackOptions& opts( g_Conf->EmuOptions.Speedhacks );

	opts.EECycleRate		= m_slider_eecycle->GetValue();
	opts.VUCycleSteal		= m_slider_vustealer->GetValue();

	opts.WaitLoop			= m_check_waitloop->GetValue();
	opts.fastCDVD			= m_check_fastCDVD->GetValue();
	opts.IntcStat			= m_check_intc->GetValue();
	opts.vuFlagHack			= m_check_vuFlagHack->GetValue();
	opts.vuThread			= m_check_vuThread->GetValue();

	// If the user has a command line override specified, we need to disable it
	// so that their changes take effect
	wxGetApp().Overrides.DisableSpeedhacks = false;
}

void Panels::SpeedHacksPanel::OnEnable_Toggled( wxCommandEvent& evt )
{
	AppConfig tmp=*g_Conf;
	tmp.EnablePresets=false; //if clicked, button was enabled, so not using a preset --> let EnableStuff work
	tmp.EnableSpeedHacks = m_check_Enable->GetValue();

	EnableStuff( &tmp );
	evt.Skip();
}

void Panels::SpeedHacksPanel::Defaults_Click( wxCommandEvent& evt )
{
	//Can only get here presets are disabled at the GUI (= the 'Defaults' button is enabled).
	AppConfig currentConfigWithHacksReset = *g_Conf;
	currentConfigWithHacksReset.EmuOptions.Speedhacks = Pcsx2Config::SpeedhackOptions();
	currentConfigWithHacksReset.EnablePresets=false;//speed hacks gui depends on preset, apply it as if presets are disabled
	ApplyConfigToGui( currentConfigWithHacksReset );
	evt.Skip();
}

void Panels::SpeedHacksPanel::EECycleRate_Scroll(wxScrollEvent &event)
{
	SetEEcycleSliderMsg();

	TrigLayout();

	event.Skip();
}

void Panels::SpeedHacksPanel::VUCycleRate_Scroll(wxScrollEvent &event)
{
	SetVUcycleSliderMsg();

	TrigLayout();

	event.Skip();
}

void Panels::SpeedHacksPanel::TrigLayout()
{
	// Reset the size information so wxWidgets can compute best value
	wxSize reset(-1, -1);
	m_eeSliderPanel->SetMinSize(reset);
	m_vuSliderPanel->SetMinSize(reset);

	// Take into account the current shape
	Layout();

	// Get the height of both slider boxes
	int ee_min = m_eeSliderPanel->GetSize().GetHeight();
	int vu_min = m_vuSliderPanel->GetSize().GetHeight();
	wxSize max_min(-1, std::max(ee_min, vu_min));

	// Align the small slider box on the big one.
	m_eeSliderPanel->SetMinSize(max_min);
	m_vuSliderPanel->SetMinSize(max_min);
	Layout();

	// Propagate the info to parent so main windows is resized accordingly
	wxWindow* win = this;
	do {
		win->Fit();
	} while (win = win->GetParent());
}

namespace pxGUIPanels
{
SpeedhacksPanel::SpeedhacksPanel(wxWindow *parent)
    : wxPanel(parent)
{
    auto sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    auto sizer_flags = wxSizerFlags().Expand().Border(wxALL);

    m_ee_cyclerate_messages = {
        {-3, _("Reduces the EE's cyclerate to about 50%. Big speedup, but *will* cause stuttering audio on many FMVs.")},
        {-2, _("Reduces the EE's cyclerate to about 60%. Moderate speedup, but may cause stuttering audio on many FMVs.")},
        {-1, _("Reduces the EE's cyclerate to about 75%. Mild speedup for most games with high compatibility.")},
        {-0, _("Default cyclerate (100%). This closely matches the actual speed of a real PS2 EmotionEngine.")},
        {1, _("Increases the EE's cyclerate to about 130%. Mildly increases hardware requirements, may increase in-game FPS.")},
        {2, _("Increases the EE's cyclerate to about 180%. Increases hardware requirements, may noticeably increase in-game FPS.")},
        {3, _("Increases the EE's cyclerate to about 300%. Greatly increases hardware requirements, may noticeably increase in-game FPS.\nThis setting can cause games to FAIL TO BOOT.")},
    };

    m_vu_cyclesteal_messages = {
        {0, _("Disables VU Cycle Stealing. Most compatible setting!")},
        {1, _("Mild VU Cycle Stealing. Lower compatibility, but some speedup for most games.")},
        {2, _("Moderate VU Cycle Stealing. Even lower compatibility, but significant speedups in some games.")},
        {3, _("Maximum VU Cycle Stealing. Usefulness is limited, as this will cause flickering visuals or slowdown in most games.")},
    };

    m_enable_speedhacks = new wxCheckBox(this, wxID_ANY, _("Enable speedhacks"));
    m_enable_speedhacks_text = new pxGUI::StaticText(this, wxID_ANY,
                                                     _("Speedhacks usually improve emulation speed, but can cause glitches, broken audio, and false FPS readings.  When having emulation problems, disable this panel first."));

    sizer->Add(m_enable_speedhacks, wxSizerFlags().Expand().Border(wxTOP | wxLEFT | wxRIGHT));
    sizer->Add(m_enable_speedhacks_text, wxSizerFlags().Expand().Border(wxBOTTOM | wxLEFT | wxRIGHT));

    sizer->Add(CreateSliderRow(), sizer_flags);
    sizer->Add(CreateHacksRow(), sizer_flags);

    Bind(wxEVT_CHECKBOX, &SpeedhacksPanel::EnableSpeedhacksCheckboxHandler, this, m_enable_speedhacks->GetId());
    Bind(wxEVT_SLIDER, &SpeedhacksPanel::EESliderEventHandler, this, m_ee_cyclerate->GetId());
    Bind(wxEVT_SLIDER, &SpeedhacksPanel::VUSliderEventHandler, this, m_vu_cyclesteal->GetId());

    Bind(pxEVT_SETTING, &SpeedhacksPanel::SettingEventHandler, this);
}

wxSizer *SpeedhacksPanel::CreateSliderRow()
{
    auto sizer = new wxGridSizer(2);
    auto sizer_flags = wxSizerFlags().Expand().Centre().Border(wxALL);

    auto ee_sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("EE Cyclerate [Not Recommended]"));
    m_ee_cyclerate_box = ee_sizer->GetStaticBox();
    m_ee_cyclerate = new wxSlider(ee_sizer->GetStaticBox(), wxID_ANY, 0, -3, 3, wxDefaultPosition, wxDefaultSize, wxHORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
    m_ee_cyclerate_text = new pxGUI::StaticText(ee_sizer->GetStaticBox(), wxID_ANY, m_ee_cyclerate_messages[0], wxALIGN_CENTRE_HORIZONTAL);
    wxString ee_cyclerate_tooltip(_("Setting lower values on this slider effectively reduces the clock speed of the EmotionEngine's R5900 core cpu, and typically brings big speedups to games that fail to utilize the full potential of the real PS2 hardware. Conversely, higher values effectively increase the clock speed which may bring about an increase in in-game FPS while also making games more demanding and possibly causing glitches."));
    pxSetToolTip(m_ee_cyclerate, ee_cyclerate_tooltip);
    pxSetToolTip(m_ee_cyclerate_text, ee_cyclerate_tooltip);
    ee_sizer->Add(m_ee_cyclerate, sizer_flags);
    ee_sizer->Add(m_ee_cyclerate_text, sizer_flags);

    auto vu_sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("VU Cycle Stealing [Not Recommended]"));
    m_vu_cyclesteal_box = vu_sizer->GetStaticBox();
    m_vu_cyclesteal = new wxSlider(vu_sizer->GetStaticBox(), wxID_ANY, 0, 0, 3, wxDefaultPosition, wxDefaultSize, wxHORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
    m_vu_cyclesteal_text = new pxGUI::StaticText(vu_sizer->GetStaticBox(), wxID_ANY, m_vu_cyclesteal_messages[0], wxALIGN_CENTRE_HORIZONTAL);
    wxString vu_cyclesteal_tooltip(_("This slider controls the amount of cycles the VU unit steals from the EmotionEngine.  Higher values increase the number of cycles stolen from the EE for each VU microprogram the game runs."));
    pxSetToolTip(m_vu_cyclesteal, vu_cyclesteal_tooltip);
    pxSetToolTip(m_vu_cyclesteal_text, vu_cyclesteal_tooltip);
    vu_sizer->Add(m_vu_cyclesteal, sizer_flags);
    vu_sizer->Add(m_vu_cyclesteal_text, sizer_flags);

    sizer->Add(ee_sizer, wxSizerFlags().Expand().Border(wxRIGHT));
    sizer->Add(vu_sizer, wxSizerFlags().Expand().Border(wxLEFT));

    return sizer;
}

wxSizer *SpeedhacksPanel::CreateHacksRow()
{
    auto sizer = new wxGridSizer(2);
    auto top_sizer_flags = wxSizerFlags().Expand().Border(wxTOP | wxLEFT | wxRIGHT);
    auto bottom_sizer_flags = wxSizerFlags().Expand().Border(wxBOTTOM | wxLEFT | wxRIGHT);

    auto other_sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Other Hacks"));

    m_otherhacks_box = other_sizer->GetStaticBox();
    m_intc = new wxCheckBox(other_sizer->GetStaticBox(), wxID_ANY, _("Enable INTC Spin Detection"));
    auto intc_text = new pxGUI::StaticText(other_sizer->GetStaticBox(), wxID_ANY, _("Huge speedup for some games, with almost no compatibility side effects. [Recommended]"));
    m_wait_loop = new wxCheckBox(other_sizer->GetStaticBox(), wxID_ANY, _("Enable Wait Loop Detection"));
    auto wait_loop_text = new pxGUI::StaticText(other_sizer->GetStaticBox(), wxID_ANY, _("Moderate speedup for some games, with no known side effects. [Recommended]"));
    m_fast_cdvd = new wxCheckBox(other_sizer->GetStaticBox(), wxID_ANY, _("Enable fast CDVD"));
    auto fast_cdvd_text = new pxGUI::StaticText(other_sizer->GetStaticBox(), wxID_ANY, _("Fast disc access, less loading times. [Not Recommended]"));
    wxString intc_tooltip(_("This hack works best for games that use the INTC Status register to wait for vsyncs, which includes primarily non-3D RPG titles. Games that do not use this method of vsync will see little or no speedup from this hack."));
    pxSetToolTip(m_intc, intc_tooltip);
    pxSetToolTip(intc_text, intc_tooltip);

    wxString wait_loop_tooltip(_("Primarily targetting the EE idle loop at address 0x81FC0 in the kernel, this hack attempts to detect loops whose bodies are guaranteed to result in the same machine state for every iteration until a scheduled event triggers emulation of another unit.  After a single iteration of such loops, we advance to the time of the next event or the end of the processor's timeslice, whichever comes first."));
    pxSetToolTip(m_wait_loop, wait_loop_tooltip);
    pxSetToolTip(wait_loop_text, wait_loop_tooltip);

    wxString fast_cdvd_tooltip(_("Check HDLoader compatibility lists for known games that have issues with this. (Often marked as needing 'mode 1' or 'slow DVD'"));
    pxSetToolTip(m_fast_cdvd, fast_cdvd_tooltip);
    pxSetToolTip(fast_cdvd_text, fast_cdvd_tooltip);

    other_sizer->Add(m_intc, top_sizer_flags);
    other_sizer->Add(intc_text, bottom_sizer_flags);
    other_sizer->Add(m_wait_loop, top_sizer_flags);
    other_sizer->Add(wait_loop_text, bottom_sizer_flags);
    other_sizer->Add(m_fast_cdvd, top_sizer_flags);
    other_sizer->Add(fast_cdvd_text, bottom_sizer_flags);

    auto microvu_sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("microVU Hacks"));

    m_mvu_flag = new wxCheckBox(microvu_sizer->GetStaticBox(), wxID_ANY, _("mVU Flag Hack"));
    m_mvu_flag_text = new pxGUI::StaticText(microvu_sizer->GetStaticBox(), wxID_ANY, _("Good Speedup and High Compatibility; may cause bad graphics... [Recommended]"));
    m_mtvu = new wxCheckBox(microvu_sizer->GetStaticBox(), wxID_ANY, _("MTVU (Multi-Threaded microVU1)"));
    m_mtvu_text = new pxGUI::StaticText(microvu_sizer->GetStaticBox(), wxID_ANY, _("Good Speedup and High Compatibility; may cause hanging... [Recommended if 3+ cores]"));
    wxString mvu_flag_tooltip(_("Updates Status Flags only on blocks which will read them, instead of all the time. This is safe most of the time, and Super VU does something similar by default."));
    pxSetToolTip(m_mvu_flag, mvu_flag_tooltip);
    pxSetToolTip(m_mvu_flag_text, mvu_flag_tooltip);
    wxString mtvu_tooltip(_("Runs VU1 on its own thread (microVU1-only). Generally a speedup on CPUs with 3 or more cores. This is safe for most games, but a few games are incompatible and may hang. In the case of GS limited games, it may be a slowdown (especially on dual core CPUs)."));
    pxSetToolTip(m_mtvu, mtvu_tooltip);
    pxSetToolTip(m_mtvu_text, mtvu_tooltip);
    microvu_sizer->Add(m_mvu_flag, top_sizer_flags);
    microvu_sizer->Add(m_mvu_flag_text, bottom_sizer_flags);
    microvu_sizer->Add(m_mtvu, top_sizer_flags);
    microvu_sizer->Add(m_mtvu_text, bottom_sizer_flags);

    auto restore_defaults = new wxButton(this, wxID_DEFAULT, _("Restore Defaults"));

    auto right_sizer = new wxBoxSizer(wxVERTICAL);
    right_sizer->Add(microvu_sizer, wxSizerFlags().Expand());
    right_sizer->Add(restore_defaults, wxSizerFlags().Right());

    sizer->Add(other_sizer, wxSizerFlags().Expand().Border(wxRIGHT));
    sizer->Add(right_sizer, wxSizerFlags().Expand().Border(wxLEFT));

    return sizer;
}

void SpeedhacksPanel::UpdateSpeedhacksState(bool presets_enabled)
{
    bool enable_speedhacks = m_enable_speedhacks->IsChecked();

    m_ee_cyclerate_box->Enable(enable_speedhacks && !presets_enabled);
    m_vu_cyclesteal_box->Enable(enable_speedhacks && !presets_enabled);
    m_otherhacks_box->Enable(enable_speedhacks && !presets_enabled);
    m_mvu_flag->Enable(enable_speedhacks && !presets_enabled);
    m_mvu_flag_text->Enable(enable_speedhacks && !presets_enabled);

    m_mtvu->Enable(enable_speedhacks);
    m_mtvu_text->Enable(enable_speedhacks);
}

void SpeedhacksPanel::UpdateEECyclerateMessage()
{
    int choice = m_ee_cyclerate->GetValue();

    const wxColour colour = wxColour(choice == 0 ? "Forest Green" : "Red");
    m_ee_cyclerate_text->SetText(m_ee_cyclerate_messages[choice]);
    m_ee_cyclerate_text->SetForegroundColour(colour);
}

void SpeedhacksPanel::UpdateVUStealingMessage()
{
    int choice = m_vu_cyclesteal->GetValue();

    const wxColour colour = wxColour(choice == 0 ? "Forest Green" : "Red");
    m_vu_cyclesteal_text->SetText(m_vu_cyclesteal_messages[choice]);
    m_vu_cyclesteal_text->SetForegroundColour(colour);
}

void SpeedhacksPanel::EnableSpeedhacksCheckboxHandler(wxCommandEvent &evt)
{
    evt.Skip();

    UpdateSpeedhacksState();
}

void SpeedhacksPanel::EESliderEventHandler(wxCommandEvent &evt)
{
    evt.Skip();

    int choice = evt.GetInt();
    UpdateEECyclerateMessage();
}
void SpeedhacksPanel::VUSliderEventHandler(wxCommandEvent &evt)
{
    evt.Skip();

    UpdateVUStealingMessage();
}

void SpeedhacksPanel::ApplyGUIToConfig(AppConfig &config)
{
    config.EmuOptions.Speedhacks.EECycleRate = m_ee_cyclerate->GetValue();
    config.EmuOptions.Speedhacks.VUCycleSteal = m_vu_cyclesteal->GetValue();

    config.EmuOptions.Speedhacks.IntcStat = m_intc->IsChecked();
    config.EmuOptions.Speedhacks.WaitLoop = m_wait_loop->IsChecked();
    config.EmuOptions.Speedhacks.fastCDVD = m_fast_cdvd->IsChecked();

    config.EmuOptions.Speedhacks.vuFlagHack = m_mvu_flag->IsChecked();
    config.EmuOptions.Speedhacks.vuThread = m_mtvu->IsChecked();
}

void SpeedhacksPanel::ApplyConfigToGUI(AppConfig &config, bool apply_from_preset)
{
    m_enable_speedhacks->SetValue(config.EnableSpeedHacks);

    auto ee_cyclerate = std::max<s8>(m_ee_cyclerate_messages.cbegin()->first, std::min<s8>(config.EmuOptions.Speedhacks.EECycleRate, m_ee_cyclerate_messages.crbegin()->first));
    auto vu_cyclesteal = std::max<s8>(m_vu_cyclesteal_messages.cbegin()->first, std::min<s8>(config.EmuOptions.Speedhacks.VUCycleSteal, m_vu_cyclesteal_messages.crbegin()->first));

    m_ee_cyclerate->SetValue(config.EmuOptions.Speedhacks.EECycleRate);
    m_vu_cyclesteal->SetValue(config.EmuOptions.Speedhacks.VUCycleSteal);

    UpdateEECyclerateMessage();
    UpdateVUStealingMessage();

    m_intc->SetValue(config.EmuOptions.Speedhacks.IntcStat);
    m_wait_loop->SetValue(config.EmuOptions.Speedhacks.WaitLoop);
    m_fast_cdvd->SetValue(config.EmuOptions.Speedhacks.fastCDVD);

    m_mvu_flag->SetValue(config.EmuOptions.Speedhacks.vuFlagHack);
    if (!apply_from_preset)
        m_mtvu->SetValue(config.EmuOptions.Speedhacks.vuThread);

    m_enable_speedhacks->Enable(!config.EnablePresets);
    m_enable_speedhacks_text->Enable(!config.EnablePresets);

    UpdateSpeedhacksState(config.EnablePresets);
}

void SpeedhacksPanel::SettingEventHandler(pxSettingEvent &evt)
{
    using Action = pxSettingEvent::Action;
    Action action = evt.GetAction();
    AppConfig &config = evt.GetConfig();
    if (action == Action::ApplyGUIToConfig) {
        ApplyGUIToConfig(config);
    } else {
        bool apply_from_preset = action == Action::ApplyPresetToGUI;
        ApplyConfigToGUI(config, apply_from_preset);
    }
}
}
