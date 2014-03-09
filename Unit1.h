//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
#include "PtblRV.hpp"
#include "RichView.hpp"
#include "RVEdit.hpp"
#include "RVReport.hpp"
#include "RVScroll.hpp"
#include "RVStyle.hpp"
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.StdCtrls.hpp>
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TRVReportHelper *RVReportHelper1;
	TRVStyle *RVStyle1;
	TRichViewEdit *RichViewEdit1;
	void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
