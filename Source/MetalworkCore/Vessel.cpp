#pragma warning(disable:4458)

#include "Vessel.h"
#include "Arena.h"
#include "Kinematics.h"
#include "CoreDefinitions.h"
#include "CoreInterface.h"

void Vessel::AttachWeapon(Weapon* weapon)
{
	root->JoinRevolute(weapon->root, {0, 0}, -weapon->root->offset);
	this->weapon = weapon;
}

void Vessel::SetPosition(vec2 position)
{
	RigidObject::SetPosition(position);
	if (weapon) weapon->SetPosition(position);
}

void Vessel::SetPlayerInput(vec2 move_in)
{
	const float max_speed = 10;

	float mass = root->GetMass();
	vec2 velocity = root->GetVelocity();
	float ang_vel = root->GetAngVelocity();
	vec2 accel = move_in * 0.1;
	float angle = remainderf(root->GetAngle(), 2*pi);
	
	vec2 add_velocity = accel * frame_time;
	vec2 new_velocity = velocity + add_velocity;

	float sq_speed = new_velocity.sqlen();
	if (sq_speed > sqr(max_speed))
	{
		new_velocity *= max_speed / sqrt(sq_speed);
		add_velocity = new_velocity - velocity;
		accel = add_velocity * fps;
	}

	root->ApplyForce(accel * mass);

	float desired_ang_accel = 0;

	float move_magnitude;
	vec2 move_dir = move_in.normalized(move_magnitude);
	if (move_dir != 0)
	{
		float move_angle = move_dir.rotate90_cw().angle();

		float k = min(move_magnitude / 300, 1.f);
		move_angle *= k;

		float dist = move_angle - angle;

		float move_ang_accel = copysign(100.f * k, dist);
		float move_ang_deccel = copysign(100.f * k, -dist);

		float brake_dist = 0.5 * sqr(ang_vel) / (ang_vel * dist > 0 ? -move_ang_deccel : -move_ang_accel);
		brake_dist += (ang_vel + 0.5 * move_ang_accel * frame_time) * frame_time;

		log(DisplayLog(0), "dist = {}", 180 * dist / pi);

		if ((dist > 0 && dist > brake_dist) || (dist < 0 && dist < brake_dist))
		{
			desired_ang_accel = move_ang_accel;
		}
		else if (dist != 0)
		{
			desired_ang_accel = -sqr(ang_vel) / (2 * dist);
		}
	}

	float desired_rolypoly_angvel = (cos(angle / 1) - 1) * copysign(20, angle);
	ang_vel += desired_ang_accel * frame_time;

	float delta = desired_rolypoly_angvel - ang_vel;
	desired_ang_accel += clamp(delta * fps, -20.f, 20.f);

	root->ApplyTorque(desired_ang_accel * root->GetInertia());
}