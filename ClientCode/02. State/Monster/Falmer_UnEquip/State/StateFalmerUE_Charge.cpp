#include "framework.h"
#include "StateFalmerUE_Charge.h"

#include "GameInstance.h"

#include "Falmer_UnEquip.h"
#include "Player.h"

CStateFalmerUE_Charge::CStateFalmerUE_Charge()
{
}

HRESULT CStateFalmerUE_Charge::Initialize(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	__super::Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom);

	return S_OK;
}

void CStateFalmerUE_Charge::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pMonster->Get_CurFrameIndex() == 25)
		pGameInstance->PlaySoundFile(TEXT("npc_falmer_attackpower_01.wav"), CHANNEL_MONSTER1, 1.f);


	if (m_pMonster->Get_CurFrameIndex() < 30)
		m_pMonsterTransform->Go_Foward(_fTimeDelta, m_pMonsterNavigation);

	/* 공격 중 서로의 aabb 박스가 충돌하였으면. (피격) */
	if (!strcmp(m_pPlayer->Get_CurAnimationName().c_str(), "1hm_blockidle") ||
		m_pPlayer->Get_CurState() == CPlayer::ONEHAND_ANTICIPATE)
	{
		if (pGameInstance->Collision_Enter(m_pWeaponCollider, m_pPlayerWeaponCollider))
		{
			pGameInstance->PlaySoundFile(TEXT("fx_melee_sword_other_02.wav"), CHANNEL_GUARD, 1.f);

			if (m_pPlayer->Get_IsReadyCounter())
			{
				m_pPlayer->Set_IsCounter(true);
				m_pPlayer->Set_State(CPlayer::ONEHAND_ANTICIPATE);
				m_pPlayer->Play_Animation_All(false, "1hm_blockanticipate");
			}
			else
			{
				m_pPlayer->Set_State(CPlayer::ONEHAND_BLOCKHIT);
				m_pPlayer->Play_Animation_All(false, "1hm_blockhit");
				m_pPlayerTransform->Set_Speed(m_pPlayer->GetWalkSpeed());
			}

			m_pMonsterTransform->Set_Speed(m_pMonster->GetWalkSpeed());
			m_pMonster->Play_Animation(false, "1hmstaggerbackheavy");
			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_STAGGERH);
		}
	}

	else
	{
		if (!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_forwardpowerattack1") &&
			m_pMonster->Get_CurFrameIndex() >= 26 && m_pMonster->Get_CurFrameIndex() <= 30)
		{
			if (pGameInstance->Collision_Enter(m_pWeaponCollider, m_pPlayerBodyCollider))
			{
				pGameInstance->PlaySoundFile(TEXT("wpn_impact_blunt_dirt_01.wav"), CHANNEL_MONSTER1_ATK, 1.f);

				m_pPlayer->SetHp(-20.f);
				m_pPlayer->Set_IsHit(true);
			}
		}
	}

	Safe_Release(pGameInstance);

}

void CStateFalmerUE_Charge::Late_Update(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	/* 동작이 끝났을 때 */
	if (m_pMonster->Get_IsAnimationFin() &&
		!strcmp(m_pMonster->Get_CurAnimationName().c_str(), "1hm_forwardpowerattack1"))
	{
		if (pGameInstance->Collision_Stay(m_pVecCollider[CFalmer_UnEquip::FALMERUE_COL_ATKROUND], m_pPlayerBodyCollider))
		{
			m_pMonsterTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			pGameInstance->PlaySoundFile(TEXT("npc_falmer_attack_01.wav"), CHANNEL_MONSTER1, 1.f);

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_ATK);
			m_pMonster->Play_Animation(false, "1hm_attack1");
		}
		else
		{
			m_pMonsterTransform->Set_Speed(m_pMonster->GetRunSpeed());

			m_pMonster->Set_State(CFalmer_UnEquip::FALMERUE_CHASE);
			m_pMonster->Play_Animation(true, "mtrunforward");
		}
	}

	Safe_Release(pGameInstance);
}

CStateFalmerUE_Charge* CStateFalmerUE_Charge::Create(CGameObject* _pMonster, CGameObject* _pPlayer, CTransform* _pMonsterTransform, CNavigation* _pMonsterNavigation, vector<CCollider*> _pVecColCom)
{
	CStateFalmerUE_Charge* pInstance = new CStateFalmerUE_Charge();

	if (FAILED(pInstance->Initialize(_pMonster, _pPlayer, _pMonsterTransform, _pMonsterNavigation, _pVecColCom)))
	{
		MSG_BOX("Fail Create : CStateFalmerUE_Charge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateFalmerUE_Charge::Free()
{
}

