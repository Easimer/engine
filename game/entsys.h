#pragma once

#define ENTSYS_MAX_ENTITIES 1024

#define ESPAGE_SIZ 4096		// page size / max entity size
#define ESPAGE_COUNT ENTSYS_MAX_ENTITIES

typedef unsigned char entsys_page[ESPAGE_SIZ];

class entsys {
public:
	entsys();
	~entsys();

	void update_entities();
	void draw_entities();

private:
	void* request_page();
	void free_page(void* pPage);
	friend class ientity_factory;

private:
	entsys_page* m_pPages;
	entsys_page** m_pFreePages;
};