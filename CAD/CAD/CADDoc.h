
// CADDoc.h: CCADDoc 类的接口
//


#pragma once
#include "CADView.h"

class CCADDoc : public CDocument
{
protected: // 仅从序列化创建
	CCADDoc() noexcept;
	DECLARE_DYNCREATE(CCADDoc)

	// 成员数据
public:
	// 当前绘制图形
	CShape* m_pCurrentShape;

	// 当前操作
	COperation* m_pCurrentOperation;

	// 工厂模式选择
	CShapeFactory* m_pShapeFactory;

	// 图形存储
	CList<CShape*> m_listShape;

	// 画笔画刷存储
	LOGBRUSH m_logBrush;
	LOGPEN m_logPen;

	// 当前状态
	enum {
		DRAW, SELECT, MOVE, ROTATE, FREE
	} m_eState;

	// 鼠标按下时的坐标
	CPoint m_ptMoveBegin;

	// 操作记录
	std::stack<COperation*> m_stkToUndo;	//撤销
	std::stack<COperation*> m_stkToRedo;	//重做

	// 图片缓存
	CImageList m_ImgList;

	// 特性
public:

	// 操作
public:

	// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// 实现
public:
	virtual ~CCADDoc();
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
#endif // SHARED_HANDLERS
};
