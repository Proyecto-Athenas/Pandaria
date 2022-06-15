#include "Chat.h"
#include "DatabaseEnv.h"
#include "GridNotifiersImpl.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ServiceMgr.h"
#include <unordered_map>

typedef std::unordered_map<uint8, std::unordered_map<uint8, std::unordered_map<uint16, std::list<uint32>>>> ItemMap; // type, class_id, spec_id or 0, item_id

class LevelUpData
{
public:
    static LevelUpData* instance()
    {
        static LevelUpData instance;
        return &instance;
    }

    void Load()
    {
        TC_LOG_INFO("server.loading", "Loading level up data...");

        auto oldTime = getMSTime();
        auto result = WorldDatabase.PQuery("SELECT `type`, `class_id`, `spec_id`, `item_id` FROM `level_up_item`");
        if (!result)
            return;

        do
        {
            auto fields = result->Fetch();
            _itemMap[fields[0].GetUInt8()][fields[1].GetUInt8()][fields[2].GetUInt16()].push_back(fields[3].GetUInt32());
        }
        while (result->NextRow());

        TC_LOG_INFO("server.loading", ">> Loaded level up items %u ms", GetMSTimeDiffToNow(oldTime));
    }

    std::list<uint32> GetItemForClassSpec(uint8 const &typeLevelUp, uint8 const &classId, uint16 const& specId)
	{
		auto emptyList = std::list<uint32>();
		auto typeLevelUpMap = _itemMap.find(typeLevelUp);
		if (typeLevelUpMap == _itemMap.end())
			return emptyList;

		auto classMap = typeLevelUpMap->second.find(classId);
		if (classMap == typeLevelUpMap->second.end())
			return emptyList;

		auto specMap = classMap->second.find(specId);
		if (specMap == classMap->second.end())
			specMap = classMap->second.find(CLASS_NONE);

		if (specMap == classMap->second.end())
			return emptyList;

		return specMap->second;
	}

private:
    LevelUpData() {}
    ~LevelUpData() {}

    ItemMap _itemMap;
};

enum typeboost // id items
{
    type_1 = 110102,  // PvP-Ali
    type_2 = 110103,  // PvE
    type_3 = 110104   // PvP-Horde
};

