/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Comment: Find in the future best timers and the event is not implemented.
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "azjol_nerub.h"

enum Spells
{
    SPELL_MIND_FLAY                               = 52586,
    H_SPELL_MIND_FLAY                             = 59367,
    SPELL_CURSE_OF_FATIGUE                        = 52592,
    H_SPELL_CURSE_OF_FATIGUE                      = 59368,
    SPELL_FRENZY                                  = 28747, //maybe 53361
    SPELL_SUMMON_SKITTERING_SWARMER               = 52438, //AOE Effect 140, maybe 52439
    SPELL_SUMMON_SKITTERING_SWARMER_1             = 52439, //Summon 3x 28735
    H_SPELL_ACID_SPLASH                           = 59363,
    SPELL_ACID_SPLASH                             = 52446,
    SPELL_CHARGE                                  = 24023, //maybe is another spell
	SPELL_FIXATE								  = 52537, //Bugged 52536,
    SPELL_BACKSTAB                                = 52540,
    SPELL_SHADOW_BOLT                             = 52534,
    H_SPELL_SHADOW_BOLT                           = 59357,
    SPELL_SHADOW_NOVA                             = 52535,
    H_SPELL_SHADOW_NOVA                           = 59358,
    SPELL_STRIKE                                  = 52532,
    SPELL_CLEAVE                                  = 49806,
    SPELL_ENRAGE                                  = 52470,
    SPELL_INFECTED_BITE                           = 52469,
    H_SPELL_INFECTED_BITE                         = 59364,
    SPELL_WEB_WRAP                                = 52086, //the spell is not working properly
	SPELL_WEB_WRAP_FINAL						  = 52087,
    SPELL_BLINDING_WEBS                           = 52524,
    H_SPELL_BLINDING_WEBS                         = 59365,
    SPELL_POSION_SPRAY                            = 52493,
    H_SPELL_POSION_SPRAY                          = 59366
};

enum Mobs
{
    MOB_SKITTERING_SWARMER                        = 28735,
    MOB_SKITTERING_SWARMER_CONTROLLER             = 32593,
    MOB_SKITTERING_INFECTIOR                      = 28736
};

enum Yells
{
    SAY_AGGRO                                     = -1601011,
    SAY_SLAY_1                                    = -1601012,
    SAY_SLAY_2                                    = -1601013,
    SAY_DEATH                                     = -1601014,
    SAY_SEND_GROUP_1                              = -1601020,
    SAY_SEND_GROUP_2                              = -1601021,
    SAY_SEND_GROUP_3                              = -1601022,
    SAY_SWARM_1                                   = -1601015,
    SAY_SWARM_2                                   = -1601016,
    SAY_PREFIGHT_1                                = -1601017,
    SAY_PREFIGHT_2                                = -1601018,
    SAY_PREFIGHT_3                                = -1601019
};

const Position SpawnPoint[] =
{
    { 566.164f, 682.087f, 769.079f, 2.21657f  }, //0
    { 529.042f, 706.941f, 777.298f, 1.0821f   }, //1
    { 489.975f, 671.239f, 772.131f, 0.261799f }, //2
    { 488.556f, 692.95f,  771.764f, 4.88692f  }, //3
    { 553.34f,  640.387f, 777.419f, 1.20428f  }, //4
    { 517.486f, 706.398f, 777.335f, 5.35816f  }, //5
    { 504.01f,  637.693f, 777.479f, 0.506145f }, //6
    { 552.625f, 706.408f, 777.177f, 3.4383f   }, //7
	{ 520.422f, 693.572f, 775.692f, 5.2612f   }  //8
};

class boss_krik_thir : public CreatureScript
{
public:
    boss_krik_thir() : CreatureScript("boss_krik_thir") { }

    struct boss_krik_thirAI : public ScriptedAI
    {
        boss_krik_thirAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 uiMindFlayTimer;
        uint32 uiCurseFatigueTimer;
        uint32 uiSummonTimer;
		uint32 uiOOCTextTimer;
		uint32 uiCheckWatcherTimer;
		bool doOnce;

        void Reset()
        {
            uiMindFlayTimer = 15*IN_MILLISECONDS;
            uiCurseFatigueTimer = 12*IN_MILLISECONDS;
			uiOOCTextTimer = 15000+rand()%15000;
			doOnce = false;

            if (instance)
                instance->SetData(DATA_KRIKTHIR_THE_GATEWATCHER_EVENT, NOT_STARTED);
        }

        void EnterCombat(Unit* /*who*/)
        {
            DoScriptText(SAY_AGGRO, me);
			Summon();
            uiSummonTimer = 15*IN_MILLISECONDS;

            if (instance)
                instance->SetData(DATA_KRIKTHIR_THE_GATEWATCHER_EVENT, IN_PROGRESS);
        }

