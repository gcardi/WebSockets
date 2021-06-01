//---------------------------------------------------------------------------

#ifndef DataModHTTPH
#define DataModHTTPH

#include <System.Classes.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdIntercept.hpp>
#include <IdIOHandler.hpp>
#include <IdIOHandlerSocket.hpp>
#include <IdIOHandlerStack.hpp>
#include <IdLogBase.hpp>
#include <IdLogEvent.hpp>
#include <IdSSL.hpp>
#include <IdSSLOpenSSL.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>

class TfrmMain;

class TdmHTTP : public TDataModule
{
__published:    // IDE-managed Components
    TIdHTTP *IdHTTP1;
    TIdSSLIOHandlerSocketOpenSSL *IdSSLIOHandlerSocketOpenSSL1;
    TIdLogEvent *IdLogEvent1;
    void __fastcall IdHTTP1Connected(TObject *Sender);
    void __fastcall IdLogEvent1Received(TComponent *ASender, const UnicodeString AText,
          const UnicodeString AData);
    void __fastcall IdLogEvent1Sent(TComponent *ASender, const UnicodeString AText,
          const UnicodeString AData);
    void __fastcall IdHTTP1Disconnected(TObject *Sender);
private:    // User declarations
    TfrmMain& log_;
public:     // User declarations
    __fastcall TdmHTTP(TComponent* Owner, TfrmMain& Log );
};

//---------------------------------------------------------------------------
//extern PACKAGE TdmHTTP *dmHTTP;
//---------------------------------------------------------------------------
#endif
