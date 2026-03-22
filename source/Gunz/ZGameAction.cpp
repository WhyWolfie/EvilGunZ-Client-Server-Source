#include "stdafx.h"
#include "ZGameAction.h"
#include "ZGame.h"
#include "ZGameClient.h"
#include "ZEffectManager.h"
#include "ZApplication.h"
#include "ZSoundEngine.h"
#include "ZMyCharacter.h"
#include "ZPost.h"
#include "ZModule_FireDamage.h"
#include "ZModule_ColdDamage.h"
#include "ZModule_LightningDamage.h"
#include "ZModule_PoisonDamage.h"

#define MAX_ENCHANT_DURATION	10.f

bool ZGameAction::OnCommand(MCommand* pCommand)
{
	switch (pCommand->GetID())
	{
		HANDLE_COMMAND(MC_PEER_ENCHANT_DAMAGE,		OnEnchantDamage)
		HANDLE_COMMAND(MC_PEER_REACTION,			OnReaction)
		HANDLE_COMMAND(MC_PEER_SKILL,				OnPeerSkill)
	}

	return false;
}

bool ZGameAction::OnReaction(MCommand* pCommand)
{
	float fTime;
	int nReactionID;

	pCommand->GetParameter(&fTime,			0, MPT_FLOAT);		// ½ֳ°£
	pCommand->GetParameter(&nReactionID,	1, MPT_INT);

	ZCharacter *pChar=ZGetCharacterManager()->Find(pCommand->GetSenderUID());
	if(!pChar) return true;

	switch(nReactionID)
	{
		case ZR_CHARGING	:
			{
				if(strstr(strlwr((char*)ZGetGameClient()->GetStageName()), "[button12]"))
	{
			/*pChar->m_bCharging->Set_CheckCrc(true);
			if(!pChar->IsHero())
				pChar->SetAnimationLower(ZC_STATE_CHARGE);
			ZGetEffectManager()->AddChargingEffect(pChar);

			if ( pChar->GetProperty()->nSex == MMS_MALE)
				ZGetSoundEngine()->PlaySound( "fx2/MAL05", pChar->GetPosition());
			else
				ZGetSoundEngine()->PlaySound( "fx2/FEM05", pChar->GetPosition());*/
	}
				else
				{
			pChar->m_bCharging->Set_CheckCrc(true);
			if(!pChar->IsHero())
				pChar->SetAnimationLower(ZC_STATE_CHARGE);
			ZGetEffectManager()->AddChargingEffect(pChar);

			if ( pChar->GetProperty()->nSex == MMS_MALE)
				ZGetSoundEngine()->PlaySound( "fx2/MAL05", pChar->GetPosition());
			else
				ZGetSoundEngine()->PlaySound( "fx2/FEM05", pChar->GetPosition());
		}
			}break;

		case ZR_CHARGED		: 
			{
				if(strstr(strlwr((char*)ZGetGameClient()->GetStageName()), "[button13]"))
	{
			/*pChar->m_bCharged->Set_CheckCrc(true);
			pChar->m_fChargedFreeTime.Set_CheckCrc( ZGetGame()->GetTime() + fTime);
			ZGetEffectManager()->AddChargedEffect(pChar);

			ZGetSoundEngine()->PlaySoundChargeComplete(pChar->GetSelectItemDesc(), pChar->GetPosition());*/
	}
				else
				{
			pChar->m_bCharged->Set_CheckCrc(true);
			pChar->m_fChargedFreeTime.Set_CheckCrc( ZGetGame()->GetTime() + fTime);
			ZGetEffectManager()->AddChargedEffect(pChar);

			ZGetSoundEngine()->PlaySoundChargeComplete(pChar->GetSelectItemDesc(), pChar->GetPosition());
		}
			}break;

		case ZR_BE_UPPERCUT	: {
			rvector tpos = pChar->GetPosition();
			tpos.z += 130.f;
			ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,pChar->GetUID());
			ZGetSoundEngine()->PlaySound("uppercut", tpos);
		}break;
		case ZR_DISCHARGED	: {
			//pChar->m_bCharged=false;
			pChar->m_bCharged->Set_CheckCrc(false);
		}break;
	}

	return true;
}

bool ZGameAction::OnPeerSkill(MCommand* pCommand)
{
	float fTime;
	int nSkill,sel_type;

	pCommand->GetParameter(&fTime, 0, MPT_FLOAT);
	pCommand->GetParameter(&nSkill, 1, MPT_INT);
	pCommand->GetParameter(&sel_type, 2, MPT_INT);

	ZCharacter* pOwnerCharacter = ZGetCharacterManager()->Find(pCommand->GetSenderUID());
	if (pOwnerCharacter == NULL) return true;

	switch(nSkill)	{
		// ¶ç¿ì±â ½ºÅ³
		case ZC_SKILL_UPPERCUT		: OnPeerSkill_Uppercut(pOwnerCharacter);break;
			// °­º£±â ½ºÇÃ·¡½Ã
		case ZC_SKILL_SPLASHSHOT	: OnPeerSkill_LastShot(fTime,pOwnerCharacter);break;
			// ´Ü°Ë Æ¯¼ö°ø°Ý
		case ZC_SKILL_DASH			: OnPeerSkill_Dash(pOwnerCharacter);break;
	}

	return true;
}

