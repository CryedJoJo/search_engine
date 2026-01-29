#include "../include/PageLibPreprocessor.h"
#include "../include/DictProducer.h"
#include <memory>

PageLibProcessor::PageLibProcessor(Configuration &conf, SplitTool *splitTool)
{
	//将conf指针中的路径和文件加载到本地 用于JieBa分词 去重
}

PageLibProcessor::~PageLibProcessor()
{
}

//执行预处理(没用)
void PageLibProcessor::doProcess()
{
	Mylogger *log = Mylogger::getInstance(); //日志单例 记录任务执行过程
	// string    docid = "\t<docid>";
	// cout << docid << " len:" << docid.length();
	// cout << "\n"
	//      << docid.find_first_of(">");

	ifstream _ifs("page2.txt");
	if(!_ifs.is_open())
	{
		log->error("error", __FILE__, __FUNCTION__, __LINE__);
	}

	string line;
	int    newLine = 1;
	string doc;

	int    id;
	string docid;
	string title;
	string link;
	string description;
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	string tmp;
	int    i = 12;
	//将原始网页库做分割 每篇文章分到到vector<WebPage> _pageList的一个元素中;
	int length = 0; //当前总长

	while(getline(_ifs, line))
	{
		//log->info("info", __FILE__, __FUNCTION__, __LINE__);
		doc += line + "\n";
		if(newLine <= 6)
		{
			switch(newLine)
			{
			case 1: //<doc>
				break;
			case 2: //<docid>
				docid = line.substr(8, line.length() - 16);
				id    = stoi(docid);
				break;
			case 3: //<title>
				title = line.substr(8, line.length() - 16);
				break;
			case 4: //<link>
				link = line.substr(7, line.length() - 14);
				break;
			case 5: //<description>
				description = line.substr(14, line.length() - 28);
				break;
			case 6: //</doc>
				_pageList.push_back(WebPage(doc, id, title, link, description));
				int old_length = length;
				length += doc.length();
				_offsetLib.insert({id, {old_length, length}});
				cout << id << " " << old_length << " " << length << "\n";
				doc.clear();
				newLine = 0;
				break;
			}
			newLine++;
		}
	}

	cout << _pageList.size();
	cout << '\n'
	     << _offsetLib.size();
	//log->info("info", __FILE__, __FUNCTION__, __LINE__);

	int      ii = 1;
	ofstream ofs("PageOffset.txt");
	for(; ii <= _pageList.size(); ++ii)
	{
		ofs << ii << " "
		    << _offsetLib[ii].first << " "
		    << _offsetLib[ii].second << '\n';
	}
	//将原始网页库做分割 每篇文章分到到vector<WebPage> _pageList的一个元素中;
}

//执行预处理(没用了)
void PageLibProcessor::doProcess2()
{
	Mylogger *log = Mylogger::getInstance(); //日志单例 记录任务执行过程
	// string    docid = "\t<docid>";
	// cout << docid << " len:" << docid.length();
	// cout << "\n"
	//      << docid.find_first_of(">");

	ifstream _ifs("page2.txt");
	if(!_ifs.is_open())
	{
		log->error("error", __FILE__, __FUNCTION__, __LINE__);
	}

	string line;
	int    newLine = 1;
	string doc;

	int    id;
	string docid;
	string title;
	string link;
	string description;
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	string tmp;
	int    i = 12;
	//将原始网页库做分割 每篇文章分到到vector<WebPage> _pageList的一个元素中;
	int length = 0; //当前总长

	while(getline(_ifs, line))
	{
		//log->info("info", __FILE__, __FUNCTION__, __LINE__);
		doc += line + "\n";
		if(newLine <= 6)
		{
			switch(newLine)
			{
			case 1: //<doc>
				break;
			case 2: //<docid>
				docid = line.substr(8, line.length() - 16);
				id    = stoi(docid);
				break;
			case 3: //<title>
				title = line.substr(8, line.length() - 16);
				break;
			case 4: //<link>
				link = line.substr(7, line.length() - 14);
				break;
			case 5: //<description>
				description = line.substr(14, line.length() - 28);
				break;
			case 6: //</doc>
				_pageList.push_back(WebPage(doc, id, title, link, description));
				int old_length = length;
				length += doc.length();
				_offsetLib.insert({id, {old_length, length}});
				cout << id << " " << old_length << " " << length << "\n";
				doc.clear();
				newLine = 0;
				break;
			}
			newLine++;
		}
	}

	// cout << _pageList.size();
	// cout << '\n'
	//      << _offsetLib.size();
	//log->info("info", __FILE__, __FUNCTION__, __LINE__);

	int      ii = 1;
	ofstream ofs("PageOffset.txt");
	for(; ii <= _pageList.size(); ++ii)
	{
		ofs << ii << " "
		    << _offsetLib[ii].first << " "
		    << _offsetLib[ii].second << '\n';
	}
}

