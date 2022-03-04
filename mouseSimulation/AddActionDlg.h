#pragma once
#include "afxwin.h"


// CAddActionDlg �Ի���

class CAddActionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddActionDlg)

public:
	CAddActionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddActionDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ADD_ACTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	CEdit editLeftX;
	CEdit editLeftY;
	CEdit editRightX;
	CEdit editRightY;
	CEdit editKeyInput;
	CEdit editDelay;
public:
	afx_msg void OnBnClickedRadioLeftClick();
	afx_msg void OnBnClickedRadioRightClick();
	afx_msg void OnBnClickedRadioKeyInput();
	afx_msg void OnBnClickedRadioSleep();
	afx_msg void OnBnClickedOk();
};
