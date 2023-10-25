#include "B2Vessel.h"

#include "b2dJson/b2dJson.h"


//B2Vessel::B2Vessel(b2World* world, string_view body_name)
//{
//	string path = "crafts/"s + body_name.data();
//
//	b2dJson json;
//
//	string error;
//	json.readFromFile(path.c_str(), error, world);
//
//	body = json.getBodyByName("craft");
//	body->SetLinearDamping(0.3f);
//
//	json.getAllBodies(parts);
//
//	//path = string("weapons/") + weapon;
//	//json.readFromFile(path.c_str(), msg, world);
//
//	//parts.resize(0);
//	//json.getAllBodies(parts);
//	//for (b2Body* part : parts)
//	//	part->SetTransform(part->GetPosition() + position, 0);
//
//	//b2Body* origin = json.getBodyByName("origin");
//
//	//b2RevoluteJointDef jointDef;
//	//jointDef.collideConnected = false;
//	//jointDef.Initialize(body, origin, position);
//	//world->CreateJoint(&jointDef);
//
//}