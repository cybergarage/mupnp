// COptionDialog.cpp : �����t�@�C��
//

#include "COptionDialog.h"
#include "CDmsMobilePhoto.h"
#include "stdafx.h"

// COptionDialog �_�C�A���O

IMPLEMENT_DYNAMIC(COptionDialog, CDialog)

COptionDialog::COptionDialog(CWnd* pParent /*=NULL*/)
    : CDialog(COptionDialog::IDD, pParent)
{
}

COptionDialog::~COptionDialog()
{
}

void COptionDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COptionDialog, CDialog)
END_MESSAGE_MAP()

// COptionDialog ���b�Z�[�W �n���h��
