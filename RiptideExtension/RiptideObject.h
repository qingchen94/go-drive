#pragma once

class CRiptideObject
{
public:
	CRiptideObject(void);
	virtual ~CRiptideObject(void);

	void SetPath(CComBSTR bstrPath) { m_path = bstrPath; }
	void SetName(CComBSTR bstrName) { m_name = bstrName; }
	void SetID (CComBSTR ID) { m_ID = ID; }
	void SetParentID (CComBSTR ID) { m_parentID = ID; }
	void SetFileModifiedWhen(LONGLONG ticks) { m_fileModifiedTicks = ticks; }
	void SetFileCreatedWhen(LONGLONG ticks)	{ m_fileCreatedTicks = ticks; }
	//void SetModifiedDate(LONGLONG ticks) { m_modifiedTicks = ticks; }
	//void SetCreatedDate(LONGLONG ticks)	{ m_createdTicks = ticks; }
	void SetFileSize(LONGLONG llSize) { m_fileSize = llSize; }
	void SetTrusteesRights(TrusteesRights rights)	{ m_trusteeRights = rights; }
	void SetSyncStatus(EnumSyncStatus syncStatus)	{ m_syncStatus = syncStatus; }
	void SetFileReadOnly(VARIANT_BOOL readOnly)	{ m_readOnly = (readOnly == VARIANT_TRUE) ? true : false; }
	void SetFileVersion(int nVersion)		{ m_fileVersion = nVersion; }
	void SetMD5(CComBSTR bstrMD5)	{ m_MD5 = bstrMD5; }
	void SetChangeType(int changeType)		{ m_changeType = changeType; }
	void SetHash(CComBSTR bstrHash)	{ m_hash = bstrHash; }
	void SetShared(VARIANT_BOOL bShared) { m_bShared = (bShared == VARIANT_TRUE) ? true : false; }
	void SetPublished(VARIANT_BOOL bPublished) { m_bPublished = (bPublished == VARIANT_TRUE) ? true : false; }


	CComBSTR GetID()						{ return m_ID; }
	CComBSTR GetParentID()					{ return m_parentID; }
	CComBSTR GetName()						{ return m_name; }
	CComBSTR GetPath()						{ return m_path; }
	LONGLONG GetLONGLONGFileModifiedWhen()	{ return m_fileModifiedTicks; }
	LONGLONG GetLONGLONGFileCreatedWhen()	{ return m_fileCreatedTicks; }
	//LONGLONG GetLONGLONGModifiedDate()		{ return m_modifiedTicks; }
	//LONGLONG GetLONGLONGCreatedDate()		{ return m_createdTicks; }
	CComBSTR GetMD5()						{ return m_MD5; }
	CComBSTR GetHash()						{ return m_hash; }
	LONGLONG GetSize()						{ return m_fileSize; }
	int		GetFileVersion()				{ return m_fileVersion; }
	bool	IsFileReadOnly()				{ return m_readOnly; }
	int		GetChangeType()					{ return m_changeType; }

	FILETIME GetModifiedDate();
	FILETIME GetCreatedDate();
	
	bool IsShared() { return m_bShared; }
	bool IsPublished() { return m_bPublished;}

	TrusteesRights GetTrusteesRights()	{ return m_trusteeRights; }
	EnumSyncStatus GetSyncStatus()		{ return m_syncStatus; }

private:
	CComBSTR m_ID;
	CComBSTR m_parentID;
	CComBSTR m_name;
	CComBSTR m_path;
	CComBSTR m_MD5;
	CComBSTR m_hash;
	LONGLONG m_fileModifiedTicks;
	LONGLONG m_fileCreatedTicks;
	//LONGLONG m_modifiedTicks;
	//LONGLONG m_createdTicks;
	LONGLONG m_fileSize;
	int		m_fileVersion;
	bool	m_readOnly;
	int		m_changeType;
	bool	m_bShared;
	bool	m_bPublished;

	FILETIME UnixTimeToFileTime(LONGLONG llTime);

	TrusteesRights m_trusteeRights;
	EnumSyncStatus m_syncStatus;
};