// °­º£±â Ã³¸®ÇÑ´Ù. ³»°¡ ¸Â¾Ò´ÂÁö¸¸ °Ë»çÇÑ´Ù
void ZGameAction::OnPeerSkill_LastShot(float fShotTime,ZCharacter *pOwnerCharacter)	// Ä® ¸¶Áö¸· ¹æ ½ºÇÃ·¡½Ã
{
	//jintriple3 µð¹ö±× ·¹Áö½ºÅÍ ÇÙ ¹æ¾î¿ë ºÒ¸®¾È °ª. ¸±¸®Áî ¸ðµå¿¡¼­ °°Àº if¹®Àº code optimize°úÁ¤¿¡¼­ »èÁ¦ÇØ¹ö¸®¹Ç·Î...
	bool bReturnValue;
	//jintriple3 µð¹ö±× ·¹Áö½ºÅÍ ÇÙ ¹æ¾î ÄÚµå...
	bReturnValue = pOwnerCharacter == NULL;
	if( pOwnerCharacter == NULL )
		PROTECT_DEBUG_REGISTER( bReturnValue )
			return;
	ZItem *pItem = pOwnerCharacter->GetItems()->GetItem(MMCIP_MELEE);
	bReturnValue = !pItem;
	if(!pItem) 
		PROTECT_DEBUG_REGISTER( bReturnValue )
			return;

	MMatchItemDesc* pDesc = pItem->GetDesc();
	bReturnValue = !pDesc;
	if(!pDesc) 
		PROTECT_DEBUG_REGISTER( bReturnValue )
			return;


	// fShotTime ÀÌ ±× Ä³¸¯ÅÍÀÇ ·ÎÄÃ ½Ã°£ÀÌ¹Ç·Î ³» ½Ã°£À¸·Î º¯È¯ÇØÁØ´Ù
	fShotTime -= pOwnerCharacter->m_fTimeOffset;
	/*
	float fCurrentTime = g_pGame->GetTime();

	if( abs(fCurrentTime - fShotTime ) > TIME_ERROR_BETWEEN_RECIEVEDTIME_MYTIME )
	{
#ifdef _DEBUG
		mlog("!!!!°­º£±â ÇÙ »ç¿ë!!!!Ä³¸¯ÅÍ ³×ÀÓ: %s      fShotTime : %f     fCurrentTime : %f \n", 
			pOwnerCharacter->GetUserName(), fShotTime - pOwnerCharacter->m_fTimeOffset , fCurrentTime);
#endif
		return;
	}
	ÀÌ ºÎºÐÀº ÇÙ¿¡¼­ shotÀ» ÇÑ ½Ã°£À» Á¶ÀÛÇÏ¿© º¸³»´Â °ÍÀ» °¨ÁöÇÏ¿© ÇÙÀ» ¸·´Â ÄÚµå¿´´Âµ¥ ¹Þ´Â ÂÊ¿¡¼­ ½Ã°£ °Ë»ç¸¦ ÇÏÁö ¸»°í 
	º¸³»´Â ÂÊ¿¡¼­ °Ë»ç¸¦ ÇØ¼­ shotÀ» ÇÑ ½Ã°£ÀÌ ÇØ´ç Ä³¸¯ÅÍÀÇ lacal time°ú ¸ÂÁö ¾ÊÀ¸¸é ¾Æ¿¹ ÆÐÅ¶À» º¸³»Áö ¾Êµµ·Ï ¹Ù²å´Ù. 
	µû¶ó¼­ ÇØ´ç ÄÚµå°¡ ÇÊ¿ä ¾ø°Ô µÊ. ÃßÈÄ localtimeÀ» Á¶ÀÛÇÒ °æ¿ì¸¦ ´ëºñÇØ ÁÖ¼®Ã³¸®·Î ³²°ÜµÒ..
	*/

		// ºñÁ¤»óÀûÀÎ ¹ß»ç¼Óµµ¸¦ ¹«½ÃÇÑ´Ù.
	//jintriple3 µð¹ö±× ·¹Áö½ºÅÍ ÇÙ ¹æ¾î...ÀÌ°Ç °­º£±â ÇÙ ¹æ¾î Àü¿ë..
	//bReturnValue = pOwnerCharacter->CheckValidShotTime(pItem->GetDescID(), fShotTime, pItem);

	if (pOwnerCharacter->CheckValidShotTime(pItem->GetDescID(), fShotTime, pItem))
	{
		PROTECT_DEBUG_REGISTER( pOwnerCharacter->m_dwIsValidTime == FOR_DEBUG_REGISTER )
		{
			pOwnerCharacter->UpdateValidShotTime(pItem->GetDescID(), fShotTime);
		}

	}
	else 
	{
		pOwnerCharacter->UpdateValidShotTime(pItem->GetDescID(), fShotTime);
		// //_ASSERT(FALSE);	//ÇÙ ¹æ¾îÄÚµå...¾î½äÆ®¸¦ »ðÀÔÇØ¼­ ¸»µµ ¾ÈµÇ´Â ¼Óµµ·Î ÀÌ ÇÔ¼ö°¡ ºÒ¸±¶§ °ÔÀÓÀÌ Èûµé¾îÁöµµ·Ï ÇÑ´Ù. 
		return;
	}


	/////////////////////////////////////////////////////////////////////////////////////
//#endif

	const float fRange = 300.f;			// ¹üÀ§´Â 4¹ÌÅÍ

//	if(pOwnerCharacter->m_AniState_Lower>=ZC_STATE_LOWER_ATTACK3 && pOwnerCharacter->m_AniState_Lower<=ZC_STATE_LOWER_ATTACK5)
	{
		// fShotTime ÀÌ ±× Ä³¸¯ÅÍÀÇ ·ÎÄÃ ½Ã°£ÀÌ¹Ç·Î ³» ½Ã°£À¸·Î º¯È¯ÇØÁØ´Ù
//		fShotTime-=pOwnerCharacter->m_fTimeOffset;

		rvector OwnerPosition,OwnerDir;
		bReturnValue = !pOwnerCharacter->GetHistory(&OwnerPosition,&OwnerDir,fShotTime);
		if(!pOwnerCharacter->GetHistory(&OwnerPosition,&OwnerDir,fShotTime))
			PROTECT_DEBUG_REGISTER( bReturnValue )
				return;


		rvector waveCenter = OwnerPosition; // Æø¹ßÀÇ Áß½É

		rvector _vdir = OwnerDir;
		_vdir.z = 0;
//		Normalize(_vdir);
//		waveCenter += _vdir * 180.f;

		ZC_ENCHANT zc_en_type = pOwnerCharacter->GetEnchantType();

		// »ç¿îµå
		ZGetSoundEngine()->PlaySoundSmash(pDesc, waveCenter, pOwnerCharacter->IsObserverTarget());

		// ¹Ù´ÚÀÇ wave ÀÌÆåÆ®
		{
			ZGetEffectManager()->AddSwordWaveEffect(waveCenter,0,pOwnerCharacter);
		}

		for (ZObjectManager::iterator itor = ZGetObjectManager()->begin();
			itor != ZGetObjectManager()->end(); ++itor)
		{
			ZObject* pTar = (*itor).second;
			//jintriple3 µð¹ö±× ·¹Áö½ºÅÍ ÇØÅ· ¹æ¾î ÄÚµå
			bReturnValue = pTar==NULL;
			if (pTar==NULL) 
				PROTECT_DEBUG_REGISTER( bReturnValue )
					continue;
			bReturnValue = pOwnerCharacter == pTar;
			if (pOwnerCharacter == pTar) 
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;
			bReturnValue = pTar!=ZGetGame()->m_pMyCharacter && (!pTar->IsNPC() || !((ZActor*)pTar)->IsMyControl());
			if(pTar!=ZGetGame()->m_pMyCharacter &&	// ³» Ä³¸¯ÅÍ³ª ³»°¡ Á¶Á¾ÇÏ´Â npc ¸¸ Ã¼Å©ÇÑ´Ù
				(!pTar->IsNPC() || !((ZActor*)pTar)->IsMyControl()))
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;

			bReturnValue = !ZGetGame()->CanAttack(pOwnerCharacter, pTar);
			if(!ZGetGame()->CanAttack(pOwnerCharacter,pTar)) 
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;
			//// ÆÀÇÃ·¹ÀÌ°í °°Àº ÆÀÀÌ°í ÆÀÅ³ ºÒ°¡·Î µÇ¾îÀÖÀ¸¸é ³Ñ¾î°£´Ù
			//if(ZGetGame()->GetMatch()->IsTeamPlay() &&
			//	pOwnerCharacter->IsTeam(pTar) && !g_pGame->GetMatch()->GetTeamKillEnabled()) return;

			rvector TargetPosition,TargetDir;
			
			bReturnValue = pTar->IsDie();
			if(pTar->IsDie()) 
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;
			// ÀûÀýÇÑ À§Ä¡¸¦ ¾ò¾î³¾¼ö ¾øÀ¸¸é ´ÙÀ½À¸·Î~
			bReturnValue = !pTar->GetHistory(&TargetPosition, &TargetDir, fShotTime);
			if( !pTar->GetHistory(&TargetPosition,&TargetDir,fShotTime)) 
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;

			rvector checkPosition = TargetPosition + rvector(0,0,80);
			float fDist = Magnitude(waveCenter - checkPosition);
			//jintriple3 µð¹ö±× ·¹Áö½ºÅÍ ÇØÅ· ¹æ¾î ÄÚµå
			bReturnValue = fDist >= fRange;
			if( fDist >= fRange)
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;

			bReturnValue = (!pTar) || (pTar == pOwnerCharacter);
			if( (!pTar) || (pTar == pOwnerCharacter) )
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;

			// Áß°£¿¡ º®ÀÌ ¸·°í ÀÖ´Â°¡?
			bReturnValue = ZGetGame()->CheckWall( pOwnerCharacter, pTar) == true;
			if( ZGetGame()->CheckWall( pOwnerCharacter,pTar ) == true)
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;
			// ¸·°íÀÖÀ¸¸é µ¥¹ÌÁö¸¦ ¾È¹Þ´Â´Ù
			bReturnValue = pTar->IsGuard() && DotProduct(pTar->m_Direction,OwnerDir)<0;
			if( pTar->IsGuard() && DotProduct(pTar->m_Direction,OwnerDir)<0 )
			{
				PROTECT_DEBUG_REGISTER(bReturnValue)
				{
					rvector addVel = pTar->GetPosition() - waveCenter;
					Normalize(addVel);
					addVel = 500.f*addVel;
					addVel.z = 200.f;
					pTar->AddVelocity(addVel);
					continue;
				}
			}
			//¸ðµç Á¶°ÇÀ» Åë°úÇßÀ¸¸é
			rvector tpos = pTar->GetPosition();

			tpos.z += 130.f;
			if( zc_en_type == ZC_ENCHANT_NONE ) 
				ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,pTar->GetUID());
			else 
				ZGetEffectManager()->AddSwordEnchantEffect(zc_en_type,pTar->GetPosition(),20);

			tpos -= pOwnerCharacter->m_Direction * 50.f;
			rvector fTarDir = pTar->GetPosition() - pOwnerCharacter->GetPosition();
			Normalize(fTarDir);

#define MAX_DMG_RANGE	50.f	// ¹Ý°æÀÌ¸¸Å­ ±îÁö´Â ÃÖ´ë µ¥¹ÌÁö¸¦ ´Ù ¸Ô´Â´Ù
#define MIN_DMG			0.3f	// ÃÖ¼Ò ±âº» µ¥¹ÌÁö´Â ÀÌÁ¤µµ.

			float fDamageRange = 1.f - (1.f-MIN_DMG)*( max(fDist-MAX_DMG_RANGE,0) / (fRange-MAX_DMG_RANGE));
//							pTar->OnDamagedKatanaSplash( pOwnerCharacter, fDamageRange );
#define SPLASH_DAMAGE_RATIO	.4f		// ½ºÇÃ·¡½Ã µ¥¹ÌÁö °üÅë·ü
#define SLASH_DAMAGE	3		// °­º£±âµ¥¹ÌÁö = ÀÏ¹Ý°ø°ÝÀÇ x SLASH_DAMAGE

			int damage = (int) pDesc->m_nDamage.Ref()* fDamageRange;

			// ÀÎÃ¦Æ® ¼Ó¼ºÀÌ ÀÖÀ»¶§´Â 1¹è µ¥¹ÌÁö¸¸ ¸Ô´Â´Ù. 2005.1.14
			if(zc_en_type == ZC_ENCHANT_NONE)
				damage *=  SLASH_DAMAGE;

			pTar->OnDamaged(pOwnerCharacter,pOwnerCharacter->GetPosition(),ZD_KATANA_SPLASH,MWT_KATANA,damage,SPLASH_DAMAGE_RATIO);
			pTar->OnDamagedAnimation(pOwnerCharacter,SEM_WomanSlash5);

			ZPostPeerEnchantDamage(pOwnerCharacter->GetUID(), pTar->GetUID());

/*
			if (fDist < fRange) 
			{
				if ((pTar) && (pTar != pOwnerCharacter)) 
				{
					if(g_pGame->CheckWall( pOwnerCharacter,pTar )==false) // Áß°£¿¡ º®ÀÌ ¸·°í ÀÖ´Â°¡?
					{
						// ¸·°íÀÖÀ¸¸é µ¥¹ÌÁö¸¦ ¾È¹Þ´Â´Ù
						if(pTar->IsGuard() && DotProduct(pTar->m_Direction,OwnerDir)<0 )
						{
							rvector addVel = pTar->GetPosition() - waveCenter;
							Normalize(addVel);
							addVel = 500.f*addVel;
							addVel.z = 200.f;
							pTar->AddVelocity(addVel);
						}else
						{
							rvector tpos = pTar->GetPosition();

							tpos.z += 130.f;
							if( zc_en_type == ZC_ENCHANT_NONE ) {
								ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,pTar->GetUID());
							}
							else {
								ZGetEffectManager()->AddSwordEnchantEffect(zc_en_type,pTar->GetPosition(),20);
							}
							tpos -= pOwnerCharacter->m_Direction * 50.f;
							rvector fTarDir = pTar->GetPosition() - pOwnerCharacter->GetPosition();
							Normalize(fTarDir);
#define MAX_DMG_RANGE	50.f	// ¹Ý°æÀÌ¸¸Å­ ±îÁö´Â ÃÖ´ë µ¥¹ÌÁö¸¦ ´Ù ¸Ô´Â´Ù
#define MIN_DMG			0.3f	// ÃÖ¼Ò ±âº» µ¥¹ÌÁö´Â ÀÌÁ¤µµ.
							float fDamageRange = 1.f - (1.f-MIN_DMG)*( max(fDist-MAX_DMG_RANGE,0) / (fRange-MAX_DMG_RANGE));
//							pTar->OnDamagedKatanaSplash( pOwnerCharacter, fDamageRange );
#define SPLASH_DAMAGE_RATIO	.4f		// ½ºÇÃ·¡½Ã µ¥¹ÌÁö °üÅë·ü
#define SLASH_DAMAGE	3		// °­º£±âµ¥¹ÌÁö = ÀÏ¹Ý°ø°ÝÀÇ x SLASH_DAMAGE
							int damage = (int) pDesc->m_nDamage * fDamageRange;
							// ÀÎÃ¦Æ® ¼Ó¼ºÀÌ ÀÖÀ»¶§´Â 1¹è µ¥¹ÌÁö¸¸ ¸Ô´Â´Ù. 2005.1.14
							if(zc_en_type == ZC_ENCHANT_NONE)
								damage *=  SLASH_DAMAGE;
							pTar->OnDamaged(pOwnerCharacter,pOwnerCharacter->GetPosition(),ZD_KATANA_SPLASH,MWT_KATANA,damage,SPLASH_DAMAGE_RATIO);
							pTar->OnDamagedAnimation(pOwnerCharacter,SEM_WomanSlash5);
							ZPostPeerEnchantDamage(pOwnerCharacter->GetUID(), pTar->GetUID());
						} // µ¥¹ÌÁö¸¦ ¸Ô´Â´Ù
					}
				}
			}*/
		}
#define KATANA_SHOCK_RANGE		1000.f			// 10¹ÌÅÍ±îÁö Èçµé¸°´Ù

		float fPower= (KATANA_SHOCK_RANGE-Magnitude(ZGetGame()->m_pMyCharacter->GetPosition()+rvector(0,0,50) - OwnerPosition))/KATANA_SHOCK_RANGE;
		if(fPower>0)
			ZGetGameInterface()->GetCamera()->Shock(fPower*500.f, .5f, rvector(0.0f, 0.0f, -1.0f));

	}
	/*
	else{

#ifndef _PUBLISH

		// ÀÌ°Å Ä®Áú Á¦´ë·Î ¾ÈÇÑ³ÑÀÌ´Ù. ¼ö»óÇÏ´Ù.
		char szTemp[256];
		sprintf(szTemp, "%s Ä¡Æ® ?", pOwnerCharacter->GetProperty()->szName);
		ZChatOutput(MCOLOR(0xFFFF0000), szTemp);

		mlog("anistate %d\n",pOwnerCharacter->m_AniState_Lower);

#endif//_PUBLISH

	}
	*/
}

