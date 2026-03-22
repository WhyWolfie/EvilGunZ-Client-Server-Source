#ifndef _MMATCHMAP_H
#define _MMATCHMAP_H

#include "MMatchGlobal.h"


enum MMATCH_MAP 
{
	MMATCH_MAP_MANSION			= 0,
	MMATCH_MAP_PRISON			= 1,
	MMATCH_MAP_STATION			= 2,
	MMATCH_MAP_PRISON_II		= 3,
	MMATCH_MAP_BATTLE_ARENA		= 4,
	MMATCH_MAP_TOWN				= 5,
	MMATCH_MAP_DUNGEON			= 6,
	MMATCH_MAP_RUIN				= 7,
	MMATCH_MAP_ISLAND			= 8,
	MMATCH_MAP_GARDEN			= 9,
	MMATCH_MAP_CASTLE			= 10,
	MMATCH_MAP_FACTORY			= 11,
	MMATCH_MAP_PORT				= 12,
	MMATCH_MAP_LOST_SHRINE		= 13,
	MMATCH_MAP_STAIRWAY			= 14,
	MMATCH_MAP_SNOWTOWN			= 15,
	MMATCH_MAP_HALL				= 16,
	MMATCH_MAP_CATACOMB			= 17,
	MMATCH_MAP_JAIL				= 18,
	MMATCH_MAP_SHOWERROOM		= 19,
	MMATCH_MAP_HIGH_HAVEN		= 20,
	MMATCH_MAP_CITADEL			= 21,
	MMATCH_MAP_WEAPON_SHOP		= 22,
	MMATCH_MAP_Pavilion			= 23,
	// »õ¸Ê Ãß°¡´Â ÀÌ ¹Ø¿¡ ÇÏ¼¼¿ä.

	//	MMATCH_MAP_EVENT,					// ÀÌº¥Æ® Àü¿ë¸Ê
	MMATCH_MAP_RELAYMAP				= 24,
	MMATCH_MAP_RELAYMAP2			= 25,
	MMATCH_MAP_RELAYMAP3			= 26,
	MMATCH_MAP_RELAYMAP4			= 27,
	MMATCH_MAP_RELAYMAP5			= 28,
	MMATCH_MAP_RELAYMAP6			= 29,
	MMATCH_MAP_RELAYMAP7			= 30,
	MMATCH_MAP_RELAYMAP8			= 31,
	MMATCH_MAP_RELAYMAP9			= 32,
	MMATCH_MAP_RELAYMAP10			= 33,

	MMATCH_MAP_RELAYMAP11			= 34,
	MMATCH_MAP_RELAYMAP12			= 35,
	MMATCH_MAP_RELAYMAP13			= 36,
	MMATCH_MAP_RELAYMAP14			= 37,
	MMATCH_MAP_RELAYMAP15			= 38,
	MMATCH_MAP_RELAYMAP16			= 39,
	MMATCH_MAP_RELAYMAP17			= 40,
	MMATCH_MAP_RELAYMAP18			= 41,
	MMATCH_MAP_RELAYMAP19			= 42,
	MMATCH_MAP_RELAYMAP20			= 43,

	MMATCH_MAP_RELAYMAP21			= 44,
	MMATCH_MAP_RELAYMAP22			= 45,
	MMATCH_MAP_RELAYMAP23			= 46,
	MMATCH_MAP_RELAYMAP24			= 47,
	MMATCH_MAP_RELAYMAP25			= 48,
	MMATCH_MAP_RELAYMAP26			= 49,
	MMATCH_MAP_RELAYMAP27			= 50,
	MMATCH_MAP_RELAYMAP28			= 51,
	MMATCH_MAP_RELAYMAP29			= 52,
	MMATCH_MAP_RELAYMAP30			= 53,

	MMATCH_MAP_MAX
};

#define MMATCH_MAP_COUNT	MMATCH_MAP_MAX			// ÀüÃ¼ ¸Ê °¹¼ö

#define MMATCH_MAPNAME_RELAYMAP				"RelayMap"


class MMapDesc
{
private:
	const struct MapInfo
	{
		int			nMapID;							// map id
		char		szMapName[MAPNAME_LENGTH];		// ¸Ê ÀÌ¸§
		char		szMapImageName[MAPNAME_LENGTH];	// ¸Ê ÀÌ¹ÌÁö ÀÌ¸§
		char		szBannerName[MAPNAME_LENGTH];	// º£³Ê ÀÌ¸§
		float		fExpRatio;						// °æÇèÄ¡ ½Àµæ·ü
		int			nMaxPlayers;					// ÃÖ´ë ÀÎ¿ø
		bool		bOnlyDuelMap;					// µà¾ó¸Ê Àü¿ë
		bool		bIsCTFMap;						//isctf?
	};


	// data
	MapInfo	m_MapVectors[MMATCH_MAP_COUNT];
	MMapDesc();
public:
	~MMapDesc() { }
	
	static MMapDesc* GetInstance();

	bool Initialize(const char* szFileName);
	bool Initialize(MZFileSystem* pfs, const char* szFileName);
	bool MIsCorrectMap(const int nMapID);
	

 
	bool IsMapOnlyDuel( const int nMapID);
	bool IsCTFMap( const int nMapID);
	int GetMapID( const int nMapID);
	const char* GetMapName(const int nMapID);
	const char* GetMapImageName(const char* szMapName);
	const char* GetBannerName(const char* szMapName);
	float GetExpRatio( const int nMapID); 
	int GetMaxPlayers( const int nMapID);

	int GetMapCount(){ return MMATCH_MAP_COUNT; }
};


inline MMapDesc* MGetMapDescMgr() 
{ 
	return MMapDesc::GetInstance();
}



#endif