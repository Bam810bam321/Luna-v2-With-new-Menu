// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "other_esp.h"
#include "..\autowall\autowall.h"
#include "..\ragebot\antiaim.h"
#include "..\misc\logs.h"
#include "..\misc\misc.h"
#include "..\lagcompensation\local_animations.h"

bool can_penetrate(weapon_t* weapon)
{
	auto weapon_info = weapon->get_csweapon_info();

	if (!weapon_info)
		return false;

	Vector view_angles;
	m_engine()->GetViewAngles(view_angles);

	Vector direction;
	math::angle_vectors(view_angles, direction);

	CTraceFilter filter;
	filter.pSkip = g_ctx.local();

	trace_t trace;
	util::trace_line(g_ctx.globals.eye_pos, g_ctx.globals.eye_pos + direction * weapon_info->flRange, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

	if (trace.fraction == 1.0f) //-V550
		return false;

	auto eye_pos = g_ctx.globals.eye_pos;
	auto hits = 1;
	auto damage = (float)weapon_info->iDamage;
	auto penetration_power = weapon_info->flPenetration;

	static auto damageReductionBullets = m_cvar()->FindVar(crypt_str("ff_damage_reduction_bullets"));
	static auto damageBulletPenetration = m_cvar()->FindVar(crypt_str("ff_damage_bullet_penetration"));

	return autowall::get().handle_bullet_penetration(weapon_info, trace, eye_pos, direction, hits, damage, penetration_power, damageReductionBullets->GetFloat(), damageBulletPenetration->GetFloat());
}

bool LoadModel(const char* thisModelName)
{
	const auto CustomModel = m_networkStringTableContainer()->findTable("modelprecache");

	if (CustomModel)
	{
		m_modelinfo()->GetModelIndex(thisModelName);
		int MdlNum = CustomModel->addString(false, thisModelName);

		if (MdlNum == NULL)
			return false;
	}
	return true;
}

void InitMaskModels()
{
	LoadModel("models/player/holiday/facemasks/facemask_dallas.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_battlemask.mdl");
	LoadModel("models/player/holiday/facemasks/evil_clown.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_anaglyph.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_boar.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_bunny.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_bunny_gold.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_chains.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_chicken.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_devil_plastic.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_hoxton.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_pumpkin.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_samurai.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_sheep_bloody.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_sheep_gold.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_sheep_model.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_skull.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_template.mdl");
	LoadModel("models/player/holiday/facemasks/facemask_wolf.mdl");
	LoadModel("models/player/holiday/facemasks/porcelain_doll.mdl");
}

void otheresp::MaskChanger()
{
	InitMaskModels();
	static UpdateAddonModelsFunc pUpdateAddonModels = reinterpret_cast<UpdateAddonModelsFunc>(util::FindSignature("client.dll", "55 8B EC 83 EC ? 53 8B D9 8D 45 ? 8B 08"));

	static auto currentMask = *reinterpret_cast<char***>(util::FindSignature("client.dll", "FF 35 ? ? ? ? FF 90 ? ? ? ? 8B 8F") + 0x2);

	static int oldMask = -1;

	if (!g_ctx.local())
		return;

	auto mask = mask_models[g_cfg.esp.mask_models];

	if (!LoadModel(default_mask) || !LoadModel(mask))
		return;

	if (g_cfg.esp.EnableMaskChanger)
	{
		g_ctx.local()->AddonBits() |= 0x10000;

		if (oldMask != g_cfg.esp.mask_models)
		{
			*currentMask = (char*)mask;
			pUpdateAddonModels(g_ctx.local(), true);
			oldMask = g_cfg.esp.mask_models;
		}
	}
	else
	{
		if (g_ctx.local()->AddonBits() & 0x10000)
			g_ctx.local()->AddonBits() &= ~0x10000;
	}
}


void otheresp::penetration_reticle()
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.penetration_reticle)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto color = Color::Red;

	if (!weapon->is_non_aim() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && can_penetrate(weapon))
		color = Color::Green;

	static int width, height;
	m_engine()->GetScreenSize(width, height);
	
	render::get().rect_filled(width / 2, height / 2 - 1, 1, 3, color);
	render::get().rect_filled(width / 2 - 1, height / 2, 3, 1, color);
}

