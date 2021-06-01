//---------------------------------------------------------------------------

#ifndef WebSocketsH
#define WebSocketsH

#include <System.SysUtils.hpp>

#include <IdIOHandler.hpp>
#include <IdContext.hpp>
#include <Idcustomhttpserver.hpp>
#include <IdHTTP.hpp>

#include <cstdint>

//---------------------------------------------------------------------------
namespace SvcApp {
//---------------------------------------------------------------------------
namespace WebSockets {
//---------------------------------------------------------------------------

enum class Opcode {
    Continuation = 0,
    Text = 1,
    Binary = 2,
    Close = 8,
    Ping = 9,
    Pong = 10,
};

enum class CloseStatus {
    // indicates a normal closure, meaning that the purpose for
    // which the connection was established has been fulfilled.
    Normal = 1000,

    // indicates that an endpoint is "going away", such as a server
    // going down or a browser having navigated away from a page.
    GoingAway = 1001,

    // indicates that an endpoint is terminating the connection due
    // to a protocol error.
    ProtocolError = 1002,

    // indicates that an endpoint is terminating the connection
    // because it has received a type of data it cannot accept (e.g., an
    // endpoint that understands only text data MAY send this if it
    // receives a binary message).
    UnhandledDataType = 1003,

    // reserved, the specific meaning might be defined in the future.
    Reserved = 1004,

    // reserved, MUST NOT be set as a status code in a Close control frame
    // by an endpoint. It is designated for use in applications expecting
    // a status code to indicate that no status code was actually present.
    ReservedNoStatus = 1005,

    // is a reserved value and MUST NOT be set as a status code in a Close
    // control frame by an endpoint. It is designated for use in applications
    // expecting a status code to indicate that the connection was closed
    // abnormally, e.g., without sending or receiving a Close control frame.
    ReservedAbnormal = 1006,

    // indicates that an endpoint is terminating the connection because it has
    // received data within a message that was not consistent with the type
    // of the message (e.g., non-UTF-8 [RFC3629] data within a text message).
    InconsistentData = 1007,

    // indicates that an endpoint is terminating the connection because
    // it has received a message that violates its policy. This is a generic
    // status code that can be returned when there is no other more suitable
    // status code (e.g., 1003 or 1009) or if there is a need to hide specific
    // details about the policy.
    PolicyError = 1008,

    // indicates that an endpoint is terminating the connection because
    // it has received a message that is too big for it to process.
    ToBigMessage = 1009,

    // indicates that an endpoint (client) is terminating the connection
    // because it has expected the server to negotiate one or more extension,
    // but the server didn't return them in the response message of the
    // WebSocket handshake. The list of extensions that are needed SHOULD
    // appear in the /reason/ part of the Close frame. Note that this status
    // code is not used by the server, because it can fail the WebSocket
    // handshake instead.
    MissingExtenstion = 1010,

    // indicates that a server is terminating the connection because it
    // encountered an unexpected condition that prevented it from fulfilling
    //the request.
    UnExpectedError = 1011,

    // is a reserved value and MUST NOT be set as a status code in a Close
    // control frame by an endpoint.  It is designated for use in applications
    // expecting a status code to indicate that the connection was closed
    // due to a failure to perform a TLS handshake (e.g., the server
    // certificate can't be verified).
    ReservedTLSError = 1015,
};

extern bool ToCloseStatus( uint16_t Code, CloseStatus& Status );

extern String ToString( Opcode Type );

#pragma pack( push, 1 )
using MaskData =
    union {
        uint32_t d_;
        uint8_t a_[4];
    };
#pragma pack( pop )

class WebSocket {
public:
    WebSocket() = default;
    WebSocket( WebSocket const & ) = delete;
    WebSocket& operator=( WebSocket const & ) = delete;
    virtual ~WebSocket() = default;

    TIdIOHandler& GetIOHandler() const { return DoGetIOHandler(); }

    bool ReadFrame( TBytes& InBuffer, size_t& PayloadLen, size_t& PayloadPos,
                    CloseStatus& CloseReason, String& CloseText, int Timeout )
    {
        GetIOHandler().ReadTimeout = Timeout;
        return DoReadFrame(
            InBuffer, PayloadLen, PayloadPos, CloseReason, CloseText
        );
    }