//根据配置信息读取网页库和位置偏移库的内容 (用于创建倒排索引)
int PageLibProcessor::readInforFromFile(unordered_map<string, map<int, int>> &_wd_page_fqs, unordered_map<string, int> &_wd_dc_fqs)
{
	Mylogger *log = Mylogger::getInstance(); //日志单例 记录任务执行过程
	log->info("info", __FILE__, __FUNCTION__, __LINE__);

	cppjieba::Jieba jieba(DICT_PATH,
	                      HMM_PATH,
	                      USER_DICT_PATH,
	                      IDF_PATH,
	                      STOP_WORD_PATH);
	vector<string>  words;

	log->info("info", __FILE__, __FUNCTION__, __LINE__);

	//打开去重后的网页库
	ifstream _ifss("cleanedWebPage.txt");
	string   line;
	int      newLine = 1;

	string doc;
	int    id;
	string docid;
	string title;
	string link;
	string description;
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	//读取清理后的网页

	//_wordCutter = new chinese_tool("../yuliao/stop_words_zh.txt");
	int page_nums = 0;

	while(getline(_ifss, line))
	{
		doc += line;
		if(newLine <= 6)
		{
			switch(newLine)
			{
			case 1: //<doc>
				break;
			case 2: //<docid>
				//log->info("info 2", __FILE__, __FUNCTION__, __LINE__);
				docid = line.substr(8, line.length() - 16);
				id    = stoi(docid);
				break;
			case 3: //<title>
				//log->info("info 3", __FILE__, __FUNCTION__, __LINE__);
				title = line.substr(8, line.length() - 16);
				break;
			case 4: //<link>
				//log->info("info 4", __FILE__, __FUNCTION__, __LINE__);
				link = line.substr(7, line.length() - 14);
				break;
			case 5: //<description>
				//log->info("info 5", __FILE__, __FUNCTION__, __LINE__);
				//cout << line << "\n";
				description = line.substr(14, line.length() - 28);
				break;
			case 6: //</doc>
				log->info("info 6", __FILE__, __FUNCTION__, __LINE__);

				//也不用了
				//_pageList.push_back(WebPage(doc, id, title, link, description));

				//在这儿构建倒排索引的初始数据
				{
					//切出中文文章的分词
					jieba.Cut(title + description, words, true);
					//vector<string>   _jieba_words = _wordCutter->cut(title + description, 1);
					map<string, int> _cur_page;
					set<string>      one_word;

					//遍历此文章的全部分词 进行统计
					bool culculated = false;
					for(auto &elem : words)
					{
						_cur_page[elem]++;     //TF某个词在文章中出现的次数
						one_word.insert(elem); //过滤重复分词
					}
					words.clear();
					//将这篇文章的id 及其所有分词的词频加入统计
					for(auto &elem : _cur_page)
					{
						//unordered_map<string, map<int, int>> _wd_page_fqs
						//map<string, int> _cur_page
						(_wd_page_fqs[elem.first])[id] = elem.second;
					}

					for(auto &elem : one_word) //遍历过滤后的分词
					{
						_wd_dc_fqs[elem]++; //DF统计出现此分词的文章数
					}
					//log->info("info 6", __FILE__, __FUNCTION__, __LINE__);
				}

				doc.clear();
				newLine = 0;
				log->info("info", __FILE__, __FUNCTION__, __LINE__);
				break;
			}
			//page_nums;
			newLine++;
		}
	}

	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	_ifss.close();
	return id; //文章数量 N
}

