#pragma once
#include<Windows.h>
#include<vector>
#include"Tools.h"
#define ZLIB_WINAPI
#include"../madler-zlib/zlib.h"
#include "FFXIVDLL.h"
#include "MedianCalculator.h"

class FFXIVDLL;
class MedianCalculator;
class DPSWindowController;
class DOTWindowController;
class ChatWindowController;

struct PER_USER_DMG {
	int def;
	int crit;
	int ind;
};


#define NULL_ACTOR 0xE0000000
#define SOURCE_LIMIT_BREAK (NULL_ACTOR+1)

enum ACTOR_TYPE : uint8_t {
	ACTOR_TYPE_PC = 1,
	ACTOR_TYPE_MOB = 2,
	ACTOR_TYPE_NPC = 3,
	ACTOR_TYPE_AETHERLYTE = 5,
	ACTOR_TYPE_GATHERING = 6,
	ACTOR_TYPE_EOBJ = 7,
	ACTOR_TYPE_MINION = 8
};

enum ATTACK_DAMAGE_TYPE : uint8_t {
	DMG_Unknown = 0,
	DMG_Slashing = 1,
	DMG_Piercing = 2,
	DMG_Blunt = 3,
	DMG_Magic = 5,
	DMG_Darkness = 6,
	DMG_Physical = 7,
	DMG_LimitBreak = 8,
};

enum ATTACK_ELEMENT_TYPE : uint8_t {
	ELEM_Unknown,
	ELEM_Fire,
	ELEM_Ice,
	ELEM_Air,
	ELEM_Earth,
	ELEM_Lightning,
	ELEM_Water,
	ELEM_Unaspected,
};

struct ATTACK_INFO_EACH {
	char swingtype;
	union {
		struct {
			uint8_t buffAmount;
			uint8_t critAmount;
		};
		struct {
			ATTACK_DAMAGE_TYPE damagetype;
			ATTACK_ELEMENT_TYPE elementtype;
			char data1_rr;
		};
	};
	uint16_t damage;
	char _u4 : 6;
	char mult10 : 1;
	char _u1 : 1;
	char _u3;
};

struct ATTACK_INFO_EACH_V4 {
	char swingtype;
	union {
		struct {
			char isCrit : 1;
			char isDirectHit : 1;
			char _u0 : 6;
			ATTACK_ELEMENT_TYPE elementtype : 4;
			ATTACK_DAMAGE_TYPE damagetype : 4;
		};
		struct {
			uint8_t buffAmount;
			uint8_t critAmount;
		};
	};
	char _u1;
	uint16_t damage;
	char _u2 : 6;
	char mult10 : 1;
	char _u3 : 1;
	char _u4;
};

struct ATTACK_INFO {
	ATTACK_INFO_EACH attack[8];
};

struct ATTACK_INFO_V4 {
	ATTACK_INFO_EACH_V4 attack[8];
};

struct TEMPUSERINFO {
	std::string name;
	std::wstring job;
};

struct TEMPBUFF {
	int source;
	int target;
	int buffid;
	int potency;
	float critRate;
	uint64_t applied;
	uint64_t expires;
	int simulated : 1;
	int contagioned : 1;
};

struct TEMPDMG {
	uint64_t timestamp;
	int source;
	union {
		int skill;
		int buffId;
	};
	int amount;
	char isDoT : 1;
	char isCrit : 1;
	char isDirectHit : 1;
	char isMiss : 1;
	char _u0 : 4;
};

struct TARGET_STRUCT {
	uint32_t target;
	uint32_t _u1;
};