		void MoveInLineOfSight(Unit* who)
		{
				if(!who || who->GetTypeId() != TYPEID_PLAYER)
					return;

					if(who->GetDistance2d(me) <= 50 && !doOnce)
					{
						DoScriptText(SAY_SEND_GROUP_1, me);
						uiCheckWatcherTimer = 2500;
						doOnce = true;
                    if (Creature* pGashra = Unit::GetCreature(*me, instance->GetData64(DATA_WATCHER_GASHRA)))
                    {
							pGashra->GetMotionMaster()->MovePoint(0, 533.796570f, 694.364197f, 775.665649f);
							pGashra->SetInCombatWithZone();
                    }
					}
		}
        void Summon()
        {
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[0], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[0], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[1], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[2], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[2], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[3], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[3], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_INFECTIOR, SpawnPoint[4], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[4], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_INFECTIOR, SpawnPoint[5], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[5], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_INFECTIOR, SpawnPoint[6], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[6], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[7], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                me->SummonCreature(MOB_SKITTERING_SWARMER, SpawnPoint[8], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
        }

        void UpdateAI(const uint32 diff)
        {
			if(uiCheckWatcherTimer <= diff && !UpdateVictim() && doOnce)
			{
            Creature* Watcher[3];
            if (!instance)
                return;

            for (uint8 n = 0; n < 3; ++n)
            {
                Watcher[n] = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_WATCHER_GASHRA + n));
			}

                if (!Watcher[0]->isAlive() && Watcher[1]->isAlive())
				{
                    if (Creature* pSilthik = Unit::GetCreature(*me, instance->GetData64(DATA_WATCHER_SILTHIK)))
                    {
						DoScriptText(SAY_SEND_GROUP_2, me);	
						pSilthik->GetMotionMaster()->MovePoint(0, 533.796570f, 694.364197f, 775.665649f);
						pSilthik->SetInCombatWithZone();
					}
				}
				if (!Watcher[1]->isAlive() && Watcher[2]->isAlive())
				{
                    if (Creature* pNarjil = Unit::GetCreature(*me, instance->GetData64(DATA_WATCHER_NARJIL)))
                    {
						DoScriptText(SAY_SEND_GROUP_3, me);	
						pNarjil->GetMotionMaster()->MovePoint(0, 533.796570f, 694.364197f, 775.665649f);
						pNarjil->SetInCombatWithZone();
                    }
				}
			if (!Watcher[0]->isAlive() && !Watcher[1]->isAlive() && !Watcher[2]->isAlive())
				{
						me->GetMotionMaster()->MovePoint(0, 533.796570f, 694.364197f, 775.665649f);
						me->SetInCombatWithZone();
				}
			uiCheckWatcherTimer = 12500;
			} else uiCheckWatcherTimer -= diff;

			if(!UpdateVictim() && uiOOCTextTimer <= diff && !doOnce)
			{
				uint8 uiChance = urand(1,3);
				switch(uiChance)
				{
				case 1:
					DoScriptText(SAY_PREFIGHT_1,me);
					break;
				case 2:
					DoScriptText(SAY_PREFIGHT_2,me);
					break;
				case 3:
					DoScriptText(SAY_PREFIGHT_3,me);
					break;
				}
				uiOOCTextTimer = 25000+rand()%15000;
				return;
			}else uiOOCTextTimer -= diff;

            if (!UpdateVictim())
                return;

            if (uiSummonTimer <= diff)
            {
				Summon();
                uiSummonTimer = 15000+rand()%5000;
            } else uiSummonTimer -= diff;

            if (uiMindFlayTimer <= diff)
            {
                    DoCast(me->getVictim(), SPELL_MIND_FLAY);
                    uiMindFlayTimer = 10000+rand()%7500;
                } else uiMindFlayTimer -= diff;

            if (uiCurseFatigueTimer <= diff)
            {
                if (Unit* firstTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(firstTarget, SPELL_CURSE_OF_FATIGUE);

                if (Unit* secondTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true))
                    DoCast(secondTarget, SPELL_CURSE_OF_FATIGUE);

                uiCurseFatigueTimer = 10000+rand()%8000;
            } else uiCurseFatigueTimer -= diff;

            if (!me->HasAura(SPELL_FRENZY) && HealthBelowPct(10))
                DoCast(me, SPELL_FRENZY, true);

            DoMeleeAttackIfReady();
        }
        void JustDied(Unit* /*killer*/)
        {
            DoScriptText(SAY_DEATH, me);

            if (instance)
                instance->SetData(DATA_KRIKTHIR_THE_GATEWATCHER_EVENT, DONE);
        }

