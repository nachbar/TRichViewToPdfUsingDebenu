#define UNICODE
// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DebenuPDFLibraryDLL1013.h"
#include "DebenuPDFLicenseKey.h"
#include "Unit1.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "PtblRV"
#pragma link "RichView"
#pragma link "RVEdit"
#pragma link "RVReport"
#pragma link "RVScroll"
#pragma link "RVStyle"
#pragma link "PtblRV"
#pragma link "RichView"
#pragma link "RVEdit"
#pragma link "RVReport"
#pragma link "RVScroll"
#pragma link "RVStyle"
#pragma resource "*.dfm"
TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) :
	TForm(Owner)
{
}

const int RTFPRINTINGDOTSPERINCH = 96;
const double RTFPAGEHEIGHTININCHES = 11;
const double RTFPAGEWIDTHININCHES  = 8.5;
const int    DEBENUUNITSIZE        = 72; // 72 DPI is Debenu default

typedef DebenuPDFLibraryDLL1013 DebenuPDFLibrary;

DebenuPDFLibrary* GetGlobalDebenu(
	bool fClearDocuments = false,
	bool fReleaseLibrary = false) // default false, false
{
	static DebenuPDFLibrary* gDebenuLibrary         = NULL;
	static int               gDebenuLibraryUnlocked = 0;
	if (fReleaseLibrary)
	{
		if (gDebenuLibrary)
		{
			delete(gDebenuLibrary);
			gDebenuLibrary = NULL;
		}
		return NULL;
	}
	if (gDebenuLibrary == NULL)
	{
		gDebenuLibrary = new DebenuPDFLibraryDLL1013(L"DebenuPDFLibraryDLL1013.dll");
		bool fLoaded   = gDebenuLibrary->LibraryLoaded();
		if (!fLoaded)
		{
			delete gDebenuLibrary;
			gDebenuLibrary = NULL;
			throw Exception("Unable to load Debenu library");
		}
	}
	if (!gDebenuLibraryUnlocked)
	{
	//  #define DEBENULICENSEKEY L"xxxabc123" // include your real (or trial) license key here or elsewhere
		gDebenuLibrary->UnlockKey(DEBENULICENSEKEY);

		gDebenuLibraryUnlocked = gDebenuLibrary->Unlocked();
		if (!gDebenuLibraryUnlocked)
		{
			delete gDebenuLibrary; // so we will reload next time
			gDebenuLibrary = NULL;
			throw Exception("Unable to unlock Debenu library");
		}
		gDebenuLibrary->SetOrigin(1); // top left -- set when library first loaded and unlocked
	}
	if (fClearDocuments)
	{
		while (gDebenuLibrary->DocumentCount() > 1)
		{
			gDebenuLibrary->RemoveDocument(gDebenuLibrary->GetDocumentID(1));
		}
		// Remove the last document.  It will be replaced with a new, blank document, so that DocumentCount() will still be 1
		gDebenuLibrary->RemoveDocument(gDebenuLibrary->GetDocumentID(1));
		gDebenuLibrary->SetOrigin(1); // top left -- set when documents cleared
	}
	return gDebenuLibrary;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	int outputDocID = 0; // keeps track of Debenu PDF Document that is collecting the pages
	/* to load the ReportHelper's RichView from a stream
	 RVReportHelper1->RichView->ClearAll();
	 RVReportHelper1->RichView->FormatAll();
	 if (format == TSPage::forRTF)
	 {
	 RVReportHelper1->RichView->LoadRTFFromStream(stream);
	 }
	 else
	 {
	 RVReportHelper1->RichView->LoadTextFromStream(
	 stream,
	 0,
	 0,
	 false);
	 } */

	TMemoryStream* memStream = new TMemoryStream;
	RichViewEdit1->SaveRTFToStream(
		memStream,
		false);
	memStream->Position = 0;
	RVReportHelper1->RichView->LoadRTFFromStream(memStream);
	DebenuPDFLibrary* debenu = GetGlobalDebenu(true); // clear documents

	HDC hdc = debenu->GetCanvasDC(
		RTFPRINTINGDOTSPERINCH * RTFPAGEWIDTHININCHES,
		RTFPRINTINGDOTSPERINCH * RTFPAGEHEIGHTININCHES);
	TCanvas* canvas = new TCanvas;
	canvas->Handle  = hdc;

	RVReportHelper1->RichView->TopMargin    = RTFPRINTINGDOTSPERINCH * 1.25;
	RVReportHelper1->RichView->BottomMargin = RTFPRINTINGDOTSPERINCH * 0.75;
	RVReportHelper1->RichView->RightMargin  = RTFPRINTINGDOTSPERINCH * 0.75;
	RVReportHelper1->RichView->LeftMargin   = RTFPRINTINGDOTSPERINCH * 0.75;
	RVReportHelper1->Init(
		canvas, // used just for dimensions
		RTFPRINTINGDOTSPERINCH * RTFPAGEWIDTHININCHES);
	delete memStream;
	delete canvas;
	canvas          = NULL;
	int PageCounter = 1;
	while (RVReportHelper1->FormatNextPage(RTFPRINTINGDOTSPERINCH * RTFPAGEHEIGHTININCHES))
	{
		int LastPageHeight = RVReportHelper1->GetLastPageHeight();
		HDC hdcNew         = debenu->GetCanvasDC(
			RTFPRINTINGDOTSPERINCH * RTFPAGEWIDTHININCHES,
			RTFPRINTINGDOTSPERINCH * RTFPAGEHEIGHTININCHES);
		canvas         = new TCanvas;
		canvas->Handle = hdcNew;
		RVReportHelper1->DrawPage(
			PageCounter,
			canvas,
			true,
			RTFPRINTINGDOTSPERINCH * RTFPAGEHEIGHTININCHES); // LastPageHeight);
		int LastPageHeight2 = RVReportHelper1->GetLastPageHeight();
		PageCounter++;
		int loadResult = debenu->LoadFromCanvasDC(RTFPRINTINGDOTSPERINCH, 0);
		// creates a new document - 50 DPI
		delete canvas;
		canvas = NULL;
		if (loadResult)
		{
			/* to draw some additional info on each page
			 debenu->SetOrigin(1);
			 debenu->SetTextSize(12);
			 int res =
			 debenu->DrawTextBox(DEBENUUNITSIZE * 0.5,
			 DEBENUUNITSIZE * (0.5), DEBENUUNITSIZE * 4, 12 + 4,
			 PatientName, 0); // vertically centered
			 if (int(ServiceStart) > 0)
			 {
			 debenu->SetTextSize(10);
			 debenu->DrawTextBox(
			 DEBENUUNITSIZE * 0.5,
			 (DEBENUUNITSIZE * ( 0.5) + 12 + 4),
			 DEBENUUNITSIZE * 4,
			 12 + 4,
			 ServiceStart.FormatString("dddd, mmmm d, yyyy"),
			 0); // vertically centered
			 }
			 */
			if (outputDocID == 0)
			{
				outputDocID = debenu->SelectedDocument();
			}
			else
			{
				int newLoadedDocument = debenu->SelectedDocument();
				debenu->SelectDocument(outputDocID);
				debenu->MergeDocument(newLoadedDocument); // this deletes the second document
			}
		}
	}
	debenu->SelectDocument(outputDocID);
	debenu->SaveToFile("c:\\testtrichview.pdf");
}
// ---------------------------------------------------------------------------
