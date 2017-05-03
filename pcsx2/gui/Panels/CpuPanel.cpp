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
#include "ConfigurationPanels.h"

#include <array>
#include <tuple>

using namespace pxSizerFlags;

Panels::BaseAdvancedCpuOptions::BaseAdvancedCpuOptions( wxWindow* parent )
	: BaseApplicableConfigPanel_SpecificConfig( parent )
{
	wxStaticBoxSizer*	s_round( new wxStaticBoxSizer( wxVERTICAL, this, _("Round Mode") ) );
	wxStaticBoxSizer*	s_clamp( new wxStaticBoxSizer( wxVERTICAL, this, _("Clamping Mode") ) );

	const RadioPanelItem RoundModeChoices[] =
	{
		RadioPanelItem(_("Nearest")),
		RadioPanelItem(_("Negative")),
		RadioPanelItem(_("Positive")),
		RadioPanelItem(_("Chop / Zero"))
	};

	const RadioPanelItem ClampModeChoices[] =
	{
		RadioPanelItem(_("None")),
		RadioPanelItem(_("Normal")),
	};

	m_RoundModePanel = new pxRadioPanel( this, RoundModeChoices );
	m_ClampModePanel = new pxRadioPanel( this, ClampModeChoices );

	// Highlight Default Options:

	m_RoundModePanel->SetDefaultItem( 3 );
	m_ClampModePanel->SetDefaultItem( 1 );

	// ---------------------------------
	//    The Fitting And Sizing Area
	// ---------------------------------

	wxFlexGridSizer& grid = *new wxFlexGridSizer( 4 );

	// Clever proportions selected for a fairly nice spacing.

	grid.AddGrowableCol( 0, 22 ); // round mode box size
	grid.AddGrowableCol( 1, 22 ); // clamp mode box size

	*s_round+= m_RoundModePanel		| StdExpand();
	*s_clamp+= m_ClampModePanel		| StdExpand();

	grid	+= s_round				| SubGroup();
	grid	+= s_clamp				| SubGroup();

	*this	+= grid					| StdExpand();
}

void Panels::BaseAdvancedCpuOptions::OnRestoreDefaults(wxCommandEvent& evt)
{
	RestoreDefaults();
	evt.Skip();
}

void Panels::BaseAdvancedCpuOptions::RestoreDefaults()
{
	AppConfig def;               // created with default values
	def.EnablePresets = false;   // disable presets otherwise it'll disable some widgets
	ApplyConfigToGui(def);
}

Panels::AdvancedOptionsFPU::AdvancedOptionsFPU( wxWindow* parent )
	: BaseAdvancedCpuOptions( parent )
{
	SetName( L"AdvancedOptionsFPU" );
	AddFrame(_("EE/FPU Advanced Recompiler Options"));

	m_ClampModePanel->Append(_("Extra + Preserve Sign"));
	m_ClampModePanel->Append(_("Full"));

	m_RoundModePanel->Realize();
	m_ClampModePanel->Realize();
}


Panels::AdvancedOptionsVU::AdvancedOptionsVU( wxWindow* parent )
	: BaseAdvancedCpuOptions( parent )
{
	SetName( L"AdvancedOptionsVU" );
	AddFrame(_("VU0 / VU1 Advanced Recompiler Options"));

	m_ClampModePanel->Append(_("Extra"));
	m_ClampModePanel->Append(_("Extra + Preserve Sign"));

	m_RoundModePanel->Realize();
	m_ClampModePanel->Realize();
}

