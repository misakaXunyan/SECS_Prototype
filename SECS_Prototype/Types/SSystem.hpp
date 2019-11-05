/*****************************************************************************************
							 Copyrights   SaeruHikari
Description:					System master class.
Details:
*******************************************************************************************/
#pragma once
#include "..\Managers\SArcheTypeManager.hpp"
#include "..\Managers\SEntityManager.hpp"
#include <functional>
#include <iostream>
#include "Containers\SSystemList.hpp"
// A system should: 
// 1. catch available components fast(Cache ArcheTypes ...)
// 2. catch other systems.
// 3. filter.
namespace SECS
{
	class SSystemGroup
	{
	public:
		static SSystemList* GetSystemGroupList(std::string GroupName)
		{
			if (SystemGroups.find(GroupName) != SystemGroups.end())
				return &SystemGroups[GroupName];
			else return nullptr;
		}
		static void AddSystemToGroupList(std::string GroupName, SSystem* sys)
		{
			SystemGroups[GroupName].push_back(sys);
		}
	private:
		static std::unordered_map<std::string, SSystemList> SystemGroups;
		static std::unordered_map<std::string, SSystemList> InitSystemGroups()
		{
			std::unordered_map<std::string, SSystemList> _Groups;
			return _Groups;
		}
	};
	std::unordered_map<std::string, SSystemList> SSystemGroup::SystemGroups = SSystemGroup::InitSystemGroups();
	
	
	template<typename T>
	struct Register 
	{
	public:
		Register(std::string SysName, std::string GroupName)
		{
#if defined(DEBUG) || defined(_DEBUG)
			std::cout << "REGISTER: " << SysName << " To " << GroupName << std::endl;
#endif // 
			SSystemGroup::AddSystemToGroupList(GroupName, new T());
		}
	};

#define REGISTRY_SYSTEM_TO_GROUP(__className, __groupName)\
	  struct __className##_##__groupName##_SytemRegister_Helper\
	 {\
          private:\
              static Register<__className>* m_##__className##_##__groupName##_register;\
	 };\
     Register<__className>* __className##_##__groupName##_SytemRegister_Helper::m_##__className##_##__groupName##_register = new Register<__className>(#__className, # __groupName);\



#define DEF_SYSTEM_COMPONENTS(...) __VA_ARGS__

#define DEF_SYSTEM_UPDATE(__className, __funcAddress, __THIS, ...)\
private:\
ComponentSystemData<__className, __VA_ARGS__> SystemData = ComponentSystemData<__className, __VA_ARGS__>(__THIS, __funcAddress);\
public:\


	REGISTRY_SYSTEM_TO_GROUP(SSystem, TestGroup);
	class SSystem
	{
		friend class SWorld;
	protected:
		// Template class to hold reflection informations.
		template<class T, typename ... Cs>
		struct ComponentSystemData
		{
			friend class SSystem;
			friend class SWorld;
			friend class SEntityManager;
			friend class SArcheTypeManager;
		public:
			ComponentSystemData(T* __this, void(T::* pFunc)(SEntity, Cs*...))
				:_this(__this), callback(pFunc)
			{

			}
			inline void __collectInfos_Internal(SEntityManager* entm, SArcheTypeManager* arcm)
			{
				auto res = arcm->CompsGetArcheTypes<Cs...>();
				if(arcm->CompsGetArcheTypes<Cs...>().size() > 0)
				_this->m_Types.insert(_this->m_Types.end(),
					res.begin(), res.end());
			}
			// Callback
			inline void __update_Internal(SEntityManager* EntityManager)
			{
				EntityManager->Each<T, Cs ...>(callback, _this, _this->m_Types);
			}
		public:
			std::function<void(T*, SEntity, Cs *...)> callback;
			T* _this = nullptr;
			// To fetch at initial time.
		};
	private:
		// Properties
		SArcheTypeList m_Types;
		size_t RefSystemCount = -1;
		SSystem* RefSystems = nullptr;

	public:
		DEF_SYSTEM_UPDATE(SSystem, &SSystem::Update_Implementation, this, DEF_SYSTEM_COMPONENTS(ComponentA, ComponentB));
		inline void Update_Implementation(SEntity entity, ComponentA* a, ComponentB* b)
		{
			a->x += 5;
			a->y += 5;
			a->z += 5;
		}

		inline virtual void Update(SEntityManager* EntityManager)
		{
			SystemData.__update_Internal(EntityManager);
		}

		inline virtual void CollectSystemBoostInfos(SEntityManager* entm, SArcheTypeManager* arcm)
		{
			SystemData.__collectInfos_Internal(entm, arcm);
		}

	public:
		SSystem()
		{

		}
		SSystem(SArcheTypeManager* _archeTypeManager, SEntityManager* _entityManager)
		{
			SystemData.__collectInfos_Internal(_entityManager, _archeTypeManager);
		}
		virtual ~SSystem() {};
	};
}