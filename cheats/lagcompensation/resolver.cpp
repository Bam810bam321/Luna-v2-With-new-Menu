// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "animation_system.h"
#include "..\ragebot\aim.h"
float EyeAngles[64] = { 0 };

void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
	player = e;
	player_record = record;

	original_pitch = math::normalize_pitch(pitch);
	original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
}

void resolver::initialize_yaw(player_t* e, adjust_data* record)
{
	player = e;

	player_record = record;

	player_record->left = b_yaw(player, player->m_angEyeAngles().y, 1);
	player_record->right = b_yaw(player, player->m_angEyeAngles().y, 2);
	player_record->middle = b_yaw(player, player->m_angEyeAngles().y, 3);
}

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	side = false;
	fake = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}

bool resolver::IsAdjustingBalance()
{
	for (int i = 0; i < 13; i++)
	{
		const int activity = player->sequence_activity(player_record->layers[i].m_nSequence);
		if (activity == 979)
		{
			return true;
		}
	}
	return false;
}

bool resolver::is_breaking_lby(AnimationLayer cur_layer, AnimationLayer prev_layer)
{
	if (IsAdjustingBalance())
	{
		if (IsAdjustingBalance())
		{
			if ((prev_layer.m_flCycle != cur_layer.m_flCycle) || cur_layer.m_flWeight == 1.f)
			{
				return true;
			}
			else if (cur_layer.m_flWeight == 0.f && (prev_layer.m_flCycle > 0.92f && cur_layer.m_flCycle > 0.92f))
			{
				return true;
			}
		}
		return false;
	}
	return false;
}

static auto GetSmoothedVelocity = [](float min_delta, Vector a, Vector b) {
	Vector delta = a - b;
	float delta_length = delta.Length();

	if (delta_length <= min_delta)
	{
		Vector result;

		if (-min_delta <= delta_length)
			return a;
		else
		{
			float iradius = 1.0f / (delta_length + FLT_EPSILON);
			return b - ((delta * iradius) * min_delta);
		}
	}
	else
	{
		float iradius = 1.0f / (delta_length + FLT_EPSILON);
		return b + ((delta * iradius) * min_delta);
	}
};

bool resolver::is_slow_walking()
{
	auto entity = player;
	float velocity_2D[64], old_velocity_2D[64];
	if (entity->m_vecVelocity().Length2D() != velocity_2D[entity->EntIndex()] && entity->m_vecVelocity().Length2D() != NULL) {
		old_velocity_2D[entity->EntIndex()] = velocity_2D[entity->EntIndex()];
		velocity_2D[entity->EntIndex()] = entity->m_vecVelocity().Length2D();
	}

	Vector velocity = entity->m_vecVelocity();
	Vector direction = entity->m_angEyeAngles();

	float speed = velocity.Length();
	direction.y = entity->m_angEyeAngles().y - direction.y;

	if (velocity_2D[entity->EntIndex()] > 1) {
		int tick_counter[64];
		if (velocity_2D[entity->EntIndex()] == old_velocity_2D[entity->EntIndex()])
			tick_counter[entity->EntIndex()] += 1;
		else
			tick_counter[entity->EntIndex()] = 0;

		while (tick_counter[entity->EntIndex()] > (1 / m_globals()->m_intervalpertick) * fabsf(0.1f))
			return true;
	}


	return false;
}
int last_ticks[65];
int resolver::GetChokedPackets() {
	auto ticks = TIME_TO_TICKS(player->m_flSimulationTime() - player->m_flOldSimulationTime());
	if (ticks == 0 && last_ticks[player->EntIndex()] > 0) {
		return last_ticks[player->EntIndex()] - 1;
	}
	else {
		last_ticks[player->EntIndex()] = ticks;
		return ticks;
	}
}


void resolver::get_side_trace()
{
	auto m_side = false;
	auto trace = false;
	if (m_globals()->m_curtime - lock_side > 1.0f)
	{
		auto first_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player, g_ctx.local());
		auto second_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player, g_ctx.local());

		if (first_visible != second_visible)
		{
			trace = true;
			m_side = second_visible;
			lock_side = m_globals()->m_curtime;
		}
		else
		{
			auto first_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first));
			auto second_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second));

			if (fabs(first_position - second_position) > 5.0f)
				m_side = first_position > second_position;
		}
	}
	else
		trace = true;

	if (m_side)
	{
		player_record->type = trace ? TRACE : DIRECTIONAL;
		player_record->curSide = RIGHT;
	}
	else
	{
		player_record->type = trace ? TRACE : DIRECTIONAL;
		player_record->curSide = LEFT;
	}
}