Panels::CpuPanelEE::CpuPanelEE( wxWindow* parent )
	: BaseApplicableConfigPanel_SpecificConfig( parent )
{
	*this	+= Text( pxE(L"Notice: Most games are fine with the default options.")
	) | StdExpand();

	const RadioPanelItem tbl_CpuTypes_EE[] =
	{
		RadioPanelItem(_("Interpreter"))
		.SetToolTip(_("Quite possibly the slowest thing in the universe.")),

		RadioPanelItem(_("Recompiler"))
		.SetToolTip(_("Performs just-in-time binary translation of 64-bit MIPS-IV machine code to x86."))
	};

	const RadioPanelItem tbl_CpuTypes_IOP[] =
	{
		RadioPanelItem(_("Interpreter"))
		.SetToolTip(_("Pretty slow; provided for diagnostic purposes only.")),

		RadioPanelItem(_("Recompiler"))
		.SetToolTip(_("Performs just-in-time binary translation of 32-bit MIPS-I machine code to x86."))
	};


	m_panel_RecEE	= &(new pxRadioPanel( this, tbl_CpuTypes_EE ))->SetDefaultItem( 1 );
	m_panel_RecIOP	= &(new pxRadioPanel( this, tbl_CpuTypes_IOP ))->SetDefaultItem( 1 );

	m_panel_RecEE->Realize();
	m_panel_RecIOP->Realize();

	// ---------------------------------
	//    The Fitting And Sizing Area
	// ---------------------------------

	wxFlexGridSizer&	s_recs( *new wxFlexGridSizer( 2 ) );

	s_recs.AddGrowableCol( 0, 1 );
	s_recs.AddGrowableCol( 1, 1 );

	// i18n: No point in translating PS2 CPU names :)
	wxStaticBoxSizer& s_ee	( *new wxStaticBoxSizer( wxVERTICAL, this, L"EmotionEngine" ) );
	wxStaticBoxSizer& s_iop	( *new wxStaticBoxSizer( wxVERTICAL, this, L"IOP" ) );

	s_ee	+= m_panel_RecEE	| StdExpand();
	s_ee    += m_check_EECacheEnable = &(new pxCheckBox( this, _("Enable EE Cache (Slower)") ))->SetToolTip(_("Interpreter only; provided for diagnostic"));
	s_iop	+= m_panel_RecIOP	| StdExpand();

	s_recs	+= s_ee				| SubGroup();
	s_recs	+= s_iop			| SubGroup();

	
	*this	+= &s_recs							| StdExpand();
	
	// move following line down so EE and VU panels look more uniform. 
	// Use an empty Label (std expanded) so it work with custom font sizes, too.
	//*this	+= Label(_(""))  | StdExpand();
	*this	+= new wxStaticLine( this )			| pxExpand.Border(wxALL, 18);
	*this	+= (m_advancedOptsFpu = new AdvancedOptionsFPU( this ))	| StdExpand();

	*this	+= 12;
	m_button_RestoreDefaults = new wxButton(this, wxID_DEFAULT, _("Restore Defaults"));
	*this += m_button_RestoreDefaults | StdButton();

	Bind(wxEVT_BUTTON, &CpuPanelEE::OnRestoreDefaults, this, wxID_DEFAULT);
	Bind(wxEVT_RADIOBUTTON, &CpuPanelEE::EECache_Event, this);
}

Panels::CpuPanelVU::CpuPanelVU( wxWindow* parent )
	: BaseApplicableConfigPanel_SpecificConfig( parent )
{
	*this	+= Text( pxE( L"Notice: Most games are fine with the default options.")
	) | StdExpand();

	const RadioPanelItem tbl_CpuTypes_VU[] =
	{
		RadioPanelItem(_("Interpreter"))
		.SetToolTip(_("Vector Unit Interpreter. Slow and not very compatible. Only use for diagnostics.")),

		RadioPanelItem(_("microVU Recompiler"))
		.SetToolTip(_("New Vector Unit recompiler with much improved compatibility. Recommended.")),

#ifndef DISABLE_SVU
		RadioPanelItem(_("superVU Recompiler [legacy]"))
		.SetToolTip(_("Useful for diagnosing bugs or clamping issues in the new mVU recompiler."))
#endif
	};

	m_panel_VU0 = &(new pxRadioPanel( this, tbl_CpuTypes_VU ))	->SetDefaultItem( 1 );
	m_panel_VU1 = &(new pxRadioPanel( this, tbl_CpuTypes_VU ))	->SetDefaultItem( 1 );

	m_panel_VU0->Realize();
	m_panel_VU1->Realize();

	// ---------------------------------
	//    The Fitting And Sizing Area
	// ---------------------------------

	wxFlexGridSizer&	s_recs( *new wxFlexGridSizer( 2 ) );

	s_recs.AddGrowableCol( 0, 1 );
	s_recs.AddGrowableCol( 1, 1 );

	wxStaticBoxSizer& s_vu0( *new wxStaticBoxSizer( wxVERTICAL, this, L"VU0" ) );
	wxStaticBoxSizer& s_vu1( *new wxStaticBoxSizer( wxVERTICAL, this, L"VU1" ) );

	s_vu0	+= m_panel_VU0	| StdExpand();
	s_vu1	+= m_panel_VU1	| StdExpand();

	s_recs	+= s_vu0		| SubGroup();
	s_recs	+= s_vu1		| SubGroup();

	*this	+= &s_recs							| StdExpand();
	*this	+= new wxStaticLine( this )			| pxExpand.Border(wxALL, 18);
	*this	+= ( m_advancedOptsVu=new AdvancedOptionsVU( this ))	| StdExpand();

	*this	+= 12;
	m_button_RestoreDefaults = new wxButton(this, wxID_DEFAULT, _("Restore Defaults"));
	*this += m_button_RestoreDefaults | StdButton();

	Bind(wxEVT_BUTTON, &CpuPanelVU::OnRestoreDefaults, this, wxID_DEFAULT);
}