#pragma pack(push, 1)
struct GAME_MESSAGE {
	uint32_t length; // 0 ~ 3
	uint32_t actor; //Source // 4 ~ 7
	uint32_t actor_copy; //Target // 8 ~ 11
	uint32_t _u0;  //Subpackettype? // 12 ~ 15
	enum MESSAGE_CAT1 : uint16_t {
		C1_Combat = 0x0014
	} message_cat1; // 16 ~ 17
	enum MESSAGE_CAT2 : uint16_t {
		/* Unknown cat1s
		ZoneChange = 0xCF,
		FATE = 0x143,
		MatchStartSingle = 0x6C,
		MatchStartMultiple = 0x74,
		MatchStartDutyRoulette = 0x76,
		MatchStop = 0x2DB,
		MatchConfirmed = 0x6F,
		MatchStatusInfo = 0x2DE,
		MatchFound = 0x339
		*/
		CHAT_SHOUT = 0x67,
		C2_DelBuff = 0x00EC,
		C2_UseAbilityRequestV3 = 0xF7,
		C2_UseAbilityRequestV4 = 0x10B,
		C2_UseAbilityV4T1 = 0x00F1,
		C2_UseAbilityV4T8 = 0x00F4,
		C2_UseAbilityV4T16 = 0x00F5,
		C2_UseAbilityV4T24 = 0x00F6,
		C2_UseAbilityV4T32 = 0x00F7,
		C2_StartCasting = 0x0110,
		C2_SetSkillEnabledV3 = 0x0103,
		C2_SetSkillEnabledV4 = 0x0121,
		C2_AddBuff = 0x0141,
		C2_Info1 = 0x0142,
		C2_AbilityResponse = 0x0143,
		C2_TargetMarker = 0x0144,
		C2_ActorInfo = 0x0145,
		C2_UseAbility = 0x0146,
		C2_UseAoEAbility = 0x0147,
		C2_RaidMarker = 0x0335,
	} message_cat2; // 18 ~ 19
	uint32_t _u1; // 20 ~ 23
	uint32_t seqid; // 24 ~ 27
	uint32_t _u2; // 28 ~ 31
	union {
		uint8_t data[65536];
		union {
			struct {
				uint32_t _u1;
				uint32_t skill;
				uint32_t seqid;
				uint32_t _u2;
				uint32_t target;
			}AbilityRequest;
			struct {
				uint32_t u1;
				uint32_t u2;
				uint32_t skill;
				uint32_t duration_or_skid;
				uint32_t u3;
				uint32_t u4;
				uint32_t seqid;
				uint32_t u6;
			}AbilityResponse;
			struct {
				uint16_t c1; // 32
				uint16_t _u1;
				uint32_t c5; // 36
				uint32_t c2; // 40
				uint32_t c3; // 44
				uint32_t c4; // 48

							 /*
							 c1=23: dot(c2=3) hot(c2=4) / damage=c3 / skill=c5
							 c1=15: skill cancelled / skill id=c3 / cancelled(c4=0) interrupted(c4=1)
							 c1=6: death / by=c5
							 c1=34: target icon (?)
							 c1=21: buff remove / buff=c5
							 */
			} Info1;
			struct {
				uint8_t _u1[4]; // 32
				uint32_t target; // 36
				uint8_t _u2[4]; // 40
				uint32_t skill; // 44
			} StartCasting;
			struct {
				uint32_t primaryTarget; // 32
				uint8_t _u1[8]; // 36
				uint32_t skill; // 44
				uint32_t seqid; // 48
				uint8_t _u2[12]; // 52
				uint32_t target; // 64
				uint8_t _u3[8]; // 68
				ATTACK_INFO attack;
				uint8_t _u4[80];
			} UseAbility;
			struct {
				uint32_t primaryTarget; // 32
				uint8_t _u1[8]; // 36
				uint32_t skill; // 44
				uint32_t seqid; // 48
				uint8_t _u2[16]; // 52
				ATTACK_INFO attack[16]; // 68
				uint32_t _u3; // 1092
				TARGET_STRUCT targets[16]; // 1096
			} UseAoEAbility;
			struct {
				uint32_t primaryTarget; // 32
				uint32_t _u1; // 36
				uint32_t skill; // 40
				uint32_t seqid; // 44
				uint8_t _u2[19]; // 48
				uint8_t attackCount; // 67
				uint8_t _u3[4]; // 68
				ATTACK_INFO_V4 attack[8]; // 72
			} UseAoEAbilityV4;
			struct {
				uint8_t _u1[12]; // 32
				uint32_t HP; // 44
				uint16_t MP; // 48
				uint16_t TP; // 50
				uint32_t MaxHP; // 52
				uint16_t MaxMP; // 56
				uint16_t buff_count; // 58
				struct {
					// size 16
					uint16_t _u1;
					uint16_t buffID;
					uint16_t extra;
					uint16_t _u2;
					float duration;
					uint32_t actorID;
				}buffs[4]; // 60
			} AddBuff;
			struct {
				uint8_t _u1[4]; // 32
				uint32_t HP; // 36
				uint32_t MaxHP; // 40
				uint16_t MaxMP; // 44
				uint16_t MP; // 46
				uint16_t TP; // 48
				struct {
					// size 12
					uint16_t buffID;
					uint16_t extra;
					float duration;
					uint32_t actorID;
				}buffs[160]; // 52
			} DelBuff;
			struct {
				struct {
					uint32_t _u1[2];
					float x;
					float z;
					float y;
				} markers[3];
			} RaidMarker;
			struct {
				uint32_t _u1; // 32
				uint32_t markerType; // 36
				uint32_t actorId; // 40
				uint32_t _u2[3]; // 44
				uint32_t targetId; // 56
			} TargetMarker;
			struct {
				uint16_t HP;
				uint16_t _u1;
				uint16_t MP;
				uint16_t TP;
				uint16_t GP;
			} ActorInfo;
		} Combat;
	};
};


