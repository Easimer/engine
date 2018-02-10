#pragma once

#include <mutex>
#include <atomic>

/*
A thread-safe command buffer

T - command struct, should be as small as possible
nSize - size of the buffer
bClearOnEndRead - should the front buffer be cleared on EndRead
bSortBeforeCopy - should the back buffer be sorted before Copy; commands are sorted by the first 8 bytes of the commands
*/
template<class T, unsigned nSize, bool bClearOnEndRead = false, bool bSortBeforeCopy = false>
class cmdbuf
{
public:
	cmdbuf()
	{
		m_pBackBuffer = new T[nSize];
		m_pFrontBuffer = new T[nSize];
		m_bForceCopy = true;
		m_bClosed = false;
	}
	~cmdbuf()
	{
		delete m_pFrontBuffer;
		delete m_pBackBuffer;
	}

	void BeginWrite()
	{
		m_BackBufferLock.lock();
		//std::cerr << "CMDBUF-W: Back buffer locked" << std::endl;
		m_nBackBufferCount = 0;
	}
	void Write(T& cmd)
	{
		if (m_nBackBufferCount == nSize)
		{
			std::cerr << "CMDBUF-W: Back buffer is full!!!" << std::endl;
			return;
		}
		memcpy(m_pBackBuffer + m_nBackBufferCount, &cmd, sizeof(T));

		unsigned char* pCmd = (unsigned char*)&cmd;
		for (int i = 0; i < sizeof(T); i++)
		{
			m_nChecksum = (m_nChecksum >> 1) + ((m_nChecksum & 1) << 15);
			m_nChecksum += pCmd[i];
			m_nChecksum &= 0xFFFF;
		}

		m_nBackBufferCount++;
	}
	void EndWrite()
	{
		m_BackBufferLock.unlock();
		//std::cerr << "CMDBUF-W: Back buffer unlocked" << std::endl;
		Copy();
	}

	bool BeginRead(T** pCmdBuf, size_t* nCount)
	{
		bool bLocked = m_FrontBufferLock.try_lock();
		if (!bLocked)
			return false;
		//std::cerr << "CMDBUF-R: Front buffer locked" << std::endl;
		*pCmdBuf = m_pFrontBuffer;
		*nCount = m_nFrontBufferCount;
		return true;
	}
	void EndRead()
	{
		if (bClearOnEndRead)
			m_nFrontBufferCount = 0;
		m_FrontBufferLock.unlock();
		//std::cerr << "CMDBUF-R: Front buffer unlocked" << std::endl;
	}

	void Close()
	{
		m_bClosed = true;
	}

	bool IsClosed()
	{
		return m_bClosed;
	}

	bool IsEmpty()
	{
		return m_nFrontBufferCount == 0;
	}

private:
	void Sort()
	{
#define _BBSORTKEY(i) *((size_t*)&m_pBackBuffer[i])
		size_t i = 1;
		while (i < m_nBackBufferCount)
		{
			size_t j = i;
			while (j > 0 && _BBSORTKEY(j - 1) > _BBSORTKEY(j))
			{
				T t;
				memcpy(&t, m_pBackBuffer + j, sizeof(T));
				memcpy(m_pBackBuffer + j, m_pBackBuffer - 1, sizeof(T));
				memcpy(m_pBackBuffer + j, m_pBackBuffer + (j - 1), sizeof(T));
				memcpy(m_pBackBuffer + (j - 1), &t, sizeof(T));
				j--;
			}
			i++;
		}
	}

	void Copy()
	{
		m_BackBufferLock.lock();
		//std::cerr << "CMDBUF-C: Back buffer locked" << std::endl;
		m_FrontBufferLock.lock();
		//std::cerr << "CMDBUF-C: Front buffer locked" << std::endl;
		if (m_bForceCopy || (m_nChecksum != m_nLastChecksum))
		{
			if (bSortBeforeCopy)
			{
				if (m_nBackBufferCount != 0)
				{
					Sort();
				}
			}
			memcpy(m_pFrontBuffer, m_pBackBuffer, m_nBackBufferCount * sizeof(T));
			m_nFrontBufferCount = m_nBackBufferCount;
			m_bForceCopy = false;
		}
		else
		{
			//std::cerr << "CMDBUF-C: No copy needed" << std::endl;
		}
		m_nFrontBufferCount = m_nBackBufferCount;
		m_nLastChecksum = m_nChecksum;
		m_FrontBufferLock.unlock();
		//std::cerr << "CMDBUF-C: Front buffer unlocked" << std::endl;
		m_BackBufferLock.unlock();
		//std::cerr << "CMDBUF-C: Back buffer unlocked" << std::endl;
	}

private:
	T* m_pBackBuffer;
	size_t m_nBackBufferCount;
	T* m_pFrontBuffer;
	size_t m_nFrontBufferCount;
	std::mutex m_BackBufferLock; // prevents copying to or from the back buffer; locked by BeginWrite or BeginCopy; opened by EndWrite or EndCopy
	std::mutex m_FrontBufferLock; // prevents copying to or from the front buffer; locked by BeginRead or BeginCopy; opened by EndRead or EndCopy

	unsigned long m_nChecksum;
	unsigned long m_nLastChecksum;
	bool m_bForceCopy;

	std::atomic<bool> m_bClosed;
};

// Store debug caller info in commands

#ifdef PLAT_DEBUG

typedef struct cmdbuf_debug_t {
	char szFunction[128];
	long iLine;
} cmdbuf_debug_t;

#define CMDBUF_DEBUG_DECLARE {__FUNCTION__, __LINE__}

#define CMDBUF_DEBUG cmdbuf_debug_t callerinfo;

#else

#define CMDBUG_DEBUG_DECLARE
#define CMDBUF_DEBUG

#endif

#define CMDBUF_BEGIN_CMD(name) typedef struct name {
#define CMDBUF_END_CMD(name) \
		CMDBUF_DEBUG; \
	} name;

#define CMDBUF_DEF(name, T, nSize, bClearOnEndRead, bSortBeforeCopy) class name : public cmdbuf<T, nSize, bClearOnEndRead, bSortBeforeCopy> {};
