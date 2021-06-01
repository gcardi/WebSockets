
//---------------------------------------------------------------------------

#pragma hdrstop

#include <IdSSL.hpp>

//#include "Globals.h"
#include "WSEnabledWebBrokerBridge.h"

#pragma link "IdHTTPWebBrokerBridge"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
namespace SvcApp {
//---------------------------------------------------------------------------

using WebSockets::CloseStatus;
using WebSockets::Opcode;

void __fastcall TIdHTTPWebSocketEnabledWebBrokerBridge::DoCommandGet(
                      Idcontext::TIdContext* AThread,
                      Idcustomhttpserver::TIdHTTPRequestInfo* ARequestInfo,
                      Idcustomhttpserver::TIdHTTPResponseInfo* AResponseInfo )
{
    if ( OnCommandGet ) {
        bool Handled = false;
        OnCommandGet( this, AThread, ARequestInfo, AResponseInfo, Handled );
        if ( Handled ) {
            return;
        }
    }

    if ( ARequestInfo->Document != _T( "/websocket" ) ) {  // <---
        inherited::DoCommandGet( AThread, ARequestInfo, AResponseInfo );
    }
    else {
        DoWebSocketCommand( AThread, ARequestInfo, AResponseInfo );
    }
}
//---------------------------------------------------------------------------

void TIdHTTPWebSocketEnabledWebBrokerBridge::DoWebSocketCommand(
                      Idcontext::TIdContext* AThread,
                      Idcustomhttpserver::TIdHTTPRequestInfo* ARequestInfo,
                      Idcustomhttpserver::TIdHTTPResponseInfo* AResponseInfo )
{
    WebSockets::Server::WebSocket WS(
        AThread, ARequestInfo, AResponseInfo
    );

    // Closed state (not cleanly)
    if ( WS.IsWebSocket() ) {
        // Open state

        auto& IOHandler = *AThread->Connection->IOHandler;

        TBytes Buffer;
        size_t PayloadLen {};
        size_t PayloadPos {};
        bool Masked {};
        bool Fin {};

        CloseStatus CloseReason = CloseStatus::Normal;
        String CloseText;

        if ( OnWebSocketFrameReceived ) {
            for ( ;; ) {
                Buffer.Length = 0;
                Opcode Type {};

                if ( WS.ReadFrame( Buffer, PayloadLen, PayloadPos, /*Type, Fin,*/
                                   CloseReason, CloseText, 10000 ) )
                {
                    if ( !OnWebSocketFrameReceived(
                             this, WS, AThread, ARequestInfo, AResponseInfo,
                             IOHandler, Masked,
                             Buffer.CopyRange( PayloadPos, PayloadLen ),
                             CloseReason, CloseText ) )
                    {
                        WS.SendCloseFrame( CloseReason, CloseText );
                        break;
                    }
                }
                else {
                    WS.SendCloseFrame( CloseReason, CloseText );
                    break;
                }
            }
        }
        else {
            Opcode Type {};
            TBytes Data;

            CloseStatus CloseReason;
            String CloseText;

            while ( WS.ReadMessage( Type, Data, CloseReason, CloseText, 10000 ) ) {
                if ( OnWebSocketMessageReceived ) {
                    if ( !OnWebSocketMessageReceived(
                        this, WS, AThread, ARequestInfo, AResponseInfo,
                        IOHandler, Type, Data ) )
                    {
                        break;
                    }
                }
            }
        }
        AThread->Connection->Disconnect();
    }
}

//---------------------------------------------------------------------------
} // End of namespace SvcApp
//---------------------------------------------------------------------------

