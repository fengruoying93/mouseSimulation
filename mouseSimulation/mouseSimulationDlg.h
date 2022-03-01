
// mouseSimulationDlg.h : 头文件
//

#pragma once
#include <string>

using namespace std;

#define WM_MyMessage (WM_USER+100)  //自定义消息


// CmouseSimulationDlg 对话框
class CmouseSimulationDlg : public CDialogEx
{
// 构造
public:
	CmouseSimulationDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MOUSESIMULATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	BOOL InitMyMenu(void);
	BOOL InitControls(void);
	void open_web(char *website);
	void click(int x, int y);
	void input(string &input);
	void close_app();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

//成员变量
private:
	CMenu* pMainMenu;
	int timerId;
	int cxScreen; //屏幕宽
	int cyScreen; //屏幕高

//消息处理函数
public:
	afx_msg void OnAbout();
	afx_msg void OnRun();
	afx_msg void OnSetting();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	//自定义消息
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
};
