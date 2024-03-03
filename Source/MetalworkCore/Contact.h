#pragma once

using Contact = class b2Contact;

pair<class Body*, class Body*> GetBodies(Contact* contact);
int GetNumPoints(Contact* contact);
float GetNormalImpulseMag(Contact* contact, const void* data);
vector<pair<Vec2, Vec2>> GetImpulses(Contact* contact, const void* data, Body* body);