struct GAME_MESSAGE2 {
	uint32_t length; // 0 ~ 3
	uint32_t actor; //Source // 4 ~ 7
	uint32_t actor_copy; //Target // 8 ~ 11
	uint32_t _u0;  //Subpackettype? // 12 ~ 15
	
	//!!!!FFXIV_ACT_PLUGIN uses a uint32_t which eats both CAT1 and CAT2!!!
	//!!If looking at the FFXIV_ACT_PLUGIN Opcodes remember to split them into two uint16
	//enum MESSAGE_CAT1 : uint16_t {
	//	C1_Combat = 0x0014
	//} message_cat1; // 16 ~ 17
	//enum MESSAGE_CAT2 : uint16_t {
		/* Unknown cat1s
		ZoneChange = 0xCF,
		FATE = 0x143,
		MatchStartSingle = 0x6C,
		MatchStartMultiple = 0x74,
		MatchStartDutyRoulette = 0x76,
		MatchStop = 0x2DB,
		MatchConfirmed = 0x6F,
		MatchStatusInfo = 0x2DE,
		MatchFound = 0x339
		*/
	//	C2_DelBuff = 0x00EC,
	//	C2_UseAbilityRequestV3 = 0xF7,
	//	C2_UseAbilityRequestV4 = 0x10B,
	//	C2_UseAbilityV4T1 = 0x00F1,
	//	C2_UseAbilityV4T8 = 0x00F4,
	//	C2_UseAbilityV4T16 = 0x00F5,
	//	C2_UseAbilityV4T24 = 0x00F6,
	//	C2_UseAbilityV4T32 = 0x00F7,
	//	C2_StartCasting = 0x0110,
	//	C2_SetSkillEnabledV3 = 0x0103,
	//	C2_SetSkillEnabledV4 = 0x0121,
	//	C2_AddBuff = 0x0141,
	//	C2_Info1 = 0x0142,
	//	C2_AbilityResponse = 0x0143,
	//	C2_TargetMarker = 0x0144,
	//	C2_ActorInfo = 0x0145,
	//	C2_UseAbility = 0x0146,
	//	C2_UseAoEAbility = 0x0147,
	//	C2_RaidMarker = 0x0335,
	//} message_cat2; // 18 ~ 19
	//!!!!FFXIV_ACT_PLUGIN uses a uint32_t which eats both CAT1 and CAT2!!!
	//!!If looking at the FFXIV_ACT_PLUGIN Opcodes remember to split them into two uint16
	uint16_t test; //- This is 99% of the time for us 20
	uint16_t test2; // - Building a better list
	uint32_t _u1; // 20 ~ 23
	uint32_t seqid; // 24 ~ 27
	uint32_t _u2; // 28 ~ 31
	union {
		uint8_t data[65536];
		union {
			struct {
				uint32_t _u1;
				uint32_t skill;
				uint32_t seqid;
				uint32_t _u2;
				uint32_t target;
			}AbilityRequest;
			struct {
				uint32_t u1;
				uint32_t u2;
				uint32_t skill;
				uint32_t duration_or_skid;
				uint32_t u3;
				uint32_t u4;
				uint32_t seqid;
				uint32_t u6;
			}AbilityResponse;
			struct {
				uint16_t c1; // 32
				uint16_t _u1;
				uint32_t c5; // 36
				uint32_t c2; // 40
				uint32_t c3; // 44
				uint32_t c4; // 48

