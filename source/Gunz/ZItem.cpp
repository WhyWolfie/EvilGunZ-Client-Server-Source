#include "stdafx.h"

#include "ZItem.h"
#include "crtdbg.h"

//////////////////////////////////////////////////////////////////////////////
// ZItem /////////////////////////////////////////////////////////////////////
ZItem::ZItem() : MMatchItem()
{
	m_nBulletSpare.Set_MakeCrc(0);
	m_nBulletCurrMagazine.Set_MakeCrc(0);
}

ZItem::~ZItem()
{

}

void ZItem::InitBullet(int nBulletSpare, int nBulletCurrMagazine)
{
	if(m_pDesc == NULL) return;
	if(!m_pDesc->IsSpendableItem() &&
		nBulletSpare < nBulletCurrMagazine)
	{
		nBulletCurrMagazine = nBulletSpare;
	}

	if(GetItemType() == MMIT_RANGE) 
	{		
		m_nBulletCurrMagazine.Set_CheckCrc( nBulletCurrMagazine );
		m_nBulletSpare.Set_CheckCrc( nBulletSpare - nBulletCurrMagazine );
	}
	else if(GetItemType() == MMIT_CUSTOM) 
	{
		m_nBulletCurrMagazine.Set_CheckCrc( nBulletCurrMagazine );
		m_nBulletSpare.Set_CheckCrc( nBulletSpare );
	}
}

bool ZItem::Shot()
{
	if (m_pDesc==NULL) {
//		//_ASSERT(0);
		return false;
	}

	//_ASSERT((GetItemType() == MMIT_RANGE) || 
			((GetItemType() == MMIT_MELEE) ||
			(GetItemType() == MMIT_CUSTOM));//수류탄류..

	if (GetItemType() == MMIT_MELEE)		///< melee이면 그냥 true
	{			
		return true;	
	}	
	else if (GetItemType() == MMIT_CUSTOM)	///< 수류탄이나 기타~구분~
	{	
		if (m_nBulletCurrMagazine.Ref() > 0) 
		{
			m_nBulletCurrMagazine.CheckCrc();
			m_nBulletCurrMagazine.Ref() -= 1;
			m_nBulletCurrMagazine.MakeCrc();
		}
		else return false;	
	}
	else 
	{
		if (m_nBulletCurrMagazine.Ref() > 0) 
		{
			m_nBulletCurrMagazine.CheckCrc();
			m_nBulletCurrMagazine.Ref() -= 1;
			m_nBulletCurrMagazine.MakeCrc();
		}
		else return false;
	}	

	return true;
}

bool ZItem::Reload()
{
	if (m_pDesc == NULL)
	{
		//_ASSERT(0);
		return false;
	}
	//_ASSERT(GetItemType() == MMIT_RANGE);
	

 	int nAddedBullet = m_pDesc->m_nMagazine.Ref() - m_nBulletCurrMagazine.Ref();
	if (nAddedBullet > m_nBulletSpare.Ref()) nAddedBullet = m_nBulletSpare.Ref();
	if (nAddedBullet <= 0) return false;

	m_nBulletCurrMagazine.Set_CheckCrc(m_nBulletCurrMagazine.Ref() + nAddedBullet);
	if(!strstr(strlwr((char*)ZGetGameClient()->GetStageName()), "[button08]"))
	{
		m_nBulletSpare.Set_CheckCrc(m_nBulletSpare.Ref() - nAddedBullet);
	}

	return true;
}

bool ZItem::isReloadable()
{
	if (m_pDesc == NULL) {
		//_ASSERT(0);
		return false;
	}

	if( GetItemType() == MMIT_CUSTOM ) { // 기타 무기들은 재장전이 없다~
		return false;
	}

	if(m_nBulletSpare.Ref() > 0) {
		if(m_pDesc->m_nMagazine.Ref() != m_nBulletCurrMagazine.Ref())
			return true;
	}

	return false;
}