void ZGameAction::OnPeerSkill_Uppercut(ZCharacter *pOwnerCharacter)
{
	float fShotTime=ZGetGame()->GetTime();
	rvector OwnerPosition,OwnerDir;
	OwnerPosition = pOwnerCharacter->GetPosition();
	OwnerDir = pOwnerCharacter->m_Direction;
	OwnerDir.z=0; 
	Normalize(OwnerDir);


	if ( !pOwnerCharacter->IsNPC())
	{
		if ( pOwnerCharacter->GetProperty()->nSex == MMS_MALE)
			ZGetSoundEngine()->PlaySound( "fx2/MAL_shot_01", pOwnerCharacter->GetPosition());
		else
			ZGetSoundEngine()->PlaySound( "fx2/FEM_shot_01", pOwnerCharacter->GetPosition());
	}


	for (ZObjectManager::iterator itor = ZGetObjectManager()->begin();
		itor != ZGetObjectManager()->end(); ++itor)
	{
		ZObject* pTar = (*itor).second;
		if (pOwnerCharacter == pTar) continue;

		rvector TargetPosition,TargetDir;

		if(pTar->IsDie()) continue;
		// ÀûÀýÇÑ À§Ä¡¸¦ ¾ò¾î³¾¼ö ¾øÀ¸¸é ´ÙÀ½À¸·Î~
		if( !pTar->GetHistory(&TargetPosition,&TargetDir,fShotTime)) continue;

		float fDist = Magnitude(OwnerPosition + OwnerDir*10.f - TargetPosition);

		if (fDist < 200.0f) {

			if ((pTar) && (pTar != pOwnerCharacter))
			{
				bool bCheck = false;

				if (ZGetGame()->GetMatch()->IsTeamPlay())
				{
					if (IsPlayerObject(pTar)) {
						if( pOwnerCharacter->IsTeam( (ZCharacter*)pTar ) == false){
							bCheck = true;
						}
					}
					else {
						bCheck = true;
					}
				}
				else if (ZGetGame()->GetMatch()->IsQuestDrived())
				{
					if (!IsPlayerObject(pTar)) bCheck = true;
				}
				else {
					bCheck = true;
				}

				if(ZGetGame()->CheckWall(pOwnerCharacter,pTar)==true) //Áß°£¿¡ º®ÀÌ ¸·°í ÀÖ´Â°¡?
					bCheck = false;

				if( bCheck) {//ÆÀÀÌ¾Æ´Ñ°æ¿ì¸¸

					rvector fTarDir = pTar->GetPosition() - (pOwnerCharacter->GetPosition() - 50.f*OwnerDir);
					Normalize(fTarDir);
					float fDot = D3DXVec3Dot(&OwnerDir, &fTarDir);
					if (fDot>0)
					{
						int cm = ZGetGame()->SelectSlashEffectMotion(pOwnerCharacter);//³²³à Ä® ÈÖµÎ¸£´Â ¹æÇâ

						rvector tpos = pTar->GetPosition();

						tpos.z += 130.f;

						/*
						if (IsPlayerObject(pTar))
						{
							// ¿ì¼± ÇÃ·¹ÀÌ¾î¸¸ ÀÌÆåÆ®°¡ ³ª¿Â´Ù. - effect ´Ù Á¤¸®ÇÏ°í NPCµµ ³ª¿À°Ô ¹Ù²î¾î¾ß ÇÔ -bird
							ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,(ZCharacter*)pTar);
						}
						*/

						tpos -= pOwnerCharacter->m_Direction * 50.f;

						ZGetEffectManager()->AddBloodEffect( tpos , -fTarDir);
						ZGetEffectManager()->AddSlashEffect( tpos , -fTarDir , cm );

						ZGetGame()->CheckCombo(pOwnerCharacter, pTar , true);
						if (pTar == ZGetGame()->m_pMyCharacter) 
						{
							ZGetGame()->m_pMyCharacter->SetLastThrower(pOwnerCharacter->GetUID(), ZGetGame()->GetTime()+1.0f);
							ZPostReaction(ZGetGame()->GetTime(),ZR_BE_UPPERCUT);
						}
						pTar->OnBlast(OwnerDir);

						ZCharacter* pTarCharacter = (ZCharacter*)pTar;
						if ( !pTarCharacter->IsNPC())
						{
							if ( ((ZCharacter*)pTar)->GetProperty()->nSex == MMS_MALE)
								ZGetSoundEngine()->PlaySound( "fx2/MAL07", pTar->GetPosition());
							else
								ZGetSoundEngine()->PlaySound( "fx2/FEM07", pTar->GetPosition());
						}
					}
				}
			}
		}
	}
}

