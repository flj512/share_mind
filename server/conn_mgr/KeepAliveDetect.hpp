#ifndef __BOOK_SHARE_MIND__KEEP_ALIVE_DETECT__
#define __BOOK_SHARE_MIND__KEEP_ALIVE_DETECT__

#define KEEP_ALIVE_DETECT_TEXT		"D"

class Pipe{
protected:
	int m_pipe[2];

public:
	Pipe(void);
	virtual ~Pipe(void);
	int getReadFD(void);
	int init(void);
};

class KeepAliveDetect:public Pipe{
private:
	int m_period;

public:
	KeepAliveDetect(int period);
	~KeepAliveDetect();
	void run(void);
};
#endif