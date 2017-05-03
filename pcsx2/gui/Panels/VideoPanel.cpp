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
#include "Dialogs/ConfigurationDialog.h"
#include "ConfigurationPanels.h"

#include <wx/spinctrl.h>
#include <wx/valnum.h>

#include <array>
#include <tuple>

using namespace pxSizerFlags;

// --------------------------------------------------------------------------------------
//  FramelimiterPanel Implementations
// --------------------------------------------------------------------------------------

Panels::FramelimiterPanel::FramelimiterPanel( wxWindow* parent )
	: BaseApplicableConfigPanel_SpecificConfig( parent )
{
	m_check_LimiterDisable = new pxCheckBox( this, _("Disable Framelimiting"),
		_("Useful for running benchmarks. Toggle this option in-game by pressing F4.") );

	m_check_LimiterDisable->SetToolTip( pxEt( L"Note that when Framelimiting is disabled, Turbo and SlowMotion modes will not be available either."
	) );

	m_spin_NominalPct = new wxSpinCtrl(this);
	m_spin_SlomoPct = new wxSpinCtrl(this);
	m_spin_TurboPct = new wxSpinCtrl(this);

	m_text_BaseNtsc		= CreateNumericalTextCtrl( this, 7 );
	m_text_BasePal		= CreateNumericalTextCtrl( this, 7 );

	m_spin_NominalPct	->SetRange( 10,  1000 );
	m_spin_SlomoPct		->SetRange(  1,  1000 );
	m_spin_TurboPct		->SetRange( 10,  1000 );

	// ------------------------------------------------------------
	// Sizers and Layouts

	*this += m_check_LimiterDisable | StdExpand();

	wxFlexGridSizer& s_spins( *new wxFlexGridSizer( 5 ) );
	s_spins.AddGrowableCol( 0 );

	s_spins += Label(_("Base Framerate Adjust:"))	| StdExpand();
	s_spins += 5;
	s_spins += m_spin_NominalPct					| pxBorder(wxTOP, 3);
	s_spins += Label(L"%")							| StdExpand();
	s_spins += 5;

	s_spins += Label(_("Slow Motion Adjust:"))		| StdExpand();
	s_spins += 5;
	s_spins += m_spin_SlomoPct						| pxBorder(wxTOP, 3);
	s_spins += Label(L"%")							| StdExpand();
	s_spins += 5;

	s_spins	+= Label(_("Turbo Adjust:"))			| StdExpand();
	s_spins	+= 5;
	s_spins	+= m_spin_TurboPct						| pxBorder(wxTOP, 3);
	s_spins	+= Label(L"%" )							| StdExpand();
	s_spins	+= 5;

	wxFlexGridSizer& s_fps( *new wxFlexGridSizer( 5 ) );
	s_fps.AddGrowableCol( 0 );

	s_fps	+= Label(_("NTSC Framerate:"))	| StdExpand();
	s_fps	+= 5;
	s_fps	+= m_text_BaseNtsc				| pxBorder(wxTOP, 2).Right();
	s_fps	+= Label(_("FPS"))				| StdExpand();
	s_fps	+= 5;

	s_fps	+= Label(_("PAL Framerate:"))	| StdExpand();
	s_fps	+= 5;
	s_fps	+= m_text_BasePal				| pxBorder(wxTOP, 2).Right();
	s_fps	+= Label(_("FPS"))				| StdExpand();
	s_fps	+= 5;

	*this	+= s_spins	| pxExpand;
	*this	+= 5;
	*this	+= s_fps	| pxExpand;

	AppStatusEvent_OnSettingsApplied();
}

void Panels::FramelimiterPanel::AppStatusEvent_OnSettingsApplied()
{
	ApplyConfigToGui( *g_Conf );
}