        void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

            DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_krik_thirAI(creature);
    }
};

class npc_skittering_infector : public CreatureScript
{
public:
    npc_skittering_infector() : CreatureScript("npc_skittering_infector") { }

    struct npc_skittering_infectorAI : public ScriptedAI
    {
        npc_skittering_infectorAI(Creature* creature) : ScriptedAI(creature) {
		instance   = creature->GetInstanceScript();
		}
		
        InstanceScript* instance;

		void Reset()
		{
                if (instance)
                {
                    if (Creature* pKrikthir = Unit::GetCreature(*me, instance->GetData64(DATA_KRIKTHIR_THE_GATEWATCHER)))
                    {
							float x,y,z;
							pKrikthir->GetPosition(x,y,z);
							me->GetMotionMaster()->MovePoint(0, x, y, z);
                    }
				}
		}

        void JustDied(Unit* /*killer*/)
        {
            DoCast(SPELL_ACID_SPLASH);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skittering_infectorAI(creature);
    }
};

class npc_skittering_swarmer : public CreatureScript
{
public:
    npc_skittering_swarmer() : CreatureScript("npc_skittering_swarmer") { }

    struct npc_skittering_swarmerAI : public ScriptedAI
    {
        npc_skittering_swarmerAI(Creature* creature) : ScriptedAI(creature) {
		instance   = creature->GetInstanceScript();
		}
		
        InstanceScript* instance;

		void EnterCombat()
		{
			me->CallForHelp(5);
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skittering_swarmerAI(creature);
    }
};

class npc_anub_ar_skirmisher : public CreatureScript
{
public:
    npc_anub_ar_skirmisher() : CreatureScript("npc_anub_ar_skirmisher") { }

    struct npc_anub_ar_skirmisherAI : public ScriptedAI
    {
        npc_anub_ar_skirmisherAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 uiChargeTimer;
        uint32 uiBackstabTimer;

        void Reset()
        {
            uiChargeTimer   = 11*IN_MILLISECONDS;
            uiBackstabTimer = 7*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (uiChargeTimer <= diff && IsHeroic())
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 100, true))
                {
                    DoResetThreat();
                    me->AddThreat(target, 1.0f);
                    DoCast(target, SPELL_CHARGE, true);
					DoCast(SPELL_FIXATE);
                }
                uiChargeTimer = 15000+rand()%10000;
            } else uiChargeTimer -= diff;

            if (uiBackstabTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_BACKSTAB);
                uiBackstabTimer = 3000+rand()%6000;
            } else uiBackstabTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anub_ar_skirmisherAI(creature);
    }
};

class npc_anub_ar_shadowcaster : public CreatureScript
{
public:
    npc_anub_ar_shadowcaster() : CreatureScript("npc_anub_ar_shadowcaster") { }

    struct npc_anub_ar_shadowcasterAI : public ScriptedAI
    {
        npc_anub_ar_shadowcasterAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 uiShadowBoltTimer;
        uint32 uiShadowNovaTimer;

        void Reset()
        {
            uiShadowBoltTimer = 6*IN_MILLISECONDS;
            uiShadowNovaTimer = 15*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (uiShadowBoltTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                     DoCast(target, SPELL_SHADOW_BOLT, true);
                uiShadowBoltTimer = 15*IN_MILLISECONDS;
            } else uiShadowBoltTimer -= diff;

            if (uiShadowNovaTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_SHADOW_NOVA, true);
                uiShadowNovaTimer = 17*IN_MILLISECONDS;
            } else uiShadowNovaTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anub_ar_shadowcasterAI(creature);
    }
};

class npc_anub_ar_warrior : public CreatureScript
{
public:
    npc_anub_ar_warrior() : CreatureScript("npc_anub_ar_warrior") { }

    struct npc_anub_ar_warriorAI : public ScriptedAI
    {
        npc_anub_ar_warriorAI(Creature* creature) : ScriptedAI(creature){}

        uint32 uiCleaveTimer;
        uint32 uiStrikeTimer;

        void Reset()
        {
            uiCleaveTimer = 11*IN_MILLISECONDS;
            uiStrikeTimer = 6*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (uiStrikeTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_STRIKE, true);
                uiStrikeTimer = 15*IN_MILLISECONDS;
            } else uiStrikeTimer -= diff;

            if (uiCleaveTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_CLEAVE, true);
                uiCleaveTimer = 17*IN_MILLISECONDS;
            } else uiCleaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anub_ar_warriorAI(creature);
    }
};

class npc_watcher_gashra : public CreatureScript
{
public:
    npc_watcher_gashra() : CreatureScript("npc_watcher_gashra") { }

