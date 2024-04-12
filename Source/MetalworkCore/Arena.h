#pragma once

#include "RigidWorld.h"
#include "Contact.h"

class Arena
{
public:
	struct PlayerInput
	{
		vec2f move = {0, 0};
	};

	struct StepInputs
	{
		vector<flat_map<int, PlayerInput>> clean, dirty;
	};

public:
	vector<class Vessel*> vessels;
	unique_ptr<RigidWorld> rigid_world;
	int step = 0;
	mutex step_mtx;

	vector<unique_ptr<class ArenaEvent>> events;

public:
	Arena(unique_ptr<RigidWorld> rigid_world);
	void Start();
	void Step(StepInputs inputs);
	bool CheckOutEvent(Contact* contact, const type_info* type);

	template<typename EventType, typename... ArgTypes>
	EventType* MakeEvent(Contact* contact, ArgTypes&&... args);

	vector<ArenaEvent*> GetNewEvents() { return move(events_begun); }
	vector<unique_ptr<ArenaEvent>> GetConfirmedEvents() { return move(events_confirmed); }
	vector<unique_ptr<ArenaEvent>> GetDeniedEvents() { return move(events_denied); }

	static METALWORKCORE_API unique_ptr<RigidWorld> MakeWorld();

private:
	struct Snapped
	{
		Body* body;
		vector<pair<Vec2, Vec2>> impulses;
	};

	vector<Snapped> snapped_bodies;
	vector<ArenaEvent*> events_begun;
	vector<unique_ptr<ArenaEvent>> events_confirmed, events_denied;

private:
	bool BeginContact(Contact* contact);
	void EndContact(Contact* contact);
	bool PreSolve(Contact* contact, const void* data);
	void PostSolve(Contact* contact, const void* data);

	friend class B2ContactListener;
};

class METALWORKCORE_API ArenaEvent
{
public:
	int step;
	Contact* contact = nullptr;

public:
	ArenaEvent(int step, Contact* contact = nullptr) : step(step), contact(contact) {}
	ArenaEvent(const ArenaEvent&) = delete;
	ArenaEvent& operator=(const ArenaEvent&) = delete;

	virtual ~ArenaEvent() = default;
 };

class METALWORKCORE_API ImpactEvent : public ArenaEvent
{
public:
	Float impulse;

public:
	ImpactEvent(int step, Contact* contact, Float impulse) : ArenaEvent(step, contact), impulse(impulse) {}

};