    bool ReadTextFrame( int Timeout, String& Text, String& CloseText,
                        CloseStatus& CloseReason );
    String ReadTextFrame( int Timeout );

    bool ReadMessage( Opcode& Type, TBytes& Data, CloseStatus& CloseReason,
                      String& CloseText, int Timeout )
    {
        GetIOHandler().ReadTimeout = Timeout;
        return DoReadMessage( Type, Data, CloseReason, CloseText );
    }

    void SendEmptyFrame( Opcode Type ) { DoSendEmptyFrame( Type ); }

    void SendFrame( Opcode Type, TBytes const & Data, bool Fin )
    {
        DoSendFrame( Type, Data, Fin );
    }

    void SendCloseFrame() { SendEmptyFrame( Opcode::Close ); }

    void SendCloseFrame( CloseStatus CloseReason, TBytes const & CloseData );

    void SendCloseFrame( CloseStatus CloseReason, String CloseText )
    {
        SendCloseFrame( CloseReason, TEncoding::UTF8->GetBytes( CloseText ) );
    }

    void SendPongFrame() { SendEmptyFrame( Opcode::Pong ); }

    void SendPongFrame( TBytes const & Data )
    {
        SendFrame( Opcode::Pong, Data, true );
    }

    void SendPongFrame( String Text )
    {
        SendPongFrame( TEncoding::UTF8->GetBytes( Text ) );
    }

    void SendPingFrame() { SendEmptyFrame( Opcode::Ping ); }

    void SendPingFrame( TBytes const & Data )
    {
        SendFrame( Opcode::Ping, Data, true );
    }

    void SendPingFrame( String Text )
    {
        SendPingFrame( TEncoding::UTF8->GetBytes( Text ) );
    }

    void SendFrame( String Text, bool Fin = true )
    {
        SendFrame( Opcode::Text, TEncoding::UTF8->GetBytes( Text ), Fin );
    }

    void SendFrame( TBytes const & Data, bool Fin = true )
    {
        SendFrame( Opcode::Binary, Data, Fin );
    }

    void SendMessage( Opcode Type, TBytes const & Data, CloseStatus& CloseReason,
                      String& CloseText, size_t MaxChunkSize, int Timeout )
    {
        GetIOHandler().ReadTimeout = Timeout;
        DoSendMessage( Type, Data, CloseReason, CloseText, MaxChunkSize );
    }

    void SendMessage( String Text, CloseStatus& CloseReason, String& CloseText,
                      size_t MaxChunkSize, int Timeout )
    {
         SendMessage(
            Opcode::Text, TEncoding::UTF8->GetBytes( Text ), CloseReason,
            CloseText, MaxChunkSize, Timeout
        );
    }

    void SendMessage( TBytes const & Data, CloseStatus& CloseReason, String& CloseText,
                      size_t MaxChunkSize, int Timeout )
    {
        SendMessage(
            Opcode::Binary, Data, CloseReason, CloseText, MaxChunkSize, Timeout
        );
    }

    bool IsWebSocket() const { return DoIsWebSocket(); }
protected:
    virtual TIdIOHandler& DoGetIOHandler() const = 0;