float GetBackwardYaw(player_t* player)
{
	return math::calculate_angle(g_ctx.local()->GetAbsOrigin(), player->GetAbsOrigin()).x;
}

int DetectStand(player_t* player)
{
	Vector src3D, dst3D, forward, right, up, src, dst;
	float back_two, right_two, left_two;
	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5;
	CTraceFilter filter;

	math::angle_vectors(Vector(0, GetBackwardYaw(player), 0), &forward, &right, &up);

	filter.pSkip = player;
	src3D = player->get_shoot_position();
	dst3D = src3D + (forward * 385); //Might want to experiment with other numbers, incase you don't know what the number does, its how far the trace will go. Lower = shorter.

	ray.Init(src3D, dst3D);
	m_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);
	back_two = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 30, dst3D + right * 30);
	m_trace()->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	right_two = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 30, dst3D - right * 30);
	m_trace()->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	left_two = (tr.endpos - tr.startpos).Length();

	if (left_two > right_two)
		return -1;
	else if (right_two > left_two)
		return 1;
	else
		return 0;
}

float GetForwardYaw(player_t* player)
{
	return math::normalize_yaw(GetBackwardYaw(player) + 180.f);
}

resolver_side resolver::FreeStand(player_t* e)
{

	float angle = math::normalize_yaw(e->m_angEyeAngles().y);

	bool forward = fabsf(math::normalize_yaw(angle - GetForwardYaw(e))) < 90.f;

	auto misses = g_ctx.globals.missed_shots[e->EntIndex()];

	int S = DetectStand(e);

	bool HIGHDELTA = misses >= 3;


	switch (misses % 2)
	{
	case 0:
		switch (last_side)
		{
		case RESOLVER_FIRST:
			return RESOLVER_SECOND;
			break;
		case RESOLVER_SECOND:
			return RESOLVER_FIRST;
			break;
		default:
			if (S == 1)
			{
				if (forward)
					return RESOLVER_FIRST;
				else
					return RESOLVER_ORIGINAL;
			}
			else
			{
				if (forward)
					return RESOLVER_FIRST;
				else
					return RESOLVER_SECOND;
			}
			break;
		}
		break;
	case 1:
		switch (last_side)
		{
		case RESOLVER_SECOND:
			return RESOLVER_SECOND;
			break;
		case RESOLVER_FIRST:
			return RESOLVER_FIRST;
			break;
		default:
			if (S == 1)
			{
				if (forward)
					return RESOLVER_FIRST;
				else
					return RESOLVER_SECOND;
			}
			else
			{
				if (forward)
					return RESOLVER_SECOND;
				else
					return RESOLVER_FIRST;
			}
			break;
		}
		break;
	}

}

