#include "B2World.h"

#include "box2d/b2_World.h"
#include "box2d/b2_Math.h"
#include "b2_user_settings.h"

B2World::B2World(float step_time, float gravity) : step_time(step_time)
{
	xworld = make_unique<b2World>(b2Vec2(0, gravity));
}

void B2World::Capture()
{
	/*for (void* ptr : b2releases)
	{
		allocs.erase(ptr);
		operator delete(ptr);
	}

	b2releases.clear();

	b2allocs.insert(allocs.begin(), allocs.end());

	int b2allocs_size = 0;
	for (int size : views::values(b2allocs))
		b2allocs_size += size;

	data.resize(b2allocs_size + sizeof(b2World));
	char* ptr = &data.front();
	
	memcpy(ptr, xworld.get(), sizeof(b2World));
	ptr += sizeof(b2World);

	for (auto& [alloc_ptr, size] : b2allocs)
	{
		memcpy(ptr, alloc_ptr, size);
		ptr += size;
	}

	allocs = move(b2allocs);
	captured_step = step;*/
}

void B2World::Restore()
{
	/*for (void* ptr : views::keys(b2allocs))
		operator delete(ptr);

	b2allocs.clear();
	b2releases.clear();

	char* data_ptr = &data.front();
	memcpy(xworld.get(), data_ptr, sizeof(b2World));
	data_ptr += sizeof(b2World);

	for (auto& [ptr, size] : allocs)
	{
		memcpy(ptr, data_ptr, size);
		data_ptr += size;
	}
	
	step = captured_step;*/
}

void B2World::Step()
{
	xworld->Step(step_time, 16, 16);
	++step;
}
