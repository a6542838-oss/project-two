// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: MyDrawBoardView.cpp
// AI 辅助声明: 核心逻辑由 AI 生成，已人工审核。
// =================================================================================
#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "MyDrawBoard.h"
#endif

#include "MyDrawBoardDoc.h"
#include "MyDrawBoardView.h"
#include "GdiUtils.h" 
#include "MainFrm.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMyDrawBoardView, CView)

BEGIN_MESSAGE_MAP(CMyDrawBoardView, CView)
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()

    // 工具
    ON_COMMAND(ID_TOOL_LINE, &CMyDrawBoardView::OnToolLine)
    ON_COMMAND(ID_TOOL_RECT, &CMyDrawBoardView::OnToolRect)
    ON_COMMAND(ID_TOOL_ELLIPSE, &CMyDrawBoardView::OnToolEllipse)
    ON_COMMAND(ID_TOOL_FREEHAND, &CMyDrawBoardView::OnToolFreehand) // === 新增 ===
    ON_COMMAND(ID_TOOL_COLOR, &CMyDrawBoardView::OnToolColor)

    // 线宽
    ON_COMMAND(ID_WIDTH_1, &CMyDrawBoardView::OnWidth1)
    ON_COMMAND(ID_WIDTH_3, &CMyDrawBoardView::OnWidth3)
    ON_COMMAND(ID_WIDTH_5, &CMyDrawBoardView::OnWidth5)
    ON_COMMAND(ID_WIDTH_8, &CMyDrawBoardView::OnWidth8)
    ON_UPDATE_COMMAND_UI(ID_WIDTH_1, &CMyDrawBoardView::OnUpdateWidth1)
    ON_UPDATE_COMMAND_UI(ID_WIDTH_3, &CMyDrawBoardView::OnUpdateWidth3)
    ON_UPDATE_COMMAND_UI(ID_WIDTH_5, &CMyDrawBoardView::OnUpdateWidth5)
    ON_UPDATE_COMMAND_UI(ID_WIDTH_8, &CMyDrawBoardView::OnUpdateWidth8)

    // 多语言
    ON_COMMAND(ID_LANG_CN, &CMyDrawBoardView::OnLangCN)
    ON_COMMAND(ID_LANG_EN, &CMyDrawBoardView::OnLangEN)
    ON_COMMAND(ID_LANG_JP, &CMyDrawBoardView::OnLangJP)
    ON_UPDATE_COMMAND_UI(ID_LANG_CN, &CMyDrawBoardView::OnUpdateLangCN)
    ON_UPDATE_COMMAND_UI(ID_LANG_EN, &CMyDrawBoardView::OnUpdateLangEN)
    ON_UPDATE_COMMAND_UI(ID_LANG_JP, &CMyDrawBoardView::OnUpdateLangJP)
END_MESSAGE_MAP()

CMyDrawBoardView::CMyDrawBoardView() noexcept {}
CMyDrawBoardView::~CMyDrawBoardView() {}

BOOL CMyDrawBoardView::PreCreateWindow(CREATESTRUCT& cs) { return CView::PreCreateWindow(cs); }

// === 绘制 ===
void CMyDrawBoardView::OnDraw(CDC* pDC)
{
    CMyDrawBoardDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc) return;

    CRect clientRect;
    GetClientRect(&clientRect);

    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    CBitmap memBitmap;
    memBitmap.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

    memDC.FillSolidRect(&clientRect, RGB(255, 255, 255));

    for (const auto& shape : pDoc->m_shapes) {
        shape->Draw(&memDC);
    }
    if (m_pTempShape) {
        m_pTempShape->Draw(&memDC);
    }

    pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBitmap);
}

BOOL CMyDrawBoardView::OnPreparePrinting(CPrintInfo* pInfo) { return DoPreparePrinting(pInfo); }
void CMyDrawBoardView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {}
void CMyDrawBoardView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {}

#ifdef _DEBUG
void CMyDrawBoardView::AssertValid() const { CView::AssertValid(); }
void CMyDrawBoardView::Dump(CDumpContext& dc) const { CView::Dump(dc); }
CMyDrawBoardDoc* CMyDrawBoardView::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyDrawBoardDoc)));
    return (CMyDrawBoardDoc*)m_pDocument;
}
#endif

// === 交互 ===
void CMyDrawBoardView::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_isDrawing = true;
    m_startPoint = point;
    m_pTempShape = ShapeFactory::CreateShape(m_currentTool);
    if (m_pTempShape) {
        m_pTempShape->SetStartPoint(point);
        m_pTempShape->SetEndPoint(point); // 初始点
        m_pTempShape->SetColor(m_currentColor);
        m_pTempShape->SetLineWidth(m_currentLineWidth);
    }
    CView::OnLButtonDown(nFlags, point);
}

