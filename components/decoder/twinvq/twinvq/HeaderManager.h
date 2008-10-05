/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Modified on 2000.09.06 by N. Iwakami */

// HeaderManager.h: CHeaderManager クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEADERMANAGER_H__E059C7C4_6C3B_11D4_A71A_00C04F012175__INCLUDED_)
#define AFX_HEADERMANAGER_H__E059C7C4_6C3B_11D4_A71A_00C04F012175__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "Chunk.h"


class CHeaderManager  
{
public:
	typedef std::map<std::string, CChunk> CChunkBank;	// チャンクバンク型、IDをキーにチャンクを格納しているマップ
	typedef std::string string;							// 文字列型

private:
	// メンバ変数（クラス）
	CChunkBank m_primaryChunkBank;			// 通常チャンクのチャンクバンク
	CChunkBank m_secondaryChunkBank;		// 補助チャンクのチャンクバンク
	string     m_chunkID;					// TWIN チャンクのID、通常のIDと違い "TWIN"+<バージョン識別子>で構成される。

	// メンバ関数
	CChunk GetChunk( CChunkBank *theBank, string id );						// チャンクバンクからIDでチャンクを引き出す
	void PickUpSubChunks ( CChunkBank* theChunkBank, CChunkChunk *inputChunk );	// Chunk型のチャンクからサブチャンクを取り出しチャンクバンクに登録する
	void Init( CChunkChunk& twinChunk );	// 初期化する。コンストラクタの代わりに使う
	CHeaderManager() { };					// コンストラクタ。ユーザは呼べない。代わりに Create() を使う。
											// 初期化の際にエラーが出る可能性があるためこのような仕様にした。

	// クラスの中だけで使われる例外
	class err_WrongChunkFormat { };		// チャンクの書式が正しくない

public:
	CChunk GetPrimaryChunk( string id )   { return GetChunk( &m_primaryChunkBank,   id ); }; // 通常チャンクを引き出す
	CChunk GetSecondaryChunk( string id ) { return GetChunk( &m_secondaryChunkBank, id ); }; // 補助チャンクを引き出す
	const string  GetID() { return m_chunkID; };

	static CHeaderManager* Create( CChunkChunk& twinChunk );	// チャンクマネージャを作り出す。コンストラクタの代わり
	virtual ~CHeaderManager();	// デストラクタ

	// 例外
	class err_FailGetChunk { };			// チャンクの取得に失敗した

};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ヘッダマネージャからのデータ読み出しの支援クラス
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// CUniStringInfo クラスのインターフェイス
// Unified string information, 文字列チャンクの総合情報、ヘッダマネージャから取得することができる
//
//////////////////////////////////////////////////////////////////////////////////////////////////
class CUniStringInfo
{
public:
	typedef std::string string;
	enum CharCode {
		unknown_code = -1,
		ISO_8859_1,
		Unicode,
		S_JIS,
		JIS,
		EUC,
	};
	
private:
	string m_id;				// チャンク ID
	string m_primary;			// 基本文字列
	string m_secondary;			// 補助文字列
	int m_primaryCharCode;		// 基本文字列の文字コード
	int m_secondaryCharCode;	// 補助文字列の文字コード

	void PutPrimaryInfo( CStringChunk& theChunk );
	void PutSecondaryInfo( CStringChunk& theChunk );

	// 内部で使う例外
	class err_ID { };					// 初期化の際、基本チャンクと補助チャンクの ID が食い違っている
	class err_NoCharCode { };			// 補助チャンクに文字コード情報がない

public:
	const string& GetPrimaryInfo() { return m_primary; };				// 基本文字列を返す
	const string& GetSecondaryInfo() { return m_secondary; };			// 補助文字列を返す
	const int GetPrimaryCharCode() { return m_primaryCharCode; };		// 基本文字列の文字コードを返す
	const int GetSecondaryCharCode() { return m_secondaryCharCode; };	// 補助文字列の文字コードを返す

	CUniStringInfo( string id, string primary, string secondary="", int primCode=unknown_code, int scndCode=unknown_code ); // コンストラクタ、必要な情報を全て与える
	CUniStringInfo( string id, CHeaderManager& theManager );	// コンストラクタ、ヘッダマネージャから読み出す
	virtual ~CUniStringInfo();

	// 例外
	class err_FailConstruction { };		// コンストラクトの失敗
};

template<class T>
T* GetPrimaryChunkFromHeaderManager( std::string id, CHeaderManager& theManager )
{

	try {
		CChunk src = theManager.GetPrimaryChunk(id);
		T* chunk = new T(src);
		return chunk;
	}
	catch ( CHeaderManager::err_FailGetChunk ) {
		return NULL;
	}
}

template<class T>
T* GetSecondaryChunkFromHeaderManager( std::string id, CHeaderManager& theManager )
{

	try {
		CChunk src = theManager.GetSecondaryChunk(id);
		T* chunk = new T(src);
		return chunk;
	}
	catch ( CHeaderManager::err_FailGetChunk ) {
		return NULL;
	}
}


#endif // !defined(AFX_HEADERMANAGER_H__E059C7C4_6C3B_11D4_A71A_00C04F012175__INCLUDED_)
