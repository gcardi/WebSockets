//---------------------------------------------------------------------------

#ifndef FrameReLogH
#define FrameReLogH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmeReLog : public TFrame
{
__published:    // IDE-managed Components
    TRichEdit *RichEdit1;
private:    // User declarations
public:     // User declarations
    __fastcall TfrmeReLog(TComponent* Owner);
    void LogMessage( String Text, TColor Color );

};
//---------------------------------------------------------------------------
#endif
