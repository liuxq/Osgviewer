
// OsgviewerDoc.h : interface of the COsgviewerDoc class
//


#pragma once


class COsgviewerDoc : public CDocument
{
protected: // create from serialization only
	COsgviewerDoc();
	DECLARE_DYNCREATE(COsgviewerDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	CString GetFileName() const { return m_csFileName; }
	void setCurFrameIndex(int curIndex){curFrameIndex = curIndex;};
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~COsgviewerDoc();

	virtual void OnInitialUpdate();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	afx_msg void OnTCLConnect();
	afx_msg void OnOptionPreference();
	afx_msg void OnFileExport();
	afx_msg void OnRecord();
	afx_msg void OnStop();
protected:
	CString m_csFileName;

	HANDLE mThreadHandle;
	HANDLE mThreadHandle2;

	int recordStart;
	int recordEnd;

	int curFrameIndex;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