void Panels::FramelimiterPanel::ApplyConfigToGui( AppConfig& configToApply, int flags )
{
	const AppConfig::FramerateOptions& appfps( configToApply.Framerate );
	const Pcsx2Config::GSOptions& gsconf( configToApply.EmuOptions.GS );

	if( ! (flags & AppConfig::APPLY_FLAG_FROM_PRESET) ){	//Presets don't control these: only change if config doesn't come from preset.
	
		m_check_LimiterDisable->SetValue( !gsconf.FrameLimitEnable );

		m_spin_TurboPct		->SetValue( appfps.TurboScalar.Raw );
		m_spin_SlomoPct		->SetValue( appfps.SlomoScalar.Raw );

		m_spin_TurboPct		->Enable( 1 );
		m_spin_SlomoPct		->Enable( 1 );
	}

	m_text_BaseNtsc		->ChangeValue( gsconf.FramerateNTSC.ToString() );
	m_text_BasePal		->ChangeValue( gsconf.FrameratePAL.ToString() );

	m_spin_NominalPct	->SetValue( appfps.NominalScalar.Raw );
	m_spin_NominalPct	->Enable(!configToApply.EnablePresets);

	// Vsync timing controls only on devel builds / via manual ini editing
#ifdef PCSX2_DEVBUILD
	m_text_BaseNtsc		->Enable(!configToApply.EnablePresets);
	m_text_BasePal		->Enable(!configToApply.EnablePresets);
#else
	m_text_BaseNtsc		->Enable( 0 );
	m_text_BasePal		->Enable( 0 );
#endif
}

void Panels::FramelimiterPanel::Apply()
{
	AppConfig::FramerateOptions& appfps( g_Conf->Framerate );
	Pcsx2Config::GSOptions& gsconf( g_Conf->EmuOptions.GS );

	gsconf.FrameLimitEnable	= !m_check_LimiterDisable->GetValue();

	appfps.NominalScalar.Raw	= m_spin_NominalPct	->GetValue();
	appfps.TurboScalar.Raw		= m_spin_TurboPct	->GetValue();
	appfps.SlomoScalar.Raw		= m_spin_SlomoPct	->GetValue();

	try {
		gsconf.FramerateNTSC	= Fixed100::FromString( m_text_BaseNtsc->GetValue() );
		gsconf.FrameratePAL		= Fixed100::FromString( m_text_BasePal->GetValue() );
	}
	catch( Exception::ParseError& )
	{
		throw Exception::CannotApplySettings( this )
			.SetDiagMsg(pxsFmt(
				L"Error while parsing either NTSC or PAL framerate settings.\n\tNTSC Input = %s\n\tPAL Input  = %s",
				WX_STR(m_text_BaseNtsc->GetValue()), WX_STR(m_text_BasePal->GetValue())
			) )
			.SetUserMsg(_t("Error while parsing either NTSC or PAL framerate settings.  Settings must be valid floating point numerics."));
	}

	appfps.SanityCheck();

	// If the user has a command line override specified, we need to disable it
	// so that their changes take effect
	wxGetApp().Overrides.ProfilingMode = false;
}

// --------------------------------------------------------------------------------------
//  FrameSkipPanel Implementations
// --------------------------------------------------------------------------------------

Panels::FrameSkipPanel::FrameSkipPanel( wxWindow* parent )
	: BaseApplicableConfigPanel_SpecificConfig( parent )
{
	const RadioPanelItem FrameskipOptions[] =
	{
		RadioPanelItem(
			_("Disabled [default]")
		),

		RadioPanelItem(
			_("Skip when on Turbo only (TAB to enable)")
		),

		RadioPanelItem(
			_("Constant skipping"),
			wxEmptyString,
			_("Normal and Turbo limit rates skip frames.  Slow motion mode will still disable frameskipping.")
		),
	};

	m_radio_SkipMode = new pxRadioPanel( this, FrameskipOptions );
	m_radio_SkipMode->Realize();

	m_spin_FramesToDraw = new wxSpinCtrl(this);
	m_spin_FramesToSkip = new wxSpinCtrl(this);

	// Set tooltips for spinners.


	// ------------------------------------------------------------
	// Sizers and Layouts

	*this += m_radio_SkipMode;

	wxFlexGridSizer& s_spins( *new wxFlexGridSizer( 4 ) );
	//s_spins.AddGrowableCol( 0 );

	s_spins += m_spin_FramesToDraw			| pxBorder(wxTOP, 2);
	s_spins += 10;
	s_spins += Label(_("Frames to Draw"))	| StdExpand();
	s_spins += 10;

	s_spins += m_spin_FramesToSkip			| pxBorder(wxTOP, 2);
	s_spins += 10;
	s_spins += Label(_("Frames to Skip"))	| StdExpand();
	s_spins += 10;

	*this	+= s_spins	| StdExpand();

	*this	+= Text( pxE( L"Notice: Due to PS2 hardware design, precise frame skipping is impossible. Enabling it will cause severe graphical errors in some games." )
	) | StdExpand();

	*this += 24; // Extends the right box to match the left one. Only works with (Windows) 100% dpi.

	AppStatusEvent_OnSettingsApplied();
}

void Panels::FrameSkipPanel::AppStatusEvent_OnSettingsApplied()
{
	ApplyConfigToGui( *g_Conf );
}

