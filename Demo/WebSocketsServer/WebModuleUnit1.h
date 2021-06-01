
//---------------------------------------------------------------------------
#ifndef WebModuleUnit1H
#define WebModuleUnit1H
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Web.HTTPApp.hpp>
#include <Data.DBXCommon.hpp>
#include <DataSnap.DSCommonServer.hpp>
#include <DataSnap.DSHTTPCommon.hpp>
#include <DataSnap.DSHTTPWebBroker.hpp>
#include <DataSnap.DSServer.hpp>
#include <Web.HTTPProd.hpp>
#include <DataSnap.DSAuth.hpp>
#include <Datasnap.DSClientMetadata.hpp>
#include <Datasnap.DSHTTP.hpp>
#include <Datasnap.DSMetadata.hpp>
#include <Datasnap.DSProxyJavaScript.hpp>
#include <Datasnap.DSServerMetadata.hpp>
#include <IPPeerServer.hpp>
//---------------------------------------------------------------------------
class TWebModule1 : public TWebModule
{
__published:    // IDE-managed Components
    TDSRESTWebDispatcher *DSRESTWebDispatcher1;
    TDSServer *DSServer1;
    TDSServerClass *DSServerClass1;
    TPageProducer *ServerFunctionInvoker;
    TPageProducer *ReverseString;
    TWebFileDispatcher *WebFileDispatcher1;
    TDSProxyGenerator *DSProxyGenerator1;
    TDSServerMetaDataProvider *DSServerMetaDataProvider1;
    void __fastcall DSServerClass1GetClass(TDSServerClass *DSServerClass, TPersistentClass &PersistentClass);
    void __fastcall ServerFunctionInvokerHTMLTag(TObject *Sender, TTag Tag, const UnicodeString TagString,
          TStrings *TagParams, UnicodeString &ReplaceText);
    void __fastcall WebModuleDefaultAction(TObject *Sender, TWebRequest *Request, TWebResponse *Response,
          bool &Handled);
    void __fastcall WebModuleBeforeDispatch(TObject *Sender, TWebRequest *Request, TWebResponse *Response,
          bool &Handled);
    void __fastcall WebFileDispatcher1BeforeDispatch(TObject *Sender, const UnicodeString AFileName,
          TWebRequest *Request, TWebResponse *Response, bool &Handled);
    void __fastcall WebModuleCreate(TObject *Sender);
private:    // User declarations
    TWebActionItem* FServerFunctionInvokerAction;
    bool __fastcall AllowServerFunctionInvoker(void);
public:     // User declarations
    __fastcall TWebModule1(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

