#include "ScriptMgr.h"
#include "Vehicle.h"
#include "PassiveAI.h"
#include "Player.h"
#include "Language.h"

enum WildTurkey
{
    SPELL_TURKEY_TRACKER = 62014
};

struct npc_wild_turkey : public CritterAI
{
    npc_wild_turkey(Creature* creature) : CritterAI(creature) { }

    void JustDied(Unit* killer) override
    {
        if (killer && killer->GetTypeId() == TYPEID_PLAYER)
            killer->CastSpell(killer, SPELL_TURKEY_TRACKER, true);
    }
};

// Item: Turkey Caller
enum LonelyTurkey
{
    SPELL_COSMETIC_HEARTBROKEN = 62013,
};

struct npc_lonely_turkey : public ScriptedAI
{
    npc_lonely_turkey(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        if (me->IsSummon())
            if (Unit* owner = me->ToTempSummon()->GetSummoner())
                me->GetMotionMaster()->MovePoint(0, owner->GetPositionX() + 25 * cos(owner->GetOrientation()), owner->GetPositionY() + 25 * cos(owner->GetOrientation()), owner->GetPositionZ());

        timer = 3.5 * IN_MILLISECONDS;
    }

    void UpdateAI(uint32 diff) override
    {
        if (timer <= diff)
        {
            DoCast(SPELL_COSMETIC_HEARTBROKEN);
            me->setFaction(31);
        }
        timer -= diff;
    }
private:
    uint32 timer;
};

enum TheTurkinator
{
    SPELL_KILL_COUNTER_VISUAL       = 62015,
    SPELL_KILL_COUNTER_VISUAL_MAX   = 62021
};

class spell_gen_turkey_tracker : public SpellScript
{
    PrepareSpellScript(spell_gen_turkey_tracker);

