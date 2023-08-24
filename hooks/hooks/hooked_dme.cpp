// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\misc\fakelag.h"
#include "..\..\cheats\lagcompensation\local_animations.h"
#include "..\..\cheats\visuals\player_esp.h"

IMaterial* CreateMaterial(bool lit, const std::string& material_data)
{
	static auto created = 0;
	std::string type = lit ? crypt_str("VertexLitGeneric") : crypt_str("UnlitGeneric");

	auto matname = crypt_str("Medusa.uno_") + std::to_string(created);
	++created;

	auto keyValues = new KeyValues(matname.c_str());
	static auto key_values_address = util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 56 8B F1 33 C0 8B 4D 0C 81 26 ? ? ? ? 89 46 10 8A 45 10 C6 46 03 00 89 4E 14 88 46 18 C7 46 ? ? ? ? ? C7 46"));

	using KeyValuesFn = void(__thiscall*)(void*, const char*, void*, void*);
	reinterpret_cast <KeyValuesFn> (key_values_address)(keyValues, type.c_str(), 0, 0);

	static auto load_from_buffer_address = util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"));
	using LoadFromBufferFn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*);

	reinterpret_cast <LoadFromBufferFn> (load_from_buffer_address)(keyValues, matname.c_str(), material_data.c_str(), nullptr, nullptr, nullptr);

	auto material = m_materialsystem()->CreateMaterial(matname.c_str(), keyValues);
	material->IncrementReferenceCount();

	return material;
}

using DrawModelExecute_t = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);

