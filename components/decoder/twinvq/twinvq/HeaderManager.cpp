/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Modified on 2000.09.06 by N. Iwakami */

// HeaderManager.cpp: CHeaderManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include "HeaderManager.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CHeaderManager::~CHeaderManager()
{

}

/*==============================================================================*/
/* Name:        CHeaderManager::create()                                        */
/* Description: ヘッダマネージャを生成する。                                    */
/* Return:      生成したヘッダマネージャへのポインタ、生成に失敗した場合は NULL */
/* Access:      public (static)                                                 */
/*==============================================================================*/
CHeaderManager* CHeaderManager::Create(CChunkChunk &twinChunk)
{
	try {
		// チャンクマネージャを生成する。
		CHeaderManager* theManager = NULL;
		theManager = new CHeaderManager;
		theManager->Init( twinChunk );

		// TWINチャンクのヘッダを取得する
		theManager->m_chunkID = twinChunk.GetID();
		if ( theManager->m_chunkID == "" ) {
			delete theManager;
			return NULL;
		}
		
		return theManager;
	}
	catch (err_WrongChunkFormat) {
		return NULL;
	}
}


/*============================================================================*/
/* Name:        CHeaderManager::Init()                                        */
/* Description: ヘッダマネージャの初期化をする。Create() からのみ呼ばれる。   */
/* Return:      なし                                                          */
/* Access:      external                                                      */
/*============================================================================*/
void CHeaderManager::Init(CChunkChunk &twinChunk)
{
	try {
		// 基本チャンクを基本チャンクバンクに収める。
		PickUpSubChunks( &m_primaryChunkBank, &twinChunk );

		// 補助チャンクがあったら補助チャンクバンクに収める。
		CChunkChunk scndChunk =  GetPrimaryChunk ( "SCND" );
		PickUpSubChunks( &m_secondaryChunkBank, &scndChunk );
	}

	catch (CChunkChunk::err_FailGetChunk){
		//std::cout << "Fail!!" << std::endl;
	}
	catch (err_FailGetChunk) {
		//std::cout << "Fail getting SCND chnunk" << std::endl;
	}

}


/*-------------------------------------------------------------------------------*/
/* Name:        CHeaderManager::PickUpSubChunks()                                */
/* Description: チャンクを入力して、サブチャンクを拾い出しチャンクバンクに預ける */
/* Return:      なし                                                             */
/* Access:      static                                                           */
/*-------------------------------------------------------------------------------*/
void CHeaderManager::PickUpSubChunks(CChunkBank  *theChunkBank, // In/Out: チャンクバンク
									 CChunkChunk *inputChunk)   // Input:  入力チャンク
{
	inputChunk->rewind(); // チャンクを解析する前にまき戻しを行う

	std::string id;
	CChunk *subChunk;
	try {
		while ( (subChunk = inputChunk->GetNextChunk()) ) { // チャンクからサブチャンクを取り出す
			id = subChunk->GetID();
			theChunkBank->insert( CChunkBank::value_type( id, *subChunk ) ); // 取り出したサブチャンクをチャンクバンクに登録
			delete subChunk;
		}
	}

	catch (CChunkChunk::err_FailGetChunk) {
		throw err_WrongChunkFormat();
	}

}


CChunk CHeaderManager::GetChunk(CChunkBank *theChunkBank, string id)
{
	CChunkBank::iterator answer;

	answer = theChunkBank->find( id );		// チャンクのあるなしを問い合わせる。
	if ( answer != theChunkBank->end() ){	// あれば
		return answer->second;				// そのチャンクを戻す。
	}

	throw err_FailGetChunk(); // チャンクがなければ処理を放棄する。

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CUniStringInfo クラスの実装
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CUniStringInfo::CUniStringInfo( string id, string primary, string secondary, int primCode, int scndCode )
{
	m_id = id;
	m_primary = primary;
	m_secondary = secondary;
	m_primaryCharCode   = primCode;
	m_secondaryCharCode = scndCode;
}

CUniStringInfo::CUniStringInfo( string id, CHeaderManager& theManager )
{
	// ID を設定する
	m_id = id;
	m_primaryCharCode = unknown_code;
	m_secondaryCharCode = unknown_code;

	int flag = 0;
	// 基本チャンク情報をコピーする
	try {
		CStringChunk primChunk( theManager.GetPrimaryChunk( m_id ) );
		PutPrimaryInfo( primChunk );
	}
	catch (CHeaderManager::err_FailGetChunk) {
		flag = 1;
	//	throw err_FailConstruction();
	}
	if (flag) throw err_FailConstruction();

	// 補助チャンク情報をコピーする
	try {
		CStringChunk scndChunk( theManager.GetSecondaryChunk( m_id ) );
		PutSecondaryInfo( scndChunk );
	}
	catch (CHeaderManager::err_FailGetChunk) {
	}
	catch (err_NoCharCode) {
		throw err_FailConstruction();
	}
}

CUniStringInfo::~CUniStringInfo()
{

}


void CUniStringInfo::PutPrimaryInfo(CStringChunk& theChunk)
{
	// ID をチェック
	if ( m_id == "" ) {
		m_id = theChunk.GetID();
	}
	else if ( m_id != theChunk.GetID() ) {
		throw err_ID();
	}

	// データを書き込み
	m_primary = theChunk.GetString();
}

void CUniStringInfo::PutSecondaryInfo(CStringChunk& theChunk)
{
	// ID をチェック
	if ( m_id == "" ) {
		m_id = theChunk.GetID();
	}
	else if ( m_id != theChunk.GetID() ) {
		throw err_ID();
	}

	// データを書き込み
	string secondary = theChunk.GetString();
	if ( secondary.size() < 2 ) { // 文字コード情報があるかどうかチェック
		throw err_NoCharCode();
	}

	m_primaryCharCode = secondary[0] - '0';		// 文字コードデータ
	m_secondaryCharCode = secondary[1] - '0';

	m_secondary = secondary.erase(0, 2);

}

