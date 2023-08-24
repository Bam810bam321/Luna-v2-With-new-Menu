#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"
struct m_indicator
{
	std::string m_text;
	Color m_color;

	m_indicator(const char* text, Color color) :
		m_text(text), m_color(color)
	{

	}
	m_indicator(std::string text, Color color) :
		m_text(text), m_color(color)
	{

	}

};
class otheresp : public singleton< otheresp >
{
public:
	void penetration_reticle();
	void indicators();
	void draw_indicators();
	void draw_velocity();
	void MaskChanger();
	void hitmarker_paint();
	void damage_marker_paint();
	void spread_crosshair(LPDIRECT3DDEVICE9 device);
	void automatic_peek_indicator();
	typedef void(__thiscall* UpdateAddonModelsFunc)(void*, bool);
	const char* default_mask = "models/player/holiday/facemasks/facemask_battlemask.mdl";

	struct Hitmarker
	{
		float hurt_time = FLT_MIN;
		Color hurt_color = Color::White;
		Vector point = ZERO;
	} hitmarker;


	static constexpr std::array mask_models
	{ 
		"",
		"models/player/holiday/facemasks/facemask_dallas.mdl",
		"models/player/holiday/facemasks/facemask_battlemask.mdl",
		"models/player/holiday/facemasks/evil_clown.mdl",
		"models/player/holiday/facemasks/facemask_anaglyph.mdl",
		"models/player/holiday/facemasks/facemask_boar.mdl",
		"models/player/holiday/facemasks/facemask_bunny.mdl",
		"models/player/holiday/facemasks/facemask_bunny_gold.mdl",
		"models/player/holiday/facemasks/facemask_chains.mdl",
		"models/player/holiday/facemasks/facemask_chicken.mdl",
		"models/player/holiday/facemasks/facemask_devil_plastic.mdl",
		"models/player/holiday/facemasks/facemask_hoxton.mdl",
		"models/player/holiday/facemasks/facemask_pumpkin.mdl",
		"models/player/holiday/facemasks/facemask_samurai.mdl",
		"models/player/holiday/facemasks/facemask_sheep_bloody.mdl",
		"models/player/holiday/facemasks/facemask_sheep_gold.mdl",
		"models/player/holiday/facemasks/facemask_sheep_model.mdl",
		"models/player/holiday/facemasks/facemask_skull.mdl",
		"models/player/holiday/facemasks/facemask_template.mdl",
		"models/player/holiday/facemasks/facemask_wolf.mdl",
		"models/player/holiday/facemasks/porcelain_doll.mdl",
	};

	struct Damage_marker
	{
		Vector position = ZERO;
		float hurt_time = FLT_MIN;
		Color hurt_color = Color::White;
		int damage = -1;
		int hitgroup = -1;
		float anim = 0;
		float animalpha = 0;

		void reset()
		{
			position.Zero();
			hurt_time = FLT_MIN;
			hurt_color = Color::White;
			damage = -1;
			hitgroup = -1;
		}
	} damage_marker[65];
	std::vector<m_indicator> m_indicators;
};