							 /*
							 c1=23: dot(c2=3) hot(c2=4) / damage=c3 / skill=c5
							 c1=15: skill cancelled / skill id=c3 / cancelled(c4=0) interrupted(c4=1)
							 c1=6: death / by=c5
							 c1=34: target icon (?)
							 c1=21: buff remove / buff=c5
							 */
			} Info1;
			struct {
				uint8_t _u1[4]; // 32
				uint32_t target; // 36
				uint8_t _u2[4]; // 40
				uint32_t skill; // 44
			} StartCasting;
			struct {
				uint32_t primaryTarget; // 32
				uint8_t _u1[8]; // 36
				uint32_t skill; // 44
				uint32_t seqid; // 48
				uint8_t _u2[12]; // 52
				uint32_t target; // 64
				uint8_t _u3[8]; // 68
				ATTACK_INFO attack;
				uint8_t _u4[80];
			} UseAbility;
			struct {
				uint32_t primaryTarget; // 32
				uint8_t _u1[8]; // 36
				uint32_t skill; // 44
				uint32_t seqid; // 48
				uint8_t _u2[16]; // 52
				ATTACK_INFO attack[16]; // 68
				uint32_t _u3; // 1092
				TARGET_STRUCT targets[16]; // 1096
			} UseAoEAbility;
			struct {
				uint32_t primaryTarget; // 32
				uint32_t _u1; // 36
				uint32_t skill; // 40
				uint32_t seqid; // 44
				uint8_t _u2[19]; // 48
				uint8_t attackCount; // 67
				uint8_t _u3[4]; // 68
				ATTACK_INFO_V4 attack[8]; // 72
			} UseAoEAbilityV4;
			struct {
				uint8_t _u1[12]; // 32
				uint32_t HP; // 44
				uint16_t MP; // 48
				uint16_t TP; // 50
				uint32_t MaxHP; // 52
				uint16_t MaxMP; // 56
				uint16_t buff_count; // 58
				struct {
					// size 16
					uint16_t _u1;
					uint16_t buffID;
					uint16_t extra;
					uint16_t _u2;
					float duration;
					uint32_t actorID;
				}buffs[4]; // 60
			} AddBuff;
			struct {
				uint8_t _u1[4]; // 32
				uint32_t HP; // 36
				uint32_t MaxHP; // 40
				uint16_t MaxMP; // 44
				uint16_t MP; // 46
				uint16_t TP; // 48
				struct {
					// size 12
					uint16_t buffID;
					uint16_t extra;
					float duration;
					uint32_t actorID;
				}buffs[160]; // 52
			} DelBuff;
			struct {
				struct {
					uint32_t _u1[2];
					float x;
					float z;
					float y;
				} markers[3];
			} RaidMarker;
			struct {
				uint32_t _u1; // 32
				uint32_t markerType; // 36
				uint32_t actorId; // 40
				uint32_t _u2[3]; // 44
				uint32_t targetId; // 56
			} TargetMarker;
			struct {
				uint16_t HP;
				uint16_t _u1;
				uint16_t MP;
				uint16_t TP;
				uint16_t GP;
			} ActorInfo;
		} Combat;
	};
};


struct GAME_PACKET {
	union {
		uint32_t signature;		// 0 ~ 3		0x41A05252
		uint64_t signature_2[2];
	};
	uint64_t timestamp;		// 16 ~ 23
	uint32_t length;		// 24 ~ 27
	uint16_t _u2;			//Connection Channel // 28 ~ 29 
	uint16_t message_count;	//SubPacket Count // 30 ~ 31
	uint8_t _u3;			// 32
	uint8_t is_gzip;		// 33
	uint16_t _u4;			// 34 ~ 35
	uint32_t _u5;			// 36 ~ 39;
	uint8_t data[1];
};
#pragma pack(pop)

#define EORZEA_CONSTANT 20.571428571428573

class GameDataProcess {
	friend class Hooks;
	friend class ImGuiConfigWindow;

private:

	int version;
	int getDoTPotency(int dot);
	int getDoTDuration(int skill);

	std::map<std::wstring, DWORD> mClassColors;

	std::recursive_mutex mSocketMapLock;
	std::map<SOCKET, Tools::ByteQueue> mSent, mRecv;
	std::map<SOCKET, Tools::ByteQueue> mToSend, mToRecv;
	std::map<SOCKET, Tools::bqueue<std::pair<uint64_t, GAME_MESSAGE>>> mRecvAdd;
	GAME_PACKET mInboundPacketTemplate;
	int mInboundSequenceId;

