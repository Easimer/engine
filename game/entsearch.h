#pragma once

#include "baseentity.h"
#include "vector.h"
#include "globals.h"
#include <cmath>
#include <algorithm>

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

/// Finds the `nMax` entities within a sphere around the point `center` in an `nRadius`. nFilter filters the entities by flags: see ENT_FILTER_*
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

/// Finds the `nMax` entities nearest to the point `center`. nFilter filters the entities by flags: see ENT_FILTER_*
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