inline float anglemod(float a)
{
	a = (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
	return a;
}

float ApproachAngle(float target, float value, float speed)
{
	target = anglemod(target);
	value = anglemod(value);

	float delta = target - value;

	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

struct ResolverResult {
	float flMinBodyYaw;
	float flMaxBodyYaw;
	float flEyeYaw;
	float m_flFakeGoalFeetYaw;
	float speed;
};

float resolver::b_yaw(player_t* player, float angle, int n)
{
	auto animState = player->get_animation_state();
	float m_flFakeGoalFeetYaw = 0.0f;
	Vector velocity = player->m_vecVelocity();
	float spd = velocity.LengthSqr();
	if (spd > std::powf(1.2f * 260.0f, 2.f)) {
		Vector velocity_normalized = velocity.Normalized();
		velocity = velocity_normalized * (1.2f * 260.0f);
	}

	float m_flChokedTime = animState->m_flLastClientSideAnimationUpdateTime;
	float v25 = math::clamp(player->m_flDuckAmount() + animState->m_fLandingDuckAdditiveSomething, 0.0f, 1.0f);
	float v26 = animState->m_fDuckAmount;
	float v27 = m_flChokedTime * 6.0f;
	float v28;

	// clamp
	if ((v25 - v26) <= v27) {
		if (-v27 <= (v25 - v26))
			v28 = v25;
		else
			v28 = v26 - v27;
	}
	else {
		v28 = v26 + v27;
	}

	float flDuckAmount = math::clamp(v28, 0.0f, 1.0f);

	Vector animationVelocity = GetSmoothedVelocity(m_flChokedTime * 2000.0f, velocity, player->m_vecVelocity());
	float speed = std::fminf(animationVelocity.Length(), 260.0f);

	auto weapon = player->m_hActiveWeapon();

	float flMaxMovementSpeed = 260.0f;
	if (weapon && weapon->get_csweapon_info()) {
		flMaxMovementSpeed = std::fmaxf(weapon->get_csweapon_info()->flMaxPlayerSpeed, 0.001f);
	}

	float flRunningSpeed = speed / (flMaxMovementSpeed * 0.520f);
	float flDuckingSpeed = speed / (flMaxMovementSpeed * 0.340f);

	flRunningSpeed = math::clamp(flRunningSpeed, 0.0f, 1.0f);

	float flYawModifier = (((animState->m_flStopToFullRunningFraction * -0.3f) - 0.2f) * flRunningSpeed) + 1.0f;
	if (flDuckAmount > 0.0f) {
		float flDuckingSpeed = math::clamp(flDuckingSpeed, 0.0f, 1.0f);
		flYawModifier += (flDuckAmount * flDuckingSpeed) * (0.5f - flYawModifier);
	}

	auto test = animState->yaw_desync_adjustment();

	float flMinBodyYaw = -58.0f * flYawModifier;
	float flMaxBodyYaw = 58.0f * flYawModifier;

	float flEyeYaw = angle;
	float flEyeDiff = std::remainderf(flEyeYaw - m_flFakeGoalFeetYaw, 360.f);

	if (flEyeDiff <= flMaxBodyYaw) {
		if (flMinBodyYaw > flEyeDiff)
			m_flFakeGoalFeetYaw = fabs(flMinBodyYaw) + flEyeYaw;
	}
	else {
		m_flFakeGoalFeetYaw = flEyeYaw - fabs(flMaxBodyYaw);
	}

	m_flFakeGoalFeetYaw = std::remainderf(m_flFakeGoalFeetYaw, 360.f);

	if (speed > 0.1f || fabs(velocity.z) > 100.0f) {
		m_flFakeGoalFeetYaw = ApproachAngle(
			flEyeYaw,
			m_flFakeGoalFeetYaw,
			((animState->m_flStopToFullRunningFraction * 20.0f) + 30.0f)
			* m_flChokedTime);
	}
	else {
		m_flFakeGoalFeetYaw = ApproachAngle(
			player->m_flLowerBodyYawTarget(),
			m_flFakeGoalFeetYaw,
			m_flChokedTime * 100.0f);
	}

	float Left = flMinBodyYaw;
	float Right = flMaxBodyYaw;
	float middle = m_flFakeGoalFeetYaw;

	math::normalize_yaw(m_flFakeGoalFeetYaw);

	ResolverResult result;
	result.flMinBodyYaw = flMinBodyYaw;
	result.flMaxBodyYaw = flMaxBodyYaw;
	result.flEyeYaw = flEyeYaw;
	result.m_flFakeGoalFeetYaw = m_flFakeGoalFeetYaw;
	result.speed = speed;

	if (n == 1)
		return flMinBodyYaw;
	else if (n == 2)
		return flMaxBodyYaw;
	else if (n == 3)
		return flEyeYaw;
	else if (n == 5)
		return m_flFakeGoalFeetYaw;

	if (n == 4)
	{
		return speed;
	} // get player speed
}

bool resolver::IsFakewalking(player_t* entity)
{
	bool
		bFakewalking = false,
		stage1 = false,			// stages needed cause we are iterating all layers, eitherwise won't work :)
		stage2 = false,
		stage3 = false;

	for (int i = 0; i < entity->animlayer_count(); i++)
	{
		if (player_record->layers[i].m_nSequence == 26 && player_record->layers[i].m_flWeight < 0.4f)
			stage1 = true;
		if (player_record->layers[i].m_nSequence == 7 && player_record->layers[i].m_flWeight > 0.001f)
			stage2 = true;
		if (player_record->layers[i].m_nSequence == 2 && player_record->layers[i].m_flWeight == 0)
			stage3 = true;
	}

	if (stage1 && stage2)
		bFakewalking = (stage3 || (player->m_fFlags() & FL_DUCKING)); // since weight from stage3 can be 0 aswell when crouching, we need this kind of check, cause you can fakewalk while crouching, thats why it's nested under stage1 and stage2	
	else
		bFakewalking = false;

	return bFakewalking;
}

bool CanSeeHitbox(player_t* entity, int HITBOX)
{
	return g_ctx.local()->CanSeePlayer(entity, entity->hitbox_position(HITBOX));
}

struct {
	float movinglby[64];
}records;

bool resolver::Saw(player_t* entity)
{
	if (!(CanSeeHitbox(entity, HITBOX_HEAD) && CanSeeHitbox(entity, HITBOX_LEFT_FOOT) && CanSeeHitbox(entity, HITBOX_RIGHT_FOOT))
		|| (CanSeeHitbox(entity, HITBOX_HEAD) && CanSeeHitbox(entity, HITBOX_LEFT_FOOT) && CanSeeHitbox(entity, HITBOX_RIGHT_FOOT)))
		return false;

	return true;
}

resolver_side resolver::TraceSide(player_t* entity)
{
	auto first_visible = util::visible(g_ctx.globals.eye_pos, entity->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.positive), player, g_ctx.local());
	auto second_visible = util::visible(g_ctx.globals.eye_pos, entity->hitbox_position_matrix(HITBOX_LEFT_FOOT, player_record->matrixes_data.negative), player, g_ctx.local());
	auto main_visible = util::visible(g_ctx.globals.eye_pos, entity->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.main), player, g_ctx.local());
	if (main_visible)
	{
		if (first_visible)
			return RESOLVER_FIRST;
		else if (second_visible)
			return  RESOLVER_SECOND;
	}
	else
	{
		if (first_visible)
			return  RESOLVER_FIRST;
		else if (second_visible)
			return  RESOLVER_SECOND;
	}

	return RESOLVER_ORIGINAL;
}

