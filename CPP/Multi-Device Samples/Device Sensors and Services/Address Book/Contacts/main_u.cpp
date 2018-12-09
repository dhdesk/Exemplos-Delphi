//---------------------------------------------------------------------------
// Copyright (c) 2016 Embarcadero Technologies, Inc. All rights reserved.  
//
// This software is the copyrighted property of Embarcadero Technologies, Inc. 
// ("Embarcadero") and its licensors. You may only use this software if you 
// are an authorized licensee of Delphi, C++Builder or RAD Studio 
// (the "Embarcadero Products").  This software is subject to Embarcadero's 
// standard software license and support agreement that accompanied your 
// purchase of the Embarcadero Products and is considered a Redistributable, 
// as such term is defined thereunder. Your use of this software constitutes 
// your acknowledgement of your agreement to the foregoing software license 
// and support agreement. 
//---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "main_u.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
#pragma resource ("*.iPhone47in.fmx", _PLAT_IOS)

TForm1 *Form1;

// ---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {

}

// ---------------------------------------------------------------------------
// Add a new contact to List box
void __fastcall TForm1::AddListViewItem(TAddressBookContact *Contact) {

	TListViewItem *ListViewItem = ListViewContacts->Items->Add();
	__try {
		ListViewItem->Text = Contact->DisplayName;
		ListViewItem->Tag = Contact->ID;
	}
	__finally {
		ListViewItem->Free();
	}
}

// ---------------------------------------------------------------------------
// Fill the Combo box with existed group names
void __fastcall TForm1::FillGroupList(TAddressBookSource *Source) {
	int i;
	fGroups = new TAddressBookGroups();
	AddressBook1->AllGroups(Source, fGroups);
	__try {
		ComboBox1->BeginUpdate();
		ComboBox2->BeginUpdate();
		ComboBox1->Clear();
		ComboBox2->Clear();
		for (i = 0; i < fGroups->Count; i++) {
			ComboBox1->Items->Add(fGroups->Items[i]->Name);
			ComboBox2->Items->Add(fGroups->Items[i]->Name);
		}
	}
	__finally {
		ComboBox1->Visible = (fGroups->Count != 0);
		ComboBox2->Visible = ComboBox1->Visible;
		btnRemoveGroup->Visible = ComboBox1->Visible;
		ComboBox2->EndUpdate();
		ComboBox1->EndUpdate();
	}

}

