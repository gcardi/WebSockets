//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <IdHashSHA.hpp>
#include <IdCoderMIME.hpp>

#include <memory>

#include <anafestica/FileVersionInfo.h>

#include "RegexDefs.h"
#include "WebSockets.h"
#include "FormMain.h"
#include "DataModHTTP.h"

using std::make_unique;

using Anafestica::TFileVersionInfo;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FrameReLog"
#pragma resource "*.dfm"

TfrmMain *frmMain;

//---------------------------------------------------------------------------


__fastcall TfrmMain::TfrmMain(TComponent* Owner)
    : TfrmMain( Owner, StoreOpts::All, nullptr )
{
}
//---------------------------------------------------------------------------

__fastcall TfrmMain::TfrmMain( TComponent* Owner, StoreOpts StoreOptions,
                               Anafestica::TConfigNode* const RootNode )
    : TConfigRegistryForm( Owner, StoreOptions, RootNode )
{
    Init();
}
//---------------------------------------------------------------------------

__fastcall TfrmMain::~TfrmMain()
{
    try {
        Destroy();
    }
    catch ( ... ) {
    }
}
//---------------------------------------------------------------------------

void TfrmMain::Init()
{
    SetupCaption();
    RestoreProperties();
}
//---------------------------------------------------------------------------

void TfrmMain::Destroy()
{
    SaveProperties();
}
//---------------------------------------------------------------------------

String TfrmMain::GetModuleFileName()
{
    return GetModuleName( reinterpret_cast<unsigned>( HInstance ) );
}
//---------------------------------------------------------------------------

void TfrmMain::SetupCaption()
{
    TFileVersionInfo const Info( GetModuleFileName() );
    Caption =
        Format(
            _D( "%s, Ver %s" ),
            ARRAYOFCONST( (
                Application->Title,
                Info.ProductVersion
            ) )
        );
}
//---------------------------------------------------------------------------

void TfrmMain::RestoreProperties()
{
    RESTORE_LOCAL_PROPERTY( BaseURL );
    RESTORE_LOCAL_PROPERTY( SendText );
    RESTORE_LOCAL_PROPERTY( SendBinary );
    RESTORE_LOCAL_PROPERTY( MFLength );
    RESTORE_LOCAL_PROPERTY( MFChunkSize );
}
//---------------------------------------------------------------------------

void TfrmMain::SaveProperties() const
{
    SAVE_LOCAL_PROPERTY( BaseURL );
    SAVE_LOCAL_PROPERTY( SendText );
    SAVE_LOCAL_PROPERTY( SendBinary );
    SAVE_LOCAL_PROPERTY( MFLength );
    SAVE_LOCAL_PROPERTY( MFChunkSize );
}
//---------------------------------------------------------------------------

String TfrmMain::GetBaseURL() const
{
    return edtURL->Text;
}
//---------------------------------------------------------------------------

void TfrmMain::SetBaseURL( String Val )
{
    edtURL->Text = Val;
}
//---------------------------------------------------------------------------

String TfrmMain::GetSendText() const
{
    return edtSendText->Text;
}
//---------------------------------------------------------------------------

void TfrmMain::SetSendText( String Val )
{
    edtSendText->Text = Val;
}
//---------------------------------------------------------------------------

String TfrmMain::GetSendBinary() const
{
    return edtSendBinary->Text;
}
//---------------------------------------------------------------------------

void TfrmMain::SetSendBinary( String Val )
{
    edtSendBinary->Text = Val;
}
//---------------------------------------------------------------------------

String TfrmMain::GetMFLength() const
{
    return edtMFLength->Text;
}
//---------------------------------------------------------------------------

void TfrmMain::SetMFLength( String Val )
{
    edtMFLength->Text = Val;
}
//---------------------------------------------------------------------------

String TfrmMain::GetMFChunkSize() const
{
    return edtMFChunkSize->Text;
}
//---------------------------------------------------------------------------

void TfrmMain::SetMFChunkSize( String Val )
{
    edtMFChunkSize->Text = Val;
}
//---------------------------------------------------------------------------

void TfrmMain::LogMessage( String Text, TColor Color )
{
    frmeReLog1->LogMessage( Text, Color );
}
//---------------------------------------------------------------------------

void TfrmMain::LogMessage( String Fmt, TVarRec const * Args, int Args_High,
                           TColor Color )
{
    LogMessage( Format( Fmt, Args, Args_High ), Color );
}
//---------------------------------------------------------------------------