void CMyDrawBoardView::OnMouseMove(UINT nFlags, CPoint point)
{
    CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
    if (pFrame) {
        CString strStatus;
        if (m_currentLang == AppLang::English)
            strStatus.Format(_T("Pos: %d, %d"), point.x, point.y);
        else if (m_currentLang == AppLang::Japanese)
            strStatus.Format(_T("座標: %d, %d"), point.x, point.y);
        else
            strStatus.Format(_T("坐标: %d, %d"), point.x, point.y);
        pFrame->SetMessageText(strStatus);
    }

    if (m_isDrawing && (nFlags & MK_LBUTTON) && m_pTempShape) {
        // 对于自由曲线，SetEndPoint = 追加新点
        // 对于其他图形，SetEndPoint = 更新终点
        m_pTempShape->SetEndPoint(point);
        Invalidate();
    }
    CView::OnMouseMove(nFlags, point);
}

void CMyDrawBoardView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_isDrawing && m_pTempShape) {
        m_pTempShape->SetEndPoint(point);
        m_isDrawing = false;
        CMyDrawBoardDoc* pDoc = GetDocument();
        auto cmd = std::make_shared<CAddShapeCommand>(m_pTempShape, pDoc->m_shapes);
        m_commandManager.ExecuteCommand(cmd);
        pDoc->SetModifiedFlag();
        m_pTempShape = nullptr;
        Invalidate();
    }
    CView::OnLButtonUp(nFlags, point);
}

void CMyDrawBoardView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (GetKeyState(VK_CONTROL) < 0) {
        if (nChar == 'Z') { m_commandManager.Undo(); Invalidate(); }
        else if (nChar == 'Y') { m_commandManager.Redo(); Invalidate(); }
    }
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

// === 工具函数 ===
void CMyDrawBoardView::OnToolLine() { m_currentTool = ShapeType::Line; }
void CMyDrawBoardView::OnToolRect() { m_currentTool = ShapeType::Rectangle; }
void CMyDrawBoardView::OnToolEllipse() { m_currentTool = ShapeType::Ellipse; }
void CMyDrawBoardView::OnToolFreehand() { m_currentTool = ShapeType::Freehand; } // === 新增 ===
void CMyDrawBoardView::OnToolColor() {
    CColorDialog dlg(m_currentColor);
    if (dlg.DoModal() == IDOK) m_currentColor = dlg.GetColor();
}

void CMyDrawBoardView::OnWidth1() { m_currentLineWidth = 1; }
void CMyDrawBoardView::OnWidth3() { m_currentLineWidth = 3; }
void CMyDrawBoardView::OnWidth5() { m_currentLineWidth = 5; }
void CMyDrawBoardView::OnWidth8() { m_currentLineWidth = 8; }
void CMyDrawBoardView::OnUpdateWidth1(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLineWidth == 1); }
void CMyDrawBoardView::OnUpdateWidth3(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLineWidth == 3); }
void CMyDrawBoardView::OnUpdateWidth5(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLineWidth == 5); }
void CMyDrawBoardView::OnUpdateWidth8(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLineWidth == 8); }

// === 多语言 ===
void CMyDrawBoardView::OnLangCN() { m_currentLang = AppLang::Chinese; UpdateMenuText(); }
void CMyDrawBoardView::OnLangEN() { m_currentLang = AppLang::English; UpdateMenuText(); }
void CMyDrawBoardView::OnLangJP() { m_currentLang = AppLang::Japanese; UpdateMenuText(); }
void CMyDrawBoardView::OnUpdateLangCN(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLang == AppLang::Chinese); }
void CMyDrawBoardView::OnUpdateLangEN(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLang == AppLang::English); }
void CMyDrawBoardView::OnUpdateLangJP(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLang == AppLang::Japanese); }

