//---------------------------------------------------------------------------

// This software is Copyright (c) 2015 Embarcadero Technologies, Inc.
// You may only use this software if you are an authorized licensee
// of an Embarcadero developer tools product.
// This software is considered a Redistributable as defined under
// the software license agreement that comes with the Embarcadero Products
// and is subject to that software license agreement.

//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "uMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
#pragma resource ("*.LgXhdpiPh.fmx", _PLAT_ANDROID)

TCameraRollForm *CameraRollForm;
//---------------------------------------------------------------------------
__fastcall TCameraRollForm::TCameraRollForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TCameraRollForm::TakePhotoFromLibraryAction1DidFinishTaking(TBitmap *Image)
{
	// Assign the image retrieved from the Photo Library to the TImage component.
	imgPhotoLibraryImage->Bitmap->Assign(Image);
}
//---------------------------------------------------------------------------

