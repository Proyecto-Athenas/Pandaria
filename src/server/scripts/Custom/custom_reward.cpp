#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"

class Played_Time_Reward : public PlayerScript
{
public:
    Played_Time_Reward() : PlayerScript("Played_Time_Reward") { }
    uint32 timeInterval = sWorld->getIntConfig(CONFIG_PLAYED_TIME_REWARD) * 1000;

    void OnUpdate(Player* player, uint32 diff) override
    {
        player->ptr_Interval += diff;
		//ChatHandler(player->GetSession()).PSendSysMessage("time : %u , player: %u ", player->ptr_Interval, player->GetGUIDLow());

        if (player->ptr_Interval >= timeInterval)
        {
            player->ptr_Interval -= timeInterval;
            if (player->IsInWorld() && sWorld->getBoolConfig(CONFIG_BONUS_TIME_REWARD))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Bonus for played time.");

                PreparedStatement* stmt = FusionCMSDatabase.GetPreparedStatement(FUSION_UPD_BATTLEPAY_VP_COINS);
                stmt->setUInt32(0, sConfigMgr->GetIntDefault("PlayedTimeReward.vp", 0));
                stmt->setUInt32(1, player->GetSession()->GetAccountId());
                FusionCMSDatabase.Query(stmt);
            }
        }
    }
};

void AddSC_custom_reward()
{
	new Played_Time_Reward();
}
