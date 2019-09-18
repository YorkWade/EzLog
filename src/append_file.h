#ifndef EZ_APPENDFILE_H
#define EZ_APPENDFILE_H

#include "NoCopyable.h"
#include "string_piece.h"


// not thread safe
class AppendFile : NoCopyable
{
public:
	explicit AppendFile(StringArg filename);

	~AppendFile();

	void append(const char* logline, size_t len);

	void flush();

	off_t writtenBytes() const { return writtenBytes_; }

private:

	size_t write(const char* logline, size_t len);

	FILE* fp_;
	char buffer_[64*1024];
	off_t writtenBytes_;
};

#endif