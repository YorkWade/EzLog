#ifndef EZ_NOCOPY_H
#define EZ_NOCOPY_H

class NoCopyable
{
protected:
	NoCopyable() {}
	~NoCopyable() {}
private:// emphasize the following members are private
	NoCopyable( const NoCopyable& );
	NoCopyable& operator=( const NoCopyable& );
};



#endif