class Level_Up : public ItemScript
{
public:
	Level_Up() : ItemScript("Level_Up") {}

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {

        player->PlayerTalkClass->ClearMenus();

        if (player->IsInCombat() || player->InArena() || player->InBattleground()) //Item is not usable in combat, arenas and battlegrounds. This can be modified to your taste.
        {
            player->GetSession()->SendNotification("You may not use this token whilst you are in combat or present in an arena or battleground.");
        }
        else if(player->HasItemCount(item->GetEntry(), 1, true)) //verify that the characters have the item
        {
            switch (player->getClass())
            {
                case CLASS_WARRIOR:
                    player->ADD_GOSSIP_ITEM_DB(65142, 0, GOSSIP_SENDER_MAIN, SPEC_WARRIOR_ARMS);
                    player->ADD_GOSSIP_ITEM_DB(65142, 1, GOSSIP_SENDER_MAIN, SPEC_WARRIOR_FURY);
                    player->ADD_GOSSIP_ITEM_DB(65142, 2, GOSSIP_SENDER_MAIN, SPEC_WARRIOR_PROTECTION);
                    break;
                case CLASS_PALADIN:
                    player->ADD_GOSSIP_ITEM_DB(65142, 3, GOSSIP_SENDER_MAIN, SPEC_PALADIN_HOLY);
                    player->ADD_GOSSIP_ITEM_DB(65142, 4, GOSSIP_SENDER_MAIN, SPEC_PALADIN_PROTECTION);
                    player->ADD_GOSSIP_ITEM_DB(65142, 5, GOSSIP_SENDER_MAIN, SPEC_PALADIN_RETRIBUTION);
                    break;
                case CLASS_HUNTER:
                    //player->ADD_GOSSIP_ITEM_DB(65142, 6, GOSSIP_SENDER_MAIN, SPEC_HUNTER_BEAST_MASTERY);
                    player->ADD_GOSSIP_ITEM_DB(65142, 7, GOSSIP_SENDER_MAIN, SPEC_HUNTER_MARKSMANSHIP);
                    //player->ADD_GOSSIP_ITEM_DB(65142, 8, GOSSIP_SENDER_MAIN, SPEC_HUNTER_SURVIVAL);
                    break;
                case CLASS_ROGUE:
                    player->ADD_GOSSIP_ITEM_DB(65142, 9, GOSSIP_SENDER_MAIN, SPEC_ROGUE_ASSASSINATION);
                    //player->ADD_GOSSIP_ITEM_DB(65142, 10, GOSSIP_SENDER_MAIN, SPEC_ROGUE_COMBAT);
                    //player->ADD_GOSSIP_ITEM_DB(65142, 11, GOSSIP_SENDER_MAIN, SPEC_ROGUE_SUBTLETY);
                    break;
                case CLASS_PRIEST:
                    player->ADD_GOSSIP_ITEM_DB(65142, 12, GOSSIP_SENDER_MAIN, SPEC_PRIEST_DISCIPLINE);
                    //player->ADD_GOSSIP_ITEM_DB(65142, 13, GOSSIP_SENDER_MAIN, SPEC_PRIEST_HOLY);
                    player->ADD_GOSSIP_ITEM_DB(65142, 14, GOSSIP_SENDER_MAIN, SPEC_PRIEST_SHADOW);
                    break;
                case CLASS_DEATH_KNIGHT:
                    player->ADD_GOSSIP_ITEM_DB(65142, 15, GOSSIP_SENDER_MAIN, SPEC_DEATH_KNIGHT_BLOOD);
                    player->ADD_GOSSIP_ITEM_DB(65142, 16, GOSSIP_SENDER_MAIN, SPEC_DEATH_KNIGHT_FROST);
                    //player->ADD_GOSSIP_ITEM_DB(65142, 17, GOSSIP_SENDER_MAIN, SPEC_DEATH_KNIGHT_UNHOLY);
                    break;
                case CLASS_SHAMAN:
                    player->ADD_GOSSIP_ITEM_DB(65142, 18, GOSSIP_SENDER_MAIN, SPEC_SHAMAN_ELEMENTAL);
                    player->ADD_GOSSIP_ITEM_DB(65142, 19, GOSSIP_SENDER_MAIN, SPEC_SHAMAN_ENHANCEMENT);
                    player->ADD_GOSSIP_ITEM_DB(65142, 20, GOSSIP_SENDER_MAIN, SPEC_SHAMAN_RESTORATION);
                    break;
                case CLASS_MAGE:
                    player->ADD_GOSSIP_ITEM_DB(65142, 21, GOSSIP_SENDER_MAIN, SPEC_MAGE_ARCANE);
                    //player->ADD_GOSSIP_ITEM_DB(65142, 22, GOSSIP_SENDER_MAIN, SPEC_MAGE_FIRE);
                    //player->ADD_GOSSIP_ITEM_DB(65142, 23, GOSSIP_SENDER_MAIN, SPEC_MAGE_FROST);
                    break;
                case CLASS_WARLOCK:
                    player->ADD_GOSSIP_ITEM_DB(65142, 24, GOSSIP_SENDER_MAIN, SPEC_WARLOCK_AFFLICTION);
                    //player->ADD_GOSSIP_ITEM_DB(65142, 25, GOSSIP_SENDER_MAIN, SPEC_WARLOCK_DEMONOLOGY);
                    //player->ADD_GOSSIP_ITEM_DB(65142, 26, GOSSIP_SENDER_MAIN, SPEC_WARLOCK_DESTRUCTION);
                    break;
                case CLASS_MONK:
                    player->ADD_GOSSIP_ITEM_DB(65142, 27, GOSSIP_SENDER_MAIN, SPEC_MONK_BREWMASTER);
                    player->ADD_GOSSIP_ITEM_DB(65142, 28, GOSSIP_SENDER_MAIN, SPEC_MONK_WINDWALKER);
                    player->ADD_GOSSIP_ITEM_DB(65142, 29, GOSSIP_SENDER_MAIN, SPEC_MONK_MISTWEAVER);
                    break;
                case CLASS_DRUID:
                    player->ADD_GOSSIP_ITEM_DB(65142, 30, GOSSIP_SENDER_SEC_BANK, SPEC_DRUID_FERAL);
                    player->ADD_GOSSIP_ITEM_DB(65142, 31, GOSSIP_SENDER_SEC_BANK, SPEC_DRUID_GUARDIAN);
                    player->ADD_GOSSIP_ITEM_DB(65142, 32, GOSSIP_SENDER_SEC_BANK, SPEC_DRUID_RESTORATION);
                    player->ADD_GOSSIP_ITEM_DB(65142, 33, GOSSIP_SENDER_SEC_BANK, SPEC_DRUID_BALANCE);
                    break;
                default:
                    break;
            }
            //player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bye", GOSSIP_SENDER_MAIN, 1);

            player->SEND_GOSSIP_MENU(724017, item->GetGUID());
        }
        else
        {
            ChatHandler(player->GetSession()).SendSysMessage("You do not have the necessary token.");
        }
        return true;
    }

    void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        uint8 typeLevelUpTemp = 1; // normal boost PvP
        switch (item->GetEntry())
        {
        case type_1:
            typeLevelUpTemp = 1;
            break;
		case type_2:
			typeLevelUpTemp = 2;
			break;
		case type_3:
			typeLevelUpTemp = 3;
			break;
        default:
            break;
        }

		auto items = LevelUpData::instance()->GetItemForClassSpec(typeLevelUpTemp, player->getClass(), action);
        //ChatHandler(player->GetSession()).PSendSysMessage("type : %u , player class: %u and spec: %u", typeLevelUpTemp, player->getClass(), action);

		SQLTransaction trans = CharacterDatabase.BeginTransaction();