void CMyDrawBoardView::UpdateMenuText()
{
    CMenu* pMenu = AfxGetMainWnd()->GetMenu();
    if (!pMenu) return;

    if (m_currentLang == AppLang::English) {
        pMenu->ModifyMenu(ID_FILE_NEW, MF_BYCOMMAND | MF_STRING, ID_FILE_NEW, _T("New"));
        pMenu->ModifyMenu(ID_FILE_OPEN, MF_BYCOMMAND | MF_STRING, ID_FILE_OPEN, _T("Open"));
        pMenu->ModifyMenu(ID_FILE_SAVE, MF_BYCOMMAND | MF_STRING, ID_FILE_SAVE, _T("Save"));

        pMenu->ModifyMenu(ID_TOOL_LINE, MF_BYCOMMAND | MF_STRING, ID_TOOL_LINE, _T("Line"));
        pMenu->ModifyMenu(ID_TOOL_RECT, MF_BYCOMMAND | MF_STRING, ID_TOOL_RECT, _T("Rectangle"));
        pMenu->ModifyMenu(ID_TOOL_ELLIPSE, MF_BYCOMMAND | MF_STRING, ID_TOOL_ELLIPSE, _T("Ellipse"));
        pMenu->ModifyMenu(ID_TOOL_FREEHAND, MF_BYCOMMAND | MF_STRING, ID_TOOL_FREEHAND, _T("Freehand")); // === 新增 ===
        pMenu->ModifyMenu(ID_TOOL_COLOR, MF_BYCOMMAND | MF_STRING, ID_TOOL_COLOR, _T("Color..."));

        pMenu->ModifyMenu(ID_WIDTH_1, MF_BYCOMMAND | MF_STRING, ID_WIDTH_1, _T("1 px"));
        pMenu->ModifyMenu(ID_WIDTH_3, MF_BYCOMMAND | MF_STRING, ID_WIDTH_3, _T("3 px"));
        AfxGetMainWnd()->SetWindowText(_T("MyDrawBoard - English Mode"));
    }
    else if (m_currentLang == AppLang::Japanese) {
        pMenu->ModifyMenu(ID_FILE_NEW, MF_BYCOMMAND | MF_STRING, ID_FILE_NEW, _T("新規作成"));
        pMenu->ModifyMenu(ID_FILE_OPEN, MF_BYCOMMAND | MF_STRING, ID_FILE_OPEN, _T("開く"));
        pMenu->ModifyMenu(ID_FILE_SAVE, MF_BYCOMMAND | MF_STRING, ID_FILE_SAVE, _T("保存"));

        pMenu->ModifyMenu(ID_TOOL_LINE, MF_BYCOMMAND | MF_STRING, ID_TOOL_LINE, _T("直線"));
        pMenu->ModifyMenu(ID_TOOL_RECT, MF_BYCOMMAND | MF_STRING, ID_TOOL_RECT, _T("長方形"));
        pMenu->ModifyMenu(ID_TOOL_ELLIPSE, MF_BYCOMMAND | MF_STRING, ID_TOOL_ELLIPSE, _T("楕円"));
        pMenu->ModifyMenu(ID_TOOL_FREEHAND, MF_BYCOMMAND | MF_STRING, ID_TOOL_FREEHAND, _T("自由曲線")); // === 新增 ===
        pMenu->ModifyMenu(ID_TOOL_COLOR, MF_BYCOMMAND | MF_STRING, ID_TOOL_COLOR, _T("色設定..."));

        pMenu->ModifyMenu(ID_WIDTH_1, MF_BYCOMMAND | MF_STRING, ID_WIDTH_1, _T("1 ピクセル"));
        pMenu->ModifyMenu(ID_WIDTH_3, MF_BYCOMMAND | MF_STRING, ID_WIDTH_3, _T("3 ピクセル"));
        AfxGetMainWnd()->SetWindowText(_T("MyDrawBoard - 日本語モード"));
    }
    else { // Chinese
        pMenu->ModifyMenu(ID_FILE_NEW, MF_BYCOMMAND | MF_STRING, ID_FILE_NEW, _T("新建"));
        pMenu->ModifyMenu(ID_FILE_OPEN, MF_BYCOMMAND | MF_STRING, ID_FILE_OPEN, _T("打开"));
        pMenu->ModifyMenu(ID_FILE_SAVE, MF_BYCOMMAND | MF_STRING, ID_FILE_SAVE, _T("保存"));

        pMenu->ModifyMenu(ID_TOOL_LINE, MF_BYCOMMAND | MF_STRING, ID_TOOL_LINE, _T("直线"));
        pMenu->ModifyMenu(ID_TOOL_RECT, MF_BYCOMMAND | MF_STRING, ID_TOOL_RECT, _T("矩形"));
        pMenu->ModifyMenu(ID_TOOL_ELLIPSE, MF_BYCOMMAND | MF_STRING, ID_TOOL_ELLIPSE, _T("椭圆"));
        pMenu->ModifyMenu(ID_TOOL_FREEHAND, MF_BYCOMMAND | MF_STRING, ID_TOOL_FREEHAND, _T("自由曲线")); // === 新增 ===
        pMenu->ModifyMenu(ID_TOOL_COLOR, MF_BYCOMMAND | MF_STRING, ID_TOOL_COLOR, _T("选择颜色..."));

        pMenu->ModifyMenu(ID_WIDTH_1, MF_BYCOMMAND | MF_STRING, ID_WIDTH_1, _T("1 像素"));
        pMenu->ModifyMenu(ID_WIDTH_3, MF_BYCOMMAND | MF_STRING, ID_WIDTH_3, _T("3 像素"));
        AfxGetMainWnd()->SetWindowText(_T("MyDrawBoard - 中文模式"));
    }

    AfxGetMainWnd()->DrawMenuBar();
}