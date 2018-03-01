#include "stdafx.h"
#include "material.h"

material::material(const mdlc::qc_parser& qcp)
{
	if (!qcp.is_cmd("shader"))
	{
		PRINT_ERR("No shader defined in material ");
		ASSERT(0);
	}
	// TODO
}