bool ValidPitch(player_t* entity)
{
	int pitch = entity->m_angEyeAngles().x;
	return pitch == 0 || pitch > 89 || pitch < -89;
}

//void resolver::NoSpreadResolver()
//{
//	if (last_side != RESOLVER_ORIGINAL)
//	{
//		switch (last_side)
//		{
//		case RESOLVER_ORIGINAL:
//			g_ctx.globals.missed_shots[player->EntIndex()] = 0;
//			fake = true;
//			break;
//		case RESOLVER_ZERO:
//			player_record->type = BRUTEFORCE;
//			player_record->side = RESOLVER_LOW_FIRST;
//
//			was_first_bruteforce = false;
//			was_second_bruteforce = false;
//			return;
//		case RESOLVER_FIRST:
//			player_record->type = BRUTEFORCE;
//			player_record->side = was_second_bruteforce ? RESOLVER_ZERO : RESOLVER_SECOND;
//
//			was_first_bruteforce = true;
//			return;
//		case RESOLVER_SECOND:
//			player_record->type = BRUTEFORCE;
//			player_record->side = was_first_bruteforce ? RESOLVER_ZERO : RESOLVER_FIRST;
//
//			was_second_bruteforce = true;
//			return;
//		case RESOLVER_LOW_FIRST:
//			player_record->type = BRUTEFORCE;
//			player_record->side = RESOLVER_LOW_SECOND;
//			return;
//		case RESOLVER_LOW_SECOND:
//			player_record->type = BRUTEFORCE;
//			player_record->side = RESOLVER_FIRST;
//			return;
//		}
//	}
//	switch (g_ctx.globals.missed_shots[player->EntIndex()])
//	{
//	case 0:
//		player_record->side = RESOLVER_SECOND;
//		break;
//
//	case 1:
//		player_record->side = RESOLVER_FIRST;
//		break;
//
//	case 2:
//		player_record->side = RESOLVER_LOW_FIRST;
//		break;
//
//	case 3:
//		player_record->side = RESOLVER_LOW_SECOND;
//		break;
//
//	case 4:
//		player_record->side = RESOLVER_FIRST;
//		break;
//
//	case 5:
//		player_record->side = RESOLVER_SECOND;
//		break;
//	}
//	if (g_ctx.globals.missed_shots[player->EntIndex()] == 5)
//		g_ctx.globals.missed_shots[player->EntIndex()] = 0;
//}