void Panels::FrameSkipPanel::ApplyConfigToGui( AppConfig& configToApply, int flags )
{
	const AppConfig::FramerateOptions& appfps( configToApply.Framerate );
	const Pcsx2Config::GSOptions& gsconf( configToApply.EmuOptions.GS );

	m_radio_SkipMode->SetSelection( appfps.SkipOnLimit ? 2 : (appfps.SkipOnTurbo ? 1 : 0) );

	m_spin_FramesToDraw->SetValue( gsconf.FramesToDraw );
	m_spin_FramesToDraw->Enable(!configToApply.EnablePresets);
	m_spin_FramesToSkip->SetValue( gsconf.FramesToSkip );
	m_spin_FramesToSkip->Enable(!configToApply.EnablePresets);

	this->Enable(!configToApply.EnablePresets);
}


void Panels::FrameSkipPanel::Apply()
{
	AppConfig::FramerateOptions& appfps( g_Conf->Framerate );
	Pcsx2Config::GSOptions& gsconf( g_Conf->EmuOptions.GS );

	gsconf.FramesToDraw = m_spin_FramesToDraw->GetValue();
	gsconf.FramesToSkip = m_spin_FramesToSkip->GetValue();

	switch( m_radio_SkipMode->GetSelection() )
	{
		case 0:
			appfps.SkipOnLimit = false;
			appfps.SkipOnTurbo = false;
			gsconf.FrameSkipEnable = false;
		break;

		case 1:
			appfps.SkipOnLimit = false;
			appfps.SkipOnTurbo = true;
			//gsconf.FrameSkipEnable = true;
		break;

		case 2:
			appfps.SkipOnLimit = true;
			appfps.SkipOnTurbo = true;
			gsconf.FrameSkipEnable = true;
		break;
	}

	appfps.SanityCheck();
}

// --------------------------------------------------------------------------------------
//  VideoPanel Implementation
// --------------------------------------------------------------------------------------

Panels::VideoPanel::VideoPanel( wxWindow* parent ) :
	BaseApplicableConfigPanel_SpecificConfig( parent )
{
	wxPanelWithHelpers* left	= new wxPanelWithHelpers( this, wxVERTICAL );
	wxPanelWithHelpers* right	= new wxPanelWithHelpers( this, wxVERTICAL );

	m_check_SynchronousGS = new pxCheckBox( left, _("Use Synchronized MTGS"),
		_t("For troubleshooting potential bugs in the MTGS only, as it is potentially very slow.")
	);

	m_check_DisableOutput = new pxCheckBox( left, _("Disable all GS output"),
		_t("Completely disables all GS plugin activity; ideal for benchmarking EEcore components.")
	);

	m_check_SynchronousGS->SetToolTip( pxEt( L"Enable this if you think MTGS thread sync is causing crashes or graphical errors.")
	) ;

	m_check_DisableOutput->SetToolTip( pxEt( L"Removes any benchmark noise caused by the MTGS thread or GPU overhead.  This option is best used in conjunction with savestates: save a state at an ideal scene, enable this option, and re-load the savestate.\n\nWarning: This option can be enabled on-the-fly but typically cannot be disabled on-the-fly (video will typically be garbage)."
	) );

	m_check_DisableOutput->Hide(); // Band-aid fix since currently broken

	//GSWindowSettingsPanel* winpan = new GSWindowSettingsPanel( left );
	//winpan->AddFrame(_("Display/Window"));

	m_span = new FrameSkipPanel( right );
	m_span->AddFrame(_("Frame Skipping"));

	m_fpan = new FramelimiterPanel( left );
	m_fpan->AddFrame(_("Framelimiter"));

	wxFlexGridSizer* s_table = new wxFlexGridSizer( 2 );
	s_table->AddGrowableCol( 0, 1 );
	s_table->AddGrowableCol( 1, 1 );

	*right		+= m_span		| pxExpand;
	*right		+= 5;

	*left		+= m_fpan		| pxExpand;
	*left		+= 5;
	*left		+= m_check_SynchronousGS | StdExpand();
	*left		+= m_check_DisableOutput;

	*s_table	+= left		| StdExpand();
	*s_table	+= right	| StdExpand();

	*this		+= s_table	| pxExpand;

	AppStatusEvent_OnSettingsApplied();
}

void Panels::VideoPanel::OnOpenWindowSettings( wxCommandEvent& evt )
{
	AppOpenDialog<Dialogs::ComponentsConfigDialog>( this );

	// don't evt.skip, this prevents the Apply button from being activated. :)
}

