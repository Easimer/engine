#pragma once

#include "globals.h"
#include "renderer.h"

#define SET_MODEL(mdlname) m_iModelID = gpGlobals->pRenderer->load_model(mdlname)
