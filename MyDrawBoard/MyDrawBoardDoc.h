#pragma once
#include <vector>
#include <memory>
#include "ShapeEngine.h" // 一定要包含这个

class CMyDrawBoardDoc : public CDocument
{
protected: // 仅从序列化创建
    CMyDrawBoardDoc() noexcept;
    DECLARE_DYNCREATE(CMyDrawBoardDoc)

    // 特性
public:
    // === 核心数据：存放所有图形 ===
    std::vector<std::shared_ptr<IShape>> m_shapes;

    // 操作
public:

    // 重写
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
    virtual void InitializeSearchContent();
    virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif

    // 实现
public:
    virtual ~CMyDrawBoardDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    // 生成的消息映射函数
protected:
    DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
    // 用于为搜索处理程序设置搜索内容的 Helper 函数
    void SetSearchContent(const CString& value);
#endif
};