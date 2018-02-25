#pragma once

#include <map>
#include <string>

enum estat_type {
	ESTAT_T_INT		= 0,
	ESTAT_T_UINT	= 1,
	ESTAT_T_FLOAT	= 2,
	ESTAT_T_DOUBLE	= 3,
	ESTAT_T_MAX		= 4
};

union estat_value {
	int nVal;
	unsigned nUVal;
	float flVal;
	double lflVal;
};

enum estat_category {
	ESTAT_C_ENTSYS		= 0,
	ESTAT_C_RENDERER	= 1,
	ESTAT_C_GAME		= 2,
	ESTAT_C_MISC		= 3,
	ESTAT_C_MAX			= 4
};

class estat_container {
public:
	estat_container()
	{
		for (size_t i = 0; i < ESTAT_T_MAX; i++)
		{
			categories.emplace((estat_category)i, std::map<std::string, std::pair<estat_type, estat_value>>());
		}
	}

	void update_stat(const estat_category category, const std::string& name, const estat_type type, const estat_value value)
	{
		categories[category][name] = { type, value };
	}

	int& get_stat_i(const estat_category category, const std::string& name)
	{
		if (!categories[category].count(name))
		{
			estat_value val;
			val.nVal = 0;
			categories[category][name] = { ESTAT_T_INT, val };
			return categories[category][name].second.nVal;
		}
		ASSERT(categories[category][name].first == ESTAT_T_INT);
		return categories[category][name].second.nVal;
	}

	unsigned& get_stat_u(const estat_category category, const std::string& name)
	{
		if (!categories[category].count(name))
		{
			estat_value val;
			val.nUVal = 0;
			categories[category][name] = { ESTAT_T_UINT, val };
			return categories[category][name].second.nUVal;
		}
		ASSERT(categories[category][name].first == ESTAT_T_UINT);
		return categories[category][name].second.nUVal;
	}

	float& get_stat_fl(const estat_category category, const std::string& name)
	{
		if (!categories[category].count(name))
		{
			estat_value val;
			val.flVal = 0;
			categories[category][name] = { ESTAT_T_FLOAT, val };
			return categories[category][name].second.flVal;
		}
		ASSERT(categories[category][name].first == ESTAT_T_FLOAT);
		return categories[category][name].second.flVal;
	}

	double& get_stat_lfl(const estat_category category, const std::string& name)
	{
		if (!categories[category].count(name))
		{
			estat_value val;
			val.lflVal = 0;
			categories[category][name] = { ESTAT_T_DOUBLE, val };
			return categories[category][name].second.lflVal;
		}
		ASSERT(categories[category][name].first == ESTAT_T_DOUBLE);
		return categories[category][name].second.lflVal;
	}

	std::map<std::string, std::pair<estat_type, estat_value>> get_category(const estat_category category)
	{
		return categories[category];
	}

	const char* get_category_name(const estat_category category)
	{
		switch (category)
		{
		case ESTAT_C_ENTSYS: return "Entity System";
		case ESTAT_C_RENDERER: return "Renderer";
		case ESTAT_C_GAME: return "Game";
		case ESTAT_C_MISC: return "Miscellaneous";
		default: return "Unknown";
		}
	}

	std::map<estat_category, std::map<std::string, std::pair<estat_type, estat_value>>> categories;
};