    struct npc_watcher_gashraAI : public ScriptedAI
    {
        npc_watcher_gashraAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 uiWebWrapTimer;
        uint32 uiInfectedBiteTimer;
		uint32 uiWebEnsareTimer;

        void Reset()
        {
            uiWebWrapTimer      = 11*IN_MILLISECONDS;
            uiInfectedBiteTimer = 4*IN_MILLISECONDS;
			uiWebEnsareTimer = 0;
        }

        void EnterCombat(Unit* /*who*/)
        {
            DoCast(me, SPELL_ENRAGE, true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if(uiWebEnsareTimer <= diff)
			{
				
			} else uiWebEnsareTimer -= diff;

            if (uiWebWrapTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_WEB_WRAP, true);
					uiWebEnsareTimer = 2500;
				}
                uiWebWrapTimer = 17*IN_MILLISECONDS;
            } else uiWebWrapTimer -= diff;

            if (uiInfectedBiteTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_INFECTED_BITE, true);
                uiInfectedBiteTimer = 15*IN_MILLISECONDS;
            } else uiInfectedBiteTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_watcher_gashraAI(creature);
    }
};

class npc_watcher_narjil : public CreatureScript
{
public:
    npc_watcher_narjil() : CreatureScript("npc_watcher_narjil") { }

    struct npc_watcher_narjilAI : public ScriptedAI
    {
        npc_watcher_narjilAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 uiWebWrapTimer;
        uint32 uiInfectedBiteTimer;
        uint32 uiBindingWebsTimer;

        void Reset()
        {
            uiWebWrapTimer      = 11*IN_MILLISECONDS;
            uiInfectedBiteTimer = 4*IN_MILLISECONDS;
            uiBindingWebsTimer = 17*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (uiWebWrapTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(target, SPELL_WEB_WRAP, true);
                uiWebWrapTimer = 15*IN_MILLISECONDS;
            } else uiWebWrapTimer -= diff;

            if (uiInfectedBiteTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_INFECTED_BITE, true);
                uiInfectedBiteTimer = 11*IN_MILLISECONDS;
            } else uiInfectedBiteTimer -= diff;

            if (uiBindingWebsTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_BLINDING_WEBS, true);
                uiBindingWebsTimer = 17*IN_MILLISECONDS;
            } else uiBindingWebsTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_watcher_narjilAI(creature);
    }
};

class npc_watcher_silthik : public CreatureScript
{
public:
    npc_watcher_silthik() : CreatureScript("npc_watcher_silthik") { }

    struct npc_watcher_silthikAI : public ScriptedAI
    {
        npc_watcher_silthikAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 uiWebWrapTimer;
        uint32 uiInfectedBiteTimer;
        uint32 uiPoisonSprayTimer;

        void Reset()
        {
            uiWebWrapTimer      = 11*IN_MILLISECONDS;
            uiInfectedBiteTimer = 4*IN_MILLISECONDS;
            uiPoisonSprayTimer  = 15*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (uiWebWrapTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(target, SPELL_WEB_WRAP, true);

                uiWebWrapTimer = 15*IN_MILLISECONDS;
            } else uiWebWrapTimer -= diff;

            if (uiInfectedBiteTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_INFECTED_BITE, true);
                uiInfectedBiteTimer = 15*IN_MILLISECONDS;
            } else uiInfectedBiteTimer -= diff;

            if (uiPoisonSprayTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_POSION_SPRAY, true);
                uiPoisonSprayTimer = 17*IN_MILLISECONDS;
            } else uiPoisonSprayTimer -= diff;

            DoMeleeAttackIfReady();

        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_watcher_silthikAI(creature);
    }
};

class achievement_watch_him_die : public AchievementCriteriaScript
{
    public:
        achievement_watch_him_die() : AchievementCriteriaScript("achievement_watch_him_die")
        {
        }

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            InstanceScript* instance = target->GetInstanceScript();
            Creature* Watcher[3];
            if (!instance)
                return false;

            for (uint8 n = 0; n < 3; ++n)
            {
                Watcher[n] = ObjectAccessor::GetCreature(*target, instance->GetData64(DATA_WATCHER_GASHRA + n));
                if (Watcher[n] && !Watcher[n]->isAlive())
                    return false;
            }

            return true;
        }
};


void AddSC_boss_krik_thir()
{
    new boss_krik_thir();
    new npc_skittering_infector();
    new npc_anub_ar_skirmisher();
    new npc_anub_ar_shadowcaster();
    new npc_watcher_gashra();
    new npc_anub_ar_warrior();
    new npc_watcher_silthik();
    new npc_watcher_narjil();
	new npc_skittering_swarmer();
    new achievement_watch_him_die();
}
