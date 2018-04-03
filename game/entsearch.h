#pragma once

#include "globals.h"
#include <math/vector.h>
#include "entsys.h"
#include "baseentity.h"
#include <cmath>
#include <algorithm>
#include <regex>

#ifndef gpGlobals
struct engine_globals;
extern engine_globals* gpGlobals;
#endif

template<typename T>
inline
typename std::enable_if< std::is_integral< T >::value, T >::type
_abs(T&& val) {
	return std::abs(std::forward<T>(val));
}

template<typename T>
inline
typename std::enable_if< std::is_floating_point< T >::value, T >::type
_abs(T&& val) {
	return std::fabs(std::forward<T>(val));
}

// Finds the `nMax` entities within a sphere around the point `center` in an `nRadius`. nFilter filters the entities by flags: see ENT_FILTER_*
template<typename Tr, typename Tv>
inline size_t UTIL_EntitiesInSphere(const Tv& center, Tr nRadius, base_entity** pResults, size_t nMax, int nFilter = ENT_FILTER_ALL) {
	// Index in pResults
	size_t iIndex = 0;
	Tr nRadiusSquare = nRadius * nRadius;

	std::vector<std::pair<size_t, char[64]>> vecEnts;
	gpGlobals->pEntSys->get_entities(vecEnts);

	for (auto& ent : vecEnts) {
		base_entity* pEnt = gpGlobals->pEntSys->get_entity(ent.first);
		if (!pEnt)
			continue;

		if (nFilter != ENT_FILTER_ALL) {
			if ((pEnt->get_filter() & nFilter) == 0) {
				continue;
			}
		}

		vec3 pos = pEnt->get_abspos();
		Tr nDeltaX = _abs<Tr>(center[0] - pos[0]);
		Tr nDeltaY = _abs<Tr>(center[1] - pos[1]);
		Tr nDeltaZ = _abs<Tr>(center[2] - pos[2]);
		Tr nLengthSquare = nDeltaX * nDeltaX + nDeltaY * nDeltaY + nDeltaZ * nDeltaZ;

		if (nLengthSquare <= nRadiusSquare) {
			pResults[iIndex] = pEnt;
			iIndex++;
			if (nMax == iIndex)
				break;
		}
	}
	return iIndex;
}

// Finds the `nMax` entities nearest to the point `center`. nFilter filters the entities by flags: see ENT_FILTER_*
template<typename Tv, typename Tr = float>
size_t UTIL_NearestEntities(const Tv& center, base_entity** pResults, size_t nMax, int nFilter = ENT_FILTER_ALL) {
	std::vector<std::pair<size_t, char[64]>> vecEnts;
	gpGlobals->pEntSys->get_entities(vecEnts);
	std::vector<std::pair<base_entity*, Tr>> vecDistances;

	for (auto& ent : vecEnts) {
		base_entity* pEnt = gpGlobals->pEntSys->get_entity(ent.first);
		if (!pEnt)
			continue;

		if (nFilter != ENT_FILTER_ALL) {
			if ((pEnt->get_filter() & nFilter) == 0) {
				continue;
			}
		}

		vec3 pos = pEnt->get_abspos();
		Tr nDeltaX = _abs<Tr>(center[0] - pos[0]);
		Tr nDeltaY = _abs<Tr>(center[1] - pos[1]);
		Tr nDeltaZ = _abs<Tr>(center[2] - pos[2]);
		Tr nLengthSquare = nDeltaX * nDeltaX + nDeltaY * nDeltaY + nDeltaZ * nDeltaZ;

		vecDistances.push_back({ pEnt, nLengthSquare });
	}

	std::sort(vecDistances.begin(), vecDistances.end(), [](const std::pair<base_entity*, Tr>& a, const std::pair<base_entity*, Tr>& b) {
		return a.second < b.second;
	});

#undef min
	size_t nCount = std::min(nMax, vecDistances.size());

	for (size_t iIndex = 0; iIndex < nCount; iIndex++) {
		pResults[iIndex] = vecDistances[iIndex].first;
	}

	return nCount;
}

// Search for an entity by classname using a regex
// Returns first match, or if no entities match, nullptr
// Best used to find entities that are never used more than once
// (e.g. game state, global light, etc.)
//
// Hint: "^light_.*" matches all classnames starting with "light_"
template<typename T>
base_entity* UTIL_FindEntityByClassname(const T& sClassnameRegex) {
	static_assert(std::is_same<T, std::string>::value || std::is_same<T, char*>::value || std::is_convertible<T, std::string>::value || std::is_convertible<T, char*>::value, "Regex type is not a string or not convertible to string");
	std::vector<std::pair<size_t, char[64]>> vecEnts;
	gpGlobals->pEntSys->get_entities(vecEnts);

	std::regex re(sClassnameRegex);

	for (auto&& kv : vecEnts) {
		if (std::regex_match(kv.second, re)) {
			return gpGlobals->pEntSys->get_entity(kv.first);
		}
	}
	return nullptr;
}

// Search for entities by classname using a regex
// Returns first match, or if no entities match, nullptr
//
// Hint: "^light_.*" matches all classnames starting with "light_"
template<typename T>
size_t UTIL_FindEntitiesByClassname(const T& sClassnameRegex, base_entity** pResults, size_t nMax) {
	static_assert(std::is_same<T, std::string>::value || std::is_same<T, char*>::value || std::is_convertible<T, std::string>::value || std::is_convertible<T, char*>::value, "Regex type is not a string or not convertible to string");
	std::vector<std::pair<size_t, char[64]>> vecEnts;
	size_t iIndex = 0;
	gpGlobals->pEntSys->get_entities(vecEnts);

	std::regex re(sClassnameRegex);

	for (auto&& kv : vecEnts) {
		if (std::regex_match(kv.second, re)) {
			pResults[iIndex] = gpGlobals->pEntSys->get_entity(kv.first);
			iIndex++;
			if (iIndex == nMax) {
				return iIndex;
			}
		}
	}
	return iIndex;
}

// Find the nearest entity from a base_entity* array of `nMax` size
template<typename Tv, typename Tr = float>
base_entity* UTIL_NearestEntity(const Tv& center, base_entity** pResults, size_t nMax) {

	if (!pResults || !nMax)
		return nullptr;

	if (nMax == 1)
		return *pResults;

	base_entity* pNearest = nullptr;
	float flDistance = -1;

	for (size_t i = 0; i < nMax; i++) {
		base_entity* pEnt = pResults[i];
		vec3 pos = pEnt->get_abspos();
		Tr nDeltaX = _abs<Tr>(center[0] - pos[0]);
		Tr nDeltaY = _abs<Tr>(center[1] - pos[1]);
		Tr nDeltaZ = _abs<Tr>(center[2] - pos[2]);
		Tr nLengthSquare = nDeltaX * nDeltaX + nDeltaY * nDeltaY + nDeltaZ * nDeltaZ;
		if (nLengthSquare < flDistance || flDistance < 0) {
			flDistance = nLengthSquare;
			pNearest = pEnt;
		}
	}

	return pNearest;
}