//////////////////////////////////////////////////
// HP 와 AP 데미지 비율 계산 공식~
float ZItem::GetPiercingRatio(MMatchWeaponType wtype, RMeshPartsType partstype)
{
	float fRatio = 0.5f;

	bool bHead = false;

	if(partstype == eq_parts_head) { // 헤드샷 구분~ 
		bHead = true;
	}

	switch(wtype) {
		case MWT_DAGGER:		// 단검
		case MWT_DUAL_DAGGER:	// 양손단검
		case MWT_DAGGER_RARE:		// 단검
		case MWT_DUAL_DAGGER_RARE:	// 양손단검
		case MWT_DAGGER_EXOTIC:		// 단검
		case MWT_DUAL_DAGGER_EXOTIC:	// 양손단검
			{
				if(bHead)	fRatio = 0.75f;
				else		fRatio = 0.7f;
			}
			break;

		case MWT_KATANA:		// 카타나
		case MWT_KATANA_RARE:		// 카타나
		case MWT_KATANA_EXOTIC:		// 카타나
			{
				if(bHead)	fRatio = 0.65f;
				else		fRatio = 0.6f;
			}
			break;

		case MWT_DOUBLE_KATANA:
		case MWT_DOUBLE_KATANA_RARE:
		case MWT_DOUBLE_KATANA_EXOTIC:
			{
				if(bHead)	fRatio = 0.65f;
				else		fRatio = 0.6f;
			}
			break;

		case MWT_GREAT_SWORD:
		case MWT_GREAT_SWORD_RARE:
		case MWT_GREAT_SWORD_EXOTIC:
			{
				if(bHead)	fRatio = 0.65f;
				else		fRatio = 0.6f;
			}
			break;

		case MWT_PISTOL:
		case MWT_PISTOLx2:
		case MWT_PISTOL_RARE:
		case MWT_PISTOLx2_RARE:
		case MWT_PISTOL_EXOTIC:
		case MWT_PISTOLx2_EXOTIC:
			{
				if(bHead)	fRatio = 0.7f;
				else		fRatio = 0.5f;
			}
			break;

		case MWT_REVOLVER:
		case MWT_REVOLVERx2:
		case MWT_REVOLVER_RARE:
		case MWT_REVOLVERx2_RARE:
		case MWT_REVOLVER_EXOTIC:
		case MWT_REVOLVERx2_EXOTIC:
			{
				if(bHead)	fRatio = 0.75f; //0.9
				else		fRatio = 0.55f; //0.7
			}
			break;

		case MWT_SMG:
		case MWT_SMGx2:			// 서브머신건
		case MWT_SMG_RARE:
		case MWT_SMGx2_RARE:			// 서브머신건
		case MWT_SMG_EXOTIC:
		case MWT_SMGx2_EXOTIC:			// 서브머신건
			{
				if(bHead)	fRatio = 0.5f;
				else		fRatio = 0.3f;
			}
			break;

		case MWT_SHOTGUN:	
		case MWT_SAWED_SHOTGUN:
		case MWT_SHOTGUN_RARE:	
		case MWT_SAWED_SHOTGUN_RARE:
		case MWT_SHOTGUN_EXOTIC:	
		case MWT_SAWED_SHOTGUN_EXOTIC:
			{
				if(bHead)	fRatio = 0.2f;
				else		fRatio = 0.2f;
			}
			break;

		case MWT_MACHINEGUN:	// 머신건
		case MWT_MACHINEGUN_RARE:	// 머신건
		case MWT_MACHINEGUN_EXOTIC:	// 머신건
			{
				if(bHead)	fRatio = 0.8f;
				else		fRatio = 0.4f;
			}
			break;

		case MWT_RIFLE:			// 돌격소총
		case MWT_RIFLE_RARE:			// 돌격소총
		case MWT_RIFLE_EXOTIC:			// 돌격소총
			{
				if(bHead)	fRatio = 0.8f;
				else		fRatio = 0.4f;
			}
			break;

		case MWT_SNIFER:		//우선은 라이플처럼...
		case MWT_SNIFER_RARE:		//우선은 라이플처럼...
		case MWT_SNIFER_EXOTIC:		//우선은 라이플처럼...
			{
				if(bHead)	fRatio = 0.8f;
				else		fRatio = 0.4f;
			}
			break;

		case MWT_WFGUN:		//우선은 라이플처럼...
		case MWT_WFGUN_RARE:		//우선은 라이플처럼...
		case MWT_WFGUN_EXOTIC:		//우선은 라이플처럼...
			{
				if(bHead)	fRatio = 0.8f;
				else		fRatio = 0.4f;
			}
			break;

		case MWT_FRAGMENTATION:
		case MWT_FLASH_BANG:
		case MWT_SMOKE_GRENADE:
		case MWT_ROCKET:		// 로켓런쳐
		case MWT_FRAGMENTATION_RARE:
		case MWT_FLASH_BANG_RARE:
		case MWT_SMOKE_GRENADE_RARE:
		case MWT_ROCKET_RARE:		// 로켓런쳐
		case MWT_FRAGMENTATION_EXOTIC:
		case MWT_FLASH_BANG_EXOTIC:
		case MWT_SMOKE_GRENADE_EXOTIC:
		case MWT_ROCKET_EXOTIC:		// 로켓런쳐
			{
				if(bHead)	fRatio = 0.4f;
				else		fRatio = 0.4f;
			}
			break;

		case MWT_TRAP:
		case MWT_TRAP_RARE:
		case MWT_TRAP_EXOTIC:
			{
				// 트랩은 fRatio가 의미가 없고, 트랩이 가진 인챈트 속성의 관통률을 따른다
				fRatio = 0;
			}
			break;

		case MWT_DYNAMITYE:
		case MWT_DYNAMITYE_RARE:
		case MWT_DYNAMITYE_EXOTIC:
			{
				fRatio = 0.4f;
			}
			break;
		default:
			break;
	}

	return fRatio;
}

float ZItem::GetKnockbackForce()
{
	float fKnockbackForce = 0.0f;

	MMatchItemDesc* pDesc = GetDesc();
	if(pDesc) {
		if (pDesc->m_nType.Ref() == MMIT_MELEE) {
			fKnockbackForce = 200.0f;
		}
		else 
			if (pDesc->m_nType.Ref() == MMIT_RANGE) {
				if (pDesc->m_pEffect != NULL)
					fKnockbackForce = (float)(pDesc->m_pEffect->m_nKnockBack);
				else
					fKnockbackForce = 200.0f;
			}
	}
	else {
		//_ASSERT(0);
		return 0;
	}

	return fKnockbackForce;
}

void ZItem::ShiftFugitiveValues()
{
	m_nBulletSpare.ShiftHeapPos_CheckCrc();
	m_nBulletCurrMagazine.ShiftHeapPos_CheckCrc();
}