void ZGameAction::OnPeerSkill_Dash(ZCharacter *pOwnerCharacter)
{
	if(pOwnerCharacter->m_AniState_Lower.Ref()!=ZC_STATE_LOWER_UPPERCUT) return;

	float fShotTime=ZGetGame()->GetTime();
	rvector OwnerPosition,OwnerDir;
	OwnerPosition = pOwnerCharacter->GetPosition();
	OwnerDir = pOwnerCharacter->m_Direction;
	OwnerDir.z=0; 
	Normalize(OwnerDir);

	ZItem *pItem = pOwnerCharacter->GetItems()->GetItem(MMCIP_MELEE);
	if(!pItem) return;
	MMatchItemDesc *pDesc = pItem->GetDesc();
	if(!pDesc) { return; }

//	ZGetEffectManager()->AddSkillDashEffect(pOwnerCharacter->GetPosition(),pOwnerCharacter->m_Direction,pOwnerCharacter);

//	for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin();
//		itor != ZGetCharacterManager()->end(); ++itor)
	for (ZObjectManager::iterator itor = ZGetObjectManager()->begin();
		itor != ZGetObjectManager()->end(); ++itor)
	{
//		ZCharacter* pTar = (*itor).second;
		ZObject* pTar = (*itor).second;

		if (pOwnerCharacter == pTar) continue;

		rvector TargetPosition,TargetDir;

		if(pTar->IsDie()) continue;

		// ÀûÀýÇÑ À§Ä¡¸¦ ¾ò¾î³¾¼ö ¾øÀ¸¸é ´ÙÀ½À¸·Î~
		if( !pTar->GetHistory(&TargetPosition,&TargetDir,fShotTime)) continue;

		float fDist = Magnitude(OwnerPosition + OwnerDir*10.f - TargetPosition);

		if (fDist < 600.0f) {// 6m

			if ((pTar) && (pTar != pOwnerCharacter)) {

				bool bCheck = false;
/*
				if (ZGetGame()->GetMatch()->IsTeamPlay()){
					if( pOwnerCharacter->IsTeam( pTar ) == false){
						bCheck = true;
					}
				}
				else {
					bCheck = true;
				}
*/
				if (ZGetGame()->GetMatch()->IsTeamPlay()){
					if (IsPlayerObject(pTar)) {
						if( pOwnerCharacter->IsTeam( (ZCharacter*)pTar ) == false){
							bCheck = true;
						}
					}
					else {
						bCheck = true;
					}
				}
				else {
					bCheck = true;
				}

				if(ZGetGame()->CheckWall(pOwnerCharacter,pTar)==true) //Áß°£¿¡ º®ÀÌ ¸·°í ÀÖ´Â°¡?
					bCheck = false;

				if( bCheck) {//ÆÀÀÌ¾Æ´Ñ°æ¿ì¸¸
					//				if( pOwnerCharacter->IsTeam( pTar ) == false) {//ÆÀÀÌ¾Æ´Ñ°æ¿ì¸¸

					rvector fTarDir = pTar->GetPosition() - pOwnerCharacter->GetPosition();
					Normalize(fTarDir);

					float fDot = D3DXVec3Dot(&OwnerDir, &fTarDir);

					bool bDamage = false;

					if( fDist < 100.f) { // 1m ¾ÈÂÊÀº ¾Õ¿¡¸¸ ÀÖ¾îµµ..
						if(fDot > 0.f) {
							bDamage = true;
						}
					}
					else if(fDist < 300.f) {
						if(fDot > 0.5f) {
							bDamage = true;
						}
					}
					else {// 2m ~ 6m
						if(fDot > 0.96f) {
							bDamage = true;
						}
					}

					if ( bDamage ) {

						int cm = ZGetGame()->SelectSlashEffectMotion(pOwnerCharacter);//³²³à Ä® ÈÖµÎ¸£´Â ¹æÇâ

						float add_time = 0.3f * (fDist / 600.f);
						float time = ZGetGame()->GetTime() + add_time;			// °Å¸®¿¡ µû¶ó¼­ ½Ã°£À» ´Þ¸®ÇÏµµ·Ï ¼öÁ¤ÇÏ±â..

						rvector tpos = pTar->GetPosition();

						tpos.z += 180.f;//Á»´õ ³ôÀÓ

						ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,pTar->GetUID(),(DWORD)(add_time*1000));
//						ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,pTar);

						tpos -= pOwnerCharacter->m_Direction * 50.f;

//						ZGetEffectManager()->AddBloodEffect( tpos , -fTarDir);
//						ZGetEffectManager()->AddSlashEffect( tpos , -fTarDir , cm );
						// ¼Ò¸®µµ Æ¯Á¤ ½Ã°£ µÚ¿¡
						ZGetSoundEngine()->PlaySound("uppercut", tpos );

						if (pTar == ZGetGame()->m_pMyCharacter) {
							rvector _dir = pTar->GetPosition() - pOwnerCharacter->GetPosition();
							_dir.z = 0.f;

//							m_pMyCharacter->OnDashAttacked(_dir);
							ZGetGame()->m_pMyCharacter->ReserveDashAttacked( pOwnerCharacter->GetUID(), time,_dir );
						}
						pTar->OnBlastDagger(OwnerDir,OwnerPosition);

						float fDamage = pDesc->m_nDamage.Ref() * 1.5f;// ±âº» ¹«±âµ¥¹ÌÁö * 150 %
						float fRatio = pItem->GetPiercingRatio( pDesc->m_nWeaponType.Ref() , eq_parts_chest );

						if(ZGetGame()->CanAttack(pOwnerCharacter,pTar))//°ø°Ý °¡´ÉÇÑ °æ¿ì¸¸.. Äù½ºÆ®ÀÇ °æ¿ì µ¥¹ÌÁö´Â ¾ø´Ù..
							pTar->OnDamagedSkill(pOwnerCharacter,pOwnerCharacter->GetPosition(),ZD_MELEE,MWT_DAGGER,MWT_DAGGER_RARE,fDamage,fRatio);

						ZGetGame()->CheckCombo(pOwnerCharacter, pTar,true);
					}

				}//IsTeam
			}
		}
	}
}