void Panels::CpuPanelEE::Apply()
{
	Pcsx2Config::RecompilerOptions& recOps( g_Conf->EmuOptions.Cpu.Recompiler );
	recOps.EnableEE		  = !!m_panel_RecEE->GetSelection();
	recOps.EnableIOP	  = !!m_panel_RecIOP->GetSelection();
	recOps.EnableEECache  = m_check_EECacheEnable->GetValue();
}

void Panels::CpuPanelEE::AppStatusEvent_OnSettingsApplied()
{
	ApplyConfigToGui( *g_Conf );
}

void Panels::CpuPanelEE::ApplyConfigToGui( AppConfig& configToApply, int flags )
{
	const Pcsx2Config::RecompilerOptions& recOps( configToApply.EmuOptions.Cpu.Recompiler );
	m_panel_RecEE->SetSelection( (int)recOps.EnableEE );
	m_panel_RecIOP->SetSelection( (int)recOps.EnableIOP );

	m_panel_RecEE->Enable(!configToApply.EnablePresets);
	m_panel_RecIOP->Enable(!configToApply.EnablePresets);

	//EECache option is exclusive to the EE Interpreter.
	m_check_EECacheEnable->SetValue(recOps.EnableEECache);
	m_check_EECacheEnable->Enable(!configToApply.EnablePresets && m_panel_RecEE->GetSelection() == 0);
	m_button_RestoreDefaults->Enable(!configToApply.EnablePresets);

	if( flags & AppConfig::APPLY_FLAG_MANUALLY_PROPAGATE )
	{
		m_advancedOptsFpu->ApplyConfigToGui( configToApply, true );
	}
}

void Panels::CpuPanelEE::OnRestoreDefaults(wxCommandEvent &evt)
{
	AppConfig def;               // created with default values
	def.EnablePresets = false;   // disable presets otherwise it'll disable some widgets
	ApplyConfigToGui(def);

	if( BaseAdvancedCpuOptions* opts = (BaseAdvancedCpuOptions*)FindWindowByName(L"AdvancedOptionsFPU") )
		opts->RestoreDefaults();

	evt.Skip();
}


void Panels::CpuPanelVU::Apply()
{
	Pcsx2Config::RecompilerOptions& recOps( g_Conf->EmuOptions.Cpu.Recompiler );
	recOps.EnableVU0	= m_panel_VU0->GetSelection() > 0;
	recOps.EnableVU1	= m_panel_VU1->GetSelection() > 0;

#ifndef DISABLE_SVU
	recOps.UseMicroVU0	= m_panel_VU0->GetSelection() == 1;
	recOps.UseMicroVU1	= m_panel_VU1->GetSelection() == 1;
#endif
}

void Panels::CpuPanelVU::AppStatusEvent_OnSettingsApplied()
{
	ApplyConfigToGui( *g_Conf );
}

void Panels::CpuPanelVU::ApplyConfigToGui( AppConfig& configToApply, int flags )
{
	Pcsx2Config::RecompilerOptions& recOps( configToApply.EmuOptions.Cpu.Recompiler );
#ifndef DISABLE_SVU
	if( recOps.UseMicroVU0 )
		m_panel_VU0->SetSelection( recOps.EnableVU0 ? 1 : 0 );
	else
		m_panel_VU0->SetSelection( recOps.EnableVU0 ? 2 : 0 );
#else
	m_panel_VU0->SetSelection( recOps.EnableVU0 ? 1 : 0 );
#endif

#ifndef DISABLE_SVU
	if( recOps.UseMicroVU1 )
		m_panel_VU1->SetSelection( recOps.EnableVU1 ? 1 : 0 );
	else
		m_panel_VU1->SetSelection( recOps.EnableVU1 ? 2 : 0 );
#else
	m_panel_VU1->SetSelection( recOps.EnableVU1 ? 1 : 0 );
#endif

	m_panel_VU0->Enable(!configToApply.EnablePresets);
	m_panel_VU1->Enable(!configToApply.EnablePresets);
	m_button_RestoreDefaults->Enable(!configToApply.EnablePresets);

	if ( flags & AppConfig::APPLY_FLAG_MANUALLY_PROPAGATE )
	{
		m_advancedOptsVu->ApplyConfigToGui( configToApply, true );
	}
}


