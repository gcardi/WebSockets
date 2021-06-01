
//---------------------------------------------------------------------------

#ifndef FormUnit1H
#define FormUnit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.AppEvnts.hpp>
#include <Vcl.ComCtrls.hpp>

#include <atomic>

#include "WSEnabledWebBrokerBridge.h"

//---------------------------------------------------------------------------

class TForm1 : public TForm
{
__published:    // IDE-managed Components
    TButton *ButtonOpenBrowser;
    TApplicationEvents *ApplicationEvents1;
    TEdit *EditPort;
    TButton *ButtonStop;
    TButton *ButtonStart;
    TLabel *Label1;
    TRichEdit *RichEdit1;
    TCheckBox *cboxLogFlow;
    void __fastcall ButtonStartClick(TObject *Sender);
    void __fastcall ButtonStopClick(TObject *Sender);
    void __fastcall ButtonOpenBrowserClick(TObject *Sender);
    void __fastcall ApplicationEvents1Idle(TObject *Sender, bool &Done);
    void __fastcall cboxLogFlowClick(TObject *Sender);
private:    // User declarations
    using ServerType = SvcApp::TIdHTTPWebSocketEnabledWebBrokerBridge;

    ServerType *FServer;
    std::atomic<bool> logFlow_ { true };

    void StartServer();
    void __fastcall ConnectEvtHndlr( Idcontext::TIdContext* AContext );
    void __fastcall DisconnectEvtHndlr( Idcontext::TIdContext* AContext );
    void LogPeer( Idcontext::TIdContext& AContext );
    void __fastcall IdHTTPServer1CommandGet(
                                  TObject *Sender, TIdContext *AContext,
                                  TIdHTTPRequestInfo *ARequestInfo,
                                  TIdHTTPResponseInfo *AResponseInfo,
                                  bool& Handled );
    bool __fastcall WSMessageRcv( System::TObject* Sender,
                                  SvcApp::WebSockets::Server::WebSocket& WS,
                                  Idcontext::TIdContext* AContext,
                                  TIdHTTPRequestInfo* ARequestInfo,
                                  TIdHTTPResponseInfo* AResponseInfo,
                                  TIdIOHandler &IOHandler,
                                  SvcApp::WebSockets::Opcode Type,
                                  TBytes Data );
public:     // User declarations
    __fastcall TForm1(TComponent* Owner);
    void Log( String S );
    void Log( String Msg, System::TVarRec *Args, int Args_High );
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif


