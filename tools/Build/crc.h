#ifndef __included_crc_h
#define __included_crc_h

class CRC
{
	private:
		unsigned int m_crc;

	public:
		void Process(const void *_data, size_t _size);
		void ProcessFile(const char *_fileName);
		int GetCRC() const
		{
			return m_crc;
		}
};

#endif
