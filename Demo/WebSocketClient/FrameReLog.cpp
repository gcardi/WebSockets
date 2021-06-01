//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrameReLog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TFrame1 *Frame1;
//---------------------------------------------------------------------------

__fastcall TfrmeReLog::TfrmeReLog(TComponent* Owner)
    : TFrame(Owner)
{
}
//---------------------------------------------------------------------------

void TfrmeReLog::LogMessage( String Text, TColor Color )
{
    RichEdit1->SelStart = RichEdit1->GetTextLen();
    RichEdit1->SelAttributes->Color = Color;
    RichEdit1->SelText = Text;
    RichEdit1->Perform( EM_SCROLLCARET, 0, nullptr );
}
//---------------------------------------------------------------------------
