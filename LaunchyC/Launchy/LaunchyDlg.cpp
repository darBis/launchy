/*
Launchy: Application Launcher
Copyright (C) 2005  Josh Karlin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// LaunchyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Launchy.h"
#include "LaunchyDlg.h"
#include "HotkeyDialog.h"
#include "Skin.h"
#include "SkinChooser.h"
#include "DirectoryChooser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLaunchyDlg dialog

#define DELAY_TIMER 100
#define UPDATE_TIMER 101

CLaunchyDlg::CLaunchyDlg(CWnd* pParent /*=NULL*/)
: CDialogSK(CLaunchyDlg::IDD, pParent)
, DelayTimer(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);

	atLaunch = true;

	DelayTimer = 100;
}

void CLaunchyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogSK::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Input, InputBox);
	DDX_Control(pDX, IDC_PREVIEW, Preview);
}

BEGIN_MESSAGE_MAP(CLaunchyDlg, CDialogSK)
	ON_MESSAGE(WM_HOTKEY,OnHotKey)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_Input, &CLaunchyDlg::OnCbnSelchangeInput)
	ON_WM_TIMER()
	ON_WM_ENDSESSION()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()




// CLaunchyDlg message handlers

BOOL CLaunchyDlg::OnInitDialog()
{
	CDialogSK::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon



	// BEGIN SKINNING FUNCTIONS
	EnableEasyMove();                       // enable moving of
	// the dialog by
	// clicking
	// anywhere in
	// the dialog


	options.reset(new Options());
	smarts.reset(new LaunchySmarts());

	applySkin();

	BOOL m_isKeyRegistered = RegisterHotKey(GetSafeHwnd(), 100,
		options->mod_key, options->vkey);

	ASSERT(m_isKeyRegistered != FALSE);


	SetTimer(UPDATE_TIMER, 1200000, NULL);

	// In order to subclass the combobox list and edit controls
	// we have to first paint the controls to make sure the message
	// mapping is setup before we use the controls.
	InputBox.ShowDropDown(true);
	InputBox.ShowDropDown(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLaunchyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogSK::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLaunchyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CLaunchyDlg::OnHotKey(WPARAM wParam, LPARAM lParam) {
	if (wParam == 100) {
		if (atLaunch)
		{
			this->Visible = false;
			atLaunch = false;
		}
		this->Visible = !this->Visible;
		if (Visible)
		{
			this->ShowWindow(SW_SHOW);
			this->ActivateTopParent();
			this->InputBox.SetFocus();
		}
		else {
			this->ShowWindow(SW_HIDE);
		}
	}
	return 1;
}

void CLaunchyDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if(this->atLaunch) {
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
		if (options->posX != -1 && options->posY != -1) {
			lpwndpos->x = options->posX;
			lpwndpos->y = options->posY;
		}
	}
	CDialogSK::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
}

void CLaunchyDlg::OnClose()
{
	options.reset();
	smarts.reset();
	// TODO: Add your message handler code here and/or call default
	CDialogSK::OnClose();
}

void CLaunchyDlg::OnDestroy()
{
	OnClose();
	CDialogSK::OnDestroy();

	// TODO: Add your message handler code here
}




void CLaunchyDlg::OnCbnSelchangeInput()
{
	// TODO: Add your control notification handler code here
}




BOOL CLaunchyDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN)
	{
		SetTimer(DELAY_TIMER, 1000, NULL);
		if(pMsg->wParam==VK_RETURN) {
			this->ShowWindow(SW_HIDE);
			this->Visible = false;


			if (InputBox.typed != searchTxt) {
				CString x;
				options->Associate(InputBox.typed, searchTxt);
			}

			smarts->Launch();
			pMsg->wParam = NULL;
		}

		if (pMsg->wParam==VK_ESCAPE) {
			this->ShowWindow(SW_HIDE);
			this->Visible = false;
			pMsg->wParam = NULL;
		}

	} 

	if (pMsg->message == WM_CHAR) {
		SetTimer(DELAY_TIMER, 1000, NULL);

	}
	return CDialogSK::PreTranslateMessage(pMsg);
}

void CLaunchyDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == DELAY_TIMER) {
		if (Visible && InputBox.m_edit.GetWindowTextLengthW() > 0 &&
			InputBox.m_listbox.GetCount() > 1) {
			InputBox.ShowDropDown(true);
		}
		KillTimer(DELAY_TIMER);
		CDialogSK::OnTimer(nIDEvent);
	}
	else if (nIDEvent == UPDATE_TIMER) {
		smarts->LoadCatalog();
	}
}

void CLaunchyDlg::OnEndSession(BOOL bEnding)
{
	if (options != NULL) { options->Store(); }
	CDialogSK::OnEndSession(bEnding);

	// TODO: Add your message handler code here
}

void CLaunchyDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: Add your message handler code here
	// Load the desired menu
	CMenu mnuPopupSubmit;
	mnuPopupSubmit.LoadMenu(IDR_MENU1);


	// Get a pointer to the first item of the menu
	CMenu *mnuPopupMenu = mnuPopupSubmit.GetSubMenu(0);
	ASSERT(mnuPopupMenu);
	
	DWORD selection = mnuPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON| TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, this);

	if (selection == ID_SETTINGS_SKINS) {
		SkinChooser dlg;
		dlg.DoModal();
	}
	else if (selection == ID_SETTINGS_HOTKEY) {
		CHotkeyDialog dlg;
		dlg.DoModal();
	}

	else if (selection == ID_SETTINGS_DIRECTORIES) {
		DirectoryChooser dlg;
		dlg.DoModal();
	}

	else if (selection == ID_EXIT) {
		this->EndDialog(1);
	}
}

void CLaunchyDlg::applySkin()
{
	if (options->skin == NULL) {
		return;
	}

	SetBitmap(options->skin->bgFile);
//	SetBitmap (IDB_BACKGROUND);             // set background
	// bitmap
	SetStyle (LO_STRETCH);                   // resize dialog to
	// the size of
	// the bitmap
	if (options->skin->transparency != -1) {
		SetTransparent(options->skin->transparency);
	} else {
		SetTransparentColor(RGB(options->skin->red, options->skin->green, options->skin->blue));    // set red as
	}
	// the transparent
	// color

/*	CRect rect;
	GetClientRect(&rect);

	CString x;
	MoveWindow(0,0,405,81,1);
	x.Format(_T("%d,%d,%d,%d"),rect.left, rect.top, rect.Width(), rect.Height()); 
AfxMessageBox(x);
*/
	MoveWindow(options->posX, options->posY, options->skin->width, options->skin->height,1);
	InputBox.MoveWindow(options->skin->inputRect,1);
	Preview.MoveWindow(options->skin->resultRect,1);

	CFont ff1, ff2;
	ff1.CreatePointFont(240,_T("Tahoma"));
/*	LOGFONT lf;
	options->skin->m_FontInput.GetLogFont(&lf);
	ff1.CreateFontIndirectW(&lf);
	
	options->skin->m_FontResult.GetLogFont(&lf);
	ff2.CreateFontIndirectW(&lf);

	InputBox.setfo
	*/
		this->SetFont(&ff1);

//	InputBox.SetFont(&ff1);
//	Preview.SetFont(&ff2);


	InputBox.SetTextColor(options->skin->inputFontRGB);
	Preview.SetTextColor(options->skin->resultFontRGB);

	// Widget backgrounds

	InputBox.SetBackColor(options->skin->inputRGB);
	Preview.SetBackColor(options->skin->resultRGB);	
}