    virtual bool DoReadFrame( TBytes& InBuffer, size_t& PayloadLen,
                              size_t& PayloadPos, CloseStatus& Reason,
                              String& CloseText ) = 0;
    virtual void DoSendEmptyFrame( Opcode Type ) = 0;
    virtual void DoSendFrame( Opcode Type, TBytes const & Data, bool Fin ) = 0;
    virtual bool DoReadMessage( Opcode& Type, TBytes& Data, CloseStatus& Reason,
                                String& CloseText );
    virtual bool DoReadFragmentedData( TBytes& AData, CloseStatus& CloseReason,
                                       String& CloseText );
    virtual void DoTryToProcessControlFrame( TBytes& Buffer );
    virtual void DoSendMessage( Opcode& Type, TBytes const & Data,
                                CloseStatus& CloseReason, String& CloseText,
                                size_t MaxChunkSize );
    virtual bool DoIsWebSocket() const { return true; }
    static bool ReadFrameHeader( TIdIOHandler& IOHandler, TBytes& InBuffer,
                                 size_t& PayloadLen, size_t& PayloadPos,
                                 CloseStatus& CloseReason, String& CloseText );
    static TBytes BuildFrameHeader( Opcode Type, int DataLen, bool Fin, bool Masked );
    static void SendFrame( TIdIOHandler& IOHandler, Opcode Type,
                           TBytes const & Data, bool Fin, bool Masked );
    static Opcode GetOpcode( TBytes const & Data ) {
        return static_cast<Opcode>( Data[0] & 15 );
    }
    static bool GetFin( TBytes const & Data ) { return Data[0] & 128; }
    static bool GetMasked( TBytes const & Data ) { return Data[1] & 128; }
    static System::Byte GetLen( TBytes const & Data ) { return Data[1] & 127; }
private:
};

//---------------------------------------------------------------------------
namespace Client {
//---------------------------------------------------------------------------

class WebSocket final : public WebSockets::WebSocket {
public:
    WebSocket( TIdHTTP& IdHTTP, String URL,
               Idheaderlist::TIdHeaderList* AdditionalCustomHeader = nullptr );
    WebSocket( WebSocket const & ) = delete;
    WebSocket& operator=( WebSocket const & ) = delete;
protected:
    virtual TIdIOHandler& DoGetIOHandler() const override;
    virtual bool DoReadFrame( TBytes& InBuffer, size_t& PayloadLen,
                              size_t& PayloadPos, CloseStatus& Reason,
                              String& CloseText ) override;
    virtual void DoSendEmptyFrame( Opcode Type ) override;
    virtual void DoSendFrame( Opcode Type, TBytes const & Data,
                              bool Fin ) override;
private:
    TIdHTTP& idHTTP_;

    void TryToUpgrade( String URL, Idheaderlist::TIdHeaderList* AdditionalCustomHeader );
    uint32_t GenerateMask() const;
    static String GenerateSecretKey();
};

//---------------------------------------------------------------------------
} // End of namespace Client
//---------------------------------------------------------------------------
namespace Server {
//---------------------------------------------------------------------------

class WebSocket final : public WebSockets::WebSocket {
public:
    WebSocket( Idcontext::TIdContext* AThread,
               Idcustomhttpserver::TIdHTTPRequestInfo* ARequestInfo,
               Idcustomhttpserver::TIdHTTPResponseInfo* AResponseInfo )
      : thread_( *AThread )
      , requestInfo_( *ARequestInfo )
      , responseInfo_( *AResponseInfo )
      , isWebSocket_( TryToUpgrade() )
    {}
    WebSocket( WebSocket const & ) = delete;
    WebSocket& operator=( WebSocket const & ) = delete;
    Idcustomhttpserver::TIdHTTPRequestInfo& GetRequestInfo() const { return requestInfo_; }
    Idcustomhttpserver::TIdHTTPResponseInfo& GetResponseInfo() const { return responseInfo_; }
protected:
    virtual TIdIOHandler& DoGetIOHandler() const override;
    virtual bool DoReadFrame( TBytes& InBuffer, size_t& PayloadLen,
                              size_t& PayloadPos, CloseStatus& Reason,
                              String& CloseText ) override;
    virtual void DoSendEmptyFrame( Opcode Type ) override;
    virtual void DoSendFrame( Opcode Type, TBytes const & Data,
                              bool Fin ) override;
    bool DoIsWebSocket() const override { return isWebSocket_; }
private:
    Idcontext::TIdContext& thread_;
    Idcustomhttpserver::TIdHTTPRequestInfo& requestInfo_;
    Idcustomhttpserver::TIdHTTPResponseInfo& responseInfo_;
    bool isWebSocket_;

    bool TryToUpgrade();
};

//---------------------------------------------------------------------------
} // End of namespace Server
//---------------------------------------------------------------------------
} // End of namespace WebSockets
//---------------------------------------------------------------------------
} // End of namespace SvcApp
//---------------------------------------------------------------------------
#endif
