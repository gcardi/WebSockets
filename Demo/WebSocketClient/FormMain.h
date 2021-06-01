//---------------------------------------------------------------------------

#ifndef FormMainH
#define FormMainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ActnMan.hpp>
#include <Vcl.PlatformDefaultStyleActnCtrls.hpp>
#include "FrameReLog.h"

#include <anafestica/PersistFormVCL.h>
#include <anafestica/CfgRegistrySingleton.h>

//---------------------------------------------------------------------------

using TConfigRegistryForm =
    Anafestica::TPersistFormVCL<Anafestica::TConfigRegistrySingleton>;

class TfrmMain : public TConfigRegistryForm
{
__published:    // IDE-managed Components
    TEdit *edtURL;
    TButton *Button1;
    TActionManager *ActionManager1;
    TAction *actWebSocketsSendSFText;
    TPageControl *PageControl1;
    TTabSheet *tbshtLog;
    TTabSheet *tbshtResponse;
    TRichEdit *reResponse;
    TTabSheet *tbshtBody;
    TRichEdit *reBody;
    TfrmeReLog *frmeReLog1;
    TTabSheet *tbshtRequest;
    TRichEdit *reRequest;
    TEdit *edtSendText;
    TAction *actWebSocketsSendSFBinary;
    TButton *Button2;
    TStatusBar *StatusBar1;
    TEdit *edtSendBinary;
    TAction *actWebSocketsSendMFText;
    TButton *Button3;
    TEdit *edtMFLength;
    TEdit *edtMFChunkSize;
    TLabel *Label1;
    TLabel *Label2;
    TButton *Button4;
    TLabel *Label3;
    void __fastcall actWebSocketsSendSFTextExecute(TObject *Sender);
    void __fastcall actWebSocketsSendSFBinaryExecute(TObject *Sender);
    void __fastcall actWebSocketsSendSFTextUpdate(TObject *Sender);
    void __fastcall actWebSocketsSendSFBinaryUpdate(TObject *Sender);
    void __fastcall actWebSocketsSendMFTextExecute(TObject *Sender);
    void __fastcall actWebSocketsSendMFTextUpdate(TObject *Sender);
private:    // User declarations
    void Init();
    void Destroy();
    static String GetModuleFileName();
    void SetupCaption();
    void RestoreProperties();
    void SaveProperties() const;
    String GetBaseURL() const;
    void SetBaseURL( String Val );
    String GetSendText() const;
    void SetSendText( String Val );
    String GetSendBinary() const;
    void SetSendBinary( String Val );
    String GetMFLength() const;
    void SetMFLength( String Val );
    String GetMFChunkSize() const;
    void SetMFChunkSize( String Val );
    __property String BaseURL = { read = GetBaseURL, write = SetBaseURL };
    __property String SendText = { read = GetSendText, write = SetSendText };
    __property String SendBinary = { read = GetSendBinary, write = SetSendBinary };
    __property String MFLength = { read = GetMFLength, write = SetMFLength };
    __property String MFChunkSize = { read = GetMFChunkSize, write = SetMFChunkSize };
public:     // User declarations
    __fastcall TfrmMain(TComponent* Owner);
    __fastcall TfrmMain( TComponent* Owner, StoreOpts StoreOptions,
                         Anafestica::TConfigNode* const RootNode = 0 );
    __fastcall ~TfrmMain();
    void LogMessage( String Text, TColor Color = clWindowText );
    void LogMessage( String Fmt, TVarRec const * Args, int Args_High,
                     TColor Color = clWindowText );
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
