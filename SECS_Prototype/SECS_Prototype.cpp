﻿// SECS_Prototype.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <windows.h>
#include "TemplateUtils/TemplateUtils.hpp"
#include "TemplateUtils/TemplatePackUtils.hpp"
#include "Managers/SEntityManager.hpp"
#include <sysinfoapi.h>
#include <synchapi.h>
#include "Types/SWorld.hpp"

using namespace std;
using namespace SECS;


void TestNew()
{
	::Sleep(1000);
	double time3 = ::GetTickCount64();
	for (int i = 0; i < 200000; i++)
	{
		void* ptr = malloc(4 * sizeof(Actor));
		new(ptr) Actor();
		ptr = (unsigned char*)ptr + sizeof(Actor);
		new(ptr) Actor();
		ptr = (unsigned char*)ptr + sizeof(Actor);
		new(ptr) Actor();
		ptr = (unsigned char*)ptr + sizeof(Actor);
		new(ptr) Actor();
		ptr = (unsigned char*)ptr + sizeof(Actor);
	}
	double time4 = ::GetTickCount64();
	std::cout << time4 - time3 << std::endl;

	SWorld* world = new SWorld();
	double time = ::GetTickCount64();
	for (int i = 0; i < 200000; i++)
	{
		SWorld::CreateEntity<ComponentA, ComponentB, ComponentD>(world);
		SWorld::CreateEntity<ComponentA, ComponentB, ComponentD>(world);
		SWorld::CreateEntity<ComponentA, ComponentB, ComponentD>(world);
		SWorld::CreateEntity<ComponentA, ComponentB, ComponentD>(world);
		//SEntityManager::CreateEntity<float>();
	}
	double time2 = ::GetTickCount64();
	std::cout << time2 - time << std::endl;
}

int main()
{

	// SEntityManager::CreateEntity<CompTs...>();  return a valid entity.
	// Need to do:
	// 1. Entity manager: Create the entity. The entity value(index and generation).
	//    After creation of the index:														√
	// 2. Create or find an archetype. Archetype is a specific combination of components.
	//    This is used for chunk structure and combination infos(just like type).			√
	// 3. After creating the archetype, we will construct it:								√
	//    (1) If we already have one, find a valid chunk.									√
	//    (2) If not, create a new chunk list.												√	
	//    (3) Place entity and create all components of template input in the chunk.		√
	// 4. Finish construction and work with freeChunkList pointer, etc...					√
	SWorld* world = new SWorld();
	auto en0 = SWorld::CreateEntity<ComponentA, ComponentB, ComponentC, ComponentD>(world); //0
	auto en1 = SWorld::CreateEntity<ComponentA, ComponentD>(world);  
	auto en2 = SWorld::CreateEntity<ComponentA>(world);
	auto en3 = SWorld::CreateEntity<ComponentB>(world);
	auto en4 = SWorld::CreateEntity<ComponentA, ComponentC, ComponentD>(world);
	auto en5 = SWorld::CreateEntity<ComponentC>(world);
	auto en6 = SWorld::CreateEntity<ComponentA, ComponentB>(world);                         //6


	SSystem* sys = new SSystem(world->GetArcheTypeManager(), world->GetEntityManager());

	::Sleep(1000);
	float time = ::GetTickCount();
	for (int i = 0; i < 5000000; i++)
	{
		sys->Update();
	}
	float time2 = ::GetTickCount();
	std::cout << time2 - time << std::endl;
	
	time = ::GetTickCount();
	for (int i = 0; i < 5000000; i++)
	{
		world->Each<ComponentA, ComponentB>([=](SEntity entity, ComponentA* a, ComponentB* b) {
			
		});
	}
	time2 = ::GetTickCount();
	std::cout << time2 - time << std::endl;
	

	//! DO NOT support multi component like this:
	//SEntityManager::CreateEntity<ComponentA, ComponentA, ComponentA>();
	

	//(unimplemented)(4)! Case needs customized constructor: 
	//auto entity = SEntityManager::RegistryEntity<ComponentA, ComponentB, ComponentC>();
	//SEntityManager::ConsturctComponent<ComponentA>(params...);
	//SEntityManager::ConsturctComponent<ComponentB>(params...);

	return 0;
}

