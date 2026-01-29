#include "../include/PageLib.h"

PageLib::~PageLib()
{
}

PageLib::PageLib(Configuration &conf, DirScanner &dirScanner, FileProcessor &fileProcessor)
{
	_dirScanner    = dirScanner;
	_fileProcessor = fileProcessor;
}

void PageLib::create()
{
}

void PageLib::store()
{
}