String ToHex2( TBytes const & Buffer )
{
    auto SB = make_unique<TStringBuilder>();
    for ( auto& b : const_cast<TBytes&>( Buffer ) ) {
        SB->AppendFormat( _D( "%.2X " ), ARRAYOFCONST( ( b ) ) );
    }
    return SB->ToString();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actWebSocketsSendSFTextExecute(TObject *Sender)
{
    try {
        auto HTTP = make_unique<TdmHTTP>( nullptr, *this );
        SvcApp::WebSockets::Client::WebSocket WS{ *HTTP->IdHTTP1, BaseURL };

        LogMessage(
            _D( "Send: %s\n" ),
            ARRAYOFCONST( ( SendText ) ),
            clMaroon
        );

        WS.SendFrame( SendText );

        LogMessage(
            _D( "Rcv: %s\n" ),
            ARRAYOFCONST( ( WS.ReadTextFrame( 10000 ) ) ),
            clGreen
        );

        WS.SendCloseFrame( SvcApp::WebSockets::CloseStatus::Normal, _D( "Ciao!" ) );
    }
    catch ( Exception const & E ) {
        LogMessage( _D( "Failed: %s\n" ), ARRAYOFCONST( ( E.Message ) ), clRed );
    }
    catch ( ... ) {
        LogMessage( _D( "Failed: reason unknown\n" ), clRed );
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actWebSocketsSendSFTextUpdate(TObject *Sender)
{
    auto& Act = static_cast<TAction&>( *Sender );
    Act.Enabled = !SendText.IsEmpty();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actWebSocketsSendSFBinaryExecute(TObject *Sender)
{
    try {
        TBytes Buffer;
        Buffer.Length = SendBinary.Length() / 2;
        HexToBin( SendBinary.c_str(), &Buffer[0], Buffer.Length );
        auto HTTP = make_unique<TdmHTTP>( nullptr, *this );
        SvcApp::WebSockets::Client::WebSocket WS{ *HTTP->IdHTTP1, BaseURL };
        WS.SendFrame( Buffer );
        WS.SendCloseFrame( SvcApp::WebSockets::CloseStatus::Normal, _D( "Bai!" ) );
    }
    catch ( Exception const & E ) {
        LogMessage( _D( "Failed: %s\n" ), ARRAYOFCONST( ( E.Message ) ), clRed );
    }
    catch ( ... ) {
        LogMessage( _D( "Failed: reason unknown\n" ), clRed );
    }
}
//---------------------------------------------------------------------------

static bool IsValidHexSequence( String Text )
{
    static regex_type re( _D( "^\\s*(\?:[[:xdigit:]]{2})+\\s*$" ) );
    return boost::regex_match( Text.c_str(), re );
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actWebSocketsSendSFBinaryUpdate(TObject *Sender)
{
    auto& Act = static_cast<TAction&>( *Sender );
    Act.Enabled = IsValidHexSequence( SendBinary );
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actWebSocketsSendMFTextExecute(TObject *Sender)
{
    try {
        auto HTTP = make_unique<TdmHTTP>( nullptr, *this );
        SvcApp::WebSockets::Client::WebSocket WS{ *HTTP->IdHTTP1, BaseURL };
        SvcApp::WebSockets::CloseStatus CloseReason;
        String CloseText;

        auto SB = make_unique<TStringBuilder>();
        size_t const MFLen = MFLength.ToInt();
        size_t const Cnt = MFLen / SendText.Length();
        for ( size_t Idx = 0 ; Idx < Cnt ; ++Idx ) {
            SB->Append( SendText );
        }

        if ( auto const Res = MFLen % SendText.Length() ) {
            SB->Append( SendText.SubString( 1, Res ) );
        }

        WS.SendMessage( SB->ToString(), CloseReason, CloseText, MFChunkSize.ToInt(), 10000 );
        WS.SendCloseFrame( SvcApp::WebSockets::CloseStatus::Normal, _D( "Ciao!" ) );
    }
    catch ( Exception const & E ) {
        LogMessage( _D( "Failed: %s\n" ), ARRAYOFCONST( ( E.Message ) ), clRed );
    }
    catch ( ... ) {
        LogMessage( _D( "Failed: reason unknown\n" ), clRed );
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actWebSocketsSendMFTextUpdate(TObject *Sender)
{
    auto& Act = static_cast<TAction&>( *Sender );
    Act.Enabled = !SendText.IsEmpty() && MFLength.ToIntDef( -1 ) > 0 &&
                  MFChunkSize.ToIntDef( -1 ) > 0;
}
//---------------------------------------------------------------------------