bool ZGameAction::OnEnchantDamage(MCommand* pCommand)
{
	MUID ownerUID;
	MUID targetUID;
	pCommand->GetParameter(&ownerUID,	0, MPT_UID);
	pCommand->GetParameter(&targetUID,	1, MPT_UID);

	ZCharacter* pOwnerCharacter = ZGetCharacterManager()->Find(ownerUID);
	ZObject* pTarget= ZGetObjectManager()->GetObject(targetUID);

	if (pOwnerCharacter == NULL || pTarget == NULL ) return true;

	MMatchItemDesc* pDesc = pOwnerCharacter->GetEnchantItemDesc();
	if(pDesc)
	{
		switch(pOwnerCharacter->GetEnchantType())
		{
			case ZC_ENCHANT_FIRE :
				ApplyFireEnchantDamage(pTarget, pOwnerCharacter, pDesc->m_nDamage.Ref(), pDesc->m_nDelay.Ref());
				break;
			case ZC_ENCHANT_COLD :
				ApplyColdEnchantDamage(pTarget, pDesc->m_nLimitSpeed.Ref(), pDesc->m_nDelay.Ref());
				break;
			case ZC_ENCHANT_POISON :
				ApplyPoisonEnchantDamage(pTarget, pOwnerCharacter, pDesc->m_nDamage.Ref(), pDesc->m_nDelay.Ref());
				break;
			case ZC_ENCHANT_LIGHTNING : 
				ApplyLightningEnchantDamage(pTarget, pOwnerCharacter, pDesc->m_nDamage.Ref(), pDesc->m_nDelay.Ref());
				break;
		};
	}

	return true;
}