void Panels::VideoPanel::Apply()
{
	g_Conf->EmuOptions.GS.SynchronousMTGS	= m_check_SynchronousGS->GetValue();
	g_Conf->EmuOptions.GS.DisableOutput		= m_check_DisableOutput->GetValue();
}

void Panels::VideoPanel::AppStatusEvent_OnSettingsApplied()
{
	ApplyConfigToGui(*g_Conf);
}

void Panels::VideoPanel::ApplyConfigToGui( AppConfig& configToApply, int flags ){
	
	m_check_SynchronousGS->SetValue( configToApply.EmuOptions.GS.SynchronousMTGS );
	m_check_DisableOutput->SetValue( configToApply.EmuOptions.GS.DisableOutput );

	m_check_SynchronousGS->Enable(!configToApply.EnablePresets);
	m_check_DisableOutput->Enable(!configToApply.EnablePresets);

	if( flags & AppConfig::APPLY_FLAG_MANUALLY_PROPAGATE )
	{
		m_span->ApplyConfigToGui( configToApply, true );
		m_fpan->ApplyConfigToGui( configToApply, true );
	}

	Layout();
}

namespace pxGUIPanels
{
GSPanel::GSPanel(wxWindow *parent)
    : wxPanel(parent)
{
    auto sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    auto sizer_flags = wxSizerFlags().Expand().Border(wxALL);

    auto grid_sizer = new wxGridSizer(2);
    grid_sizer->Add(CreateFrameLimiterBox(), wxSizerFlags().Expand().Border(wxRIGHT));
    grid_sizer->Add(CreateFrameSkipBox(), wxSizerFlags().Expand().Border(wxLEFT));
    sizer->Add(grid_sizer, sizer_flags);

    m_sync_mtgs = new wxCheckBox(this, wxID_ANY, _("Use Synchronized MTGS"));
    m_sync_mtgs_text = new pxGUI::StaticText(this, wxID_ANY, _("For troubleshooting potential bugs in the MTGS only, as it is potentially very slow."));

    sizer->Add(m_sync_mtgs, wxSizerFlags().Expand().Border(wxTOP | wxLEFT | wxRIGHT));
    sizer->Add(m_sync_mtgs_text, wxSizerFlags().Expand().Border(wxBOTTOM | wxLEFT | wxRIGHT));

    m_disable_gs_output = new wxCheckBox(this, wxID_ANY, _("Disable all GS output"));
    m_disable_gs_output_text = new pxGUI::StaticText(this, wxID_ANY, _("Completely disables all GS plugin activity; ideal for benchmarking EEcore components."));

    sizer->Add(m_disable_gs_output, wxSizerFlags().Expand().Border(wxTOP | wxLEFT | wxRIGHT).ReserveSpaceEvenIfHidden());
    sizer->Add(m_disable_gs_output_text, wxSizerFlags().Expand().Border(wxBOTTOM | wxLEFT | wxRIGHT).ReserveSpaceEvenIfHidden());

    // Broken options
    m_disable_gs_output->Hide();
    m_disable_gs_output_text->Hide();

    Bind(pxEVT_SETTING, &GSPanel::SettingEventHandler, this);
}

wxSizer *GSPanel::CreateFrameLimiterBox()
{
    auto sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Framelimiter"));
    auto parent = sizer->GetStaticBox();
    auto sizer_flags = wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT);
    auto left_sizer_flags = wxSizerFlags().Expand().Border(wxTOP | wxBOTTOM | wxRIGHT);
    auto right_sizer_flags = wxSizerFlags().Expand().Border(wxTOP | wxBOTTOM | wxLEFT);

    m_disable_framelimiter = new wxCheckBox(parent, wxID_ANY, _("Disable Framelimiting"));
    auto disable_framelimiter_text = new pxGUI::StaticText(parent, wxID_ANY, _("Useful for running benchmarks. Toggle this option in-game by pressing F4."));

    sizer->Add(m_disable_framelimiter, sizer_flags);
    sizer->Add(disable_framelimiter_text, sizer_flags);
    sizer->AddSpacer(10);

    // Framerate
    auto speed_sizer = new wxFlexGridSizer(2);
    speed_sizer->AddGrowableCol(0);

    m_base_framerate_text = new pxGUI::StaticText(parent, wxID_ANY, _("Base Framerate Adjust (%):"));
    m_base_framerate = new wxSpinCtrl(parent);
    m_base_framerate->SetRange(10, 1000);
    speed_sizer->Add(m_base_framerate_text, left_sizer_flags);
    speed_sizer->Add(m_base_framerate, right_sizer_flags);