    bool Validate(SpellInfo const* /*spellEntry*/)
    {
        if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL))
            return false;
        if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL_MAX))
            return false;
        return true;
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster()->ToPlayer()->GetAura(SPELL_KILL_COUNTER_VISUAL_MAX))
            return;

        Unit* caster = GetCaster();

        if (!caster || !caster->ToPlayer())
            return;

        if (Aura* aura = caster->GetAura(GetSpellInfo()->Id))
        {
            switch (aura->GetStackAmount())
            {
                case 10:
                    caster->ToPlayer()->SendPersonalMessage(LANG_TURKEY_HUNTER, CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL);
                    caster->CastSpell(caster, SPELL_KILL_COUNTER_VISUAL, true);
                    break;
                case 20:
                    caster->ToPlayer()->SendPersonalMessage(LANG_TURKEY_DOMINATION, CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL);
                    caster->CastSpell(caster, SPELL_KILL_COUNTER_VISUAL, true);
                    break;
                case 30:
                    caster->ToPlayer()->SendPersonalMessage(LANG_TURKEY_SLAUGHTER, CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL);
                    caster->CastSpell(caster, SPELL_KILL_COUNTER_VISUAL, true);
                    break;
                case 40:
                    caster->ToPlayer()->SendPersonalMessage(LANG_TURKEY_TRIUMPH, CHAT_MSG_RAID_BOSS_WHISPER, LANG_UNIVERSAL);
                    caster->CastSpell(caster, SPELL_KILL_COUNTER_VISUAL, true);
                    caster->CastSpell(caster, SPELL_KILL_COUNTER_VISUAL_MAX, true); // Achievement Credit
                    caster->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                    break;
                default:
                    break;
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_turkey_tracker::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class spell_gen_feast_on : public SpellScript
{
    PrepareSpellScript(spell_gen_feast_on);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        int32 bp0 = GetSpellInfo()->Effects[EFFECT_0].CalcValue();

        Unit* caster = GetCaster();
        if (caster->IsVehicle())
            if (Unit* player = caster->GetVehicleKit()->GetPassenger(0))
                caster->CastSpell(player, bp0, true, nullptr, nullptr, player->ToPlayer()->GetGUID());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gen_feast_on::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum WellFedPilgrimsBounty
{
    // Feast On
    SPELL_A_SERVING_OF_TURKEY           = 61807,
    SPELL_A_SERVING_OF_CRANBERRIES      = 61804,
    SPELL_A_SERVING_OF_STUFFING         = 61806,
    SPELL_A_SERVING_OF_SWEET_POTATOES   = 61808,
    SPELL_A_SERVING_OF_PIE              = 61805,

    // Well Fed
    SPELL_WELL_FED_AP                   = 65414,
    SPELL_WELL_FED_ZM                   = 65412,
    SPELL_WELL_FED_HIT                  = 65416,
    SPELL_WELL_FED_HASTE                = 65410,
    SPELL_WELL_FED_SPIRIT               = 65415,

    // Pilgrim's Paunch
    SPELL_THE_SPIRIT_OF_SHARING         = 61849,
};

class spell_gen_well_fed_pilgrims_bounty : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_well_fed_pilgrims_bounty(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_well_fed_pilgrims_bounty_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_well_fed_pilgrims_bounty_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_well_fed_pilgrims_bounty_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Player* target = GetHitPlayer();
                if (!target)
                    return;

                Aura const* Turkey = target->GetAura(SPELL_A_SERVING_OF_TURKEY);
                Aura const* Cranberies = target->GetAura(SPELL_A_SERVING_OF_CRANBERRIES);
                Aura const* Stuffing = target->GetAura(SPELL_A_SERVING_OF_STUFFING);
                Aura const* SweetPotatoes = target->GetAura(SPELL_A_SERVING_OF_SWEET_POTATOES);
                Aura const* Pie = target->GetAura(SPELL_A_SERVING_OF_PIE);

                if (Aura const* aura = target->GetAura(_triggeredSpellId1))
                {
                    if (aura->GetStackAmount() == 5)
                        target->CastSpell(target, _triggeredSpellId2, true);
                }

                // The Spirit of Sharing - Achievement Credit
                if (!target->GetAura(SPELL_THE_SPIRIT_OF_SHARING))
                {
                    if ((Turkey && Turkey->GetStackAmount() == 5) && (Cranberies && Cranberies->GetStackAmount() == 5) && (Stuffing && Stuffing->GetStackAmount() == 5) &&
                        (SweetPotatoes && SweetPotatoes->GetStackAmount() == 5) && (Pie && Pie->GetStackAmount() == 5))
                        target->CastSpell(target, SPELL_THE_SPIRIT_OF_SHARING, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_well_fed_pilgrims_bounty_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_well_fed_pilgrims_bounty_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum OnPlatePilgrimsBounty
{
    // "FOOD FIGHT!" - Achivement Credit
    SPELL_ON_PLATE_TURKEY           = 61928,
    SPELL_ON_PLATE_CRANBERRIES      = 61925,
    SPELL_ON_PLATE_STUFFING         = 61927,
    SPELL_ON_PLATE_SWEET_POTATOES   = 61929,
    SPELL_ON_PLATE_PIE              = 61926,

    // Sharing is Caring - Achivement Credit
    SPELL_PASS_THE_TURKEY           = 66373,
    SPELL_PASS_THE_CRANBERRIES      = 66372,
    SPELL_PASS_THE_STUFFING         = 66375,
    SPELL_PASS_THE_SWEET_POTATOES   = 66376,
    SPELL_PASS_THE_PIE              = 66374,

    SPELL_RIDE_VEHICLE_CHAIRS       = 65403,
};

// 66250 - Pass The Turkey
// 66259 - Pass The Stuffing
// 66260 - Pass The Pie
// 66261 - Pass The Cranberries
// 66262 - Pass The Sweet Potatoes
class spell_gen_on_plate_pilgrims_bounty : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_on_plate_pilgrims_bounty(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_on_plate_pilgrims_bounty_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_on_plate_pilgrims_bounty_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_on_plate_pilgrims_bounty_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (caster->IsVehicle())
                {
                    Unit* player = caster->GetVehicleKit()->GetPassenger(0);
                    if (!player)
                        return;

                    if (!GetHitUnit()->HasAura(SPELL_RIDE_VEHICLE_CHAIRS))
                        return;

                    player->CastSpell(GetHitUnit(), _triggeredSpellId1, true, nullptr, nullptr, player->GetGUID());
                    player->CastSpell(player, _triggeredSpellId2, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_on_plate_pilgrims_bounty_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_on_plate_pilgrims_bounty_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum BountifulFeast
{
    // Bountiful Feast
    SPELL_BOUNTIFUL_FEAST_DRINK          = 66041,
    SPELL_BOUNTIFUL_FEAST_FOOD           = 66478,
    SPELL_BOUNTIFUL_FEAST_REFRESHMENT    = 66622,
};

class spell_gen_bountiful_feast : public SpellScript
{
    PrepareSpellScript(spell_gen_bountiful_feast);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_DRINK, true);
        caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_FOOD, true);
        caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_REFRESHMENT, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_bountiful_feast::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

enum PilgrimsBountyBuffFood
{
    // Pilgrims Bounty Buff Food
    SPELL_WELL_FED_AP_TRIGGER       = 65414,
    SPELL_WELL_FED_ZM_TRIGGER       = 65412,
    SPELL_WELL_FED_HIT_TRIGGER      = 65416,
    SPELL_WELL_FED_HASTE_TRIGGER    = 65410,
    SPELL_WELL_FED_SPIRIT_TRIGGER   = 65415,
};

class spell_pilgrims_bounty_buff_food : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId;
    public:
        spell_pilgrims_bounty_buff_food(const char* name, uint32 triggeredSpellId = 0) : SpellScriptLoader(name), _triggeredSpellId(triggeredSpellId) { }

        class spell_pilgrims_bounty_buff_food_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pilgrims_bounty_buff_food_AuraScript)
        private:
            uint32 _triggeredSpellId;

        public:
            spell_pilgrims_bounty_buff_food_AuraScript(uint32 triggeredSpellId) : AuraScript(), _triggeredSpellId(triggeredSpellId) { }

            bool Load()
            {
                _handled = false;
                return true;
            }

            void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& canBeRecalculated)
            {
                if (Unit* caster = GetCaster())
                {
                    amount = caster->CountPctFromMaxHealth(4 * 5);
                    canBeRecalculated = false;
                }
            }

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (_handled)
                    return;

                Unit* caster = GetCaster();
                if (!caster)
                    return;

                _handled = true;
                caster->CastSpell(caster, _triggeredSpellId, true);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pilgrims_bounty_buff_food_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_REGEN);
                if (_triggeredSpellId)
                    OnEffectPeriodic += AuraEffectPeriodicFn(spell_pilgrims_bounty_buff_food_AuraScript::HandleTriggerSpell, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

            bool _handled;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pilgrims_bounty_buff_food_AuraScript(_triggeredSpellId);
        }
};

class spell_pilgrims_bounty_buff_drink : public AuraScript
{
    PrepareAuraScript(spell_pilgrims_bounty_buff_drink);

    void CalculateAmount(AuraEffect const* aurEff, float& amount, bool& canBeRecalculated)
    {
        if (Unit* caster = GetCaster())
        {
            amount = CalculatePct(caster->GetMaxPower(POWER_MANA), 4 * 5);
            canBeRecalculated = false;
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pilgrims_bounty_buff_drink::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN);
    }
};


enum spell_pavo_op
{
	SPELL_INSTA_KILL = 80468,
	// phase 1
	SPELL_THUNDER_CLAP = 147683, // atronar
	SPELL_MORTAL_WOUND = 54378, // herida mortal (reduce la curacion)
	SPELL_WAILING_WINDS = 136338, //Arco nova
	//phase 2
	SPELL_FRENZY = 74853, // frenesi
	SPELL_TOXIC_STORM = 144005, // tormenta toxica
	//phase 3
	SPELL_FIRE_STORM = 147998, // tormenta de fuego
	SPELL_FIERY_CHARGE = 147704, // carga de fuego
	SPELL_MOLTEN_INFERNO = 147880, // infierno de arrabio
	// phase 4
	SPELL_BERSERK = 26662 // rabiar
};

enum events_pavo_op
{
	// phase 1
	EVENT_WOUND = 1,
	EVENT_MORTAL_WOUND,
	EVENT_WINDS,
	//phase 2
	EVENT_FRENZY,
	EVENT_TOXIC_STORM,
	//phase 3
	EVENT_FIRE_STORM,
	EVENT_FIERY_CHARGE,
	EVENT_MOLTEN_INFERNO,
	//phase 4
	EVENT_BERSERK
};

enum Phases_pavo_op
{
	PHASE_1 = 1,
	PHASE_2,
	PHASE_3,
	PHASE_4
};

enum Summons_pavo_op
{
	NPC_MINI_PAVO = 300021
};

class boss_pavito_op : public CreatureScript
{
public:
    boss_pavito_op() : CreatureScript("boss_pavito_op") { }

    struct boss_pavito_opAI : public ScriptedAI
    {
		boss_pavito_opAI(Creature* creature) : ScriptedAI(creature), summons(me) {}

		SummonList summons;
		EventMap events;

		void Reset() override
		{
			me->setFaction(35);
			me->DeMorph();
			me->SetObjectScale(5);
			events.SetPhase(PHASE_1);
			me->RemoveAurasDueToSpell(SPELL_BERSERK);
			me->RemoveAurasDueToSpell(SPELL_FRENZY);

			summons.DespawnAll();
			events.Reset();
		}
		
		void EnterCombat(Unit* who) override
		{
			events.SetPhase(PHASE_1);
			me->MonsterYell("Malditos mortales, ustedes y sus hornos!!", LANG_UNIVERSAL, 0);
			DoCast(me, SPELL_THUNDER_CLAP, true);
			
			DoPlaySoundToSet(me, 11466);

			events.ScheduleEvent(EVENT_WOUND, 5 * IN_MILLISECONDS, 0, PHASE_1);
			events.ScheduleEvent(EVENT_WINDS, 16 * IN_MILLISECONDS, 0, PHASE_1);
			events.ScheduleEvent(EVENT_BERSERK, 300 * IN_MILLISECONDS);
		}

		//When killed a unit:
		void KilledUnit(Unit* /*who*/)
		{
			me->MonsterYell("Me los devorare a todos!!!", LANG_UNIVERSAL, 0);
			//me->SummonCreature(NPC_MINI_PAVO, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30 * IN_MILLISECONDS);
			DoSpawnCreature(NPC_MINI_PAVO, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30 * IN_MILLISECONDS);
			DoPlaySoundToSet(me, 11466);
			//DoPlaySoundToSet(me, SOUND_ONSLAY);
		}

		void JustSummoned(Creature* summon)
		{
			if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40, true))
				summon->AI()->AttackStart(target);

			summons.Summon(summon);
		}

		void JustDied(Unit* /*killer*/) override
		{
			summons.DespawnAll();
			me->MonsterYell("Mi hijo me vengara!!!", LANG_UNIVERSAL, 0);
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage) override
		{
			if(events.IsInPhase(PHASE_1) && me->GetHealthPct() <= 60.0f)
			{
				events.SetPhase(PHASE_2);
				me->MonsterYell("No llegaras al proximo evento!!!", LANG_UNIVERSAL, 0);
				events.ScheduleEvent(EVENT_FRENZY, 2 * IN_MILLISECONDS, 0, PHASE_2);
			}

			if (events.IsInPhase(PHASE_2) && me->GetHealthPct() <= 30.0f)
			{
				events.SetPhase(PHASE_3);

				me->MonsterYell("Los cocinare!!", LANG_UNIVERSAL, 0);
				events.ScheduleEvent(EVENT_FIRE_STORM, 1 * IN_MILLISECONDS, 0, PHASE_3);
				events.ScheduleEvent(EVENT_FIERY_CHARGE, 6 * IN_MILLISECONDS, 0, PHASE_3);
				events.ScheduleEvent(EVENT_MOLTEN_INFERNO, 11 * IN_MILLISECONDS, 0, PHASE_3);
			}

			if (events.IsInPhase(PHASE_3) && me->GetHealthPct() <= 10.0f)
			{
				events.SetPhase(PHASE_4);
				events.ScheduleEvent(EVENT_BERSERK, 2 * IN_MILLISECONDS, 0, PHASE_4);
			}
		}

		void UpdateAI(uint32 diff) override
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_WOUND:
					if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO,0, 0.0f, true))
						DoCast(target, SPELL_MORTAL_WOUND);
					me->MonsterYell("Lo unico que te comeras seran mis plumas!!", LANG_UNIVERSAL, 0);
					events.ScheduleEvent(EVENT_WOUND, 8 * IN_MILLISECONDS, 0, PHASE_1);
					break;

				case EVENT_WINDS:
					DoCast(me, SPELL_WAILING_WINDS);
					me->MonsterYell("Como es posible que esten en la cima de la cadena alimenticia?", LANG_UNIVERSAL, 0);
					events.ScheduleEvent(EVENT_WINDS, 25 * IN_MILLISECONDS, 0, PHASE_1);
					break;

				case EVENT_FRENZY:
					DoCast(me, SPELL_FRENZY);
					events.ScheduleEvent(EVENT_TOXIC_STORM, 3 * IN_MILLISECONDS, 0, PHASE_2);
					break;

				case EVENT_TOXIC_STORM:
					if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 60.0f, true))
						DoCast(target, SPELL_TOXIC_STORM);
					me->MonsterYell("llego el pavo brbrlrblr!!!", LANG_UNIVERSAL, 0);
					events.ScheduleEvent(EVENT_TOXIC_STORM, 24 * IN_MILLISECONDS, 0, PHASE_2);
					break;

				case EVENT_FIRE_STORM:
					if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 100.0f, true))
						DoCast(target, SPELL_FIRE_STORM);
					events.ScheduleEvent(EVENT_FIRE_STORM, 15 * IN_MILLISECONDS, 0, PHASE_3);
					break;

				case EVENT_FIERY_CHARGE:
					if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 60.0f, true))
						DoCast(target, SPELL_FIERY_CHARGE);
					events.ScheduleEvent(EVENT_FIERY_CHARGE, 20 * IN_MILLISECONDS, 0, PHASE_3);
					break;

				case EVENT_MOLTEN_INFERNO:
					if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0, 0.0f, true))
						DoCast(target, SPELL_MOLTEN_INFERNO);

					//me->CastSpell(target, SPELL_MOLTEN_INFERNO, true);
					events.ScheduleEvent(EVENT_MOLTEN_INFERNO, 25 * IN_MILLISECONDS, 0, PHASE_3);
					break;

				case EVENT_BERSERK:
					DoCast(me, SPELL_BERSERK);
					if (!events.IsInPhase(PHASE_4))
						events.SetPhase(PHASE_4);

					DoPlaySoundToSet(me, 7143);
					me->MonsterYell("brbrbrbrl brbrbrbrl", LANG_UNIVERSAL, 0);
					break;
				}
			}

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_pavito_opAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Solo soy un huevo pasado!!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(1, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->setFaction(14); //Set our faction to hostile towards all
			player->CastSpell(player, SPELL_INSTA_KILL, true); //kill player
			creature->SetDisplayId(21774); // morph pavo			
			creature->SetObjectScale(10);
        }

        return true;
    }
};