void Panels::CpuPanelVU::OnRestoreDefaults(wxCommandEvent &evt)
{
	AppConfig def;               // created with default values
	def.EnablePresets = false;   // disable presets otherwise it'll disable some widgets
	ApplyConfigToGui(def);

	if( BaseAdvancedCpuOptions* opts = (BaseAdvancedCpuOptions*)FindWindowByName(L"AdvancedOptionsVU") )
		opts->RestoreDefaults();

	evt.Skip();
}

void Panels::BaseAdvancedCpuOptions::ApplyRoundmode( SSE_MXCSR& mxcsr )
{
	mxcsr.RoundingControl	= m_RoundModePanel->GetSelection();
	mxcsr.DenormalsAreZero	= 1;
	mxcsr.FlushToZero		= 1;
}

void Panels::AdvancedOptionsFPU::Apply()
{
	Pcsx2Config::CpuOptions& cpuOps( g_Conf->EmuOptions.Cpu );
	Pcsx2Config::RecompilerOptions& recOps( cpuOps.Recompiler );

	cpuOps.sseMXCSR = Pcsx2Config::CpuOptions().sseMXCSR;		// set default
	ApplyRoundmode( cpuOps.sseMXCSR );

	const int clampSel		= m_ClampModePanel->GetSelection();

	recOps.fpuOverflow		= clampSel >= 1;
	recOps.fpuExtraOverflow	= clampSel >= 2;
	recOps.fpuFullMode		= clampSel >= 3;

	cpuOps.ApplySanityCheck();
}

void Panels::AdvancedOptionsFPU::AppStatusEvent_OnSettingsApplied()
{
	ApplyConfigToGui( *g_Conf );
}

void Panels::AdvancedOptionsFPU::ApplyConfigToGui( AppConfig& configToApply, int flags )
{
	const Pcsx2Config::CpuOptions& cpuOps( configToApply.EmuOptions.Cpu );
	const Pcsx2Config::RecompilerOptions& recOps( cpuOps.Recompiler );

	m_RoundModePanel->SetSelection( cpuOps.sseMXCSR.RoundingControl );

	if( recOps.fpuFullMode )			m_ClampModePanel->SetSelection( 3 );
	else if( recOps.fpuExtraOverflow )	m_ClampModePanel->SetSelection( 2 );
	else if( recOps.fpuOverflow )		m_ClampModePanel->SetSelection( 1 );
	else								m_ClampModePanel->SetSelection( 0 );

	this->Enable(!configToApply.EnablePresets);
}

void Panels::AdvancedOptionsVU::Apply()
{
	Pcsx2Config::CpuOptions& cpuOps( g_Conf->EmuOptions.Cpu );
	Pcsx2Config::RecompilerOptions& recOps( cpuOps.Recompiler );

	cpuOps.sseVUMXCSR = Pcsx2Config::CpuOptions().sseVUMXCSR;		// set default
	ApplyRoundmode( cpuOps.sseVUMXCSR );

	const int clampSel		= m_ClampModePanel->GetSelection();

	recOps.vuOverflow		= clampSel >= 1;
	recOps.vuExtraOverflow	= clampSel >= 2;
	recOps.vuSignOverflow	= clampSel >= 3;

	cpuOps.ApplySanityCheck();
}

void Panels::AdvancedOptionsVU::AppStatusEvent_OnSettingsApplied()
{
	ApplyConfigToGui( *g_Conf );
}

void Panels::AdvancedOptionsVU::ApplyConfigToGui( AppConfig& configToApply, int flags )
{
	const Pcsx2Config::CpuOptions& cpuOps( configToApply.EmuOptions.Cpu );
	const Pcsx2Config::RecompilerOptions& recOps( cpuOps.Recompiler );

	m_RoundModePanel->SetSelection( cpuOps.sseVUMXCSR.RoundingControl );

	if( recOps.vuSignOverflow )			m_ClampModePanel->SetSelection( 3 );
	else if( recOps.vuExtraOverflow )	m_ClampModePanel->SetSelection( 2 );
	else if( recOps.vuOverflow )		m_ClampModePanel->SetSelection( 1 );
	else								m_ClampModePanel->SetSelection( 0 );

	this->Enable(!configToApply.EnablePresets);
}

