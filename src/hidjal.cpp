#include "ScriptPCH.h"
#include "Chat.h"
#include "Language.h"
#include "Player.h"  
#include "Group.h"
#include "GroupMgr.h"

std::string IntToString(uint32 value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

class HIDJAL : public CreatureScript
{
public: HIDJAL() : CreatureScript("HIDJAL") { }

    struct HIDJALAI : public ScriptedAI
    { HIDJALAI(Creature* creature) : ScriptedAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (me->IsWithinDistInMap(who, 30.0f) && who->GetTypeId() == TYPEID_PLAYER)
                who->ToPlayer()->TeleportTo(1, 5257.938965f, -1920.361816f, 1355.876953f, 3.578476f);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new HIDJALAI(creature);
    }
}; 

using namespace std;
 
class world_pvp : public PlayerScript
{
public: world_pvp() : PlayerScript("world_pvp") { }

    void OnPVPKill(Player * killer, Player * victim)
    {
        if (victim->GetMapId() == 1 && victim->GetZoneId() == 616 && victim->GetAreaId() == 616)
        {
            PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_IP_COUNT);
            stmt->setUInt32(0, killer->GetSession()->GetAccountId());
            PreparedQueryResult result = LoginDatabase.Query(stmt);

            if(!result) 
               return;

            Field* fields = result->Fetch();
            uint16 count = fields[0].GetUInt16();

            if (!count) 
                return;

            if (killer == victim)
                return;
            else
                killer->AddItem(20559, 1);

            uint32 spawnpoints[]={1,2,3,4,5};
            int random= urand(1,5);

            if (spawnpoints[random] == 1)
            {
                victim->TeleportTo(1, 5257.938965f, -1920.361816f, 1355.876953f, 3.578476f);
                victim->ResurrectPlayer(1.0f, false);
            }					
            else if (spawnpoints[random] == 2)
            {
                victim->TeleportTo(1, 5218.667969f, -2082.810303f, 1280.192505f, 0.6970f);
                victim->ResurrectPlayer(1.0f, false);
            }
            else if (spawnpoints[random] == 3)
            { 
                victim->TeleportTo(1, 5372.805664f, -2187.191406f, 1297.666748f, 3.0713f);
                victim->ResurrectPlayer(1.0f, false);
            }					
            else if (spawnpoints[random] == 4)
            { 
                victim->TeleportTo(1, 5365.471680f, -2121.912354f, 1295.847412f, 3.989835f);
                victim->ResurrectPlayer(1.0f, false);
            }
            else if (spawnpoints[random] == 5)
            { 
                victim->TeleportTo(1, 5248.508789f, -2067.746582f, 1276.984375f, 6.2506f);
                victim->ResurrectPlayer(1.0f, false);
            }

            killer->SaveToDB(false, false);
        }
    }

    void OnMapChanged(Player* plr)
    {
        if (plr->GetZoneId() == 616 && plr->GetAreaId() == 616)
        {
            plr->SaveRecallPosition();
            std::ostringstream battle_for_the_hyjal_announce_enter;
            battle_for_the_hyjal_announce_enter << "Player |cff00FF00" << plr->GetName() << "|r enters into battle for |cffFF0000[Hidjal]|r";
            sWorld->SendZoneText(616, battle_for_the_hyjal_announce_enter.str().c_str(), NULL);
        }

        if (plr->GetZoneId() != 616 && plr->GetAreaId() != 616)
            return;

        Player* player;
        uint32 numPlayers = 0;
        Map::PlayerList const &players = plr->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            if (player = itr->GetSource()->ToPlayer())
                if (player->GetZoneId()== 616 && !player->IsGameMaster())
                    numPlayers++;

        std::ostringstream private_message_on_hyjal_1;
        std::ostringstream private_message_on_hyjal_2;
        private_message_on_hyjal_1 << "|cffFFFFFFYou entered the battle for |cffFF0000[Hidjal]|r";
        private_message_on_hyjal_2 << "|cff45fe00Players in BattleZone:|r |cffff0000" + IntToString(numPlayers) + "|r ";
        ChatHandler(plr->GetSession()).PSendSysMessage(private_message_on_hyjal_1.str().c_str());
        ChatHandler(plr->GetSession()).PSendSysMessage(private_message_on_hyjal_2.str().c_str());
    }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        if (newArea == 616 && player->GetGroup())
        {
            SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP); 
            UpdatePvPState(true);

            if (player->GetGroup())
            {
                player->GetGroup()->Disband();
                    return;
            }
        }
    }
 
};