		while (!items.empty())
		{
			MailSender mail(MAIL_NORMAL, 0, MAIL_STATIONERY_GM);
			MailDraft draft("Level UP items", "Thank you for supporting Athenas WoW Server!");

			for (uint8 i = 0; !items.empty() && i < MAX_MAIL_ITEMS; ++i)
			{
				if (Item* item = Item::CreateItem(items.front(), 1, player))
				{
					item->SaveToDB(trans);
					draft.AddItem(item);
					//ChatHandler(player->GetSession()).PSendSysMessage("item: %u", items.front());
				}
				items.pop_front();
			}
			draft.SendMailTo(trans, MailReceiver(player, player->GetGUIDLow()), mail);
		}

		CharacterDatabase.CommitTransaction(trans);

		ChatHandler(player->GetSession()).PSendSysMessage("Check your mailbox!");

        //Item is destroyed on useage.
        player->DestroyItemCount(item->GetEntry(), 1, true);
        //save pj
        player->SaveToDB();

        player->CLOSE_GOSSIP_MENU();
    }
};

class LevelUpLoader : public WorldScript
{
    public:
        LevelUpLoader() : WorldScript("LevelUpLoader") {}

        void OnStartup() override
        {
			LevelUpData::instance()->Load();
        }
};

enum flyingspell
{
    SPELL_ARTISAN_RIDING        = 34091,
    SPELL_COLD_WHEATHER_FLYING  = 54197,
    SPELL_FLIGHT_MASTER_LICENSE = 90267,
    SPELL_WISDOM_OF_FOUR_WINDS  = 115913
};

class npc_Level_Up : public CreatureScript
{
public:
    npc_Level_Up() : CreatureScript("npc_Level_Up") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (player->IsInCombat())
        {
            player->GetSession()->SendNotification("You may not use this token whilst you are in combat.");
        }
        else
        {
			if (player->GetTeam() == ALLIANCE)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Obtener mi Item Boost - PvP(Aliance)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            }
			else
			{
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Obtener mi Item Boost - PvP(Horde)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            }
            
            //player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Obtener mi Item Boost - PvE", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Solo 1 por IP", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
			//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Solo para cuentas NUEVAS", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Todo queda registrado, evita el BAN", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bye", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);

            player->SEND_GOSSIP_MENU(724017, creature->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        bool tele_panda = false;
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF + 1 || action == GOSSIP_ACTION_INFO_DEF + 2)
        {
			std::string ipAccount = player->GetSession()->GetRemoteAddress(); // Verificar la IP
			QueryResult ResultIp = CharacterDatabase.PQuery("select count(*) from promo_boost where ip_account = '%s'", ipAccount.c_str());
			Field *rfieldsIp = ResultIp->Fetch();
			int64 resultIp = rfieldsIp[0].GetInt32();

			int64 account = player->GetSession()->GetAccountId();

			if (resultIp >= 1)
			{
				ChatHandler(player->GetSession()).PSendSysMessage("Segun nuestra base de datos ya obtuviste el boost gratis.");
			}
			else
			{
                switch(action)
                {
                case GOSSIP_ACTION_INFO_DEF+1:
					player->AddItem(110102, 1);
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
					player->AddItem(110104, 1);
                    break;
                default:
                    break;
                }

				player->AddItem(110103, 1);
                player->ModifyMoney(2000000);
                player->GiveLevel(90);
                player->InitTalentForLevel();
                player->SetUInt32Value(PLAYER_FIELD_XP, 0);
                
                if(!player->HasSpell(SPELL_ARTISAN_RIDING))
                    player->LearnSpell(SPELL_ARTISAN_RIDING, false);
                
                if(!player->HasSpell(SPELL_COLD_WHEATHER_FLYING))
                    player->LearnSpell(SPELL_COLD_WHEATHER_FLYING, false);
                
                if(!player->HasSpell(SPELL_FLIGHT_MASTER_LICENSE))
                    player->LearnSpell(SPELL_FLIGHT_MASTER_LICENSE, false);
                
                if(!player->HasSpell(SPELL_WISDOM_OF_FOUR_WINDS))
                    player->LearnSpell(SPELL_WISDOM_OF_FOUR_WINDS, false);
                
				ChatHandler(player->GetSession()).PSendSysMessage("Disfruta tu item Boost, atentamente: El Calvo");
				CharacterDatabase.PQuery("INSERT INTO `promo_boost` (`account`, `ip_account`) VALUES ('%ld','%s')", account, ipAccount.c_str());
                tele_panda = true;
			}
        }
        player->CLOSE_GOSSIP_MENU();

        if(tele_panda)
        {
            if (player->GetTeam() == ALLIANCE)
                player->TeleportTo(870, 821.626f, 253.306f, 503.919f, 3.74204f);
            else
                player->TeleportTo(870, 1678.71f, 931.309f, 471.42f, 0.160629f);
        }
        
        return true;
    }
};

void AddSC_Level_Up() // Add to scriptloader normally
{
    new Level_Up();
    new LevelUpLoader();
    new npc_Level_Up();
}