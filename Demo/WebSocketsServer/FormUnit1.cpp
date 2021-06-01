
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <IdSync.hpp>
#include <Datasnap.DSSession.hpp>

#include <memory>
#include <algorithm>

#include "FormUnit1.h"

using std::make_unique;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

class TLogNotify : public TIdNotify
{
protected:
    TForm1* FForm;
    String FMsg;

    void __fastcall DoNotify()
    {
        FForm->RichEdit1->SetFocus();
        FForm->RichEdit1->SelStart = FForm->RichEdit1->GetTextLen();
        //FForm->RichEdit1->SelAttributes->Color = Color;
        FForm->RichEdit1->SelText = FMsg;
        FForm->RichEdit1->Perform( EM_SCROLLCARET, NativeUInt{}, NativeInt{} );
    }

public:
    __fastcall TLogNotify(TForm1* Form, const String &S)
        : TIdNotify(), FForm( Form ), FMsg(S)
    {
    }
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    FServer = new ServerType( this );
    FServer->OnConnect = &ConnectEvtHndlr;
    FServer->OnDisconnect = &DisconnectEvtHndlr;
    FServer->OnCommandGet = &IdHTTPServer1CommandGet;
    FServer->OnWebSocketMessageReceived = &WSMessageRcv;
}
//---------------------------------------------------------------------------

void TForm1::Log( String S )
{
    ( new TLogNotify( this, S ) )->Notify();
}
//---------------------------------------------------------------------------

void TForm1::Log( String Msg, System::TVarRec *Args, int Args_High )
{
    ( new TLogNotify( this, Format( Msg, Args, Args_High ) ) )->Notify();
}
//---------------------------------------------------------------------------

void TForm1::LogPeer( Idcontext::TIdContext& AContext )
{
    Log(
        Format(
            _D( "host=%s, port=%d" ),
            ARRAYOFCONST( (
                AContext.Connection->Socket->Binding->PeerIP,
                AContext.Connection->Socket->Binding->PeerPort
            ) )
        )
    );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ConnectEvtHndlr( Idcontext::TIdContext* AContext )
{
    Log( _D( "Client connected: " ) );
    LogPeer( *AContext );
    Log( _D( "\n" ) );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DisconnectEvtHndlr( Idcontext::TIdContext* AContext )
{
    Log( _D( "Client disconnected: " ) );
    LogPeer( *AContext );
    Log( _D( "\n" ) );
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

void __fastcall TForm1::IdHTTPServer1CommandGet(
                                  TObject *Sender, TIdContext *AContext,
                                  TIdHTTPRequestInfo *ARequestInfo,
                                  TIdHTTPResponseInfo *AResponseInfo,
                                  bool& Handled )
{
    Log(
        Format(
            _D( "ARequestInfo->RawHTTPCommand=%s\n" ),
            ARRAYOFCONST( (
                ARequestInfo->RawHTTPCommand
            ) )
        )
    );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonStartClick(TObject *Sender)
{
    StartServer();
}
//---------------------------------------------------------------------------

void TerminateThreads(void)
{
    if ( TDSSessionManager::Instance != nullptr ) {
        TDSSessionManager::Instance->TerminateAllSessions();
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonStopClick(TObject *Sender)
{
    TerminateThreads();
    FServer->Active = false;
    FServer->Bindings->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonOpenBrowserClick(TObject *Sender)
{
    StartServer();
    String url =
        Format(
            _D( "http://localhost:%s" ),
            ARRAYOFCONST( ( EditPort->Text ) )
        );
    ShellExecute( 0, nullptr, url.c_str(), nullptr, nullptr, SW_SHOWNOACTIVATE );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ApplicationEvents1Idle(TObject *Sender, bool &Done)
{
    ButtonStart->Enabled = !FServer->Active;
    ButtonStop->Enabled = FServer->Active;
    EditPort->Enabled = !FServer->Active;
}
//---------------------------------------------------------------------------

void TForm1::StartServer()
{
    if ( !FServer->Active ) {
        FServer->Bindings->Clear();
        FServer->DefaultPort = StrToInt(EditPort->Text);
        FServer->Active = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cboxLogFlowClick(TObject *Sender)
{
    logFlow_ = cboxLogFlow->Checked;
}
//---------------------------------------------------------------------------

bool __fastcall TForm1::WSMessageRcv( System::TObject* Sender,
                                      SvcApp::WebSockets::Server::WebSocket& WS,
                                      Idcontext::TIdContext* AContext,
                                      TIdHTTPRequestInfo* ARequestInfo,
                                      TIdHTTPResponseInfo* AResponseInfo,
                                      TIdIOHandler &IOHandler,
                                      SvcApp::WebSockets::Opcode Type,
                                      TBytes Data )
{
    auto Txt = TEncoding::UTF8->GetString( Data );
    Log( _D( "Received message \'%s\'\r\n" ), ARRAYOFCONST(( Txt )) );

    //SvcApp::WebSockets::Server::WebSocket WS{ AContext, ARequestInfo, AResponseInfo };

    WS.SendFrame( Txt );

    return true;
}
//---------------------------------------------------------------------------