void __stdcall hooks::hooked_dme(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone_to_world)
{
	static auto original_fn = modelrender_hook->get_func_address <DrawModelExecute_t> (21);
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

	if (!g_cfg.player.enable)
		return original_fn(m_modelrender(), ctx, state, info, bone_to_world);

	if (m_engine()->IsTakingScreenshot() && g_cfg.misc.anti_screenshot)
		return original_fn(m_modelrender(), ctx, state, info, bone_to_world);

	if (!info.pModel)
		return original_fn(m_modelrender(), ctx, state, info, bone_to_world);

	auto model_entity = static_cast<player_t *>(m_entitylist()->GetClientEntity(info.entity_index));
	auto name = m_modelinfo()->GetModelName(info.pModel);

	auto is_player = strstr(name, "models/player") && model_entity->is_alive() && (g_cfg.player.type[ENEMY].chams[PLAYER_CHAMS_VISIBLE] || g_cfg.player.type[TEAM].chams[PLAYER_CHAMS_VISIBLE] || g_cfg.player.type[LOCAL].chams[PLAYER_CHAMS_VISIBLE] || g_cfg.player.fake_chams_enable || g_cfg.player.backtrack_chams);
	auto is_weapon = strstr(name, "weapons/v_") && !strstr(name, "arms") && g_cfg.esp.enable_weapon_chams;
	auto is_arms = strstr(name, "arms") && g_cfg.esp.enable_arms_chams;
	auto is_sleeve = strstr(name, "sleeve") && g_cfg.esp.enable_arms_chams;
	auto is_drugmod = strstr(name, "models") && !strstr(name, "sleeve") && !strstr(name, "arms") && !strstr(name, "weapons/v_") && !strstr(name, "models/player")  /* && strstr(name, "cs_assault") && strstr(name, "cs_havana") && strstr(name, "cs_office") && strstr(name, "de_aztec") && strstr(name, "cs_italy") && strstr(name, "de_cache") && strstr(name, "de_cbble") && strstr(name, "de_dust") && strstr(name, "de_dust") && strstr(name, "de_mirage") && strstr(name, "de_nuke") && strstr(name, "de_train") && strstr(name, "dust2_legacy") && strstr(name, "floors") && strstr(name, "grass") && strstr(name, "metal") && strstr(name, "trash") && strstr(name, "wood") */ && g_cfg.esp.drugsmod;

	if (m_modelrender()->IsForcedMaterialOverride() && !is_weapon && !is_arms && !is_sleeve)
		return original_fn(m_modelrender(), ctx, state, info, bone_to_world);

	m_renderview()->SetColorModulation(1.0f, 1.0f, 1.0f); 

	if (!is_player && !is_weapon && !is_arms && !is_sleeve)
		return original_fn(m_modelrender(), ctx, state, info, bone_to_world);

	static IMaterial* materials[] =
	{
		CreateMaterial(true, crypt_str(R"#("VertexLitGeneric"
			{
				"$basetexture"				"vgui/white"
				"$ignorez"					"0"
				"$envmap"					" "
				"$nofog"					"1"
				"$model"					"1"
				"$nocull"					"0"
				"$selfillum"				"1"
				"$halflambert"				"1"
				"$znearer"					"0"
				"$flat"						"0"
				"$wireframe"				"0"
			}
		)#")),
		CreateMaterial(false, crypt_str(R"#("UnlitGeneric"
			{
				"$basetexture"				"vgui/white"
				"$ignorez"					"0"
				"$envmap"					" "
				"$nofog"					"1"
				"$model"					"1"
				"$nocull"					"0"
				"$selfillum"				"1"
				"$halflambert"				"1"
				"$znearer"					"0"
				"$flat"						"1"
				"$wireframe"				"0"
			}
		)#"))
	};

	auto called_original = false;

	if (is_player)
	{
		auto type = ENEMY;

		if (model_entity == g_ctx.local())
			type = LOCAL;
		else if (model_entity->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			type = TEAM;

		if (type == ENEMY)
		{
			auto alpha_modifier = playeresp::get().esp_alpha_fade[model_entity->EntIndex()];

			auto material = materials[g_cfg.player.type[ENEMY].chams_type];
			auto double_material = materials[g_cfg.player.type[ENEMY].double_material_material];

			if (material && double_material && !material->IsErrorMaterial() && !double_material->IsErrorMaterial())
			{
				if (g_cfg.player.type[ENEMY].chams[PLAYER_CHAMS_VISIBLE] && g_cfg.player.type[ENEMY].chams[PLAYER_CHAMS_INVISIBLE])
				{
					auto alpha = (float)g_cfg.player.backtrack_chams_color.a() / 255.0f;;

					if (g_cfg.player.backtrack_chams)
					{
						auto backtrack_material = materials[g_cfg.player.backtrack_chams_material];

						if (backtrack_material && !backtrack_material->IsErrorMaterial())
						{
							matrix3x4_t matrix[MAXSTUDIOBONES];

							if (util::get_backtrack_matrix(model_entity, matrix))
							{
								float backtrack_color[3] =
								{
									g_cfg.player.backtrack_chams_color[0] / 255.0f,
									g_cfg.player.backtrack_chams_color[1] / 255.0f,
									g_cfg.player.backtrack_chams_color[2] / 255.0f
								};

								m_renderview()->SetBlend(alpha * alpha_modifier);
								util::color_modulate(backtrack_color, backtrack_material);

								backtrack_material->IncrementReferenceCount(); 
								backtrack_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

								m_modelrender()->ForcedMaterialOverride(backtrack_material);
								original_fn(m_modelrender(), ctx, state, info, matrix);
								m_modelrender()->ForcedMaterialOverride(nullptr);
							}
						}
					}

					alpha = (float)g_cfg.player.type[ENEMY].xqz_color.a() / 255.0f;

					float xqz_color[3] =
					{
						g_cfg.player.type[ENEMY].xqz_color[0] / 255.0f,
						g_cfg.player.type[ENEMY].xqz_color[1] / 255.0f,
						g_cfg.player.type[ENEMY].xqz_color[2] / 255.0f
					};

					m_renderview()->SetBlend(alpha * alpha_modifier); 
					util::color_modulate(xqz_color, material);

					material->IncrementReferenceCount();
					material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

					m_modelrender()->ForcedMaterialOverride(material);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);
					
					alpha = (float)g_cfg.player.type[ENEMY].chams_color.a() / 255.0f;

					float normal_color[3] =
					{
						g_cfg.player.type[ENEMY].chams_color[0] / 255.0f,
						g_cfg.player.type[ENEMY].chams_color[1] / 255.0f,
						g_cfg.player.type[ENEMY].chams_color[2] / 255.0f
					};

					m_renderview()->SetBlend(alpha * alpha_modifier);
					util::color_modulate(normal_color, material);

					material->IncrementReferenceCount();
					material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					m_modelrender()->ForcedMaterialOverride(material);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);

					if (g_cfg.player.type[ENEMY].animated_material)
					{
						auto animated_material = materials[9];
						
						if (animated_material)
						{
							alpha = (float)g_cfg.player.type[ENEMY].animated_material_color.a() / 255.0f;

							float animated_color[3] =
							{
								g_cfg.player.type[ENEMY].animated_material_color[0] / 255.0f,
								g_cfg.player.type[ENEMY].animated_material_color[1] / 255.0f,
								g_cfg.player.type[ENEMY].animated_material_color[2] / 255.0f
							};

							m_renderview()->SetBlend(alpha * alpha_modifier);
							util::color_modulate(animated_color, animated_material);

							animated_material->IncrementReferenceCount();
							animated_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

							m_modelrender()->ForcedMaterialOverride(animated_material);
							original_fn(m_modelrender(), ctx, state, info, bone_to_world);
							m_modelrender()->ForcedMaterialOverride(nullptr);
						}
					}

					if (g_cfg.player.type[ENEMY].double_material && g_cfg.player.type[ENEMY].chams_type != 6)
					{
						alpha = (float)g_cfg.player.type[ENEMY].double_material_color.a() / 255.0f;

						float double_color[3] =
						{
							g_cfg.player.type[ENEMY].double_material_color[0] / 255.0f,
							g_cfg.player.type[ENEMY].double_material_color[1] / 255.0f,
							g_cfg.player.type[ENEMY].double_material_color[2] / 255.0f
						};

						m_renderview()->SetBlend(alpha * alpha_modifier);
						util::color_modulate(double_color, double_material);

						double_material->IncrementReferenceCount();
						double_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

						m_modelrender()->ForcedMaterialOverride(double_material);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);
					}

					called_original = true;
				}
				else if (g_cfg.player.type[ENEMY].chams[PLAYER_CHAMS_VISIBLE])
				{
					auto alpha = (float)g_cfg.player.backtrack_chams_color.a() / 255.0f;;

					if (g_cfg.player.backtrack_chams)
					{
						auto backtrack_material = materials[g_cfg.player.backtrack_chams_material];

						if (backtrack_material && !backtrack_material->IsErrorMaterial())
						{
							matrix3x4_t matrix[MAXSTUDIOBONES];

							if (util::get_backtrack_matrix(model_entity, matrix))
							{
								float backtrack_color[3] =
								{
									g_cfg.player.backtrack_chams_color[0] / 255.0f,
									g_cfg.player.backtrack_chams_color[1] / 255.0f,
									g_cfg.player.backtrack_chams_color[2] / 255.0f
								};

								m_renderview()->SetBlend(alpha * alpha_modifier);
								util::color_modulate(backtrack_color, backtrack_material);

								backtrack_material->IncrementReferenceCount();
								backtrack_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

								m_modelrender()->ForcedMaterialOverride(backtrack_material);
								original_fn(m_modelrender(), ctx, state, info, matrix);
								m_modelrender()->ForcedMaterialOverride(nullptr);
							}
						}
					}

					alpha = (float)g_cfg.player.type[ENEMY].chams_color.a() / 255.0f;

					float normal_color[3] =
					{
						g_cfg.player.type[ENEMY].chams_color[0] / 255.0f,
						g_cfg.player.type[ENEMY].chams_color[1] / 255.0f,
						g_cfg.player.type[ENEMY].chams_color[2] / 255.0f
					};

					m_renderview()->SetBlend(alpha * alpha_modifier);
					util::color_modulate(normal_color, material);

					material->IncrementReferenceCount();
					material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					m_modelrender()->ForcedMaterialOverride(material);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);

					if (g_cfg.player.type[ENEMY].animated_material)
					{
						auto animated_material = materials[9];

						if (animated_material)
						{
							alpha = (float)g_cfg.player.type[ENEMY].animated_material_color.a() / 255.0f;

							float animated_color[3] =
							{
								g_cfg.player.type[ENEMY].animated_material_color[0] / 255.0f,
								g_cfg.player.type[ENEMY].animated_material_color[1] / 255.0f,
								g_cfg.player.type[ENEMY].animated_material_color[2] / 255.0f
							};

							m_renderview()->SetBlend(alpha * alpha_modifier);
							util::color_modulate(animated_color, animated_material);

							animated_material->IncrementReferenceCount();
							animated_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

							m_modelrender()->ForcedMaterialOverride(animated_material);
							original_fn(m_modelrender(), ctx, state, info, bone_to_world);
							m_modelrender()->ForcedMaterialOverride(nullptr);
						}
					} 

					if (g_cfg.player.type[ENEMY].double_material && g_cfg.player.type[ENEMY].chams_type != 6)
					{
						alpha = (float)g_cfg.player.type[ENEMY].double_material_color.a() / 255.0f;

						float double_color[3] =
						{
							g_cfg.player.type[ENEMY].double_material_color[0] / 255.0f,
							g_cfg.player.type[ENEMY].double_material_color[1] / 255.0f,
							g_cfg.player.type[ENEMY].double_material_color[2] / 255.0f
						};

						m_renderview()->SetBlend(alpha * alpha_modifier);
						util::color_modulate(double_color, double_material);

						double_material->IncrementReferenceCount();
						double_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

						m_modelrender()->ForcedMaterialOverride(double_material);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);
					}

					called_original = true;
				}
			}

			if (!called_original)
				return original_fn(m_modelrender(), ctx, state, info, bone_to_world);
		}
		else if (type == TEAM)
		{
			auto material = materials[g_cfg.player.type[TEAM].chams_type];
			auto double_material = materials[g_cfg.player.type[TEAM].double_material_material];

			if (material && double_material && !material->IsErrorMaterial() && !double_material->IsErrorMaterial())
			{
				if (g_cfg.player.type[TEAM].chams[PLAYER_CHAMS_VISIBLE] && g_cfg.player.type[TEAM].chams[PLAYER_CHAMS_INVISIBLE])
				{
					auto alpha = (float)g_cfg.player.type[TEAM].xqz_color.a() / 255.0f;

					float xqz_color[3] =
					{
						g_cfg.player.type[TEAM].xqz_color[0] / 255.0f,
						g_cfg.player.type[TEAM].xqz_color[1] / 255.0f,
						g_cfg.player.type[TEAM].xqz_color[2] / 255.0f
					};

					m_renderview()->SetBlend(alpha);
					util::color_modulate(xqz_color, material);

					material->IncrementReferenceCount();
					material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

					m_modelrender()->ForcedMaterialOverride(material);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);

					alpha = (float)g_cfg.player.type[TEAM].chams_color.a() / 255.0f;

					float normal_color[3] =
					{
						g_cfg.player.type[TEAM].chams_color[0] / 255.0f,
						g_cfg.player.type[TEAM].chams_color[1] / 255.0f,
						g_cfg.player.type[TEAM].chams_color[2] / 255.0f
					};

					m_renderview()->SetBlend(alpha);
					util::color_modulate(normal_color, material);

					material->IncrementReferenceCount();
					material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					m_modelrender()->ForcedMaterialOverride(material);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);

					if (g_cfg.player.type[TEAM].animated_material)
					{
						auto animated_material = materials[9];

						if (animated_material)
						{
							alpha = (float)g_cfg.player.type[TEAM].animated_material_color.a() / 255.0f;

							float animated_color[3] =
							{
								g_cfg.player.type[TEAM].animated_material_color[0] / 255.0f,
								g_cfg.player.type[TEAM].animated_material_color[1] / 255.0f,
								g_cfg.player.type[TEAM].animated_material_color[2] / 255.0f
							};

							m_renderview()->SetBlend(alpha);
							util::color_modulate(animated_color, animated_material);

							animated_material->IncrementReferenceCount();
							animated_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

							m_modelrender()->ForcedMaterialOverride(animated_material);
							original_fn(m_modelrender(), ctx, state, info, bone_to_world);
							m_modelrender()->ForcedMaterialOverride(nullptr);
						}
					}

					if (g_cfg.player.type[TEAM].double_material && g_cfg.player.type[TEAM].chams_type != 6)
					{
						alpha = (float)g_cfg.player.type[TEAM].double_material_color.a() / 255.0f;

						float double_color[3] =
						{
							g_cfg.player.type[TEAM].double_material_color[0] / 255.0f,
							g_cfg.player.type[TEAM].double_material_color[1] / 255.0f,
							g_cfg.player.type[TEAM].double_material_color[2] / 255.0f
						};

						m_renderview()->SetBlend(alpha);
						util::color_modulate(double_color, double_material);

						double_material->IncrementReferenceCount();
						double_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

						m_modelrender()->ForcedMaterialOverride(double_material);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);
					}

					called_original = true;
				}
				else if (g_cfg.player.type[TEAM].chams[PLAYER_CHAMS_VISIBLE])
				{
					auto alpha = (float)g_cfg.player.type[TEAM].chams_color.a() / 255.0f;

					float normal_color[3] =
					{
						g_cfg.player.type[TEAM].chams_color[0] / 255.0f,
						g_cfg.player.type[TEAM].chams_color[1] / 255.0f,
						g_cfg.player.type[TEAM].chams_color[2] / 255.0f
					};

					m_renderview()->SetBlend(alpha);
					util::color_modulate(normal_color, material);

					material->IncrementReferenceCount();
					material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					m_modelrender()->ForcedMaterialOverride(material);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);

					if (g_cfg.player.type[TEAM].animated_material)
					{
						auto animated_material = materials[9];

						if (animated_material)
						{
							alpha = (float)g_cfg.player.type[TEAM].animated_material_color.a() / 255.0f;

							float animated_color[3] =
							{
								g_cfg.player.type[TEAM].animated_material_color[0] / 255.0f,
								g_cfg.player.type[TEAM].animated_material_color[1] / 255.0f,
								g_cfg.player.type[TEAM].animated_material_color[2] / 255.0f
							};

							m_renderview()->SetBlend(alpha);
							util::color_modulate(animated_color, animated_material);

							animated_material->IncrementReferenceCount();
							animated_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

							m_modelrender()->ForcedMaterialOverride(animated_material);
							original_fn(m_modelrender(), ctx, state, info, bone_to_world);
							m_modelrender()->ForcedMaterialOverride(nullptr);
						}
					}

					if (g_cfg.player.type[TEAM].double_material && g_cfg.player.type[TEAM].chams_type != 6)
					{
						alpha = (float)g_cfg.player.type[TEAM].double_material_color.a() / 255.0f;

						float double_color[3] =
						{
							g_cfg.player.type[TEAM].double_material_color[0] / 255.0f,
							g_cfg.player.type[TEAM].double_material_color[1] / 255.0f,
							g_cfg.player.type[TEAM].double_material_color[2] / 255.0f
						};

						m_renderview()->SetBlend(alpha);
						util::color_modulate(double_color, double_material);

						double_material->IncrementReferenceCount();
						double_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

						m_modelrender()->ForcedMaterialOverride(double_material);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);
					}

					called_original = true;
				}
			}

			if (!called_original)
				return original_fn(m_modelrender(), ctx, state, info, bone_to_world);
		}
		else if (m_input()->m_fCameraInThirdPerson)
		{
			auto alpha_modifier = 1.0f;

			if (g_cfg.player.transparency_in_scope && g_ctx.globals.scoped)
				alpha_modifier = g_cfg.player.transparency_in_scope_amount;

			auto material = materials[g_cfg.player.type[LOCAL].chams_type];
			auto double_material = materials[g_cfg.player.fake_double_material_material];

			if (material && double_material && !material->IsErrorMaterial() && !double_material->IsErrorMaterial())
			{
				if (g_cfg.player.type[LOCAL].chams[PLAYER_CHAMS_VISIBLE] && g_cfg.player.type[LOCAL].chams[PLAYER_CHAMS_INVISIBLE])
				{
					auto alpha = (float)g_cfg.player.type[LOCAL].xqz_color.a() / 255.0f * alpha_modifier;

					float xqz_color[3] =
					{
						g_cfg.player.type[LOCAL].xqz_color[0] / 255.0f,
						g_cfg.player.type[LOCAL].xqz_color[1] / 255.0f,
						g_cfg.player.type[LOCAL].xqz_color[2] / 255.0f
					};

					m_renderview()->SetBlend(alpha);
					util::color_modulate(xqz_color, material);

					material->IncrementReferenceCount();
					material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

					m_modelrender()->ForcedMaterialOverride(material);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);

					alpha = (float)g_cfg.player.type[LOCAL].chams_color.a() / 255.0f * alpha_modifier;

					float normal_color[3] =
					{
						g_cfg.player.type[LOCAL].chams_color[0] / 255.0f,
						g_cfg.player.type[LOCAL].chams_color[1] / 255.0f,
						g_cfg.player.type[LOCAL].chams_color[2] / 255.0f
					};

					m_renderview()->SetBlend(alpha);
					util::color_modulate(normal_color, material);

					material->IncrementReferenceCount();
					material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					m_modelrender()->ForcedMaterialOverride(material);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);

					if (g_cfg.player.type[LOCAL].animated_material)
					{
						auto animated_material = materials[9];

						if (animated_material)
						{
							auto alpha = (float)g_cfg.player.type[LOCAL].animated_material_color.a() / 255.0f;

							float animated_color[3] =
							{
								g_cfg.player.type[LOCAL].animated_material_color[0] / 255.0f,
								g_cfg.player.type[LOCAL].animated_material_color[1] / 255.0f,
								g_cfg.player.type[LOCAL].animated_material_color[2] / 255.0f
							};

							m_renderview()->SetBlend(alpha);
							util::color_modulate(animated_color, animated_material);

							animated_material->IncrementReferenceCount();
							animated_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

							m_modelrender()->ForcedMaterialOverride(animated_material);
							original_fn(m_modelrender(), ctx, state, info, bone_to_world);
							m_modelrender()->ForcedMaterialOverride(nullptr);
						}
					}

					if (g_cfg.player.type[LOCAL].double_material && g_cfg.player.type[LOCAL].chams_type != 6)
					{
						alpha = (float)g_cfg.player.type[LOCAL].double_material_color.a() / 255.0f * alpha_modifier;

						float double_color[3] =
						{
							g_cfg.player.type[LOCAL].double_material_color[0] / 255.0f,
							g_cfg.player.type[LOCAL].double_material_color[1] / 255.0f,
							g_cfg.player.type[LOCAL].double_material_color[2] / 255.0f
						};

						m_renderview()->SetBlend(alpha);
						util::color_modulate(double_color, double_material);

						double_material->IncrementReferenceCount();
						double_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

						m_modelrender()->ForcedMaterialOverride(double_material);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);
					}

					called_original = true;
				}
				else if (g_cfg.player.type[LOCAL].chams[PLAYER_CHAMS_VISIBLE])
				{
					auto alpha = (float)g_cfg.player.type[LOCAL].chams_color.a() / 255.0f * alpha_modifier;

					float normal_color[3] =
					{
						g_cfg.player.type[LOCAL].chams_color[0] / 255.0f,
						g_cfg.player.type[LOCAL].chams_color[1] / 255.0f,
						g_cfg.player.type[LOCAL].chams_color[2] / 255.0f
					};

					m_renderview()->SetBlend(alpha);
					util::color_modulate(normal_color, material);

					material->IncrementReferenceCount();
					material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					m_modelrender()->ForcedMaterialOverride(material);
					original_fn(m_modelrender(), ctx, state, info, bone_to_world);
					m_modelrender()->ForcedMaterialOverride(nullptr);

					if (g_cfg.player.type[LOCAL].animated_material)
					{
						auto animated_material = materials[9];

						if (animated_material)
						{
							alpha = (float)g_cfg.player.type[LOCAL].animated_material_color.a() / 255.0f * alpha_modifier;

							float animated_color[3] =
							{
								g_cfg.player.type[LOCAL].animated_material_color[0] / 255.0f,
								g_cfg.player.type[LOCAL].animated_material_color[1] / 255.0f,
								g_cfg.player.type[LOCAL].animated_material_color[2] / 255.0f
							};

							m_renderview()->SetBlend(alpha);
							util::color_modulate(animated_color, animated_material);

							animated_material->IncrementReferenceCount();
							animated_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

							m_modelrender()->ForcedMaterialOverride(animated_material);
							original_fn(m_modelrender(), ctx, state, info, bone_to_world);
							m_modelrender()->ForcedMaterialOverride(nullptr);
						}
					}

					if (g_cfg.player.type[LOCAL].double_material && g_cfg.player.type[LOCAL].chams_type != 6)
					{
						alpha = (float)g_cfg.player.type[LOCAL].double_material_color.a() / 255.0f * alpha_modifier;

						float double_color[3] =
						{
							g_cfg.player.type[LOCAL].double_material_color[0] / 255.0f,
							g_cfg.player.type[LOCAL].double_material_color[1] / 255.0f,
							g_cfg.player.type[LOCAL].double_material_color[2] / 255.0f
						};

						m_renderview()->SetBlend(alpha);
						util::color_modulate(double_color, double_material);

						double_material->IncrementReferenceCount();
						double_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

						m_modelrender()->ForcedMaterialOverride(double_material);
						original_fn(m_modelrender(), ctx, state, info, bone_to_world);
						m_modelrender()->ForcedMaterialOverride(nullptr);
					}

					called_original = true;
				}
			}

			if (!called_original && g_cfg.player.layered)
			{
				m_renderview()->SetBlend(alpha_modifier);
				m_renderview()->SetColorModulation(1.0f, 1.0f, 1.0f);

				original_fn(m_modelrender(), ctx, state, info, bone_to_world);
			}

			if (g_cfg.player.fake_chams_enable)
			{
				if (!local_animations::get().local_data.visualize_lag)
				{
					for (auto& i : g_ctx.globals.fake_matrix)
					{
						i[0][3] += info.origin.x;
						i[1][3] += info.origin.y;
						i[2][3] += info.origin.z;
					}
				}

				auto alpha = (float)g_cfg.player.fake_chams_color.a() / 255.0f;
				material = materials[g_cfg.player.fake_chams_type];

				float fake_color[3] =
				{
					g_cfg.player.fake_chams_color[0] / 255.0f,
					g_cfg.player.fake_chams_color[1] / 255.0f,
					g_cfg.player.fake_chams_color[2] / 255.0f
				};

				m_renderview()->SetBlend(alpha);
				util::color_modulate(fake_color, material);

				material->IncrementReferenceCount();
				material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

				m_modelrender()->ForcedMaterialOverride(material);
				original_fn(m_modelrender(), ctx, state, info, g_ctx.globals.fake_matrix);
				m_modelrender()->ForcedMaterialOverride(nullptr);

				if (g_cfg.player.fake_animated_material)
				{
					auto animated_material = materials[9];

					if (animated_material)
					{
						alpha = (float)g_cfg.player.fake_animated_material_color.a() / 255.0f;

						float animated_color[3] =
						{
							g_cfg.player.fake_animated_material_color[0] / 255.0f,
							g_cfg.player.fake_animated_material_color[1] / 255.0f,
							g_cfg.player.fake_animated_material_color[2] / 255.0f
						};

						m_renderview()->SetBlend(alpha);
						util::color_modulate(animated_color, animated_material);

						animated_material->IncrementReferenceCount();
						animated_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

						m_modelrender()->ForcedMaterialOverride(animated_material);
						original_fn(m_modelrender(), ctx, state, info, g_ctx.globals.fake_matrix);
						m_modelrender()->ForcedMaterialOverride(nullptr);
					}
				}

				if (g_cfg.player.fake_double_material && g_cfg.player.fake_chams_type != 6 && double_material)
				{
					alpha = (float)g_cfg.player.fake_double_material_color.a() / 255.0f;

					float double_color[3] =
					{
						g_cfg.player.fake_double_material_color[0] / 255.0f,
						g_cfg.player.fake_double_material_color[1] / 255.0f,
						g_cfg.player.fake_double_material_color[2] / 255.0f
					};

					m_renderview()->SetBlend(alpha);
					util::color_modulate(double_color, double_material);

					double_material->IncrementReferenceCount();
					double_material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

					m_modelrender()->ForcedMaterialOverride(double_material);
					original_fn(m_modelrender(), ctx, state, info, g_ctx.globals.fake_matrix);
					m_modelrender()->ForcedMaterialOverride(nullptr);
				}

				if (!local_animations::get().local_data.visualize_lag)
				{
					for (auto& i : g_ctx.globals.fake_matrix)
					{
						i[0][3] -= info.origin.x;
						i[1][3] -= info.origin.y;
						i[2][3] -= info.origin.z;
					}
				}
			}

			if (!called_original && !g_cfg.player.layered)
			{
				m_renderview()->SetBlend(alpha_modifier);
				m_renderview()->SetColorModulation(1.0f, 1.0f, 1.0f);

				original_fn(m_modelrender(), ctx, state, info, bone_to_world);
			}
		}
	}
}