﻿// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "misc.h"
#include "fakelag.h"
#include "..\ragebot\aim.h"
#include "..\visuals\world_esp.h"
#include "prediction_system.h"
#include "logs.h"
int pre_flags = 0;

void misc::edgebug(CUserCmd* pCmd) {

	if (g_ctx.local()->get_move_type() == MOVETYPE_LADDER || g_ctx.local()->get_move_type() == MOVETYPE_NOCLIP) return;

	if (engineprediction::get().backup_data.flags & FL_ONGROUND && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
		pCmd->m_buttons |= IN_DUCK;

}
bool jumpbugged = false;

void misc::jumpbug(CUserCmd* pCmd) {

	auto poop = g_ctx.local()->m_fFlags();
	bool unduck = false;
	static bool niggacheck = false;

	if ((g_ctx.local()->m_fFlags() & (1 << 0)) && !(engineprediction::get().backup_data.flags & (1 << 0))) {

		if (g_cfg.misc.bunnyhop) {
			g_cfg.misc.bunnyhop = false;
			niggacheck = true;
		}

		if (unduck) {
			pCmd->m_buttons &= ~IN_DUCK;
			pCmd->m_buttons |= IN_JUMP;
			unduck = false;
			jumpbugged = true;
		}

		pCmd->m_buttons |= IN_DUCK;
		pCmd->m_buttons &= ~IN_JUMP;
		unduck = true;
		jumpbugged = false;
	}
	else if (niggacheck) {
		g_cfg.misc.bunnyhop = true;
		niggacheck = false;
	}

}
void misc::watermark()
{
	if (!g_cfg.menu.watermark)
		return;

	auto width = 0, height = 0;

	std::string name_cheat;

	std::string fps;

	std::string ping;

	std::string ticks;

	std::string time;

	m_engine()->GetScreenSize(width, height); 

	if (g_cfg.menu.watermark_addr[CHEAT])
	{
		if (BETA)
			name_cheat = crypt_str("Medusa.uno [beta]");
		else
			name_cheat = crypt_str("Medusa.uno");
	}
	else
		name_cheat = crypt_str("");

	if (g_cfg.menu.watermark_addr[FPS])
	{
		fps = crypt_str(" | fps: ") + std::to_string(g_ctx.globals.framerate);
	}
	else
		fps = crypt_str("");

	if (g_cfg.menu.watermark_addr[PING])
	{
		ping = crypt_str(" | ping: ") + std::to_string(g_ctx.globals.ping);
	}
	else
		ping = crypt_str("");

	if (g_cfg.menu.watermark_addr[TICKS])
	{
		ticks = crypt_str(" | ticks: ") + std::to_string(g_ctx.globals.ticks_allowed);
	}
	else
		ticks = crypt_str("");

	if (g_cfg.menu.watermark_addr[TIME])
	{
		time = crypt_str(" | time: " + g_ctx.globals.time);
	}
	else
		time = crypt_str("");

	auto watermark = crypt_str(name_cheat) + crypt_str(fps) + crypt_str(ping) + crypt_str(ticks) + crypt_str(time);

	auto box_width = render::get().text_width(fonts[KEYBINDSLIST], watermark.c_str()) + 10;

	render::get().rect_filled(width - 10 - box_width, 10, box_width, 2, Color(147, 190, 66,255));
	render::get().rect_filled(width - 10 - box_width, 12, box_width, 18, Color(32, 32, 32,255));

	render::get().text(fonts[KEYBINDSLIST], width - 10 - box_width + 5, 21, Color(255, 255, 255, 220), HFONT_CENTERED_Y, watermark.c_str());
}

void misc::NoDuck(CUserCmd* cmd)
{
	if (!g_cfg.misc.noduck)
		return;

	if (m_gamerules()->m_bIsValveDS())
		return;

	cmd->m_buttons |= IN_BULLRUSH;
}

void misc::ChangeRegion()
{
	switch (g_cfg.misc.region_changer) {
	case 0:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster waw");
		break;
	case 1:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster atl");
		break;
	case 2:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster bom");
		break;
	case 3:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster can");
		break;
	case 4:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster canm");
		break;
	case 5:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster cant");
		break;
	case 6:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster canu");
		break;
	case 7:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster dxb");
		break;
	case 8:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster eat");
		break;
	case 9:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster fra");
		break;
	case 10:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster gru");
		break;
	case 11:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster hkg");
		break;
	case 12:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster iad");
		break;
	case 13:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster jnb");
		break;
	case 14:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lax");
		break;
	case 15:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lhr");
		break;
	case 16:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lim");
		break;
	case 17:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lux");
		break;
	case 18:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster maa");
		break;
	case 19:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster mad");
		break;
	case 20:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster man");
		break;
	case 21:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster okc");
		break;
	case 22:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster ord");
		break;
	case 23:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster par");
		break;
	case 24:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwg");
		break;
	case 25:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwj");
		break;
	case 26:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwu");
		break;
	case 27:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pww");
		break;
	case 28:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwz");
		break;
	case 29:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster scl");
		break;
	case 30:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sea");
		break;
	case 31:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sgp");
		break;
	case 32:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sha");
		break;
	case 33:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sham");
		break;
	case 34:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shat");
		break;
	case 35:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shau");
		break;
	case 36:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shb");
		break;
	case 37:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sto");
		break;
	case 38:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sto2");
		break;
	case 39:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster syd");
		break;
	case 40:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsn");
		break;
	case 41:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnm");
		break;
	case 42:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnt");
		break;
	case 43:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnu");
		break;
	case 44:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tyo");
		break;
	case 45:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tyo1");
		break;
	case 46:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster vie");
		break;
	case 47:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster ams");
		break;
	}
}

void misc::ChatSpamer()
{
	if (!g_cfg.misc.chat)
		return;

	static std::string chatspam[] = 
	{ 
		crypt_str("Medusa.uno, Hi im a loser."),
	};

	static auto lastspammed = 0;

	if (GetTickCount() - lastspammed > 800)
	{
		lastspammed = GetTickCount();

		srand(m_globals()->m_tickcount);
		std::string msg = crypt_str("say ") + chatspam[rand() % 4];

		m_engine()->ExecuteClientCmd(msg.c_str());
	}
}

void misc::AutoCrouch(CUserCmd* cmd)
{
	if (g_cfg.ragebot.fake_duck_bypass) /* Big steppa FD */
	{
		if (fakelag::get().condition)
		{
			g_ctx.globals.fakeducking = false;
			return;
		}

		if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
		{
			g_ctx.globals.fakeducking = false;
			return;
		}

		if (m_gamerules()->m_bIsValveDS())
		{
			g_ctx.globals.fakeducking = false;
			return;
		}

		if (!key_binds::get().get_key_bind_state(20))
		{
			g_ctx.globals.fakeducking = false;
			return;
		}

		if (!g_ctx.globals.fakeducking && m_clientstate()->iChokedCommands != 4)
			return;

		if (m_clientstate()->iChokedCommands >= 4)
			cmd->m_buttons |= IN_DUCK;
		else
			cmd->m_buttons &= ~IN_DUCK;

		g_ctx.globals.fakeducking = true;
	}

	else /* Normal fakeduck */

	{
		if (fakelag::get().condition)
		{
			g_ctx.globals.fakeducking = false;
			return;
		}

		if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
		{
			g_ctx.globals.fakeducking = false;
			return;
		}

		if (m_gamerules()->m_bIsValveDS())
		{
			g_ctx.globals.fakeducking = false;
			return;
		}

		if (!key_binds::get().get_key_bind_state(20))
		{
			g_ctx.globals.fakeducking = false;
			return;
		}

		if (!g_ctx.globals.fakeducking && m_clientstate()->iChokedCommands != 7)
			return;

		if (m_clientstate()->iChokedCommands >= 7)
			cmd->m_buttons |= IN_DUCK;
		else
			cmd->m_buttons &= ~IN_DUCK;

		g_ctx.globals.fakeducking = true;
	}
}

void misc::SlideWalk(CUserCmd* cmd)
{
	int leg_movement = g_cfg.misc.leg_movement;

	if (!g_ctx.local()->is_alive()) 
		return;

	if (g_ctx.local()->get_move_type() == MOVETYPE_LADDER)
		return;

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
		return;

	if (leg_movement == 2)
	{
		leg_movement = 1;

		if (antiaim::get().condition(cmd, true) && leg_movement == 1)
		{
			if (cmd->m_forwardmove > 0.0f)
			{
				cmd->m_buttons |= IN_BACK;
				cmd->m_buttons &= ~IN_FORWARD;
			}
			else if (cmd->m_forwardmove < 0.0f)
			{
				cmd->m_buttons |= IN_FORWARD;
				cmd->m_buttons &= ~IN_BACK;
			}

			if (cmd->m_sidemove > 0.0f)
			{
				cmd->m_buttons |= IN_MOVELEFT;
				cmd->m_buttons &= ~IN_MOVERIGHT;
			}
			else if (cmd->m_sidemove < 0.0f)
			{
				cmd->m_buttons |= IN_MOVERIGHT;
				cmd->m_buttons &= ~IN_MOVELEFT;
			}

			leg_movement = 0;
		}
		else
		{
			leg_movement = 1;

			auto buttons = cmd->m_buttons & ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD);

			if (leg_movement == 1)
			{
				if (cmd->m_forwardmove <= 0.0f)
					buttons |= IN_BACK;
				else
					buttons |= IN_FORWARD;

				if (cmd->m_sidemove > 0.0f)
					goto LABEL_15;
				else if (cmd->m_sidemove >= 0.0f)
					goto LABEL_18;

				goto LABEL_17;
			}
			else
				goto LABEL_18;

			if (cmd->m_forwardmove <= 0.0f) //-V779
				buttons |= IN_FORWARD;
			else
				buttons |= IN_BACK;

			if (cmd->m_sidemove > 0.0f)
			{
			LABEL_17:
				buttons |= IN_MOVELEFT;
				goto LABEL_18;
			}

			if (cmd->m_sidemove < 0.0f)
				LABEL_15:

			buttons |= IN_MOVERIGHT;

		LABEL_18:
			cmd->m_buttons = buttons;

			leg_movement = 0;
		}

		leg_movement = 1;
	}
	else
	{
		if (antiaim::get().condition(cmd, true) && leg_movement == 1)
		{
			if (cmd->m_forwardmove > 0.0f)
			{
				cmd->m_buttons |= IN_BACK;
				cmd->m_buttons &= ~IN_FORWARD;
			}
			else if (cmd->m_forwardmove < 0.0f)
			{
				cmd->m_buttons |= IN_FORWARD;
				cmd->m_buttons &= ~IN_BACK;
			}

			if (cmd->m_sidemove > 0.0f)
			{
				cmd->m_buttons |= IN_MOVELEFT;
				cmd->m_buttons &= ~IN_MOVERIGHT;
			}
			else if (cmd->m_sidemove < 0.0f)
			{
				cmd->m_buttons |= IN_MOVERIGHT;
				cmd->m_buttons &= ~IN_MOVELEFT;
			}
		}
		else
		{
			auto buttons = cmd->m_buttons & ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD);

			if (leg_movement == 1)
			{
				if (cmd->m_forwardmove <= 0.0f)
					buttons |= IN_BACK;
				else
					buttons |= IN_FORWARD;

				if (cmd->m_sidemove > 0.0f)
					goto LABEL_42;
				else if (cmd->m_sidemove >= 0.0f)
					goto LABEL_65;

				goto LABEL_32;
			}
			else
				goto LABEL_65;

			if (cmd->m_forwardmove <= 0.0f) //-V779
				buttons |= IN_FORWARD;
			else
				buttons |= IN_BACK;

			if (cmd->m_sidemove > 0.0f)
			{
			LABEL_32:
				buttons |= IN_MOVELEFT;
				goto LABEL_65;
			}

			if (cmd->m_sidemove < 0.0f)
				LABEL_42:

			buttons |= IN_MOVERIGHT;

		LABEL_65:
			cmd->m_buttons = buttons;
		}
	}
}