void otheresp::draw_velocity()
{
	if (!g_cfg.esp.velocity_graph)
		return;

	if (!g_ctx.local())
		return;

	if (!m_engine()->IsInGame() || !g_ctx.local()->is_alive())
		return;

	static std::vector<float> velData(120, 0);

	Vector vecVelocity = g_ctx.local()->m_vecVelocity();
	float currentVelocity = sqrt(vecVelocity.x * vecVelocity.x + vecVelocity.y * vecVelocity.y);

	velData.erase(velData.begin());
	velData.push_back(currentVelocity);

	int vel = g_ctx.local()->m_vecVelocity().Length2D();

	static int width, height;
	m_engine()->GetScreenSize(width, height);
	render::get().text(fonts[INDICATORFONT], width / 2, height / 1.1, Color(0, 255, 100, 255), HFONT_CENTERED_X | HFONT_CENTERED_Y, "(%i)", vel);


	for (auto i = 0; i < velData.size() - 1; i++)
	{
		int cur = velData.at(i);
		int next = velData.at(i + 1);

		render::get().line(
			width / 2 + (velData.size() * 5 / 2) - (i - 1) * 5.f,
			height / 1.15 - (std::clamp(cur, 0, 450) * .2f),
			width / 2 + (velData.size() * 5 / 2) - i * 5.f,
			height / 1.15 - (std::clamp(next, 0, 450) * .2f), Color(255, 255, 255, 255)
		);
	}
}
void otheresp::indicators()
{
	if (!g_ctx.local()->is_alive()) 
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (g_cfg.esp.indicators[INDICATOR_FAKE] && (antiaim::get().type == ANTIAIM_LEGIT || g_cfg.antiaim.type[antiaim::get().type].desync))
	{
		auto color = Color(130, 20, 20);
		auto animstate = g_ctx.local()->get_animation_state();

		if (animstate && local_animations::get().local_data.animstate)
		{
			auto delta = fabs(math::normalize_yaw(animstate->m_flGoalFeetYaw - local_animations::get().local_data.animstate->m_flGoalFeetYaw));
			auto desync_delta = max(g_ctx.local()->get_max_desync_delta(), 58.0f);

			color = Color(130, 20 + (int)(min(delta / desync_delta, 1.0f) * 150.0f), 20);
		}


		//m_indicators.push_back(m_indicator("FAKE", color));
	}

	auto choke_indicator = false;

	if (g_cfg.esp.indicators[INDICATOR_CHOKE] && !fakelag::get().condition && !misc::get().double_tap_enabled && !misc::get().hide_shots_enabled)
	{
		m_indicators.push_back(m_indicator("FAKELAG", Color(20, 255, 20)));
		choke_indicator = true;
	}

	if (g_cfg.esp.indicators[INDICATOR_HITCHANCE] && g_ctx.globals.current_weapon != -1 && key_binds::get().get_key_bind_state(25 + g_ctx.globals.current_weapon) && !weapon->is_non_aim())
	{
		if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitchance_override)
		m_indicators.push_back(m_indicator(("HC: " + std::to_string(g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitchance_override_amount)), Color(180, 184, 255)));
	}

	if (g_cfg.esp.indicators[INDICATOR_DAMAGE] && g_ctx.globals.current_weapon != -1 && key_binds::get().get_key_bind_state(4 + g_ctx.globals.current_weapon) && !weapon->is_non_aim())
	{
		if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage > 100)
			m_indicators.push_back(m_indicator(("DMG: HP + " + std::to_string(g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage - 100)), Color(180, 180, 180)));
		else
			m_indicators.push_back(m_indicator(("DMG: " + std::to_string(g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage)), Color(180,180,180)));
	}

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(18))
		m_indicators.push_back(m_indicator("ASSIST", Color(230, 230, 230)));

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(3) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("SAFE", Color(20, 255, 20)));

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(23) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("EDGE BUG", Color(20, 20, 255)));

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(13) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("EDGE JUMP", Color(20, 20, 255)));

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(24) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("JUMP BUG", Color(20, 20, 255)));

	if (g_cfg.esp.indicators[INDICATOR_BODY_AIM] && key_binds::get().get_key_bind_state(22) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("BAIM", Color(255, 0, 0)));

	if (g_cfg.esp.indicators[INDICATOR_DUCK] && key_binds::get().get_key_bind_state(20))
		m_indicators.push_back(m_indicator("DUCK", Color(230, 230, 230)));

	if (g_cfg.esp.indicators[INDICATOR_DESYNC_SIDE] && (antiaim::get().type == ANTIAIM_LEGIT && g_cfg.antiaim.desync == 1 || antiaim::get().type != ANTIAIM_LEGIT && g_cfg.antiaim.type[antiaim::get().type].desync == 1) && !antiaim::get().condition(g_ctx.get_command()))
	{
		auto side = antiaim::get().desync_angle > 0.0f ? "AA RIGHT" : "AA LEFT";

		if (antiaim::get().type == ANTIAIM_LEGIT)
			side = antiaim::get().desync_angle > 0.0f ? "AA LEFT" : "AA RIGHT";

		m_indicators.push_back(m_indicator(side, Color(255, 255, 255)));
	}

	if (g_cfg.esp.indicators[INDICATOR_DT] && g_cfg.ragebot.double_tap && g_cfg.ragebot.double_tap_key.key > KEY_NONE && g_cfg.ragebot.double_tap_key.key < KEY_MAX && misc::get().double_tap_key)
		m_indicators.push_back(m_indicator("DT", !g_ctx.local()->m_bGunGameImmunity() && !(g_ctx.local()->m_fFlags() & FL_FROZEN) && !antiaim::get().freeze_check && misc::get().double_tap_enabled && !weapon->is_grenade() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER && weapon->can_fire(false) ? Color(20, 255, 20) : Color(130, 20, 20)));

	if (g_cfg.esp.indicators[INDICATOR_HS] && misc::get().hide_shots_key)
		m_indicators.push_back(m_indicator("ONSHOT", !g_ctx.local()->m_bGunGameImmunity() && !(g_ctx.local()->m_fFlags() & FL_FROZEN) && !antiaim::get().freeze_check && misc::get().hide_shots_enabled ? Color(20, 255, 20) : Color(130, 20, 20)));
}
void otheresp::draw_indicators()
{
	if (!g_ctx.local()->is_alive()) 
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto h = height / 2 + 100;

	for (auto& indicator : m_indicators)
	{
		render::get().gradient(5, h - 15, 30, 30, Color(0, 0, 0, 0), Color(g_cfg.esp.indicators_left_color), GRADIENT_HORIZONTAL);
		render::get().gradient(35, h - 15, 30, 30, Color(g_cfg.esp.indicators_right_color), Color(0, 0, 0, 0), GRADIENT_HORIZONTAL);
		//render::get().gradient(5, h - 15, 85, 30, Color(g_cfg.esp.indicators_left_color), Color(g_cfg.esp.indicators_right_color), GRADIENT_HORIZONTAL);
		render::get().text(fonts[INDICATORFONT], 10, h, indicator.m_color, HFONT_CENTERED_Y, indicator.m_text.c_str());
		h -= 35;
	}

	m_indicators.clear();
}