bool ZGameAction::ApplyFireEnchantDamage(ZObject* pTarget, ZObject* pOwner, int nDamage, int nDuration)
{
	if (!pTarget) return false;

	ZModule_FireDamage *pMod = (ZModule_FireDamage*)pTarget->GetModule(ZMID_FIREDAMAGE);
	if(!pMod) return false;

	// ÇÑ¹ø °É¸®¸é ÀÏÁ¤ ½Ã°£ µ¿¾ÈÀº ´Ù½Ã °É¸®Áö ¾Ê°Ô
	if (pMod->IsOnDamage() && (ZGetGame()->GetTime() - pMod->GetDamageBeginTime() < 1.f))
		return false;

	rvector soundPos = pTarget->GetPosition();
	bool bObserverTarget = pTarget->GetUID()==ZGetCombatInterface()->GetTargetUID();
	char* szSoundName = (bObserverTarget) ? "we_enfire_2d" : "we_enfire";
	ZGetSoundEngine()->PlaySound(szSoundName, soundPos);

	pMod->BeginDamage( pOwner, nDamage, nDuration * 0.001f);
	return true;
}

bool ZGameAction::ApplyColdEnchantDamage(ZObject* pTarget, int nLimitSpeed, int nDuration)
{
	if (!pTarget) return false;

	ZModule_ColdDamage *pMod = (ZModule_ColdDamage*)pTarget->GetModule(ZMID_COLDDAMAGE);
	if(!pMod) return false;

	// ÇÑ¹ø °É¸®¸é ÀÏÁ¤ ½Ã°£ µ¿¾ÈÀº ´Ù½Ã °É¸®Áö ¾Ê°Ô
	if (pMod->IsOnDamage() && (ZGetGame()->GetTime() - pMod->GetDamageBeginTime() < 1.f))
		return false;

	rvector soundPos = pTarget->GetPosition();
	bool bObserverTarget = pTarget->GetUID()==ZGetCombatInterface()->GetTargetUID();
	char* szSoundName = (bObserverTarget) ? "we_enice_2d" : "we_enice";
	ZGetSoundEngine()->PlaySound(szSoundName, soundPos);

	if (nLimitSpeed < 0 || 100 < nLimitSpeed) return false;
	pMod->BeginDamage( (float)nLimitSpeed*0.01f, nDuration * 0.001f);
	return true;
}