void Panels::CpuPanelEE::EECache_Event(wxCommandEvent& event)
{
	m_check_EECacheEnable->Enable(m_panel_RecEE->GetSelection() == 0);
	event.Skip();
}

namespace pxGUIPanels
{
static wxWindow *CreateNoticeRow(wxWindow *parent)
{
    return new pxGUI::StaticText(parent, wxID_ANY, _("Notice: Most games are fine with the default options."), wxALIGN_CENTRE_HORIZONTAL);
}

EEIOPPanel::EEIOPPanel(wxWindow *parent)
    : wxPanel(parent)
{
    auto sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    const auto sizer_flags = wxSizerFlags().Expand().Border(wxALL);

    sizer->Add(CreateNoticeRow(this), sizer_flags);
    sizer->Add(CreateEEIOPRow(), sizer_flags);
    sizer->Add(new wxStaticLine(this), sizer_flags);
    sizer->Add(CreateRoundClampingRow(), sizer_flags);

    auto restore_defaults = new wxButton(this, wxID_DEFAULT, _("Restore Defaults"));
    sizer->Add(restore_defaults, wxSizerFlags().Border(wxALL).Right());

    Bind(pxEVT_SETTING, &EEIOPPanel::SettingEventHandler, this);
}

wxSizer *EEIOPPanel::CreateEEIOPRow()
{
    auto sizer = new wxGridSizer(2);
    wxSizerFlags sizer_flags = wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT);

    const std::map<EEExecMode, std::pair<wxString, wxString>> ee_exec_mode_options{
        {EEExecMode::Interpreter, {_("Interpreter"), _("Pretty slow; provided for diagnostic purposes only.")}},
        {EEExecMode::InterpreterWithEECache, {_("Interpreter with EE Cache"), _("Pretty slow; provided for diagnostic purposes only.")}},
        {EEExecMode::Recompiler, {_("Recompiler"), _("Performs just-in-time binary translation of 64-bit MIPS-IV machine code to x86.")}},
    };

    auto ee_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "EmotionEngine");
    m_ee_choices = new pxGUI::RadioPanel<EEExecMode>(ee_sizer->GetStaticBox(), ee_exec_mode_options, EEExecMode::Recompiler);
    m_ee_choices->SetFontWeight(EEExecMode::Recompiler, wxFontWeight::wxFONTWEIGHT_BOLD);
    ee_sizer->Add(m_ee_choices, sizer_flags);

    const std::map<IOPExecMode, std::pair<wxString, wxString>> iop_exec_mode_options{
        {IOPExecMode::Interpreter, {_("Interpreter"), _("Pretty slow; provided for diagnostic purposes only.")}},
        {IOPExecMode::Recompiler, {_("Recompiler"), _("Performs just-in-time binary translation of 32-bit MIPS-I machine code to x86.")}},
    };

    auto iop_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "IOP");
    m_iop_choices = new pxGUI::RadioPanel<IOPExecMode>(iop_sizer->GetStaticBox(), iop_exec_mode_options, IOPExecMode::Recompiler);
    m_iop_choices->SetFontWeight(IOPExecMode::Recompiler, wxFontWeight::wxFONTWEIGHT_BOLD);
    iop_sizer->Add(m_iop_choices, sizer_flags);

    sizer->Add(ee_sizer, wxSizerFlags().Expand().Border(wxRIGHT));
    sizer->Add(iop_sizer, wxSizerFlags().Expand().Border(wxLEFT));

    return sizer;
}

