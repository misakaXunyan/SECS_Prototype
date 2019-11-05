#pragma once
#include "../Types/SEntity.hpp"
#include "../Types/SSystem.hpp"
#include "../Types/SComponent.hpp"
#include <vector>
#include <iostream>
#include "SArcheTypeManager.hpp"
#include <functional>

namespace SECS
{
	class SEntityManager
	{
		friend class SWorld;
		friend class SSystem;
	private:
		SEntityManager()
		{

		}
		// Refers
		struct SEntityData
		{
			SChunk* Chunk;
			int IndexInChunk = -1;
			// Generation, this generation will NEVER update unless the exact data in chunk updates.  
			// Any generation else where different from this mains that entity not alive.
			int generation = -1;
		};

		std::vector<SEntityData> m_EntityData;
		std::vector<SEntity> m_Entities;

	private:
		template<typename ... Components>
		inline SEntity CreateEntity(SArcheTypeManager* arMng) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			std::cout << std::endl << "Start Create new entity: " << std::endl;
#endif
			// Create New Entity.
			SEntity NewEntity;
			NewEntity.Index = m_Entities.size();
			NewEntity.generation += 1;

			// Get archetype of this combination of components.
			auto* _archeType = arMng->GetArcheType<Components...>(&NewEntity);
			// Get chunk with the _archtype.
			auto* _chunk = arMng->GetFreeSChunk(_archeType);
#if defined(DEBUG) || defined(_DEBUG)
			assert(_chunk != nullptr);
#endif
			// Fill in the chunk.
			// Entity data:
			SEntityData NewEntityData;
			NewEntityData.Chunk = _chunk;
			NewEntityData.IndexInChunk = _chunk->ConstructionOnChunk<Components...>(std::move(NewEntity));
			NewEntityData.generation = NewEntity.generation;
			// Ensure pushing together
			m_EntityData.push_back(NewEntityData);
			m_Entities.push_back(NewEntity);

#if defined(DEBUG) || defined(_DEBUG)
			std::cout << std::endl << "End Create new entity: Index in chunk: " << NewEntityData.IndexInChunk << std::endl << std::endl;
#endif

			return NewEntity;
		}

		// Returns false when the entity is not found.
		inline bool DestoryEntity(SEntity& entity)
		{
			/*
			if (m_EntityData.size() <= entity.Index) 
				return false;
			if (!IsEntityAlive(entity)) 
				return false;
			// Check if free chunk is empty. If so free it.
			if (m_EntityData[entity.Index].Chunk->properties->ArcheType->freeChunk->properties->Count
				== m_EntityData[entity.Index].Chunk->properties->ArcheType->freeChunk->properties->FreeUnits)
			{
				m_EntityData[entity.Index].Chunk->properties->ArcheType->chunks->pop_back();
				delete m_EntityData[entity.Index].Chunk->properties->ArcheType->freeChunk;
				if (m_EntityData[entity.Index].Chunk->properties->ArcheType->chunks->size() == 0)
					m_EntityData[entity.Index].Chunk->properties->ArcheType->freeChunk = nullptr;
				else m_EntityData[entity.Index].Chunk->properties->ArcheType->freeChunk
					= (*m_EntityData[entity.Index].Chunk->properties->ArcheType->chunks)[m_EntityData[entity.Index].Chunk->properties->ArcheType->chunks->size() - 1];		
			}
			
			// kill entity.Index    move last
			m_EntityData[entity.Index].Chunk->__moveLastEntityComponentFrom(
				m_EntityData[entity.Index].Chunk->properties->ArcheType->freeChunk,
				m_EntityData[entity.Index].IndexInChunk);
		    
			*/
			m_EntityData[entity.Index].generation += 1;
			return true;
		}

		// Check if an entity is alive.
		inline bool IsEntityAlive(const SEntity& entity)
		{
			if (m_EntityData.size() <= entity.Index) 
				return false;
			return (m_EntityData[entity.Index].generation == entity.generation);
		}

		template<typename T>
		inline T* FindComponent(const SEntity& entity)
		{
			if (!IsEntityAlive(entity)) return nullptr;
			else return m_EntityData[entity.Index].Chunk->__getCompPtr<T>(m_EntityData[entity.Index].IndexInChunk);
		}

		template<typename ... Cs>
		inline void Each(std::function<void(SEntity, Cs *...)> func, const SArcheTypeList& arclist) noexcept
		{
			for (int i = 0; i < arclist.size(); i++)
			{
				for (int j = 0; j < arclist[i]->chunks->size(); j++)
				{
					SChunk* _chunk = (*arclist[i]->chunks)[j];
					for (int i = 0; i < _chunk->properties->Count - _chunk->properties->FreeUnits; i++)
					{
						SEntity entity = *_chunk->__getEntityPtr<SEntity>(i);
						if (entity.generation == m_EntityData[entity.Index].generation)
						{
							func(entity, _chunk->__getCompPtr<Cs>(i)...);
						}
					}
				}
			}
		}

		template<typename T, typename ... Cs>
		inline void Each(std::function<void(T*, SEntity, Cs *...)> func, T* caller, const SArcheTypeList& arclist) noexcept
		{
			for (int i = 0; i < arclist.size(); i++)
			{
				for (int j = 0; j < arclist[i]->chunks->size(); j++)
				{
					SChunk* _chunk = (*arclist[i]->chunks)[j];
					for (int i = 0; i < _chunk->properties->Count - _chunk->properties->FreeUnits; i++)
					{
						SEntity entity = *_chunk->__getEntityPtr<SEntity>(i);
						if (entity.generation == m_EntityData[entity.Index].generation)
						{
							func(caller, entity, _chunk->__getCompPtr<Cs>(i)...);
						}
					}
				}
			}
			// Only iterate alive entities.
		}


		template<typename ... Cs>
		inline void Each(std::function<void(SEntity, Cs*...)> func, SArcheTypeManager* arMng) noexcept
		{
			// Deal with arclist.
			Each<Cs...>(func, arMng->CompsGetArcheTypes<Cs...>());
		}

		template<typename T, typename ... Cs>
		inline void Each(std::function<void(T*, SEntity, Cs *...)> func, T* caller, SArcheTypeManager arMng) noexcept
		{
			// Deal with arclist.
			Each<T, Cs...>(func, caller, arMng->CompsGetArcheTypes<Cs...>());
		}
	};

}