bool ZGameAction::ApplyPoisonEnchantDamage(ZObject* pTarget, ZObject* pOwner, int nDamage, int nDuration)
{
	if (!pTarget) return false;

	ZModule_PoisonDamage *pMod = (ZModule_PoisonDamage*)pTarget->GetModule(ZMID_POISONDAMAGE);
	if(!pMod) return false;

	// ÇÑ¹ø °É¸®¸é ÀÏÁ¤ ½Ã°£ µ¿¾ÈÀº ´Ù½Ã °É¸®Áö ¾Ê°Ô
	if (pMod->IsOnDamage() && (ZGetGame()->GetTime() - pMod->GetDamageBeginTime() < 1.f))
		return false;

	rvector soundPos = pTarget->GetPosition();
	bool bObserverTarget = pTarget->GetUID()==ZGetCombatInterface()->GetTargetUID();
	char* szSoundName = (bObserverTarget) ? "we_enpoison_2d" : "we_enpoison";
	ZGetSoundEngine()->PlaySound(szSoundName, soundPos);

	pMod->BeginDamage( pOwner, nDamage, nDuration * 0.001f);
	return true;
}

bool ZGameAction::ApplyLightningEnchantDamage(ZObject* pTarget, ZObject* pOwner, int nDamage, int nDuration)
{
	if (!pTarget) return false;

	ZModule_LightningDamage *pMod = (ZModule_LightningDamage*)pTarget->GetModule(ZMID_LIGHTNINGDAMAGE);
	if(!pMod) return false;

	// ÇÑ¹ø °É¸®¸é ÀÏÁ¤ ½Ã°£ µ¿¾ÈÀº ´Ù½Ã °É¸®Áö ¾Ê°Ô
	if (pMod->IsOnDamage() && (ZGetGame()->GetTime() - pMod->GetDamageBeginTime() < 1.f))
		return false;

	rvector soundPos = pTarget->GetPosition();
	bool bObserverTarget = pTarget->GetUID()==ZGetCombatInterface()->GetTargetUID();
	char* szSoundName = (bObserverTarget) ? "we_enlight_2d" : "we_enlight";
	ZGetSoundEngine()->PlaySound(szSoundName, soundPos);

	pMod->BeginDamage( pOwner, nDamage, nDuration * 0.001f);
	return true;
}