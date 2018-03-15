#include "stdafx.h"
#include "entsearch.h"

void test() {
	base_entity* testarray[128];
	UTIL_EntitiesInSphere(vec3_origin, 128, testarray, 128);
	UTIL_NearestEntities(vec3_origin, testarray, 128);
}