/*class anti_hk_farm : public PlayerScript
{
public: anti_hk_farm() : PlayerScript("anti_hk_farm") {}

    void OnPVPKill(Player * killer, Player * victim)
    {
        if (killer->GetGUID() == victim->GetGUID())
        {
            return;
        }

        if (killer->GetSession()->GetRemoteAddress() == victim->GetSession()->GetRemoteAddress())
        {

            PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_IP_COUNT);
            stmt->setUInt32(0, killer->GetSession()->GetAccountId());
            PreparedQueryResult result = LoginDatabase.Query(stmt);

            if(!result)
                return;

            Field* fields = result->Fetch();
            uint16 count = fields[0].GetUInt16();

            if (!count)
                return;

            if (count > 1)	
            {
                std::ostringstream ss;
                ss << "|cffff0000[Анти Фарм Система]|r Игрок|cffff6060 " << killer->GetName().c_str() << "|r и игрок |cffff6060" << victim->GetName().c_str() << "|r были выкинуты из игры по причине слива.";
                sWorld->SendServerMessage(SERVER_MSG_STRING, ss.str().c_str());	

                killer->GetSession()->KickPlayer();
                victim->GetSession()->KickPlayer();
            }
        }
        else
        {
            return;
        }
    }
};*/

struct BroadcastData { uint32 time; };
struct BroadcastData Broadcasts[] =
{
    {300000},
};
 
#define BROADCAST_COUNT  sizeof Broadcasts/sizeof(*Broadcasts)

class ZoneAnnounce : public WorldScript
{
public: ZoneAnnounce(): WorldScript("ZoneAnnounce") { }

    std::string GetZoneOnline()
    {
        SessionMap const& sessions = sWorld->GetAllSessions();
        uint32 count = 0;

        for (SessionMap::const_iterator it = sessions.begin(); it != sessions.end(); ++it)
        {
            if (Player* player = it->second->GetPlayer())
            {
                if (player->IsInWorld() && player->GetZoneId() == 616)
                    ++count;
            }
        }

        if (count < 1 || count >= 5) 
        {
            std::string str = "|cffff6060"+IntToString(count) + "|cff00e600 players.|r";
                return str;
        }
        if (count == 1)
        {
            std::string str = "|cffff6060"+IntToString(count) + "|cff00e600 player.|r";
                return str;
        }
        if (count == 2 || count == 3 || count == 4)
        {
            std::string str = "|cffff6060"+IntToString(count) + "|cff00e600 player.|r";
                return str;
        }
    }

    void OnStartup()
    {
        for(uint32 i = 0; i < BROADCAST_COUNT; i++)
            events.ScheduleEvent(i+1, Broadcasts[i].time);
    }

    void OnUpdate(uint32 diff)
    {
        events.Update(diff);
        while (uint32 id = events.ExecuteEvent())
        {
            if (id <= BROADCAST_COUNT)
            {
                std::ostringstream ss;
                ss << "|cffff6060[PvP|cff00e600 Zone|cffff6060 Hidjal]:|cff00e600 Current Players in zone "+ GetZoneOnline();
                sWorld->SendServerMessage(SERVER_MSG_STRING, ss.str().c_str());
                events.ScheduleEvent(id, Broadcasts[id-1].time);

                sObjectMgr->LoadVendors(); // патч обновление npc_vendor
            }    
        }
    }

private:
    EventMap events;
};

class ReviveInFarmZone : public PlayerScript
{
   public: ReviveInFarmZone() : PlayerScript("ReviveInFarmZone") { }

    void OnPlayerKilledByCreature(Creature* killer, Player* killed)
    {
        if (killed->GetZoneId() == 616 && killed->GetAreaId() == 616)
        {
            killed->ResurrectPlayer(1.0f, false);
            killed->TeleportTo(1, 5156.157227f, -1930.102539f, 1366.593628f, 5.890102f);
        }
    }
};

void AddpvpareamoduleScripts()
{
    new HIDJAL();
    new world_pvp;
    //new anti_hk_farm;
    new ZoneAnnounce();
    new ReviveInFarmZone();
}