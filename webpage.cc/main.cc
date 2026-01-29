#include "../include/PageLibPreprocessor.h"

int main()
{
	PageLibProcessor pg;
	//pg.doProcess2();
	//pg.readInforFromFile();
	//pg.curRedundantPages(); //去重网页 新索引
	pg.buildInvertIndexTable(); //载入去重网页 构建倒排索引
	return 0;
}