    auto slow_framerate_text = new pxGUI::StaticText(parent, wxID_ANY, _("Slow Motion Adjust (%):"));
    m_slow_framerate = new wxSpinCtrl(parent);
    m_slow_framerate->SetRange(1, 1000);
    speed_sizer->Add(slow_framerate_text, left_sizer_flags);
    speed_sizer->Add(m_slow_framerate, right_sizer_flags);

    auto turbo_framerate_text = new pxGUI::StaticText(parent, wxID_ANY, _("Turbo Adjust (%):"));
    m_turbo_framerate = new wxSpinCtrl(parent);
    m_turbo_framerate->SetRange(10, 1000);
    speed_sizer->Add(turbo_framerate_text, left_sizer_flags);
    speed_sizer->Add(m_turbo_framerate, right_sizer_flags);

    sizer->Add(speed_sizer, sizer_flags);
    sizer->AddSpacer(10);

    // NTSC/PAL
    auto ntsc_pal_sizer = new wxFlexGridSizer(2);
    ntsc_pal_sizer->AddGrowableCol(0);

    wxFloatingPointValidator<float> validate(2, nullptr);
    validate.SetRange(0.05f, 1000.0f);

    m_ntsc_framerate_text = new pxGUI::StaticText(parent, wxID_ANY, _("NTSC Framerate (FPS):"));
    m_ntsc_framerate = new wxTextCtrl(parent, wxID_ANY, "59.94", wxDefaultPosition, wxDefaultSize, wxTE_RIGHT, validate);
    ntsc_pal_sizer->Add(m_ntsc_framerate_text, left_sizer_flags);
    ntsc_pal_sizer->Add(m_ntsc_framerate, right_sizer_flags);

    m_pal_framerate_text = new pxGUI::StaticText(parent, wxID_ANY, _("PAL Framerate (FPS):"));
    m_pal_framerate = new wxTextCtrl(parent, wxID_ANY, "50.00", wxDefaultPosition, wxDefaultSize, wxTE_RIGHT, validate);
    ntsc_pal_sizer->Add(m_pal_framerate_text, left_sizer_flags);
    ntsc_pal_sizer->Add(m_pal_framerate, right_sizer_flags);

    sizer->Add(ntsc_pal_sizer, sizer_flags);

    return sizer;
}

wxSizer *GSPanel::CreateFrameSkipBox()
{
    const std::map<FrameSkip, std::pair<wxString, wxString>> frame_skip_options{
        {FrameSkip::Disabled, {_("Disabled [default]"), ""}},
        {FrameSkip::SkipOnTurbo, {_("Skip when on Turbo only (TAB to enable)"), ""}},
        {FrameSkip::SkipConstantly, {_("Constant skipping"), _("Normal and Turbo limit rates skip frames.  Slow motion mode will still disable frameskipping.")}},
    };

    auto sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Frame Skipping"));
    auto parent = sizer->GetStaticBox();
    m_frameskipping_box = parent;
    auto sizer_flags = wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT);
    auto left_sizer_flags = wxSizerFlags().Expand().Border(wxTOP | wxBOTTOM | wxRIGHT);
    auto right_sizer_flags = wxSizerFlags().Expand().Border(wxTOP | wxBOTTOM | wxLEFT);

    m_frame_skip_choices = new pxGUI::RadioPanel<FrameSkip>(parent, frame_skip_options, FrameSkip::Disabled);

    sizer->Add(m_frame_skip_choices, sizer_flags);
    sizer->AddSpacer(10);

    auto constant_skip_sizer = new wxFlexGridSizer(2);
    constant_skip_sizer->AddGrowableCol(0);

    m_frames_to_draw_text = new pxGUI::StaticText(parent, wxID_ANY, _("Frames to Draw:"));
    m_frames_to_draw = new wxSpinCtrl(parent);
    m_frames_to_draw->SetRange(0, 10);
    constant_skip_sizer->Add(m_frames_to_draw_text, left_sizer_flags);
    constant_skip_sizer->Add(m_frames_to_draw, right_sizer_flags);

    m_frames_to_skip_text = new pxGUI::StaticText(parent, wxID_ANY, _("Frames to Skip:"));
    m_frames_to_skip = new wxSpinCtrl(parent);
    m_frames_to_skip->SetRange(0, 10);
    constant_skip_sizer->Add(m_frames_to_skip_text, left_sizer_flags);
    constant_skip_sizer->Add(m_frames_to_skip, right_sizer_flags);

    sizer->Add(constant_skip_sizer, sizer_flags);
    sizer->AddSpacer(10);

    auto notice = new pxGUI::StaticText(parent, wxID_ANY, _("Notice: Due to PS2 hardware design, precise frame skipping is impossible. Enabling it will cause severe graphical errors in some games."), wxALIGN_CENTER_HORIZONTAL);
    sizer->Add(notice, sizer_flags);

    return sizer;
}