	uint64_t mLastSkillRequest;
	std::map<int, int> mCachedSkillCooldown;

	int mSelfId;
	std::deque<TEMPBUFF> mActiveDoT;
	TEMPDMG mLastAttack;
	std::recursive_mutex mActorMutex;
	std::map<int, int> mDamageRedir;
	std::map<int, PVOID> mActorPointers;
	struct DPS_METER_TEMP_INFO {
		PER_USER_DMG totalDamage;
		TEMPDMG maxDamage;
		int deaths;
		int dotHits;
		int totalHits;
		int critHits;
		int directHits;
		int missHits;
	};
	std::map<int, DPS_METER_TEMP_INFO> mDpsInfo;
	std::map<int, TEMPUSERINFO> mActorInfo;


	std::vector<std::pair<int, int>> mCalculatedDamages;
	uint64_t mLastIdleTime = 0;
	uint64_t mCombatResetTime = 10000;
	bool mShowTimeInfo = true;

	std::map<int, MedianCalculator> mDotApplyDelayEstimation;
	MedianCalculator mContagionApplyDelayEstimation;

	uint64_t mServerTimestamp;
	uint64_t mLocalTimestamp;
	uint64_t mLastRecv;

	FFXIVDLL *dll;
	HANDLE hUnloadEvent;

	bool mWindowsAdded = false;
	DPSWindowController &wDPS;
	DOTWindowController &wDOT;
	ChatWindowController &wChat;

	void ResolveUsers();
	void ResolveUsersInternal();
	bool IsParseTarget(uint32_t id);
	void AddDamageInfo(TEMPDMG dmg, bool direct);
	void CalculateDps(uint64_t timestamp);
	void UpdateOverlayMessage();
	void SimulateBane(uint64_t timestamp, uint32_t actor, int maxCount, TARGET_STRUCT* targets, ATTACK_INFO *attacks);
	void SimulateBane(uint64_t timestamp, uint32_t actor, int maxCount, TARGET_STRUCT* targets, ATTACK_INFO_V4 *attacks);

	void ProcessAttackInfo(int source, int target, int skill, ATTACK_INFO *info, uint64_t timestamp);
	void ProcessAttackInfo(int source, int target, int skill, ATTACK_INFO_V4 *info, uint64_t timestamp);
	void EmulateCancel(SOCKET s, int skillid, int newcd, int spent, int seqid, uint64_t when = GetTickCount64());
	void EmulateEnableSkill(SOCKET s, uint64_t when = GetTickCount64());
	void ProcessGameMessage(SOCKET s, GAME_MESSAGE *data, uint64_t timestamp, bool setTimestamp);
	void ParsePacket(Tools::ByteQueue &in, Tools::ByteQueue &out, SOCKET s, bool setTimestamp);
	void TryParsePacket(Tools::ByteQueue &in, Tools::ByteQueue &out, SOCKET s, bool setTimestamp);

	HANDLE hRecvUpdateInfoThread;
	HANDLE hRecvEvent;
	void RecvUpdateInfoThread();
	static DWORD WINAPI RecvUpdateInfoThreadExternal(PVOID p) { ((GameDataProcess*) p)->RecvUpdateInfoThread(); return 0; }

	HANDLE hSendUpdateInfoThread;
	HANDLE hSendEvent;
	void SendUpdateInfoThread();
	static DWORD WINAPI SendUpdateInfoThreadExternal(PVOID p) { ((GameDataProcess*) p)->SendUpdateInfoThread(); return 0; }

public:
	GameDataProcess(FFXIVDLL *dll, HANDLE unloadEvent);
	~GameDataProcess();

	inline int GetActorType(int id);
	inline int GetTargetId(int type);
	inline std::string GetActorName(int id);
	inline TCHAR* GetActorJobString(int id);

	void ResetMeter();
	void ReloadLocalization();

	bool IsInCombat();
	int GetVersion();

	void OnRecv(SOCKET s, char* buf, int len) {
		Tools::ByteQueue &bq = mRecv[s];
		bq.write(buf, len);
		if (bq.getUsed() >= 28)
			SetEvent(hRecvEvent);
	}

	void OnSend(SOCKET s, const char* buf, int len) {
		Tools::ByteQueue &bq = mSent[s];
		bq.write(buf, len);
		if (bq.getUsed() >= 28)
			SetEvent(hSendEvent);
	}
};