void AddSC_event_pilgrims_bounty()
{
    new creature_script<npc_wild_turkey>("npc_wild_turkey");
    new creature_script<npc_lonely_turkey>("npc_lonely_turkey");
    new spell_script<spell_gen_turkey_tracker>("spell_gen_turkey_tracker");
    new spell_script<spell_gen_feast_on>("spell_gen_feast_on");
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_ap", SPELL_A_SERVING_OF_TURKEY, SPELL_WELL_FED_AP);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_zm", SPELL_A_SERVING_OF_CRANBERRIES, SPELL_WELL_FED_ZM);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_hit", SPELL_A_SERVING_OF_STUFFING, SPELL_WELL_FED_HIT);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_haste", SPELL_A_SERVING_OF_SWEET_POTATOES, SPELL_WELL_FED_HASTE);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_spirit", SPELL_A_SERVING_OF_PIE, SPELL_WELL_FED_SPIRIT);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_turkey", SPELL_ON_PLATE_TURKEY, SPELL_PASS_THE_TURKEY);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_cranberries", SPELL_ON_PLATE_CRANBERRIES, SPELL_PASS_THE_CRANBERRIES);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_stuffing", SPELL_ON_PLATE_STUFFING, SPELL_PASS_THE_STUFFING);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_sweet_potatoes", SPELL_ON_PLATE_SWEET_POTATOES, SPELL_PASS_THE_SWEET_POTATOES);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_pie", SPELL_ON_PLATE_PIE, SPELL_PASS_THE_PIE);
    new spell_script<spell_gen_bountiful_feast>("spell_gen_bountiful_feast");
    new spell_pilgrims_bounty_buff_food("spell_gen_slow_roasted_turkey", SPELL_WELL_FED_AP_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_cranberry_chutney", SPELL_WELL_FED_ZM_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_spice_bread_stuffing", SPELL_WELL_FED_HIT_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_pumpkin_pie", SPELL_WELL_FED_SPIRIT_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_candied_sweet_potato", SPELL_WELL_FED_HASTE_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_pilgrims_bounty_buff_food");
    new aura_script<spell_pilgrims_bounty_buff_drink>("spell_pilgrims_bounty_buff_drink");
    new boss_pavito_op;
}