#define HASH( str )                                     \
    []() {                                              \
        constexpr hash32_t out{ FNV1a::get( str ) };    \
                                                        \
        return out;                                     \
    }()

void otheresp::hitmarker_paint()
{
	float m_hit_start, m_hit_end, m_hit_duration;

	if (!g_cfg.esp.hitmarker[0] && !g_cfg.esp.hitmarker[1])
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (!g_ctx.local()->is_alive())
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (hitmarker.hurt_time + 0.7f > m_globals()->m_curtime)
	{
		if (g_cfg.esp.hitmarker[0])
		{
			static int width, height;
			m_engine()->GetScreenSize(width, height);

			auto alpha = (int)((hitmarker.hurt_time + 1.0f - m_globals()->m_curtime) * 255.0f);
			g_cfg.esp.hitmarker_color.SetAlpha(alpha);

			static auto cross = m_cvar()->FindVar("weapon_debug_spread_show");
			cross->SetValue(g_cfg.esp.hitmarker_test && !g_ctx.local()->m_bIsScoped());

			float complete = 4;
			int x = width / 2, y = height / 2, alpha2 = (1.f - complete) * 240;

			constexpr int line{ 6 };

			render::get().line(x - line, y - line, x - (line / 4), y - (line / 4), { 200, 200, 200, alpha2 });
			render::get().line(x - line, y + line, x - (line / 4), y + (line / 4), { 200, 200, 200, alpha2 });
			render::get().line(x + line, y + line, x + (line / 4), y + (line / 4), { 200, 200, 200, alpha2 });
			render::get().line(x + line, y - line, x + (line / 4), y - (line / 4), { 200, 200, 200, alpha2 });
		}

		if (g_cfg.esp.hitmarker[1])
		{
			Vector world;

			if (math::world_to_screen(hitmarker.point, world))
			{
				auto alpha = (int)((hitmarker.hurt_time + 1.0f - m_globals()->m_curtime) * 255.0f);
				g_cfg.esp.hitmarker_color.SetAlpha(alpha);

				static auto cross = m_cvar()->FindVar("weapon_debug_spread_show");
				cross->SetValue(g_cfg.esp.hitmarker_test && !g_ctx.local()->m_bIsScoped());

				float complete = 4;
				int x = world.x / 2, y = world.y / 2, alpha2 = (1.f - complete) * 240;

				constexpr int line{ 6 };

				render::get().line(x - line, y - line, x - (line / 4), y - (line / 4), { 200, 200, 200, alpha2 });
				render::get().line(x - line, y + line, x - (line / 4), y + (line / 4), { 200, 200, 200, alpha2 });
				render::get().line(x + line, y + line, x + (line / 4), y + (line / 4), { 200, 200, 200, alpha2 });
				render::get().line(x + line, y - line, x + (line / 4), y - (line / 4), { 200, 200, 200, alpha2 });
			}
		}
	}
}

