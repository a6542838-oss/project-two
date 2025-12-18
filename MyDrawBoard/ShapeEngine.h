// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: ShapeEngine.h
// AI 辅助声明: 核心逻辑由 AI 生成，已人工审核。
// =================================================================================
#pragma once
#include <afxwin.h>
#include <memory>
#include <vector>
#include <cmath> // 这里的 abs 需要用到

/// @brief 图形类型枚举
enum class ShapeType {
    Line = 1,       ///< 直线
    Rectangle = 2,  ///< 矩形
    Ellipse = 3,    ///< 椭圆
    Freehand = 4    ///< === 新增：自由曲线 ===
};

/// @brief 图形抽象基类 (Interface)
class IShape {
public:
    virtual ~IShape() {}
    virtual void SetStartPoint(CPoint pt) = 0;
    virtual void SetEndPoint(CPoint pt) = 0;
    virtual void Draw(CDC* pDC) = 0;

    // 序列化
    virtual void Save(CArchive& ar) {
        ar << m_startPt << m_endPt << m_color << m_lineWidth;
    }
    virtual void Load(CArchive& ar) {
        ar >> m_startPt >> m_endPt >> m_color >> m_lineWidth;
    }

    void SetColor(COLORREF color) { m_color = color; }
    void SetLineWidth(int width) { m_lineWidth = width; }
    virtual ShapeType GetType() const = 0;

protected:
    CPoint m_startPt;
    CPoint m_endPt;
    COLORREF m_color = RGB(0, 0, 0);
    int m_lineWidth = 1;
};

// === 直线 ===
class CLineShape : public IShape {
public:
    void SetStartPoint(CPoint pt) override { m_startPt = pt; }
    void SetEndPoint(CPoint pt) override { m_endPt = pt; }
    ShapeType GetType() const override { return ShapeType::Line; }

    void Draw(CDC* pDC) override {
        CPen pen(PS_SOLID, m_lineWidth, m_color);
        CPen* pOldPen = pDC->SelectObject(&pen);
        pDC->MoveTo(m_startPt);
        pDC->LineTo(m_endPt);
        pDC->SelectObject(pOldPen);
    }
};

// === 矩形 ===
class CRectShape : public IShape {
public:
    void SetStartPoint(CPoint pt) override { m_startPt = pt; }
    void SetEndPoint(CPoint pt) override { m_endPt = pt; }
    ShapeType GetType() const override { return ShapeType::Rectangle; }

    void Draw(CDC* pDC) override {
        CPen pen(PS_SOLID, m_lineWidth, m_color);
        CPen* pOldPen = pDC->SelectObject(&pen);
        CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
        pDC->Rectangle(CRect(m_startPt, m_endPt));
        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);
    }
};

// === 椭圆 ===
class CEllipseShape : public IShape {
public:
    void SetStartPoint(CPoint pt) override { m_startPt = pt; }
    void SetEndPoint(CPoint pt) override { m_endPt = pt; }
    ShapeType GetType() const override { return ShapeType::Ellipse; }

    void Draw(CDC* pDC) override {
        CPen pen(PS_SOLID, m_lineWidth, m_color);
        CPen* pOldPen = pDC->SelectObject(&pen);
        CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
        pDC->Ellipse(CRect(m_startPt, m_endPt));
        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);
    }
};

// === 新增：自由曲线 ===
class CFreehandShape : public IShape {
private:
    std::vector<CPoint> m_points; // 存储路径点

public:
    ShapeType GetType() const override { return ShapeType::Freehand; }

    void SetStartPoint(CPoint pt) override {
        m_points.clear();
        m_points.push_back(pt);
    }

    void SetEndPoint(CPoint pt) override {
        // 简单优化：过滤过近的点
        if (!m_points.empty()) {
            CPoint last = m_points.back();
            if (abs(last.x - pt.x) < 2 && abs(last.y - pt.y) < 2) return;
        }
        m_points.push_back(pt);
    }

    void Draw(CDC* pDC) override {
        if (m_points.size() < 2) return;

        CPen pen(PS_SOLID, m_lineWidth, m_color);
        CPen* pOldPen = pDC->SelectObject(&pen);

        pDC->MoveTo(m_points[0]);
        for (size_t i = 1; i < m_points.size(); ++i) {
            pDC->LineTo(m_points[i]);
        }
        pDC->SelectObject(pOldPen);
    }

    void Save(CArchive& ar) override {
        IShape::Save(ar); // 存基础属性
        int count = (int)m_points.size();
        ar << count;
        for (const auto& pt : m_points) {
            ar << pt;
        }
    }

    void Load(CArchive& ar) override {
        IShape::Load(ar);
        int count = 0;
        ar >> count;
        m_points.clear();
        for (int i = 0; i < count; i++) {
            CPoint pt;
            ar >> pt;
            m_points.push_back(pt);
        }
    }
};

/// @brief 工厂类
class ShapeFactory {
public:
    static std::shared_ptr<IShape> CreateShape(ShapeType type) {
        switch (type) {
        case ShapeType::Line: return std::make_shared<CLineShape>();
        case ShapeType::Rectangle: return std::make_shared<CRectShape>();
        case ShapeType::Ellipse: return std::make_shared<CEllipseShape>();
        case ShapeType::Freehand: return std::make_shared<CFreehandShape>(); // 注册新类型
        default: return nullptr;
        }
    }
};