/*----------------------------------------------------------------------------*/
//@package libnafta
//@{
//class for NAFTA POS logfiles support
//(C) TiT, Kiev, Ukraine 2002.
//
//@author Oleg Yurchenko
//@pkgdoc libnafta.logfile
//@version 7.0.0.0
//@}
/*----------------------------------------------------------------------------*/
#ifndef logfile_H
#define logfile_H
/*------------------------------------------------------------------------*/
class logfile
{
protected:
		char FileName[256];
		bool CreateFlag;
		unsigned long MaxSize;

public:

    logfile(const char *file_name, bool create = true, unsigned long max_size = 10 * 1024 * 1024);
    virtual ~logfile();

    void SetFileName(const char *file_name);
		void SetCreateFlag(bool create_flag);
    void SetMaxSize(unsigned long max_size);

		const char *GetFileName() {return FileName;}
		bool GetCreateFlag() {return CreateFlag;}
		unsigned long GetMaxSize() { return MaxSize; }


    bool Info(const char *fmt, ...);

    bool Message(const char *fmt, ...);

    bool Exception(const char *fmt, ...);

};
/*------------------------------------------------------------------------*/
#endif //logfile