//对冗余的网页进行去重
void PageLibProcessor::curRedundantPages()
{
	// //加载网页库到内存
	// Mylogger *log = Mylogger::getInstance(); //日志单例 记录任务执行过程

	// ifstream _ifs("page2.txt");
	// if(!_ifs.is_open())
	// {
	// 	log->error("error", __FILE__, __FUNCTION__, __LINE__);
	// }

	// string line;
	// int    newLine = 1;
	// string doc;

	// int    id;
	// string docid;
	// string title;
	// string link;
	// string description;

	// string    _cleaned_page;
	// Simhasher simhasher("../dict2/jieba.dict.utf8", "../dict2/hmm_model.utf8", "../dict2/idf.utf8", "../dict2/stop_words.utf8");
	// size_t    topN = 5;
	// uint64_t  u64  = 0;
	// string    s;

	// unordered_map<int, uint64_t> _identity;
	// // simhasher.make(s, topN, u64);
	// // Simhasher::isEqual(u64, u64);
	// log->info("info", __FILE__, __FUNCTION__, __LINE__);
	// string tmp;
	// int    i = 12;
	// //将原始网页库做分割 每篇文章分到到vector<WebPage> _pageList的一个元素中;
	// size_t old_length = 0; //当前总长
	// int    _real_id   = 1;
	// string line1, line2, line3, line4, line5, line6;
	// int    sum  = 0;
	// int    dddd = -5;
	// while(getline(_ifs, line))
	// {

	// 	if(newLine <= 6)
	// 	{
	// 		switch(newLine)
	// 		{
	// 		case 1: //<doc>
	// 			line1 = line + '\n';
	// 			break;
	// 		case 2: //<docid>
	// 			line2 = line + '\n';
	// 			docid = line.substr(8, line.length() - 16);
	// 			id    = stoi(docid);
	// 			break;
	// 		case 3: //<title>
	// 			line3 = line + '\n';
	// 			title = line.substr(8, line.length() - 16);
	// 			break;
	// 		case 4: //<link>
	// 			line4 = line + '\n';
	// 			link  = line.substr(7, line.length() - 14);
	// 			break;
	// 		case 5: //<description>
	// 			line5       = line + '\n';
	// 			description = line.substr(14, line.length() - 28);
	// 			if(id == 1)
	// 			{
	// 				simhasher.make(title + link + description, topN, u64); //第一次计算网页的指纹 加入指纹库
	// 				_identity.insert({id, u64});
	// 			}
	// 			break;
	// 		case 6:                                                    //</doc>
	// 			simhasher.make(title + link + description, topN, u64); //>1次计算指纹
	// 			//循环比较指纹
	// 			bool equal = false;
	// 			if(id != 1)
	// 				for(auto &utmp : _identity)
	// 				{
	// 					if(Simhasher::isEqual(utmp.second, u64, 10))
	// 					{
	// 						// cout << "id:" << utmp.first << "\n";
	// 						// cout << "u64:" << utmp.second << "\n";
	// 						// cout << id << "\n";
	// 						// cout << u64 << "\n";
	// 						// dddd++;
	// 						// while(dddd > 0)
	// 						// 	;
	// 						log->info("txt equl", __FILE__, __FUNCTION__, __LINE__);
	// 						equal = true;
	// 						break;
	// 					}

	// 					_identity.insert({_real_id, u64});
	// 				}

	// 			if(!equal) //如果没有重复指纹 就加入新网页
	// 			{
	// 				//_pageList.push_back(WebPage(doc, _real_id, title, link, description));
	// 				_cleaned_page += "<doc>\n";
	// 				_cleaned_page += "\t<docid>" + to_string(_real_id) + "</docid>\n";
	// 				_cleaned_page += line3;
	// 				_cleaned_page += line4;
	// 				_cleaned_page += line5 + "</doc>\n";
	// 				//cout << _cleaned_page.length() << "\n";
	// 				_offsetLib.insert({_real_id, {old_length, _cleaned_page.length()}});
	// 				old_length = _cleaned_page.length();
	// 				_real_id++;
	// 				sum++;
	// 			}
	// 			doc.clear();
	// 			newLine = 0;
	// 			break;
	// 		}
	// 		newLine++;
	// 	}
	// }
	// _ifs.close();

	// //cout << _cleaned_page;
	// cout << sum << '\n';

	// int      ii = 1;
	// ofstream ofs("cleanedPageOffset.txt");
	// for(; ii <= _offsetLib.size(); ++ii)
	// {
	// 	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	// 	ofs << ii << ' '
	// 	    << _offsetLib[ii].first << ' '
	// 	    << _offsetLib[ii].second << '\n';
	// }
	// ofs.close();

	// ofstream _ofs("cleanedWebPage.txt");
	// if(!_ofs.is_open())
	// {
	// 	log->error("error", __FILE__, __FUNCTION__, __LINE__);
	// }
	// _ofs << _cleaned_page;
	// _ofs.close();
}

static double culculateWight(int N, int TF, int DF)
{
	return TF * log2(N / (DF + 1));
}

