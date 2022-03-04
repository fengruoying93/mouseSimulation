
// mouseSimulationDlg.h : ͷ�ļ�
//

#pragma once
#include <string>

using namespace std;

#define WM_MyMessage (WM_USER+100)  //�Զ�����Ϣ


typedef enum _tagColumnWidth		//�б�ؼ���ÿ���еĿ��ֵ
{
	SERIALNUMBERWIDTH = 40,	    //���
	ACTIONWIDTH	  = 80,		//����
	PARAM1WIDTH	  = 100,		//����1
	PARAM2WIDTH   = 100			//����2
}COLUMNWIDTH;

typedef enum _tagColumnIndex		//�б�ؼ��и����е�����ֵ
{
	ID_SERIALNUMBER = 0,			//���
	ID_ACTION = 1,				//�ļ���
	ID_PARAM1 = 2,				//�ļ�·��
	ID_PARAM2 = 3				//�ļ�ʱ��
}COLUMNINDEX;

typedef enum
{
	LEFT_CLICK = 0,
	RIGHT_CLICK,
	KEY_INPUT,
	DELAY_TIME,
}ACTION_TYPE_E;

// CmouseSimulationDlg �Ի���
class CmouseSimulationDlg : public CDialogEx
{
// ����
public:
	CmouseSimulationDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MOUSESIMULATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	BOOL InitMyMenu(void);
	BOOL InitControls(void);
	BOOL InitMyListCtrl(void);
	void AddItemToListCtrl(int action, CString &param1, CString &param2);
	void open_web(char *website);
	void click(int x, int y);
	void input(string &input);
	void close_app();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

//��Ա����
private:
	CMenu* pMainMenu;
	int timerId;
	int cxScreen; //��Ļ��
	int cyScreen; //��Ļ��
	CListCtrl m_myListCtrl; //��ʾ�����б�ؼ�
	CButton buttonAddAction;
	CButton buttonRun;
	CEdit editLoopCount;
	

//��Ϣ������
public:
	afx_msg void OnAbout();
	afx_msg void OnRun();
	afx_msg void OnSetting();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	//�Զ�����Ϣ
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonAddAction();
	afx_msg void OnBnClickedButtonRun();
};
