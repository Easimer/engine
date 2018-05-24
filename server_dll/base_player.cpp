#include "stdafx.h"
#include "base_player.h"

void base_player::precache() {
	BaseClass::precache();
	model("data/models/csoldier.emf");
}

void base_player::spawn() {
	BaseClass::spawn();
}