//创建倒排索引表
void PageLibProcessor::buildInvertIndexTable()
{
	Mylogger *log = Mylogger::getInstance();
	//加载 去重后的网页 及 索引数据 到内存

	unique_ptr<unordered_map<string, map<int, int>>> _wd_page_fqs = //TF分词 文章 频率
	    make_unique<unordered_map<string, map<int, int>>>();

	unique_ptr<unordered_map<string, int>> _wd_dc_fqs = //DF分词 出现在多少篇文章中
	    make_unique<unordered_map<string, int>>();

	int N = readInforFromFile(*_wd_page_fqs, *_wd_dc_fqs);

	//cout << N << '\n';
	//计算IDF

	//倒排索引表对象 string 单词  pair:int 文章id, double 权值
	//unordered_map<string, vector<pair<int, double>>> _invertIndexTable;

	//遍历_wd_page_fqs 构造 _invertIndexTable
	ofstream _ofs("ReverseIndex.txt");

	unique_ptr<unordered_map<int, unordered_map<string, double>>> _page_wds_wt = //文章 分词 权重
	    make_unique<unordered_map<int, unordered_map<string, double>>>();

	string wd;
	int    id;
	int    TF;
	int    DF;
	double w;
	for(auto &elem : *_wd_page_fqs)
	{
		//string
		multimap<double, int> _index;
		wd = elem.first;
		cout << elem.first << ":\n"; //单词
		_ofs << elem.first << " ";
		DF = (*_wd_dc_fqs)[elem.first]; //对应string 在多少篇文章中出现过
		for(auto &elemm : elem.second)
		{
			id = elemm.first;
			TF = elemm.second;
			w  = culculateWight(N, TF, DF);
			cout << "\tdocid:" << id << "  wd_fqs:" << TF << "  w:" << w << "\n";
			_index.insert({w, id});
			(*_page_wds_wt)[id].insert({wd, w}); //记录分词wd在 docid == id的文章中的权重w
		}
		for(auto &info : _index)
		{
			_ofs << info.second << " " << info.first << " ";
		}
		_ofs << "\n";
		log->info("info", __FILE__, __FUNCTION__, __LINE__);
	}
	_ofs.close();
	//构建归一化的倒排索引
	//构建一个以文章为单位的权重列表

	//       文章         分词     权重
	//un_map<int ,un_map<string, double>> _page_wds_wt;

	//ofstream _offs("to_sim.txt");
	double sum_w2 = 0; //这儿需要将权值平方的总和初始化为0 不然它将会是一个非常大的数值 导致下面计算出错
	string s;
	for(int i = 1; i <= (*_page_wds_wt).size(); ++i)
	{
		s << i;
		s += " docid 归一化";
		log->info(s.c_str(), __FILE__, __FUNCTION__, __LINE__);
		//cout << "docid:" << i << '\t';
		//_offs << i << "\t";
		for(auto &elem : (*_page_wds_wt)[i])
		{
			//wd = elem.first;
			w = elem.second;
			sum_w2 += (w * w);
			//cout << wd << " " << w << " ";
			//_offs << wd << " " << w << " ";
		}
		for(auto &elem : (*_page_wds_wt)[i])
		{
			wd = elem.first;
			w  = elem.second;

			double normalNum = w / sqrt(sum_w2);
			//vector 中的pair可以用sort根据 normalNum排序
			//sort(vec.begin(), vec.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {return a.second > b.second;});
			_invertIndexTable[wd].push_back({i, normalNum});
			//cout << wd << " " << w << " ";
			//_offs << wd << " " << w << " ";
		}
		sum_w2 = 0;
		//cout << "\n";
		//_offs << '\n';
	}
	//_offs.close();

	//创建 归一化倒排索引文件
	ofstream _ooffss("NormalReverseIndex.txt");
	for(auto &elem : _invertIndexTable)
	{
		wd = elem.first;
		_ooffss << wd << " ";

		//将string 出现过的文章id 按 权值降序排列
		sort(elem.second.begin(), elem.second.end(), [](const std::pair<int, double> &a, const std::pair<int, double> &b) {
			return a.second > b.second;
		});

		for(auto &elemm : elem.second)
		{
			_ooffss << elemm.first << " " << elemm.second << " ";
		}
		_ooffss << "\n";
	}
	_ooffss.close();
}

//将经过预处理之后的网页库、位置偏移库和倒排索引表写回到磁盘上
void PageLibProcessor::storeOnDisk()
{
}