// ---------------------------------------------------------------------------
// Fill the List box with display names of existed contacts.
void __fastcall TForm1::FillContactlist(TAddressBookSource *Source) {
	int i;
	TAddressBookContacts *Contacts = new TAddressBookContacts();
	__try {
		AddressBook1->AllContacts(Source, Contacts);
		__try {
			ListViewContacts->BeginUpdate();
			ListViewContacts->Items->Clear();
			for (i = 0; i < Contacts->Count; i++)
				AddListViewItem(Contacts->Items[i]);
		}
		__finally {
			ListViewContacts->EndUpdate();
		}
	}
	__finally {
		Contacts->Free();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender) {
	if (AddressBook1->Supported()) {
		// Display this information box while loading the contacts
		TDialogService::ShowMessage("Loading contacts...");
		AddressBook1->RequestPermission();
	}
	else {
		TDialogService::ShowMessage
			("This platform does not support the Address Book service.");
	}
	TabControl1->ActiveTab = TabItemContacts;

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::AddressBook1PermissionRequest(TObject *ASender,
	const UnicodeString AMessage, const bool AAccessGranted) {
	int i;
	if (AAccessGranted) {
		ActionRefresh->Execute();

	}
	else {
		ShowMessage("You cannot access Address Book. Reason: " + AMessage);
	}
}

// ---------------------------------------------------------------------------
// Clear the Add Contact form
void __fastcall TForm1::ClearAddContactForm() {
	edtFirstName->Text = "";
	edtLastName->Text = "";
	edtWorkMail->Text = "";
	ComboBox1->ItemIndex = -1;
	Image1->Bitmap->SetSize(0, 0);
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::btnClearClick(TObject *Sender) {
	ClearAddContactForm();
}

// ---------------------------------------------------------------------------

bool __fastcall TForm1::ContactExists(int ID) {
	TAddressBookContact *Contact = AddressBook1->ContactByID(ID);
	bool b = Contact != NULL;
	Contact->Free();
	return b;
}

void __fastcall TForm1::AddressBook1ExternalChange(TObject *ASender) {
	AddressBook1->RevertCurrentChangesAndUpdate();
	ActionRefresh->Execute();
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::ActionRefreshExecute(TObject *Sender) {
	// Select the default source as the current source
	fCurrentSource = AddressBook1->DefaultSource();
	FillGroupList(fCurrentSource);
	FillContactlist(fCurrentSource);
}

// ---------------------------------------------------------------------------
// Add a newly created contact to Address Book
void __fastcall TForm1::ActionAddContactExecute(TObject *Sender) {
	TAddressBookContact *Contact;
	TContactEmails *eMails;
	Contact = AddressBook1->CreateContact(fCurrentSource);

	__try {
		try {

			Contact->FirstName = edtFirstName->Text;
			Contact->LastName = edtLastName->Text;
			// Add a photo if selected
			if (!Image1->Bitmap->Size.IsZero()) {
				TBitmapSurface *photo = new TBitmapSurface();
				__try {
					photo->Assign(Image1->Bitmap);
					Contact->Photo = photo;
					Image1->Bitmap->SetSize(0, 0);
				}
				__finally {
					photo->Free();
				}
			}
			// Add the work mail
			eMails = new TContactEmails();
			__try {
				eMails->AddEmail(TContactEmail::TLabelKind::Work,
					edtWorkMail->Text);
				Contact->EMails = eMails;
			}
			__finally {
				eMails->Free();
			}
			AddressBook1->SaveContact(Contact);
		}
		catch (const EAddressBookException& E) {
			ShowMessage("Cannot create the contact. " + E.Message);
		}
		// Add the contact to the selected group, if any
		try {
			if ((ComboBox1->ItemIndex > -1) && (ComboBox1->ItemIndex <
				fGroups->Count)) {
				AddressBook1->AddContactIntoGroup
					(fGroups->Items[ComboBox1->ItemIndex], Contact);
			}
		}
		catch (const EAddressBookException& E) {
			ShowMessage("Cannot add the created contact to the group . " +
				E.Message);
		}

		ListViewContacts->BeginUpdate();
		AddListViewItem(Contact);
		TabControl1->ActiveTab = TabItemContacts;
	}
	__finally {

		Contact->Free();
		ListViewContacts->EndUpdate();
		ClearAddContactForm();

	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::ActionAddGroupExecute(TObject *Sender) {
	TAddressBookGroup *Group = AddressBook1->CreateGroup(fCurrentSource);
	if (Group != NULL) {
		__try {
			Group->Name = edtGroupName->Text;
			try {
				AddressBook1->SaveGroup(Group);
				edtGroupName->Text = "";
				TabControl1->ActiveTab = TabItemContacts;
			}
			catch (const EAddressBookException& E) {
				ShowMessage("Cannot add the group. " + E.Message);
			}
			FillGroupList(fCurrentSource);
		}
		__finally {
			Group->Free();
		}
	}
	else {
		ShowMessage("Cannot add this group.");
	}

}
// ---------------------------------------------------------------------------

void __fastcall TForm1::ActionList1Update(TBasicAction *Action, bool &Handled) {
	ActionAddContact->Enabled =
		AddressBook1->Supported() && ((edtFirstName->Text != "") ||
		(edtLastName->Text != ""));
	ActionRefresh->Enabled = AddressBook1->Supported();
	ActionAddGroup->Enabled = AddressBook1->Supported();
	ActionRemoveGroup->Visible = (fGroups != NULL) && (fGroups->Count > 0);
	ActionRemoveContact->Enabled = ListViewContacts->ItemIndex > -1;
}

// ---------------------------------------------------------------------------
// Delete the selected contact (only after a confirmation from user)
void __fastcall TForm1::DeleteContact(TObject * Sender, TModalResult AKey) {
	if (AKey == System::Uitypes::mrYes) {
		int contactIndex = ListViewContacts->ItemIndex;
		if (contactIndex > -1) {
			int ContactID =
				ListViewContacts->Items->AppearanceItem[contactIndex]->Tag;
			TAddressBookContact *Contact = AddressBook1->ContactByID(ContactID);
			__try {
				AddressBook1->RemoveContact(Contact);
				ListViewContacts->BeginUpdate();
				// Check if the contact was really deleted
				if (!ContactExists(ContactID)) {

					ListViewContacts->Items->Delete(contactIndex);
				}
				else
					ShowMessage("Cannot delete this contact: " +
					Contact->FirstName);
			}
			__finally {
				ListViewContacts->EndUpdate();
				Contact->Free();

			}
		}

	}
}

// ---------------------------------------------------------------------------
// Display a confirmation dialog box
void __fastcall TForm1::ActionRemoveContactExecute(TObject * Sender) {
	TDialogService::MessageDialog("Do you want to delete this contact?",
		TMsgDlgType::mtConfirmation,
		TMsgDlgButtons() << TMsgDlgBtn::mbYes << TMsgDlgBtn::mbNo,
		TMsgDlgBtn::mbNo, 0, DeleteContact);
}

// ---------------------------------------------------------------------------
// Remove selected group
void __fastcall TForm1::ActionRemoveGroupExecute(TObject * Sender) {
	if ((ComboBox2->ItemIndex > -1) && (ComboBox2->ItemIndex < fGroups->Count))
	{
		AddressBook1->RemoveGroup(fGroups->Items[ComboBox2->ItemIndex]);
		FillGroupList(fCurrentSource);
	}
}

// ---------------------------------------------------------------------------
// Select a photo from a device photo library
void __fastcall TForm1::TakePhotoFromLibraryAction1DidFinishTaking
	(TBitmap * Image)

{
	Image1->Bitmap->Assign(Image);
}

// ---------------------------------------------------------------------------
// Take a photo from a device camera
void __fastcall TForm1::TakePhotoFromCameraAction1DidFinishTaking
	(TBitmap * Image)

{
	Image1->Bitmap->Assign(Image);
}
// ---------------------------------------------------------------------------