void otheresp::damage_marker_paint() //edited
{
	for (auto i = 1; i < m_globals()->m_maxclients; i++) 
	{
		if (damage_marker[i].hurt_time + 2.0f > m_globals()->m_curtime)
		{
			Vector screen;

			if (!math::world_to_screen(damage_marker[i].position, screen))
				continue;

			auto alpha = (int)((damage_marker[i].hurt_time + 2.0f - m_globals()->m_curtime) * 127.5f);
			Color color = g_cfg.esp.damage_marker_color;
			color.SetAlpha(alpha);
			damage_marker[i].anim += 0.5f;

			render::get().text(fonts[DAMAGE_MARKER], screen.x, screen.y - damage_marker[i].anim, g_cfg.esp.damage_marker_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "%i", damage_marker[i].damage);

		}
	}

}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device);

void otheresp::spread_crosshair(LPDIRECT3DDEVICE9 device)
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.show_spread)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (weapon->is_non_aim())
		return;

	int w, h;
	m_engine()->GetScreenSize(w, h);

	draw_circe((float)w * 0.5f, (float)h * 0.5f, g_ctx.globals.inaccuracy * 500.0f, 50, D3DCOLOR_RGBA(g_cfg.esp.show_spread_color.r(), g_cfg.esp.show_spread_color.g(), g_cfg.esp.show_spread_color.b(), g_cfg.esp.show_spread_color.a()), D3DCOLOR_RGBA(0, 0, 0, 0), device);
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2 = nullptr;
	std::vector <CUSTOMVERTEX2> circle(resolution + 2);

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0.0f;

	circle[0].rhw = 1.0f;
	circle[0].color = color2;

	for (auto i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - radius * cos(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - radius * sin(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0.0f;

		circle[i].rhw = 1.0f;
		circle[i].color = color;
	}

	device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX2), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, nullptr); //-V107

	if (!g_pVB2)
		return;

	void* pVertices;

	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX2), (void**)&pVertices, 0); //-V107
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX2));
	g_pVB2->Unlock();

	device->SetTexture(0, nullptr);
	device->SetPixelShader(nullptr);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX2));
	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);

	g_pVB2->Release();
}

void otheresp::automatic_peek_indicator()
{
	if (!g_cfg.ragebot.quick_peek_assist)
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	static auto position = ZERO;

	if (!g_ctx.globals.start_position.IsZero())
		position = g_ctx.globals.start_position;

	if (position.IsZero())
		return;

	static auto alpha = 0.0f;

	if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18) || alpha)
	{
		if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18))
			alpha += 5.0f * m_globals()->m_frametime; 
		else
			alpha -= 5.0f * m_globals()->m_frametime;

		alpha = math::clamp(alpha, 0.0f, 1.0f);
		render::get().Draw3DFilledCircle(position, (int)(alpha * 20.0f), g_ctx.globals.fired_shot ? Color(g_cfg.misc.automatic_peek_color.r(), g_cfg.misc.automatic_peek_color.g(), g_cfg.misc.automatic_peek_color.b(), 255) : Color(g_cfg.misc.automatic_peek_color.r(), g_cfg.misc.automatic_peek_color.g(), g_cfg.misc.automatic_peek_color.b(), 255));

	}

}