wxSizer *EEIOPPanel::CreateRoundClampingRow()
{
    auto sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("EE / FPU Advanced Recompiler Options"));

    const std::map<SSE_RoundMode, std::pair<wxString, wxString>> round_mode_options{
        {SSE_RoundMode::SSEround_Nearest, {_("Nearest"), ""}},
        {SSE_RoundMode::SSEround_NegInf, {_("Negative"), ""}},
        {SSE_RoundMode::SSEround_PosInf, {_("Positive"), ""}},
        {SSE_RoundMode::SSEround_Chop, {_("Chop / Zero"), ""}},
    };

    auto round_sizer = new wxStaticBoxSizer(wxVERTICAL, sizer->GetStaticBox(), _("Round Mode"));
    m_round_choices = new pxGUI::RadioPanel<SSE_RoundMode>(round_sizer->GetStaticBox(), round_mode_options, SSE_RoundMode::SSEround_Chop);
    m_round_choices->SetFontWeight(SSE_RoundMode::SSEround_Chop, wxFontWeight::wxFONTWEIGHT_BOLD);
    round_sizer->Add(m_round_choices, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

    const std::map<EEFPUClampMode, std::pair<wxString, wxString>> ee_fpu_clamp_mode_options{
        {EEFPUClampMode::None, {_("None"), ""}},
        {EEFPUClampMode::Normal, {_("Normal"), ""}},
        {EEFPUClampMode::ExtraAndPreserveSign, {_("Extra + Preserve Sign"), ""}},
        {EEFPUClampMode::Full, {_("Full"), ""}},
    };

    auto clamp_sizer = new wxStaticBoxSizer(wxVERTICAL, sizer->GetStaticBox(), _("Clamping Mode"));
    m_clamp_choices = new pxGUI::RadioPanel<EEFPUClampMode>(clamp_sizer->GetStaticBox(), ee_fpu_clamp_mode_options, EEFPUClampMode::Normal);
    m_clamp_choices->SetFontWeight(EEFPUClampMode::Normal, wxFontWeight::wxFONTWEIGHT_BOLD);
    clamp_sizer->Add(m_clamp_choices, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

    auto round_clamp_sizer = new wxGridSizer(2);
    round_clamp_sizer->Add(round_sizer, wxSizerFlags().Expand().Border(wxRIGHT));
    round_clamp_sizer->Add(clamp_sizer, wxSizerFlags().Expand().Border(wxLEFT));
    sizer->Add(round_clamp_sizer, wxSizerFlags().Expand().Border(wxALL));

    return sizer;
}

void EEIOPPanel::ApplyGUIToConfig(AppConfig &config)
{
    Pcsx2Config::CpuOptions &cpu_options(config.EmuOptions.Cpu);

    auto ee_mode = m_ee_choices->GetValue();
    cpu_options.Recompiler.EnableEE = ee_mode == EEExecMode::Recompiler;
    cpu_options.Recompiler.EnableEECache = ee_mode == EEExecMode::InterpreterWithEECache;

    auto iop_mode = m_iop_choices->GetValue();
    cpu_options.Recompiler.EnableIOP = iop_mode == IOPExecMode::Recompiler;

    cpu_options.sseMXCSR.SetRoundMode(m_round_choices->GetValue());
    // Copied from older code - Is it necessary to set these here?
    cpu_options.sseMXCSR.DenormalsAreZero = 1;
    cpu_options.sseMXCSR.FlushToZero = 1;

    auto clamp_mode = m_clamp_choices->GetValue();
    cpu_options.Recompiler.fpuOverflow = clamp_mode >= EEFPUClampMode::Normal;
    cpu_options.Recompiler.fpuExtraOverflow = clamp_mode >= EEFPUClampMode::ExtraAndPreserveSign;
    cpu_options.Recompiler.fpuFullMode = clamp_mode >= EEFPUClampMode::Full;
}

void EEIOPPanel::ApplyConfigToGUI(AppConfig &config)
{
    Pcsx2Config::CpuOptions &cpu_options(config.EmuOptions.Cpu);

    if (cpu_options.Recompiler.EnableEE)
        m_ee_choices->SetValue(EEExecMode::Recompiler);
    else if (cpu_options.Recompiler.EnableEECache)
        m_ee_choices->SetValue(EEExecMode::InterpreterWithEECache);
    else
        m_ee_choices->SetValue(EEExecMode::Interpreter);

    if (cpu_options.Recompiler.EnableIOP)
        m_iop_choices->SetValue(IOPExecMode::Recompiler);
    else
        m_iop_choices->SetValue(IOPExecMode::Interpreter);

    m_round_choices->SetValue(cpu_options.sseMXCSR.GetRoundMode());

    if (cpu_options.Recompiler.fpuFullMode)
        m_clamp_choices->SetValue(EEFPUClampMode::Full);
    else if (cpu_options.Recompiler.fpuExtraOverflow)
        m_clamp_choices->SetValue(EEFPUClampMode::ExtraAndPreserveSign);
    else if (cpu_options.Recompiler.fpuOverflow)
        m_clamp_choices->SetValue(EEFPUClampMode::Normal);
    else
        m_clamp_choices->SetValue(EEFPUClampMode::None);

    this->Enable(!config.EnablePresets);
}

void EEIOPPanel::SettingEventHandler(pxSettingEvent &evt)
{
    using Action = pxSettingEvent::Action;
    Action action = evt.GetAction();
    AppConfig &config = evt.GetConfig();
    if (action == Action::ApplyGUIToConfig) {
        ApplyGUIToConfig(config);
    } else {
        ApplyConfigToGUI(config);
    }
}

VUPanel::VUPanel(wxWindow *parent)
    : wxPanel(parent)
{
    auto sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    const auto sizer_flags = wxSizerFlags().Expand().Border(wxALL);

    sizer->Add(CreateNoticeRow(this), sizer_flags);
    sizer->Add(CreateVURow(), sizer_flags);
    sizer->Add(new wxStaticLine(this), sizer_flags);
    sizer->Add(CreateRoundClampingRow(), sizer_flags);

    auto restore_defaults = new wxButton(this, wxID_DEFAULT, _("Restore Defaults"));
    sizer->Add(restore_defaults, wxSizerFlags().Border(wxALL).Right());

    Bind(pxEVT_SETTING, &VUPanel::SettingEventHandler, this);
}

wxSizer *VUPanel::CreateVURow()
{
    auto sizer = new wxGridSizer(2);
    wxSizerFlags sizer_flags = wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT);

    const std::map<VUExecMode, std::pair<wxString, wxString>> vu_exec_mode_options{
        {VUExecMode::Interpreter, {_("Interpreter"), _("Vector Unit Interpreter. Slow and not very compatible. Only use for diagnostics.")}},
        {VUExecMode::MicroVURecompiler, {_("microVU Recompiler"), _("New Vector Unit recompiler with much improved compatibility. Recommended.")}},
        {VUExecMode::SuperVURecompiler, {_("superVU Recompiler [legacy]"), _("Useful for diagnosing bugs or clamping issues in the new mVU recompiler.")}},
    };

    auto vu0_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "VU0");
    m_vu0_choices = new pxGUI::RadioPanel<VUExecMode>(vu0_sizer->GetStaticBox(), vu_exec_mode_options, VUExecMode::MicroVURecompiler);
    m_vu0_choices->SetFontWeight(VUExecMode::MicroVURecompiler, wxFontWeight::wxFONTWEIGHT_BOLD);
    vu0_sizer->Add(m_vu0_choices, sizer_flags);

    auto vu1_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "VU1");
    m_vu1_choices = new pxGUI::RadioPanel<VUExecMode>(vu1_sizer->GetStaticBox(), vu_exec_mode_options, VUExecMode::MicroVURecompiler);
    m_vu1_choices->SetFontWeight(VUExecMode::MicroVURecompiler, wxFontWeight::wxFONTWEIGHT_BOLD);
    vu1_sizer->Add(m_vu1_choices, sizer_flags);

    sizer->Add(vu0_sizer, wxSizerFlags().Expand().Border(wxRIGHT));
    sizer->Add(vu1_sizer, wxSizerFlags().Expand().Border(wxLEFT));

    return sizer;
}