void misc::automatic_peek(CUserCmd* cmd, float wish_yaw)
{
	if (!g_ctx.globals.weapon->is_non_aim() && key_binds::get().get_key_bind_state(18))
	{
		if (g_ctx.globals.start_position.IsZero())
		{
			g_ctx.globals.start_position = g_ctx.local()->GetAbsOrigin();

			if (!(engineprediction::get().backup_data.flags & FL_ONGROUND))
			{
				Ray_t ray;
				CTraceFilterWorldAndPropsOnly filter;
				CGameTrace trace;

				ray.Init(g_ctx.globals.start_position, g_ctx.globals.start_position - Vector(0.0f, 0.0f, 1000.0f));
				m_trace()->TraceRay(ray, MASK_SOLID, &filter, &trace);

				if (trace.fraction < 1.0f)
					g_ctx.globals.start_position = trace.endpos + Vector(0.0f, 0.0f, 2.0f);
			}
		}
		else
		{
			auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (cmd->m_buttons & IN_ATTACK || cmd->m_buttons & IN_ATTACK2);

			if (cmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
				g_ctx.globals.fired_shot = true;

			if (g_ctx.globals.fired_shot)
			{
				auto current_position = g_ctx.local()->GetAbsOrigin();
				auto difference = current_position - g_ctx.globals.start_position;

				if (difference.Length2D() > 5.0f)
				{
					auto velocity = Vector(difference.x * cos(wish_yaw / 180.0f * M_PI) + difference.y * sin(wish_yaw / 180.0f * M_PI), difference.y * cos(wish_yaw / 180.0f * M_PI) - difference.x * sin(wish_yaw / 180.0f * M_PI), difference.z);

					cmd->m_forwardmove = -velocity.x * 20.0f;
					cmd->m_sidemove = velocity.y * 20.0f;
				}
				else
				{
					g_ctx.globals.fired_shot = false;
					g_ctx.globals.start_position.Zero();
				}
			}
		}
	}
	else
	{
		g_ctx.globals.fired_shot = false;
		g_ctx.globals.start_position.Zero();
	}
}

void misc::ViewModel()
{
	if (g_cfg.esp.viewmodel_fov)
	{
		auto viewFOV = (float)g_cfg.esp.viewmodel_fov + 68.0f;
		static auto viewFOVcvar = m_cvar()->FindVar(crypt_str("viewmodel_fov"));

		if (viewFOVcvar->GetFloat() != viewFOV) //-V550
		{
			*(float*)((DWORD)&viewFOVcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewFOVcvar->SetValue(viewFOV);
		}
	}
	
	if (g_cfg.esp.viewmodel_x)
	{
		auto viewX = (float)g_cfg.esp.viewmodel_x / 2.0f;
		static auto viewXcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_x")); 

		if (viewXcvar->GetFloat() != viewX) //-V550
		{
			*(float*)((DWORD)&viewXcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewXcvar->SetValue(viewX);
		}
	}

	if (g_cfg.esp.viewmodel_y)
	{
		auto viewY = (float)g_cfg.esp.viewmodel_y / 2.0f;
		static auto viewYcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_y"));

		if (viewYcvar->GetFloat() != viewY) //-V550
		{
			*(float*)((DWORD)&viewYcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewYcvar->SetValue(viewY);
		}
	}

	if (g_cfg.esp.viewmodel_z)
	{
		auto viewZ = (float)g_cfg.esp.viewmodel_z / 2.0f;
		static auto viewZcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_z"));

		if (viewZcvar->GetFloat() != viewZ) //-V550
		{
			*(float*)((DWORD)&viewZcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewZcvar->SetValue(viewZ);
		}
	}
}

void misc::FullBright()
{		
	if (!g_cfg.player.enable)
		return;

	static auto mat_fullbright = m_cvar()->FindVar(crypt_str("mat_fullbright"));

	if (mat_fullbright->GetBool() != g_cfg.esp.bright)
		mat_fullbright->SetValue(g_cfg.esp.bright);
}

void FX_Tesla(CTeslaInfo& pInfo)
{
	using FX_TeslaFn = void(__thiscall*)(CTeslaInfo&);
	static FX_TeslaFn pEffects = (FX_TeslaFn)util::FindSignature("client.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 8B 47 18");
	if (!pEffects)
		return;

	pEffects(pInfo);
}

void misc::KillEffect(IGameEvent* pEvent)
{
	player_t* pEntity = (player_t*)m_entitylist()->GetClientEntity(m_engine()->GetPlayerForUserID(pEvent->GetInt(("userid"))));
	player_t* pAttacker = (player_t*)m_entitylist()->GetClientEntity(m_engine()->GetPlayerForUserID(pEvent->GetInt(("attacker"))));

	if (!g_cfg.esp.lightingonshot)
		return;

	if (pEntity == g_ctx.local())
		return;

	if (pEntity->EntIndex() <= 0 || pEntity->EntIndex() > 64)
		return;

	if (!pAttacker || !pEntity)
		return;

	if (pAttacker != g_ctx.local())
		return;

	CTeslaInfo teslaInfo;
	teslaInfo.m_flBeamWidth = 10.f;
	teslaInfo.m_flRadius = 500.f;
	teslaInfo.m_nEntIndex = pEntity->EntIndex();
	teslaInfo.m_vColor.Init(1.f, 1.f, 1.f);
	teslaInfo.m_vPos = pEntity->hitbox_position(8);
	teslaInfo.m_flTimeVisible = 0.75f;
	teslaInfo.m_nBeams = 12;
	teslaInfo.m_pszSpriteName = "sprites/physbeam.vmt";

	FX_Tesla(teslaInfo);
}

void misc::PovArrows(player_t* e, Color color)
{
	auto isOnScreen = [](Vector origin, Vector& screen) -> bool
	{
		if (!math::world_to_screen(origin, screen))
			return false;

		static int iScreenWidth, iScreenHeight;
		m_engine()->GetScreenSize(iScreenWidth, iScreenHeight);

		auto xOk = iScreenWidth > screen.x; 
		auto yOk = iScreenHeight > screen.y;

		return xOk && yOk;
	};

	Vector screenPos;

	if (isOnScreen(e->GetAbsOrigin(), screenPos))
		return;

	Vector viewAngles;
	m_engine()->GetViewAngles(viewAngles);

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto screenCenter = Vector2D(width * 0.5f, height * 0.5f);
	auto angleYawRad = DEG2RAD(viewAngles.y - math::calculate_angle(g_ctx.globals.eye_pos, e->GetAbsOrigin()).y - 90.0f);

	auto radius = g_cfg.player.distance;
	auto size = g_cfg.player.size;

	auto newPointX = screenCenter.x + ((((width - (size * 3)) * 0.5f) * (radius / 100.0f)) * cos(angleYawRad)) + (int)(6.0f * (((float)size - 4.0f) / 16.0f));
	auto newPointY = screenCenter.y + ((((height - (size * 3)) * 0.5f) * (radius / 100.0f)) * sin(angleYawRad));

	std::array <Vector2D, 3> points
	{
		Vector2D(newPointX - size, newPointY - size),
		Vector2D(newPointX + size, newPointY),
		Vector2D(newPointX - size, newPointY + size)
	};

	math::rotate_triangle(points, viewAngles.y - math::calculate_angle(g_ctx.globals.eye_pos, e->GetAbsOrigin()).y - 90.0f);
	render::get().triangle(points.at(0), points.at(1), points.at(2), color);
}

void misc::zeus_range()
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.taser_range)
		return;

	if (!m_input()->m_fCameraInThirdPerson)
		return;

	if (!g_ctx.local()->is_alive())  
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() != WEAPON_TASER)
		return;

	auto weapon_info = weapon->get_csweapon_info();

	if (!weapon_info)
		return;

	render::get().Draw3DRainbowCircle(g_ctx.local()->get_shoot_position(), weapon_info->flRange);
}

void misc::NightmodeFix()
{
	static auto in_game = false;

	if (m_engine()->IsInGame() && !in_game)
	{
		in_game = true;

		g_ctx.globals.change_materials = true;
		worldesp::get().changed = true;

		static auto skybox = m_cvar()->FindVar(crypt_str("sv_skyname"));
		worldesp::get().backup_skybox = skybox->GetString();
		return;
	}
	else if (!m_engine()->IsInGame() && in_game)
		in_game = false;

	static auto player_enable = g_cfg.player.enable;

	if (player_enable != g_cfg.player.enable)
	{
		player_enable = g_cfg.player.enable;
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting = g_cfg.esp.nightmode;

	if (setting != g_cfg.esp.nightmode)
	{
		setting = g_cfg.esp.nightmode;
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting_world = g_cfg.esp.world_color;

	if (setting_world != g_cfg.esp.world_color)
	{
		setting_world = g_cfg.esp.world_color;
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting_props = g_cfg.esp.props_color;

	if (setting_props != g_cfg.esp.props_color)
	{
		setting_props = g_cfg.esp.props_color;
		g_ctx.globals.change_materials = true;
	}
}

void misc::desync_arrows()
{
	if (!g_ctx.local()->is_alive())
		return;

	if (!g_cfg.antiaim.enable)
		return;

	if ((g_cfg.antiaim.manual_back.key <= KEY_NONE || g_cfg.antiaim.manual_back.key >= KEY_MAX) && (g_cfg.antiaim.manual_left.key <= KEY_NONE || g_cfg.antiaim.manual_left.key >= KEY_MAX) && (g_cfg.antiaim.manual_right.key <= KEY_NONE || g_cfg.antiaim.manual_right.key >= KEY_MAX))
		antiaim::get().manual_side = SIDE_NONE;

	if (!g_cfg.antiaim.flip_indicator)
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	static auto alpha = 1.0f;
	static auto switch_alpha = false;

	if (alpha <= 0.0f || alpha >= 1.0f)
		switch_alpha = !switch_alpha;

	alpha += switch_alpha ? 2.0f * m_globals()->m_frametime : -2.0f * m_globals()->m_frametime;
	alpha = math::clamp(alpha, 0.0f, 1.0f);

	auto color = g_cfg.antiaim.flip_indicator_color;
	color.SetAlpha(g_cfg.antiaim.flip_indicator_color.a());

	if (antiaim::get().manual_side == SIDE_BACK) {
		render::get().triangle(Vector2D(width / 2, height / 2 + 80), Vector2D(width / 2 - 10, height / 2 + 60), Vector2D(width / 2 + 10, height / 2 + 60), color);
		render::get().triangle_def(Vector2D(width / 2, height / 2 + 80), Vector2D(width / 2 - 10, height / 2 + 60), Vector2D(width / 2 + 10, height / 2 + 60), Color(color.r(), color.g(), color.b(), 255));
	}
	else if (antiaim::get().manual_side == SIDE_LEFT) {
		render::get().triangle(Vector2D(width / 2 - 55, height / 2 + 10), Vector2D(width / 2 - 75, height / 2), Vector2D(width / 2 - 55, height / 2 - 10), color);
		render::get().triangle_def(Vector2D(width / 2 - 55, height / 2 + 10), Vector2D(width / 2 - 75, height / 2), Vector2D(width / 2 - 55, height / 2 - 10), Color(color.r(), color.g(), color.b(), 255));
	}
	else if (antiaim::get().manual_side == SIDE_RIGHT) {
		render::get().triangle(Vector2D(width / 2 + 55, height / 2 - 10), Vector2D(width / 2 + 75, height / 2), Vector2D(width / 2 + 55, height / 2 + 10), color);
		render::get().triangle_def(Vector2D(width / 2 + 55, height / 2 - 10), Vector2D(width / 2 + 75, height / 2), Vector2D(width / 2 + 55, height / 2 + 10), Color(color.r(), color.g(), color.b(), 255));
	}
	if (g_cfg.antiaim.desync == 2) {
		if (antiaim::get().flip == false) {
			render::get().rect(width / 2 - 10, height / 2 + 46, 20, 4, Color(color.r(), color.g(), color.b(), 255));
			render::get().rect_filled(width / 2 - 10, height / 2 + 46, 20, 4, Color(color));
		}
	}
	else
	{
		if (antiaim::get().flip == false) {
			render::get().rect(width / 2 - 50, height / 2 - 10, 4, 20, Color(color.r(), color.g(), color.b(), 255));
			render::get().rect_filled(width / 2 - 50, height / 2 - 10, 4, 20, Color(color));
		}
		else if (antiaim::get().flip == true)
		{
			render::get().rect(width / 2 + 46, height / 2 - 10, 4, 20, Color(color.r(), color.g(), color.b(), 255));
			render::get().rect_filled(width / 2 + 46, height / 2 - 10, 4, 20, Color(color));
		}
	}
}

void misc::aimbot_hitboxes()
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.player.lag_hitbox)
		return;

	auto player = (player_t*)m_entitylist()->GetClientEntity(aim::get().last_target_index);

	if (!player)
		return;

	auto model = player->GetModel(); 

	if (!model)
		return;

	auto studio_model = m_modelinfo()->GetStudioModel(model);

	if (!studio_model)
		return;

	auto hitbox_set = studio_model->pHitboxSet(player->m_nHitboxSet());

	if (!hitbox_set)
		return;

	for (auto i = 0; i < hitbox_set->numhitboxes; i++)
	{
		auto hitbox = hitbox_set->pHitbox(i);

		if (!hitbox)
			continue;

		if (hitbox->radius == -1.0f) //-V550
			continue;

		auto min = ZERO;
		auto max = ZERO;

		math::vector_transform(hitbox->bbmin, aim::get().last_target[aim::get().last_target_index].record.matrixes_data.main[hitbox->bone], min);
		math::vector_transform(hitbox->bbmax, aim::get().last_target[aim::get().last_target_index].record.matrixes_data.main[hitbox->bone], max);

		m_debugoverlay()->AddCapsuleOverlay(min, max, hitbox->radius, g_cfg.player.lag_hitbox_color.r(), g_cfg.player.lag_hitbox_color.g(), g_cfg.player.lag_hitbox_color.b(), g_cfg.player.lag_hitbox_color.a(), 4.0f, 0, 1);
	}
}

void misc::ragdolls()
{
	if (!g_cfg.misc.ragdolls)
		return;

	for (auto i = 1; i <= m_entitylist()->GetHighestEntityIndex(); ++i)
	{
		auto e = static_cast<entity_t*>(m_entitylist()->GetClientEntity(i));

		if (!e)
			continue;

		if (e->IsDormant())
			continue;

		auto client_class = e->GetClientClass();

		if (!client_class)
			continue;

		if (client_class->m_ClassID != CCSRagdoll)
			continue;

		auto ragdoll = (ragdoll_t*)e;
		ragdoll->m_vecForce().z = 800000.0f;
	}
}

void misc::rank_reveal()
{
	if (!g_cfg.misc.rank_reveal)
		return;

	using RankReveal_t = bool(__cdecl*)(int*);
	static auto Fn = (RankReveal_t)(util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 51 A1 ? ? ? ? 85 C0 75 37")));

	int array[3] = 
	{
		0,
		0,
		0
	};

	Fn(array);
}

void misc::fast_stop(CUserCmd* m_pcmd)
{
	if (!g_cfg.misc.fast_stop)
		return;

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
		return;

	auto pressed_move_key = m_pcmd->m_buttons & IN_FORWARD || m_pcmd->m_buttons & IN_MOVELEFT || m_pcmd->m_buttons & IN_BACK || m_pcmd->m_buttons & IN_MOVERIGHT || m_pcmd->m_buttons & IN_JUMP;

	if (pressed_move_key)
		return;

	if (!((antiaim::get().type == ANTIAIM_LEGIT ? g_cfg.antiaim.desync : g_cfg.antiaim.type[antiaim::get().type].desync) && (antiaim::get().type == ANTIAIM_LEGIT ? !g_cfg.antiaim.legit_lby_type : !g_cfg.antiaim.lby_type) && (!g_ctx.globals.weapon->is_grenade() || g_cfg.esp.on_click & !(m_pcmd->m_buttons & IN_ATTACK) && !(m_pcmd->m_buttons & IN_ATTACK2))) || antiaim::get().condition(m_pcmd)) //-V648
	{
		auto velocity = g_ctx.local()->m_vecVelocity();

		if (velocity.Length2D() > 20.0f)
		{
			Vector direction;
			Vector real_view;

			math::vector_angles(velocity, direction);
			m_engine()->GetViewAngles(real_view);

			direction.y = real_view.y - direction.y;

			Vector forward;
			math::angle_vectors(direction, forward);

			static auto cl_forwardspeed = m_cvar()->FindVar(crypt_str("cl_forwardspeed"));
			static auto cl_sidespeed = m_cvar()->FindVar(crypt_str("cl_sidespeed"));

			auto negative_forward_speed = -cl_forwardspeed->GetFloat();
			auto negative_side_speed = -cl_sidespeed->GetFloat();

			auto negative_forward_direction = forward * negative_forward_speed;
			auto negative_side_direction = forward * negative_side_speed;

			m_pcmd->m_forwardmove = negative_forward_direction.x;
			m_pcmd->m_sidemove = negative_side_direction.y;
		}
	}
	else
	{
		auto velocity = g_ctx.local()->m_vecVelocity();

		if (velocity.Length2D() > 20.0f)
		{
			Vector direction;
			Vector real_view;

			math::vector_angles(velocity, direction);
			m_engine()->GetViewAngles(real_view);

			direction.y = real_view.y - direction.y;

			Vector forward;
			math::angle_vectors(direction, forward);

			static auto cl_forwardspeed = m_cvar()->FindVar(crypt_str("cl_forwardspeed"));
			static auto cl_sidespeed = m_cvar()->FindVar(crypt_str("cl_sidespeed"));

			auto negative_forward_speed = -cl_forwardspeed->GetFloat();
			auto negative_side_speed = -cl_sidespeed->GetFloat();

			auto negative_forward_direction = forward * negative_forward_speed;
			auto negative_side_direction = forward * negative_side_speed;

			m_pcmd->m_forwardmove = negative_forward_direction.x;
			m_pcmd->m_sidemove = negative_side_direction.y;
		}
		else
		{
			auto speed = 1.01f;

			if (m_pcmd->m_buttons & IN_DUCK || g_ctx.globals.fakeducking)
				speed *= 2.94117647f;

			static auto switch_move = false;

			if (switch_move)
				m_pcmd->m_sidemove += speed;
			else
				m_pcmd->m_sidemove -= speed;

			switch_move = !switch_move;
		}
	}
}

void misc::anti_anti_defensive(CUserCmd* m_pcmd) // Does not work. i will fix it
{
	if (!g_ctx.local()->is_alive())
		return;

	fakelag fl;
	auto choked = m_clientstate()->iChokedCommands;
	auto flags = engineprediction::get().backup_data.flags;
	auto velocity = engineprediction::get().backup_data.velocity.Length(); 
	auto velocity2d = engineprediction::get().backup_data.velocity.Length2D();
	auto next_command_number = m_pcmd->m_command_number + 1;
	auto user_cmd = m_input()->GetUserCmd(next_command_number);
	bool started_peeking = false;

	if (g_cfg.ragebot.anti_anti_defensive && g_cfg.ragebot.enable && g_ctx.globals.current_weapon != -1 && !started_peeking && velocity >= 5.0f)
	{
		auto predicted_eye_pos = g_ctx.globals.eye_pos + engineprediction::get().backup_data.velocity * m_globals()->m_intervalpertick * (float)g_cfg.antiaim.triggers_fakelag_amount * 1.0f;

		for (auto i = 1; i < m_globals()->m_maxclients; i++)
		{
			auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

			if (!e->valid(true))
				continue;

			auto records = &player_records[i]; //-V826

			if (records->empty())
				continue;

			auto record = &records->front();

			if (!record->valid())
				continue;

			scan_data predicted_data;
			aim::get().scan(record, predicted_data, predicted_eye_pos);

			if (predicted_data.valid())
			{
				scan_data data;
				aim::get().scan(record, data, g_ctx.globals.eye_pos);

				if (!data.valid())
				{
					misc::teleport(m_pcmd);
					return;
				}
			}
		}
	}
}

void misc::spectators_list()
{
	if (!g_cfg.misc.spectators_list)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	std::vector <std::string> spectators;

	for (int i = 1; i < m_globals()->m_maxclients; i++)
	{
		auto e = static_cast<player_t *>(m_entitylist()->GetClientEntity(i));

		if (!e)
			continue;

		if (e->is_alive())
			continue;

		if (e->IsDormant())
			continue;

		if (e->m_hObserverTarget().Get() != g_ctx.local())
			continue;

		player_info_t player_info;
		m_engine()->GetPlayerInfo(i, &player_info);

		spectators.push_back(player_info.szName);
	}

	for (auto i = 0; i < spectators.size(); i++)
	{
		int width, heigth;
		m_engine()->GetScreenSize(width, heigth);

		auto x = render::get().text_width(fonts[LOGS], spectators.at(i).c_str()) + 6; //-V106
		auto y = i * 16;

		render::get().text(fonts[LOGS], width - x, g_cfg.menu.watermark ? y + 30 : y + 6, Color::White, HFONT_CENTERED_NONE, spectators.at(i).c_str()); //-V106
	}
}

bool misc::teleport(CUserCmd* m_pcmd)
{
	teleport_enabled = true;

	static auto recharge_teleport = g_cfg.ragebot.recharge_dt;
	const auto weapon_knife = g_ctx.globals.weapon->is_knife();
	static auto last_teleport = 0;
	bool remove_knife = false;

	if (recharge_teleport)
	{
		recharge_teleport = false;
		recharging_double_tap = true;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (recharging_double_tap)
	{
		if (g_cfg.ragebot.double_tap && g_cfg.ragebot.faster_recharge)
		{
			auto recharge_time = g_ctx.globals.weapon->can_double_tap() ? TIME_TO_TICKS(0.5f) : TIME_TO_TICKS(0.6f);

			if (!aim::get().should_stop && fabs(g_ctx.globals.fixed_tickbase - last_teleport) > recharge_time)
			{
				last_teleport = 0;

				recharging_double_tap = false;
				double_tap_key = true;
			}
			else if (m_pcmd->m_buttons & IN_ATTACK)
				last_teleport = g_ctx.globals.fixed_tickbase;
		}
		else
		{
			auto recharge_time = g_ctx.globals.weapon->can_double_tap() ? TIME_TO_TICKS(1.0f) : TIME_TO_TICKS(2.0f);

			if (!aim::get().should_stop && fabs(g_ctx.globals.fixed_tickbase - last_teleport) > recharge_time)
			{
				last_teleport = 0;

				recharging_double_tap = false;
				double_tap_key = true;
			}
			else if (m_pcmd->m_buttons & IN_ATTACK)
				last_teleport = g_ctx.globals.fixed_tickbase;
		}
	}

	if (!g_cfg.ragebot.enable)
	{
		teleport_enabled = false;
		double_tap_key = false;
		hide_shots_key = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (!g_cfg.ragebot.secret && !g_cfg.ragebot.anti_anti_defensive)
	{
		teleport_enabled = false;
		double_tap_key = false;
		hide_shots_key = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (g_ctx.local()->m_bGunGameImmunity() || g_ctx.local()->m_fFlags() & FL_FROZEN)
	{
		teleport_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (g_ctx.globals.fakeducking)
	{
		teleport_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (antiaim::get().freeze_check)
		return true;

	auto max_tickbase_shift = 16;

	if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER && g_ctx.send_packet && (((m_pcmd->m_buttons & IN_ATTACK || !m_pcmd->m_buttons & IN_ATTACK2))) && !remove_knife)
	{

		auto next_command_number = m_pcmd->m_command_number + 1;
		auto user_cmd = m_input()->GetUserCmd(next_command_number);

		memcpy(user_cmd, m_pcmd, sizeof(CUserCmd)); //-V598
		user_cmd->m_command_number = next_command_number;

		//util::copy_command(user_cmd, max_tickbase_shift);

		if (g_ctx.globals.aimbot_working)
		{
			g_ctx.globals.double_tap_aim = true;
			g_ctx.globals.double_tap_aim_check = true;
		}

		recharge_teleport = true;
		teleport_enabled = false;

		last_teleport = g_ctx.globals.fixed_tickbase;
	}
	else if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
		g_ctx.globals.tickbase_shift = g_cfg.ragebot.dt_shift - g_cfg.ragebot.dt_tolerance;

	return true;
}


bool can_shift_shot(int ticks)
{
	if (!g_ctx.local() || !g_ctx.local()->m_hActiveWeapon())
		return false;

	auto tickbase = g_ctx.local()->m_nTickBase();
	auto curtime = m_globals()->m_intervalpertick * (tickbase - ticks);

	if (curtime < g_ctx.local()->m_flNextAttack())
		return false;

	if (curtime < g_ctx.local()->m_hActiveWeapon()->m_flNextPrimaryAttack())
		return false;

	return true;
}

bool misc::double_tap(CUserCmd* m_pcmd)
{
	double_tap_enabled = true;

	static auto recharge_rapid_fire = false;
	static bool firing_dt = false;

	if (recharge_rapid_fire)
	{
		recharge_rapid_fire = false;
		recharging_double_tap = true;

		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.tickbase_shift = 0;
		return false;
	}

	auto max_tickbase_shift = g_ctx.globals.weapon->get_max_tickbase_shift();

	if (recharging_double_tap)
	{
		if (can_shift_shot(max_tickbase_shift) && !aim::get().should_stop)
		{
			recharging_double_tap = false;
			double_tap_key = true;
			firing_dt = false;
		}
		else if (m_pcmd->m_buttons & IN_ATTACK)
			firing_dt = true;
	}

	if (!g_cfg.ragebot.enable)
	{
		double_tap_enabled = false;
		double_tap_key = false;

		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.tickbase_shift = 0;
		return false;
	}

	if (!g_cfg.ragebot.double_tap)
	{
		double_tap_enabled = false;
		double_tap_key = false;

		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.tickbase_shift = 0;
		return false;
	}

	static bool was_in_dt = false;

	if (g_cfg.ragebot.double_tap_key.key <= KEY_NONE || g_cfg.ragebot.double_tap_key.key >= KEY_MAX)
	{
		double_tap_enabled = false;
		double_tap_key = false;

		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.tickbase_shift = 0;
		return false;
	}

	if (double_tap_key && g_cfg.ragebot.double_tap_key.key != g_cfg.antiaim.hide_shots_key.key)
		hide_shots_key = false;

	if (!double_tap_key || g_ctx.local()->m_bGunGameImmunity() || g_ctx.local()->m_fFlags() & FL_FROZEN || g_ctx.globals.fakeducking)
	{
		double_tap_enabled = false;

		if (!firing_dt && was_in_dt)
		{
			g_ctx.globals.trigger_teleport = true;
			g_ctx.globals.teleport_amount = max_tickbase_shift;

			was_in_dt = false;
		}

		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.tickbase_shift = 0;

		return false;
	}

	if (m_gamerules()->m_bIsValveDS())
	{
		double_tap_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.tickbase_shift = 0;
		return false;
	}

	if (antiaim::get().freeze_check)
		return true;

	was_in_dt = true;

	if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER
		&& g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER
		&& (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2 && g_ctx.globals.weapon->is_knife())) //-V648
	{
		auto next_command_number = m_pcmd->m_command_number + 1;
		auto user_cmd = m_input()->GetUserCmd(next_command_number);

		memcpy(user_cmd, m_pcmd, sizeof(CUserCmd));
		user_cmd->m_command_number = next_command_number;

		util::copy_command(user_cmd, max_tickbase_shift);

		if (g_ctx.globals.aimbot_working)
		{
			g_ctx.globals.double_tap_aim = true;
			g_ctx.globals.double_tap_aim_check = true;
		}

		recharge_rapid_fire = true;
		double_tap_enabled = false;
		double_tap_key = false;

		g_ctx.send_packet = true;
		firing_dt = true;
	}
	else if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
		g_ctx.globals.tickbase_shift = max_tickbase_shift;

	return true;
}

void misc::hide_shots(CUserCmd* m_pcmd, bool should_work)
{
	hide_shots_enabled = true;

	if (!g_cfg.ragebot.enable)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;

		if (should_work)
		{
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.next_tickbase_shift = 0;
		}

		return;
	}

	if (!g_cfg.antiaim.hide_shots)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;

		if (should_work)
		{
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.next_tickbase_shift = 0;
		}

		return;
	}

	if (g_cfg.antiaim.hide_shots_key.key <= KEY_NONE || g_cfg.antiaim.hide_shots_key.key >= KEY_MAX)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;

		if (should_work)
		{
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.next_tickbase_shift = 0;
		}

		return;
	}

	if (!should_work && double_tap_key)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;
		return;
	}

	if (!hide_shots_key)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	double_tap_key = false;

	if (g_ctx.local()->m_bGunGameImmunity() || g_ctx.local()->m_fFlags() & FL_FROZEN)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	if (g_ctx.globals.fakeducking)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	if (antiaim::get().freeze_check)
		return;

	g_ctx.globals.next_tickbase_shift = m_gamerules()->m_bIsValveDS() ? 6 : 9;

	auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);
	auto weapon_shoot = m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || m_pcmd->m_buttons & IN_ATTACK2 && g_ctx.globals.weapon->is_knife() || revolver_shoot;

	if (g_ctx.send_packet && !g_ctx.globals.weapon->is_grenade() && weapon_shoot)
		g_ctx.globals.tickbase_shift = g_ctx.globals.next_tickbase_shift;
}

// need to fix left hand knife it doesn't switch back after toggle off
//void misc::left_hand_knife(ClientFrameStage_t stage)
//{
//	if (!g_cfg.misc.left_knife)
//		return;
//
//	if (stage != ClientFrameStage_t::FRAME_RENDER_START && stage != ClientFrameStage_t::FRAME_RENDER_END)
//		return;
//
//	const static auto cl_righthand = m_cvar()->FindVar(crypt_str("cl_righthand"));
//
//	const auto weapon_knife = g_ctx.globals.weapon->is_knife();
//
//	if (weapon_knife)
//		cl_righthand->SetValue(false);
//	else
//		cl_righthand->SetValue(true);
//
//}

void misc::fake_ping(ClientFrameStage_t stage)
{
	auto net_fakelag = m_cvar()->FindVar("net_fakelag");
	if (g_cfg.misc.fake_ping)
		net_fakelag->SetValue(g_cfg.misc.ping / 2);
	else
		net_fakelag->SetValue(0);
}

void misc::break_prediction(CUserCmd* cmd) {

	if (!g_cfg.ragebot.break_lag_comp)
		return;

	if (!g_ctx.local()->is_alive())
		return;
}