void GSPanel::ApplyGUIToConfig(AppConfig &config)
{
    config.EmuOptions.GS.FrameLimitEnable = !m_disable_framelimiter->IsChecked();
    config.Framerate.NominalScalar.SetRaw(m_base_framerate->GetValue());
    config.Framerate.SlomoScalar.SetRaw(m_slow_framerate->GetValue());
    config.Framerate.TurboScalar.SetRaw(m_turbo_framerate->GetValue());
    config.EmuOptions.GS.FramerateNTSC.FromString(m_ntsc_framerate->GetValue());
    config.EmuOptions.GS.FrameratePAL.FromString(m_pal_framerate->GetValue());

    auto frameskip_choice = m_frame_skip_choices->GetValue();
    config.Framerate.SkipOnLimit = frameskip_choice == FrameSkip::SkipConstantly;
    config.Framerate.SkipOnTurbo = frameskip_choice == FrameSkip::SkipOnTurbo;

    config.EmuOptions.GS.FramesToDraw = m_frames_to_draw->GetValue();
    config.EmuOptions.GS.FramesToSkip = m_frames_to_skip->GetValue();

    config.EmuOptions.GS.SynchronousMTGS = m_sync_mtgs->GetValue();
    config.EmuOptions.GS.DisableOutput = m_disable_gs_output->GetValue();
}

void GSPanel::ApplyConfigToGUI(AppConfig &config, bool apply_from_preset)
{
    // Framelimiting
    if (!apply_from_preset)
        m_disable_framelimiter->SetValue(!config.EmuOptions.GS.FrameLimitEnable);
    m_base_framerate->SetValue(config.Framerate.NominalScalar.Raw);
    if (!apply_from_preset) {
        m_slow_framerate->SetValue(config.Framerate.SlomoScalar.Raw);
        m_turbo_framerate->SetValue(config.Framerate.TurboScalar.Raw);
    }
    m_ntsc_framerate->SetValue(config.EmuOptions.GS.FramerateNTSC.ToString());
    m_pal_framerate->SetValue(config.EmuOptions.GS.FrameratePAL.ToString());

    m_base_framerate_text->Enable(!config.EnablePresets);
    m_base_framerate->Enable(!config.EnablePresets);
#if PCSX2_DEVBUILD
    m_ntsc_framerate_text->Enable(!config.EnablePresets);
    m_ntsc_framerate->Enable(!config.EnablePresets);
    m_pal_framerate_text->Enable(!config.EnablePresets);
    m_pal_framerate->Enable(!config.EnablePresets);
#else
    m_ntsc_framerate_text->Enable(0);
    m_ntsc_framerate->Enable(0);
    m_pal_framerate_text->Enable(0);
    m_pal_framerate->Enable(0);
#endif

    // Frameskipping
    if (config.Framerate.SkipOnLimit)
        m_frame_skip_choices->SetValue(FrameSkip::SkipConstantly);
    else if (config.Framerate.SkipOnTurbo)
        m_frame_skip_choices->SetValue(FrameSkip::SkipOnTurbo);
    else
        m_frame_skip_choices->SetValue(FrameSkip::Disabled);

    m_frameskipping_box->Enable(!config.EnablePresets);

    m_frames_to_draw->SetValue(config.EmuOptions.GS.FramesToDraw);
    m_frames_to_skip->SetValue(config.EmuOptions.GS.FramesToSkip);

    // Misc
    m_sync_mtgs->SetValue(config.EmuOptions.GS.SynchronousMTGS);
    m_disable_gs_output->SetValue(config.EmuOptions.GS.DisableOutput);

    m_sync_mtgs->Enable(!config.EnablePresets);
    m_sync_mtgs_text->Enable(!config.EnablePresets);

    m_disable_gs_output->Enable(!config.EnablePresets);
    m_disable_gs_output_text->Enable(!config.EnablePresets);
}

void GSPanel::SettingEventHandler(pxSettingEvent &evt)
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