wxSizer *VUPanel::CreateRoundClampingRow()
{
    auto sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("VU0 / VU1 Advanced Recompiler Options"));

    const std::map<SSE_RoundMode, std::pair<wxString, wxString>> round_mode_options{
        {SSE_RoundMode::SSEround_Nearest, {_("Nearest"), ""}},
        {SSE_RoundMode::SSEround_NegInf, {_("Negative"), ""}},
        {SSE_RoundMode::SSEround_PosInf, {_("Positive"), ""}},
        {SSE_RoundMode::SSEround_Chop, {_("Chop / Zero"), ""}},
    };

    auto round_sizer = new wxStaticBoxSizer(wxVERTICAL, sizer->GetStaticBox(), _("Round Mode"));
    m_round_choices = new pxGUI::RadioPanel<SSE_RoundMode>(round_sizer->GetStaticBox(), round_mode_options, SSE_RoundMode::SSEround_Chop);
    m_round_choices->SetFontWeight(SSE_RoundMode::SSEround_Chop, wxFontWeight::wxFONTWEIGHT_BOLD);
    round_sizer->Add(m_round_choices, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

    const std::map<VUClampMode, std::pair<wxString, wxString>> vu_clamp_mode_options{
        {VUClampMode::None, {_("None"), ""}},
        {VUClampMode::Normal, {_("Normal"), ""}},
        {VUClampMode::Extra, {_("Extra"), ""}},
        {VUClampMode::ExtraAndPreserveSign, {_("Extra + Preserve Sign"), ""}},
    };

    auto clamp_sizer = new wxStaticBoxSizer(wxVERTICAL, sizer->GetStaticBox(), _("Clamping Mode"));
    m_clamp_choices = new pxGUI::RadioPanel<VUClampMode>(clamp_sizer->GetStaticBox(), vu_clamp_mode_options, VUClampMode::Normal);
    m_clamp_choices->SetFontWeight(VUClampMode::Normal, wxFontWeight::wxFONTWEIGHT_BOLD);
    clamp_sizer->Add(m_clamp_choices, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

    auto round_clamp_sizer = new wxGridSizer(2);
    round_clamp_sizer->Add(round_sizer, wxSizerFlags().Expand().Border(wxRIGHT));
    round_clamp_sizer->Add(clamp_sizer, wxSizerFlags().Expand().Border(wxLEFT));
    sizer->Add(round_clamp_sizer, wxSizerFlags().Expand().Border(wxALL));

    return sizer;
}

void VUPanel::ApplyGUIToConfig(AppConfig &config)
{
    Pcsx2Config::CpuOptions &cpu_options(config.EmuOptions.Cpu);

    auto vu0_mode = m_vu0_choices->GetValue();
    cpu_options.Recompiler.EnableVU0 = vu0_mode != VUExecMode::Interpreter;
    cpu_options.Recompiler.UseMicroVU0 = vu0_mode == VUExecMode::MicroVURecompiler;

    auto vu1_mode = m_vu1_choices->GetValue();
    cpu_options.Recompiler.EnableVU1 = vu1_mode != VUExecMode::Interpreter;
    cpu_options.Recompiler.UseMicroVU1 = vu1_mode == VUExecMode::MicroVURecompiler;

    cpu_options.sseVUMXCSR.SetRoundMode(m_round_choices->GetValue());
    // Copied from older code - Is it necessary to set these here?
    cpu_options.sseVUMXCSR.DenormalsAreZero = 1;
    cpu_options.sseVUMXCSR.FlushToZero = 1;

    auto clamp_mode = m_clamp_choices->GetValue();
    cpu_options.Recompiler.vuOverflow = clamp_mode >= VUClampMode::Normal;
    cpu_options.Recompiler.vuExtraOverflow = clamp_mode >= VUClampMode::Extra;
    cpu_options.Recompiler.vuSignOverflow = clamp_mode >= VUClampMode::ExtraAndPreserveSign;
}

void VUPanel::ApplyConfigToGUI(AppConfig &config)
{
    Pcsx2Config::CpuOptions &cpu_options(config.EmuOptions.Cpu);

    if (!cpu_options.Recompiler.EnableVU0)
        m_vu0_choices->SetValue(VUExecMode::Interpreter);
    else if (!cpu_options.Recompiler.UseMicroVU0)
        m_vu0_choices->SetValue(VUExecMode::SuperVURecompiler);
    else
        m_vu0_choices->SetValue(VUExecMode::MicroVURecompiler);

    if (!cpu_options.Recompiler.EnableVU1)
        m_vu1_choices->SetValue(VUExecMode::Interpreter);
    else if (!cpu_options.Recompiler.UseMicroVU1)
        m_vu1_choices->SetValue(VUExecMode::SuperVURecompiler);
    else
        m_vu1_choices->SetValue(VUExecMode::MicroVURecompiler);

    m_round_choices->SetValue(cpu_options.sseVUMXCSR.GetRoundMode());

    if (cpu_options.Recompiler.vuSignOverflow)
        m_clamp_choices->SetValue(VUClampMode::ExtraAndPreserveSign);
    else if (cpu_options.Recompiler.vuExtraOverflow)
        m_clamp_choices->SetValue(VUClampMode::Extra);
    else if (cpu_options.Recompiler.vuOverflow)
        m_clamp_choices->SetValue(VUClampMode::Normal);
    else
        m_clamp_choices->SetValue(VUClampMode::None);

    this->Enable(!config.EnablePresets);
}

void VUPanel::SettingEventHandler(pxSettingEvent &evt)
{
    using Action = pxSettingEvent::Action;
    Action action = evt.GetAction();
    AppConfig &config = evt.GetConfig();
    if (action == Action::ApplyGUIToConfig) {
        ApplyGUIToConfig(config);
    } else {
        ApplyConfigToGUI(config);
    }
}
}