void resolver::resolve_yaw()
{

	player_info_t player_info;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	if (player_info.fakeplayer || !g_ctx.local()->is_alive() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum()) //-V807
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	if (aim::get().last_target[player->EntIndex()].record.type != LBY)
	{
		switch (last_side)
		{
		case RESOLVER_ORIGINAL:
			g_ctx.globals.missed_shots[player->EntIndex()] = 0;
			fake = true;
			break;
		case RESOLVER_ZERO:
			player_record->type = LAYERS;
			player_record->side = RESOLVER_LOW_FIRST;

			was_first_bruteforce = false;
			was_second_bruteforce = false;
			return;
		case RESOLVER_FIRST:
			player_record->type = TRACE;
			player_record->side = RESOLVER_LOW_SECOND;

			was_first_bruteforce = false;
			was_second_bruteforce = false;
			return;
		case RESOLVER_SECOND:
			player_record->type = DIRECTIONAL;
			player_record->side = RESOLVER_LOW_SECOND_20;

			was_first_bruteforce = false;
			was_second_bruteforce = false;
			return;
		case RESOLVER_LOW_FIRST:
			player_record->type = ORIGINAL;
			player_record->side = RESOLVER_LOW_SECOND;
			return;
		case RESOLVER_LOW_SECOND:
			player_record->type = BRUTEFORCE;
			player_record->side = RESOLVER_FIRST;
			return;
		}
	}

	auto animstate = player->get_animation_state();

	if (!animstate)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	if (!ValidPitch(player))
	{
		if (g_ctx.globals.missed_shots[player->EntIndex()] == 0)
			player_record->side = RESOLVER_ORIGINAL;
		else
			player_record->side = FreeStand(player);
		return;
	}

	if (fabs(original_pitch) > -89.0f)
		fake = false;
	else if (!fake)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}


	if (fabs(original_pitch) > 89.0f)
		fake = true;
	else if (!fake)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return;
	}

	if (player->m_vecVelocity().Length2D() > 50)
		records.movinglby[player->EntIndex()] = animstate->m_flGoalFeetYaw;

	if (player->m_vecVelocity().Length2D() > 50)
	{
		records.movinglby[player->EntIndex()] = animstate->m_flGoalFeetYaw;
	}
	

	auto delta = math::normalize_yaw(player->m_angEyeAngles().y - original_goal_feet_yaw);
	auto valid_lby = true;

	if (animstate->m_velocity > 0.1f || fabs(animstate->flUpVelocity) > 100.f)
		valid_lby = animstate->m_flTimeSinceStartedMoving < 0.22f;

	/*auto spread = m_cvar()->FindVar("weapon_accuracy_nospread")->GetInt();
	if (spread == 1)
	{
		NoSpreadResolver();
		return;
	}*/

	if (player->m_vecVelocity().Length2D() > 235 || !player->m_fFlags() & FL_ONGROUND)
		player_record->side = RESOLVER_ORIGINAL;
	else if (g_ctx.globals.missed_shots[player->EntIndex()] == 0)
		player_record->side = RESOLVER_SECOND;
	else if (Saw(player))
	{
		if (TraceSide(player) != RESOLVER_ORIGINAL)
			player_record->side = TraceSide(player);
		player_record->type = TRACE;
	}
	else if (fabs(delta) > 30.0f && valid_lby)
	{
		if (g_ctx.globals.missed_shots[player->EntIndex()])
			delta = -delta;

		if (delta > 35.0f)
		{
			player_record->type = LBY;
			player_record->side = RESOLVER_FIRST;
		}
		else if (delta < -35.0f)
		{
			player_record->type = LBY;
			player_record->side = RESOLVER_SECOND;
		}
	}
	else
	{
		auto trace = false;
		if (m_globals()->m_curtime - lock_side > 2.0f)
		{
			auto first_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.positive), player, g_ctx.local());
			auto second_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.negative), player, g_ctx.local());

			if (first_visible != second_visible)
			{
				trace = true;
				side = second_visible;
				lock_side = m_globals()->m_curtime;
			}
			else
			{
				auto first_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.positive));
				auto second_position = g_ctx.globals.eye_pos.DistTo(player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.negative));

				if (fabs(first_position - second_position) > 1.0f)
					side = first_position > second_position;
			}
		}
		else
			trace = true;

		if (side)
		{
			player_record->type = trace ? TRACE : DIRECTIONAL;
			player_record->side = RESOLVER_FIRST;
		}
		else
		{
			player_record->type = trace ? TRACE : DIRECTIONAL;
			player_record->side = RESOLVER_SECOND;
		}
	}
}

float resolver::resolve_pitch()
{
	return original_pitch;
}