#pragma warning(disable:4458)

#include "Vessel.h"
#include "Arena.h"
#include "CoreDefinitions.h"

void Vessel::AttachWeapon(Weapon* weapon)
{
	shared_ptr<Joint> joint = world->CreateRevoluteJoint(root, weapon->root, {0, 0});
	AddJoint(move(joint), weapon_name);
	weapon->SetOwner(this);
	this->weapon = weapon;
}

void Vessel::SetPosition(Vec2 position)
{
	RigidObject::SetPosition(position);
	if (weapon) weapon->SetPosition(position);
}

void Vessel::SetPlayerInput(Vec2 move_in)
{
	const float max_speed = 15;

	root->SetLinearDamping(0.8);
	root->SetGravityScale(0);

	float mass = root->GetMass();
	Vec2 velocity = root->GetVelocity();
	float ang_vel = root->GetAngVelocity();
	Vec2 accel = move_in * 0.1;
	float angle = remainderf(root->GetAngle(), 2*pi);
	
	Vec2 add_velocity = accel * frame_time;
	Vec2 new_velocity = velocity + add_velocity;

	float sq_speed = SqLen(new_velocity);
	if (sq_speed > sqr(max_speed))
	{
		new_velocity *= max_speed / sqrt(sq_speed);
		add_velocity = new_velocity - velocity;
		accel = add_velocity * fps;
	}

	root->ApplyForce(accel * mass);

	float desired_ang_accel = 0;
	float rolypoly_angle = angle;

	float move_magnitude;

	Vec2 move_dir = Normalized(move_in, move_magnitude);

	if (move_dir != 0)
	{
		float move_angle = Angle(Rotated90CW(move_dir));

		float k = min(move_magnitude / 600, 1.f);
		move_angle *= k;

		rolypoly_angle -= move_angle;

		float dist = move_angle - angle;

		constexpr float m_accel = 50;
		constexpr float m_deccel = 50;

		float move_ang_accel = copysign(m_accel, dist);
		float move_ang_deccel = copysign(m_deccel, -dist);

		float brake_dist = 0.5 * sqr(ang_vel) / (ang_vel * dist > 0 ? -move_ang_deccel : -move_ang_accel);
		brake_dist += (ang_vel + 0.5 * move_ang_accel * frame_time) * frame_time;

		if ((dist > 0 && dist > brake_dist) || (dist < 0 && dist < brake_dist))
		{
			//desired_ang_accel = move_ang_accel;
		}
		else if (dist != 0)
		{
			desired_ang_accel = -sqr(ang_vel) / (2 * dist);
			//desired_ang_accel = clamp(desired_ang_accel, -m_deccel, m_deccel);
		}
	}

	float desired_rolypoly_angvel = (cos(rolypoly_angle / 1) - 1) * copysign(20, rolypoly_angle);
	ang_vel += desired_ang_accel * frame_time;

	float delta = desired_rolypoly_angvel - ang_vel;
	desired_ang_accel += clamp(delta * fps, -100.f, 100.f);

	root->ApplyTorque(desired_ang_accel * root->GetInertia());
}