//---------------------------------------------------------------------------

#ifndef WSEnabledWebBrokerBridgeH
#define WSEnabledWebBrokerBridgeH

#include <System.hpp>
#include <IdHTTPWebBrokerBridge.hpp>

#include <memory>

#include "WebSockets.h"

//---------------------------------------------------------------------------
namespace SvcApp {
//---------------------------------------------------------------------------

using TIdHTTPWSCommandGetEvent =
    void __fastcall ( __closure * )(
        System::TObject* Sender, Idcontext::TIdContext* AContext,
        TIdHTTPRequestInfo* ARequestInfo, TIdHTTPResponseInfo* AResponseInfo,
        bool& Handled
    );

using TIdHTTPWSFrameReceivedEvent =
    bool __fastcall ( __closure * )(
        System::TObject* Sender, WebSockets::Server::WebSocket& WS,
        Idcontext::TIdContext* AContext, TIdHTTPRequestInfo* ARequestInfo,
        TIdHTTPResponseInfo* AResponseInfo, TIdIOHandler &IOHandler,
        bool Masked, TBytes PayloadData, WebSockets::CloseStatus& CloseReason,
        String& CloseText
    );

using TIdHTTPWSMessageReceivedEvent =
    bool __fastcall ( __closure * )(
        System::TObject* Sender, WebSockets::Server::WebSocket& WS,
        Idcontext::TIdContext* AContext, TIdHTTPRequestInfo* ARequestInfo,
        TIdHTTPResponseInfo* AResponseInfo, TIdIOHandler &IOHandler,
        WebSockets::Opcode Type, TBytes Data
    );

class TIdHTTPWebSocketEnabledWebBrokerBridge : public TIdHTTPWebBrokerBridge
{
    using inherited = TIdHTTPWebBrokerBridge;

public:
    __fastcall TIdHTTPWebSocketEnabledWebBrokerBridge(System::Classes::TComponent* AOwner) override : inherited( AOwner ) { }
    __fastcall TIdHTTPWebSocketEnabledWebBrokerBridge(void) : inherited() { }

    void ConfigureLoopbackHandshake( int Port );

    virtual void __fastcall DoCommandGet(
        Idcontext::TIdContext* AThread,
        Idcustomhttpserver::TIdHTTPRequestInfo* ARequestInfo,
        Idcustomhttpserver::TIdHTTPResponseInfo* AResponseInfo
    ) override;

    __property TIdHTTPWSCommandGetEvent OnCommandGet = {
        read = onCommandGet_, write = onCommandGet_
    };

    __property TIdHTTPWSFrameReceivedEvent OnWebSocketFrameReceived = {
        read = onWebSocketFrameReceived_, write = onWebSocketFrameReceived_
    };

    __property TIdHTTPWSMessageReceivedEvent OnWebSocketMessageReceived = {
        read = onWebSocketMessageReceived_, write = onWebSocketMessageReceived_
    };

protected:
    virtual void DoWebSocketCommand( Idcontext::TIdContext* AThread,
                                     Idcustomhttpserver::TIdHTTPRequestInfo* ARequestInfo,
                                     Idcustomhttpserver::TIdHTTPResponseInfo* AResponseInfo );
private:
    TIdHTTPWSCommandGetEvent onCommandGet_ { nullptr };
    TIdHTTPWSFrameReceivedEvent onWebSocketFrameReceived_ { nullptr };
    TIdHTTPWSMessageReceivedEvent onWebSocketMessageReceived_ { nullptr };
    std::unique_ptr<TStringList> allowedOrigins_ { std::make_unique<TStringList>() };
    WebSockets::Server::HandshakeOptions handshakeOptions_ {
        allowedOrigins_.get(), nullptr, false, true
    };
};


//---------------------------------------------------------------------------
} // End of namespace SvcApp {
//---------------------------------------------------------------------------
#endif
