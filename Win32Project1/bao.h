#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <windows.h>
#include <mutex>
#include <io.h>
#include <thread>
#include <algorithm>
#include "stdafx.h"
#include "XMLformat.h"
HWND hbt1, hbt2, hbt3, het1, hbt4, het2, hbt6;
using namespace std;
mutex out_mutex;
int threshold = 5 * 64;
void debugPrint(string info)
{
	char *buf = new char[0x1FFFF];
	string tmp;
	GetWindowText(het1, buf, 0x1FFFF);
	tmp = buf + info;
	memcpy(buf, tmp.c_str(), tmp.size());
	SetWindowText(het1, buf);
	delete[]buf;
}
class Context {
public:
	vector<string> *filenamelist;
	int start;
	int end;
	ofstream *out;
	Context(vector<string> *filenamelist, int start, int end, ofstream *out) :filenamelist(filenamelist), start(start), end(end), out(out) {}
};
class OneTap {
public:
	int type;
	double score;
	int len;
	int timepoint;
	int inCombine;
	int cannotDivide;
	OneTap(int type, int timepoint, int inCombine = 0, int len = 1, int cannotDivide = 0) :type(type), timepoint(timepoint), len(len), inCombine(inCombine), cannotDivide(cannotDivide) { calscore();  };
	void Print()
	{
		string info = to_string(type) + '\t' + to_string(timepoint) + "\r\n";
		debugPrint(info);
	}
	bool operator < (const OneTap &a)
	{
		return a.timepoint < this->timepoint;
	}
	void calscore() {

		if (type == LONG)
			score = 780 * len;
		else if (type == PinballLong)
			score = 780 * len;
		else if (type == PinballSlip && len == 2)
		{
			score = 2600 * 2;
			len = 1;
		}
		else if (type == BubbleLong)
			score = 780 * len;
		else if (type == BubbleSlip)
			score = 2600 * len;
		else
			score = 2600;
	};
};
class ComboWithScore {
public:
	double score;
	ComboWithScore(double score) : score(score){}
};
class SComboWithScore : public ComboWithScore 
{
public:
	int combo;
	SComboWithScore(int combo, double score) : ComboWithScore(score), combo(combo){}
};
class MComboWithScore : public ComboWithScore
{
public:
	int combo1;
	int combo2;
	MComboWithScore(int combo1, int combo2, double score) :ComboWithScore(score), combo1(combo1), combo2(combo2){}
};
bool MComboWithScoreScomp(const MComboWithScore &a, const MComboWithScore &b)
{
	return a.score >= b.score;
}
bool SComboWithScoreScomp(const SComboWithScore &a, const SComboWithScore &b)
{
	return a.score >= b.score;
}
class Solution {
public:
	double imax;
	double remax;
	int inCombine0;
	int inCombine1;
	double incase0;
	double incase1;
	double maxscore1;
	double maxscore2;
	int mark1, mark2;
	int mark3, mark4;
	OneTap *max3;
	OneTap *max4;
	int cunvalid;
	int cunmax;
	int cun1, cun2;
	int flag;
	OneTap *max1;
	OneTap *max2;
	Solution(double imax, double remax, int inCombine0=0, int inCombine1=0, double incase0=NULL, double incase1=NULL) : imax(imax), remax(remax), 
		inCombine0(inCombine0), inCombine1(inCombine1), incase0(incase0), incase1(incase1)
	{
		flag = inCombine0 + inCombine1;
	};
	void Print()
	{
		string sb;
		char buf[0x7FFF];
		GetWindowText(het1, buf, 0x7FFF);
		sb = to_string(mark1) + '\t' + to_string(mark2) + '\t'+ to_string(mark3) + '\t' + to_string(mark4) + "\r\n";
		strcat(buf, sb.c_str());
		SetWindowText(het1, buf);
	}
};
class twoNum {
public:
	int limit;
	int fire;
	twoNum(int limit, int fire) :limit(limit), fire(fire) {};
};

struct bubbleSingleTap{
	UINT startBar;
	UINT startPos;
	UINT track;
	UINT type;
	UINT endBar;
	UINT endPos;
};

struct bubbleMulTap {
	UINT startBar;
	UINT startPos;
	UINT track;
	UINT type;
	UINT endBar;
	UINT endPos;
	UINT ID;
};

typedef vector<OneTap> TapLink;
string getFromRefer(string line)
{
	int first = line.find('"');
	int last = line.find('"', first + 1);
	string sub = line.substr(first + 1, last - first - 1);
	return sub;
}



OneTap NoteParse(string line, int inCombine)
{
	int note_type = line.find("note_type");
	string type = line.substr(note_type + 11, 5);
	int Type;
	if (type.find("short") != -1)
		Type = SHORT;
	else if (type.find("long") != -1)
		Type = LONG;
	else if (type.find("slip") != -1)
		Type = SLIP;
	else
		exit(1);
	int Bar = line.find("Bar");
	int Pos = line.find("Pos");
	string bAr = line.substr(Bar, 13);
	string bar = getFromRefer(bAr);
	string pOs = line.substr(Pos, 15);
	string pos = getFromRefer(pOs);
	int TimePoint = atoi(bar.c_str()) * 64 + atoi(pos.c_str());
	if (Type == LONG)
	{
		int ENDBar = line.find("EndBar");
		int ENDPos = line.find("EndPos");
		string ENDbAr = line.substr(ENDBar, 13);
		string ENDbar = getFromRefer(ENDbAr);
		string ENDpOs = line.substr(ENDPos, 15);
		string ENDpos = getFromRefer(ENDpOs);
		int EndTimePoint = atoi(ENDbar.c_str()) * 64 + atoi(ENDpos.c_str());
		int len;
		if (!inCombine)
		{
			len = (EndTimePoint - TimePoint) / 4 + 1;
			if ((EndTimePoint - TimePoint) % 4 != 0)
				len++;
		}
		else
		{
			len = (EndTimePoint - TimePoint) / 4;
			if ((EndTimePoint - TimePoint) % 4 != 0)
				len++;
		}
		return OneTap(Type, TimePoint, inCombine, len);
	}
	else
	{
		return OneTap(Type, TimePoint, inCombine);
	}
}
void PrintDbg(string message)
{
	char buf[0x7FFF];
	message += "\r\n";
	GetWindowText(het1, buf, 0x7FFF);
	strcat(buf, message.c_str());
	SetWindowText(het1, buf);
}
OneTap PinParse(string line)
{
	int note_type = line.find("note_type");
	string type = line.substr(note_type + 11, 16);
	int Type;
	if (type.find("PinballSeries") != -1)
		Type = PinballSeries;
	else if (type.find("PinballSingle") != -1)
		Type = PinballSingle;
	else if (type.find("PinballSlip") != -1)
		Type = PinballSlip;
	else if (type.find("PinballLong") != -1)
		Type = PinballLong;
	else
		exit(1);
	int Bar = line.find("Bar");
	int Pos = line.find("Pos");
	string bAr = line.substr(Bar, 13);
	string bar = getFromRefer(bAr);
	string pOs = line.substr(Pos, 15);
	string pos = getFromRefer(pOs);
	int TimePoint = atoi(bar.c_str()) * 64 + atoi(pos.c_str());
	if (Type == PinballLong)
	{
		int ENDBar = line.find("EndBar");
		int ENDPos = line.find("EndPos");
		string ENDbAr = line.substr(ENDBar, 13);
		string ENDbar = getFromRefer(ENDbAr);
		string ENDpOs = line.substr(ENDPos, 15);
		string ENDpos = getFromRefer(ENDpOs);
		int EndTimePoint = atoi(ENDbar.c_str()) * 64 + atoi(ENDpos.c_str());
		int len = (EndTimePoint - TimePoint) / 4 + 1;
		if ((EndTimePoint - TimePoint) % 4 != 0)
			len++;
		return OneTap(Type, TimePoint, 0, len);
	}
	else
	{
		if (Type == PinballSlip)
		{
			int ison = line.find("Son=");
			if(line[ison + 5]!='\"')
				return OneTap(Type, TimePoint, 0);
			else
				return OneTap(Type, TimePoint, 0,2);
		}
		else
			return OneTap(Type, TimePoint, 0);
	}
}

OneTap BubbleParse(string line)
{
	int note_type = line.find("Type=");
	string type = line.substr(note_type + 6, 1);
	int Type;
	if (type.find("0") != -1)
		Type = BubbleSingle;
	else if (type.find("1") != -1)
		Type = BubbleLong;
	else if (type.find("2") != -1)
		Type = BubbleSlip;
	else
		exit(1);
	int Bar = line.find("Bar");
	int Pos = line.find("BeatPos");
	string bAr = line.substr(Bar, 13);
	string bar = getFromRefer(bAr);
	string pOs = line.substr(Pos, 15);
	string pos = getFromRefer(pOs);
	int TimePoint = atoi(bar.c_str()) * 64 + atoi(pos.c_str());
	if (Type == BubbleLong || Type == BubbleSlip)
	{
		int ENDBar = line.find("EndBar");
		int ENDPos = line.find("EndPos");
		string ENDbAr = line.substr(ENDBar, 13);
		string ENDbar = getFromRefer(ENDbAr);
		string ENDpOs = line.substr(ENDPos, 15);
		string ENDpos = getFromRefer(ENDpOs);
		int EndTimePoint = atoi(ENDbar.c_str()) * 64 + atoi(ENDpos.c_str());
		double len = (EndTimePoint - TimePoint) / 8.0 + 1;
		/*if ((EndTimePoint - TimePoint) % 4 != 0)
		len++;*/
		return OneTap(Type, TimePoint, 0, len);
	}
	else
	{
			return OneTap(Type, TimePoint, 0);
	}
}


bool comp(const OneTap &a, const OneTap &b)
{
	return a.timepoint < b.timepoint;
}
string UTF8_To_string(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}
int checkIncombo(int target, string buf)
{
	int combostart = buf.rfind("<CombineNote>", target);
	int comboend = buf.rfind("</CombineNote>", target);
	if (combostart == string::npos && comboend == string::npos)
		return 0;
	else if (comboend == string::npos)
		return 1;
	else {
		if (combostart > comboend)
			return 1;
		else
			return 0;
	}
}
int XMLParse(string filename, TapLink &Taplink, int type, ofstream &out)
{
	string title;
	fstream xml(filename);
	string buf;
	string line;
	if (xml)
	{
		while (getline(xml, line))
		{
			buf += line;
		}
	}
	else
	{
		MessageBox(NULL, "无法打开文件请确认目录！", "警告", MB_OK);
		exit(1);
	}
	buf  = UTF8_To_string(buf);
	int ttitle = buf.find("<Title>");
	int endttitle = buf.find("</Title>");
	int aartilst = buf.find("<Artist>");
	int endarttilst = buf.find("</Artist>");
	string artist = buf.substr(aartilst + 8, endarttilst - aartilst - 8);
	int inCombine = 0;
	if (ttitle != -1 && endttitle != -1)
		title = buf.substr(ttitle + 7, endttitle - ttitle - 7);
	else
		title = "error";
	int showindex = buf.find("<Section type=\"showtime\"");
	int showend = buf.find("/>", showindex);
	string showline = buf.substr(showindex, showend - showindex + 2);
	showindex = showline.find("startbar=");
	string startbar = showline.substr(showindex + 9, 6);
	int startbb = atoi(getFromRefer(startbar).c_str());
	int start = buf.find("<Normal>");
	int end = buf.find("</Normal>");
	buf = buf.substr(start + 9, end - start - 9);
	if (out)
		out << filename << ',' << title << "," << artist << ",";
	int i = buf.find("<Note ");
	int firstTime = 1;
	do{
		int last = 0;
		if(type != bubble)
			last = buf.find("/>", i);
		else
			last = buf.find("</Note>", i);
		string Tap = buf.substr(i, last - i + 2);
		if (type == idol)
		{
			inCombine = checkIncombo(i, buf);
			if (inCombine && firstTime)
			{
				Taplink.push_back(NoteParse(Tap, 0));
				firstTime = 0;
			}
			else if (inCombine && !firstTime)
			{
				Taplink.push_back(NoteParse(Tap, inCombine));
			}
			else
			{

				Taplink.push_back(NoteParse(Tap, 0));
			}
		}
		else if(type == pinball)
		{
			Taplink.push_back(PinParse(Tap));
		}
		else
		{
			Taplink.push_back(BubbleParse(Tap));
		}
		i = buf.find("<Note ", last);
		string checkFirst = buf.substr(last, i - last);
		if (checkFirst.find("</CombineNote>") != string::npos)
			firstTime = 1;
	}while (i >= 0);
	return startbb * 64;
}

int FindNextPinball(char *buffer, int size)
{
	for (int i = 0; i+6 < size; i++)
	{
		if (buffer[i] == 'P'
			&& buffer[i + 1] == 'i'
			&& buffer[i + 2] == 'n'
			&& buffer[i + 3] == 'b'
			&& buffer[i + 4] == 'a'
			&& buffer[i + 5] == 'l'
			&& buffer[i + 6] == 'l')
		{
			return i;
		}
	}
	return -1;
}

string GetNextStr(char *buffer, int size)
{
	string ret;
	for (int i = 0; i < size; i++)
	{
		if (buffer[i] != 0)
			ret += buffer[i];
		else
			return ret;
	}
}

int FindLineEnd(char *buffer, int size)
{
	for (int i = 0; i < size; i++)
		if (buffer[i] == 0x40)
			return i;
	return -1;
}

int getShowTime(char *buffer, int size)
{
	for (int i = 0; i+8 < size; i++)
	{
		if (buffer[i] == 's'
			&& buffer[i + 1] == 'h'
			&& buffer[i + 2] == 'o'
			&& buffer[i + 3] == 'w'
			&& buffer[i + 4] == 't'
			&& buffer[i + 5] == 'i'
			&& buffer[i + 6] == 'm'
			&& buffer[i + 7] == 'e'
			&& buffer[i + 8] == 0)
		{
			return (unsigned int)(i + 9);
		}
	}
}

int FindNextBubble(unsigned char *buffer, int size)
{
	for (int i = 0; i + 8 < size; i++)
	{
		if (   buffer[i] == 0xE6
			&& buffer[i + 1] == 0xB3
			&& buffer[i + 2] == 0xA2
			&& buffer[i + 3] == 0xE6
			&& buffer[i + 4] == 0xB5
			&& buffer[i + 5] == 0xAA
			&& buffer[i + 6] == 0xE5
			&& buffer[i + 7] == 0xBD
			&& buffer[i + 8] == 0xA2
			)
		{
			return i;
		}
	}
	return -1;
}

int intParser(char *buffer)
{
	return *((int *)buffer);
}

int isValidSingleBubble(char *buffer)
{
	bubbleSingleTap *p = (bubbleSingleTap *)buffer;
	if (p->type != 0)
		return 0;
	if (p->startPos > 64)
		return 0;
	if (p->endPos != 0)
		return 0;
	if (p->startBar > 200 || p->startBar < 4)
		return 0;
	if (p->endBar != 0)
		return 0;
	if (p->startPos % 2 == 1)
		return 0;
	return 1;
}

int isValidMulBubble(char *buffer)
{
	bubbleMulTap *p = (bubbleMulTap *)buffer;
	if (p->type != 1 && p->type != 2)
		return 0;
	if (p->startPos > 64 || p->endPos > 64)
		return 0;
	if (p->endBar < p->startBar)
		return 0;
	if (p->startBar > 200 || p->startBar < 4)
		return 0;
	if (p->endBar > 200 || p->endBar < 4)
		return 0;
	if (p->endBar - p->startBar > 10)
		return 0;
	if(p->endPos%2==1 || p->startPos%2==1)
		return 0;
	return 1;
}

int BubbleGetStart(char *buffer, int size)
{
	for (int i = size - 1; i >= 0; i--)
		if (buffer[i] == 64)
			return i;
	return -1;
}
class IndexWithTpye
{
public:
	int offset;
	int Type;
	IndexWithTpye(int offset, int Type) :offset(offset), Type(Type) {}
	int valid() {
		if (offset == -1 && Type == -1)
			return 0;
		return 1;
	}
};
IndexWithTpye getFirstMatchBubble(char *buffer, int size)
{
	for (int i = 0; i+24 < size; i++)
	{
		if (isValidSingleBubble(buffer + i))
			return IndexWithTpye(i, 0);
		if (isValidMulBubble(buffer + i))
			return IndexWithTpye(i, 1);
	}
	return IndexWithTpye(-1,-1);
}

tuple<int, string, string> BubbleBytesParser(string filename, TapLink &Taplink, vector<pair<int, int>> &buin)
{
	string title;
	//MessageBox(0, filename.c_str(), "debug", MB_OK);
	ifstream Bytesfile(filename, ios::in | ios::binary | ios::ate);
	char *buffer;
	int i = 0;
	streampos fsize;
	UINT size;
	if (Bytesfile)
	{
		fsize = Bytesfile.tellg();
		size = (UINT)fsize;
		buffer = new char[size];
		Bytesfile.seekg(0, ios::beg);
		Bytesfile.read(buffer, size);
		Bytesfile.close();
	}
	else
	{
		MessageBox(NULL, "CANNOT READ FILE!", "ERROR", MB_OK);
		exit(1);
	}
	int showtime = *(unsigned int *)(buffer+getShowTime(buffer, size));
	string Title = GetNextStr(buffer + 0x45, size - 0x45);
	i = 0x45 + Title.size() + 5;
	string Artist = GetNextStr(buffer + i, size - i);
	i += Artist.size();
	int FirstTapLast = FindNextBubble((unsigned char *)(buffer + i), size - i);
	int start = BubbleGetStart(buffer, i + FirstTapLast);

	int showtimeI = getShowTime(buffer, size);
	int showtimeII = getShowTime(buffer + showtimeI + 8, size);
	int indicator = showtimeI + 8 + showtimeII;
	unsigned int *q = (unsigned int *)(buffer + indicator + 1);
	while (*q != 64 || q[2] > 20)
	{
		q = (unsigned int *)(buffer + ++indicator);
	}
	indicator += 4;
	int *num = (int *)(buffer + indicator);
	indicator += 4;
	for (int j = 0; j < *num; j++)
	{
		int *qq = (int *)(buffer + indicator);
		int *qqq = (int *)(buffer + indicator + 4);
		buin.push_back(pair<int, int>{*qq, *qqq});
		indicator += 8;
	}

	if (FirstTapLast == -1 || start == -1)
	{
		MessageBox(0, "Error", "Error", MB_OK);
		exit(1);
	}	
	i = start;
	int ID = 0;
	while (i < size)
	{
		int Type;
		IndexWithTpye validPoint = getFirstMatchBubble(buffer + i, size - i);
		if (!validPoint.valid())
		{
			delete[]buffer;
			return tuple<int, string, string>{showtime * 64, Title, Artist};
		}
			
		i += validPoint.offset;
		int LineOffset = FindNextBubble((unsigned char *)(buffer + i), size - i);
		if (LineOffset == -1)
		{
			delete[]buffer;
			return tuple<int, string, string>{showtime * 64, Title, Artist};
		}
		if (validPoint.Type == 0)
		{
			Type = BubbleSingle;
			bubbleSingleTap *q = (bubbleSingleTap *)(buffer + i);
			//cout << ID << '\t' << q->startBar << '\t' << q->startPos << '\t' << q->type << '\t' << q->endBar << '\t' << q->endPos << endl;

			UINT TimePoint = q->startBar * 64 + q->startPos;
			Taplink.push_back(OneTap(Type, TimePoint, 0));
		}
		else
		{
			bubbleMulTap *q = (bubbleMulTap *)(buffer + i);
			//cout << ID << '\t' << q->startBar << '\t' << q->startPos << '\t' << q->type << '\t' << q->endBar << '\t' << q->endPos << endl;

			if (q->type == 1)
				Type = BubbleLong;
			if (q->type == 2)
				Type = BubbleSlip;
			UINT TimePoint = q->startBar * 64 + q->startPos;
			UINT EndTimePoint = q->endBar * 64 + q->endPos;
			double len = (EndTimePoint - TimePoint) / 8.0 + 1;
			Taplink.push_back(OneTap(Type, TimePoint, 0, len));
		}
		i += LineOffset;
		ID++;
	}
}
tuple<int, string, string> PinballBytesParser(string filename, TapLink &Taplink)
{
	string title;
	//MessageBox(0, filename.c_str(), "debug", MB_OK);
	ifstream Bytesfile(filename, ios::in | ios::binary | ios::ate);
	char *buffer;
	int i = 0;
	streampos fsize;
	UINT size;
	if (Bytesfile)
	{
		fsize = Bytesfile.tellg();
		size = (UINT)fsize;
		buffer = new char[size];
		Bytesfile.seekg(0, ios::beg);
		Bytesfile.read(buffer, size);
		Bytesfile.close();
	}
	else
	{
		MessageBox(NULL, "CANNOT READ FILE!", "ERROR", MB_OK);
		exit(1);
	}
	int showtime = *(unsigned int *)(buffer+getShowTime(buffer, size));
	string Title = GetNextStr(buffer + 0x46, size - 0x46);
	i = 0x46 + Title.size() + 5;
	string Artist = GetNextStr(buffer + i, size - i);
	i += Artist.size();
	while (i < size)
	{
		int Type;
		unsigned int *p;
		int LineOffset = FindNextPinball(buffer + i, size - i);
		if (LineOffset == -1)
		{
			delete[]buffer;
			return tuple<int, string, string>{showtime * 64, Title, Artist};
		}			
		i += LineOffset;
		string type = GetNextStr(buffer + i, size - i);
		if (type.find("PinballSeries") != -1)
			Type = PinballSeries;
		else if (type.find("PinballSingle") != -1)
			Type = PinballSingle;
		else if (type.find("PinballSlip") != -1)
			Type = PinballSlip;
		else if (type.find("PinballLong") != -1)
			Type = PinballLong;
		else
			exit(1);
		i += type.size();
		p = (unsigned int *)(buffer + 1 + i);
		UINT StartBar = *p;
		p = (unsigned int *)(buffer + 5 + i);
		UINT StartPos = *p;;
		int TimePoint = StartBar * 64 + StartPos;
		if (Type == PinballSingle || Type == PinballSeries)
		{
			Taplink.push_back(OneTap(Type, TimePoint));
		}
		else if (Type == PinballSlip)
		{
			p = (unsigned int *)(buffer + 5 + 0x10 + i);
			UINT son = *p;
			if (son == 2)
				Taplink.push_back(OneTap(Type, TimePoint, 0, 2));
			else
				Taplink.push_back(OneTap(Type, TimePoint));
		}
		else if(Type == PinballLong)
		{
			p = (unsigned int *)(buffer + i + 9);
			UINT EndBar = *p;
			p = (unsigned int *)(buffer + i + 13);
			UINT EndPos = *p;
			UINT EndTimePoint = EndBar * 64 + EndPos;
			int len = (EndTimePoint - TimePoint) / 4 + 1;
			int rrest = 0;
			if ((EndTimePoint - TimePoint) % 4 != 0)
			{
				rrest = (EndTimePoint - TimePoint) % 4;
				len++;
			}			
			Taplink.push_back(OneTap(Type, TimePoint, 0, len, rrest));
		}
		i += 17;
	}	
}

int FindidolSlip(char *buffer, int size)
{
	for (int i = 0; i < size+4; i++)
	{
		if (buffer[i] == 's'
			&& buffer[i + 1] == 'l'
			&& buffer[i + 2] == 'i'
			&& buffer[i + 3] == 'p'
			&& buffer[i + 4] == 0
			)
			return i;
	}
	return -1;
}

int FindidolLong(char *buffer, int size)
{
	for (int i = 0; i < size + 4; i++)
	{
		if (buffer[i] == 'l'
			&& buffer[i + 1] == 'o'
			&& buffer[i + 2] == 'n'
			&& buffer[i + 3] == 'g'
			&& buffer[i + 4] == 0
			)
			return i;
	}
	return -1;
}



int FindidolShort(char *buffer, int size)
{
	for (int i = 0; i < size + 5; i++)
	{
		if (buffer[i] == 's'
			&& buffer[i + 1] == 'h'
			&& buffer[i + 2] == 'o'
			&& buffer[i + 3] == 'r'
			&& buffer[i + 4] == 't'
			&& buffer[i+5] == 0
			)
			return i;
	}
	return -1;
}


int FindidolLeft(char *buffer, int size)
{
	for (int i = 0; i < size + 4; i++)
	{
		if (buffer[i] == 'L'
			&& buffer[i + 1] == 'e'
			&& buffer[i + 2] == 'f'
			&& buffer[i + 3] == 't'
			&&( buffer[i + 4] == '1'
			|| buffer[i + 4] == '2'
			))
			return i;
	}
	return -1;
}

int FindidolRight(char *buffer, int size)
{
	for (int i = 0; i < size + 5; i++)
	{
		if (buffer[i] == 'R'
			&& buffer[i + 1] == 'i'
			&& buffer[i + 2] == 'g'
			&& buffer[i + 3] == 'h'
			&& buffer[i + 4] == 't'
			&& (buffer[i + 5] == '1'
				|| buffer[i + 5] == '2'
				))
			return i;
	}
	return -1;
}

int FindidolMiddle(char *buffer, int size)
{
	for (int i = 0; i < size + 6; i++)
	{
		if (buffer[i] == 'M'
			&& buffer[i + 1] == 'i'
			&& buffer[i + 2] == 'd'
			&& buffer[i + 3] == 'd'
			&& buffer[i + 4] == 'l'
			&& buffer[i + 5] == 'e'
			&& buffer[i+6] == 0
				)
			return i;
	}
	return -1;
}
int FindNextIdol(char *buffer, int size)
{
	int left = FindidolLeft(buffer, size);
	int right = FindidolRight(buffer, size);
	int middle = FindidolMiddle(buffer, size);
	vector<int> ssort = { left, right, middle };
	std::sort(ssort.begin(), ssort.end());
	for (int i = 0; i < 3; i++)
		if (ssort[i] != -1 && ssort[i] < size)
			return ssort[i];
	return -1;
}
class idolWithType {
public:
	int offset;
	int type;
	idolWithType(int offset, int type) :offset(offset), type(type) {}
	int isValid()
	{
		if (offset == -1 && type == -1)
			return 0;
		return 1;
	}
};
idolWithType FindIdolType(char *buffer, int size)
{
	int Short = FindidolShort(buffer, size);
	int Long = FindidolLong(buffer, size);
	int Slip = FindidolSlip(buffer, size);
	int ssort[3] = { Short, Long, Slip };
	std::sort(ssort, ssort + 3);
	for (int i = 0; i < 3; i++)
		if (ssort[i] > 0)
		{
			if (ssort[i] == Short)
				return idolWithType(ssort[i], SHORT);
			if(ssort[i] == Long)
				return idolWithType(ssort[i], LONG);
			if (ssort[i] == Slip)
				return idolWithType(ssort[i], SLIP);
		}
	return idolWithType(-1,-1);
}
unsigned int Char2Uint(char *buffer)
{
	return *(unsigned int *)buffer;
}

tuple<int, string, string> IdolBytesParser(string filename, TapLink &Taplink)
{
	string title;
	//MessageBox(0, filename.c_str(), "debug", MB_OK);
	ifstream Bytesfile(filename, ios::in | ios::binary | ios::ate);
	char *buffer;
	int i = 0;
	streampos fsize;
	UINT size;
	if (Bytesfile)
	{
		fsize = Bytesfile.tellg();
		size = (UINT)fsize;
		buffer = new char[size];
		Bytesfile.seekg(0, ios::beg);
		Bytesfile.read(buffer, size);
		Bytesfile.close();
	}
	else
	{
		MessageBox(NULL, "CANNOT READ FILE!", "ERROR", MB_OK);
		exit(1);
	}
	unsigned int showtime = *(unsigned int *)(buffer + getShowTime(buffer, size));
	string Title = GetNextStr(buffer + 0x43, size - 0x43);
	i = 0x43 + Title.size() + 5;
	string Artist = GetNextStr(buffer + i, size - i);
	i += Artist.size();
	int combinecnt = 0;
	int ID = 0;
	while (i < size)
	{
		int Type;
		unsigned int *p;
		int LineOffset = FindNextIdol(buffer + i, size - i);
		if (LineOffset == -1)
		{
			delete[]buffer;
			return tuple<int, string, string>{showtime * 64, Title, Artist};
		}
		i += LineOffset;
		if (i >= size)
		{
			delete[]buffer;
			return tuple<int, string, string>{showtime * 64, Title, Artist};
		}
		idolWithType idolIndex = FindIdolType(buffer + i, size - i);
		if (!idolIndex.isValid())
		{
			string tmp = filename + "\r\n" + to_string(i) + '\t' + to_string(LineOffset) + '\t' + to_string(size);
			MessageBox(0, tmp.c_str(), "Error", MB_OK);
			exit(1);
		}
		Type = idolIndex.type;
		if (Type == SLIP)
		{
			int startBar = Char2Uint(buffer + i - 17);
			int startPos = Char2Uint(buffer + i - 13);
			int TimePoint = startBar * 64 + startPos;
			int inCombine = 0;
			if (combinecnt != 0)
			{
				inCombine = 1;
				combinecnt--;
			}			
			else
			{
				int combineflag1 = Char2Uint(buffer + i - 25);
				if (combineflag1 == 1)
				{
					int combineflag2 = Char2Uint(buffer + i - 21);
					if (combineflag2 > 0 && combineflag2 < 30)
					{
						combinecnt = combineflag2 - 1;
					}			
				}
			}
			Taplink.push_back(OneTap(Type, TimePoint, inCombine));
			//if (filename.find("100168.xml") != -1)
				//cout << ID << '\t' << startBar << '\t' << startPos << '\t' << inCombine << '\t' << "slip" << endl;
		}
		else if (Type == LONG)
		{
			int startBar = Char2Uint(buffer + i - 12);
			int startPos = Char2Uint(buffer + i - 8);
			int endBar = Char2Uint(buffer + i + idolIndex.offset + 5);
			int endPos = Char2Uint(buffer + i + idolIndex.offset + 9);
			int TimePoint = startBar * 64 + startPos;
			int EndTimePoint = endBar * 64 + endPos;
			int inCombine = 0;
			int len;
			if (combinecnt != 0)
			{
				inCombine = 1;
				combinecnt--;
			}
			else
			{
				int combineflag1 = Char2Uint(buffer + i -20);
				if (combineflag1 == 1)
				{
					int combineflag2 = Char2Uint(buffer + i - 16);
					if (combineflag2 > 0 && combineflag2 < 30)
					{
						combinecnt = combineflag2 - 1;
					}
				}
			}
			int rrest = 0;
			if (!inCombine)
			{
				len = (EndTimePoint - TimePoint) / 4 + 1;
				if ((EndTimePoint - TimePoint) % 4 != 0)
				{
					rrest = (EndTimePoint - TimePoint) % 4;
					len++;
				}
					
			}
			else
			{
				len = (EndTimePoint - TimePoint) / 4;
				if ((EndTimePoint - TimePoint) % 4 != 0)
				{
					rrest = (EndTimePoint - TimePoint) % 4;
					len++;
				}			
			}
			Taplink.push_back(OneTap(Type, TimePoint, inCombine, len, rrest));
			//if (filename.find("100168.xml") != -1)
				//cout << ID << '\t' << startBar << '\t' << startPos << '\t' << inCombine << '\t' << "long\t" << endBar << '\t' << endPos << endl;
		}
		else {
			int startBar = Char2Uint(buffer + i - 12);
			int startPos = Char2Uint(buffer + i - 8);
			int TimePoint = startBar * 64 + startPos;
			Taplink.push_back(OneTap(Type, TimePoint));
			//if (filename.find("100168.xml") != -1)
				//cout << ID << '\t' << startBar << '\t' << startPos << '\t' << '\t' << "short" << endl;
		}
		i += idolIndex.offset;
		ID++;
	}
	delete[]buffer;

}

void PrintInfo(TapLink Taplink)
{
	int sum = 0;
	for (int i = 0; i < Taplink.size(); i++)
	{
		sum += Taplink[i].len;
		cout << sum << endl;
	}
	system("pause");
}
int getCombo(const TapLink &Taplink, int mark)
{
	int combo = 0;
	for (int k = 0; k < mark; k++)
		combo += Taplink[k].len;
	return combo;
}
Solution maxBao(TapLink &Taplink, double BPM, vector<SComboWithScore> &SingleSort)
{
	double max = -1;
	int base;
	int mark = -1;
	int i;
	int mark2;
	int remark;
	double combo = 0;
	double remax = -1;
	double recombo = 0;
	for (i = 0; i < Taplink.size(); i++)
	{
		int sum = 0;
		int j;
		double tmp = 0;
		base = Taplink[i].timepoint;
		for (j = i; sum <= threshold && j<Taplink.size(); j++)
		{
				sum = Taplink[j].timepoint - base;
		}
		if (j == Taplink.size() && Taplink[j - 1].timepoint - base <= threshold)
			j++;
		j--;
		for (int k = i; k < j; k++)
			tmp += Taplink[k].score;
		SingleSort.push_back(SComboWithScore(getCombo(Taplink, i),tmp));
		if (tmp > remax)
		{
			if (tmp > max)
			{
				remax = max;
				remark = mark;
				mark = i;
				mark2 = j;
				max = tmp;
			}
			else
			{
				remax = tmp;
				remark = i;
			}
		}
	}
	//cout << Taplink[mark].timepoint / 64 << '\t' << Taplink[mark].timepoint % 64 << endl;
	int Nocombo=0, Norecombo=0;
	for (int k = 0; k < mark; k++)
		combo += Taplink[k].len;
	for (int k = 0; k < remark; k++)
		recombo += Taplink[k].len;
		Solution tmp(combo, recombo);
		tmp.max1 = &Taplink[mark];
		tmp.max2 = &Taplink[remark];
		tmp.maxscore1 = max;
		tmp.maxscore2 = remax;
		tmp.mark1 = mark;
		tmp.mark2 = mark2;
		return tmp;	
}
Solution checkCunBao(TapLink &Taplink, int threshold, int showtime, vector<MComboWithScore> &MultiSort)
{
	int first, last;
	double sum = 0;

	for (int i = 0; i < Taplink.size(); i++)
		if (Taplink[i].timepoint + threshold > showtime)
		{
			first = i;
			break;
		}
	for (int i = 0; i < Taplink.size(); i++)
		if (Taplink[i].timepoint > showtime + 64)
		{
			last = i;
			break;
		}
	for (int i = first; i < last; i++)
		sum += Taplink[i].score;
	int mark1 = -1, mark3 = -1;
	int mark2 = last;
	int mark4;
	double max = -1, max2 = -1, max1 = -1;
	for (int i = first; i < last; i++)
	{
		int rest = threshold - (showtime - Taplink[i].timepoint) + threshold;
		double submax = -1;
		int submark = -1;
		int subsubmark = -1;
		for (int j = last; j < Taplink.size(); j++)
		{
			int tmp = 0;
			int timelong = 0;
			int base = Taplink[j].timepoint;
			int k;
			for (k = j; k < Taplink.size() && timelong <= rest; k++)
				timelong = Taplink[k].timepoint - base;
			if (k == Taplink.size() && Taplink[k - 1].timepoint - base <= threshold)
				k++;
			k--;
			for (int l = j; l < k; l++)
				tmp += Taplink[l].score;
			if (tmp > submax)
			{
				submax = tmp;
				submark = j;
				subsubmark = k;
			}
		}
		MultiSort.push_back(MComboWithScore(getCombo(Taplink, i), getCombo(Taplink, submark), sum + submax));
		if (sum + submax > max)
		{
			max1 = sum;
			max = sum + submax;
			mark1 = i;
			mark3 = submark;
			mark4 = subsubmark;
			max2 = submax;
		}
		sum -= Taplink[i].score;
	}
	int combo1=0, combo2=0;
	for (int k = 0; k < mark1; k++)
		combo1 += Taplink[k].len;
	for (int k = 0; k < mark3; k++)
		combo2 += Taplink[k].len;
	Solution tmp(combo1, combo2);
	tmp.max1 = &Taplink[mark1];
	tmp.max2 = &Taplink[mark3];
	tmp.maxscore1 = max1;
	tmp.maxscore2 = max2;
	tmp.mark1 = mark1;
	tmp.mark2 = mark2;
	tmp.mark4 = mark4;
	tmp.mark3 = mark3;
	return tmp;
}

Solution ShuangBao(TapLink &Taplink, double BPM, vector<MComboWithScore> &MultiSort)
{
	double max = -1;
	double max1, max2;
	int base, bbase;
	int mark1 = -1;
	int mark2 = -1;
	int mark3, mark4;
	int i;
	double combo1 = 0, combo2 = 0;
	for (i = 0; i < Taplink.size(); i++)
	{
		double submax = -1;
		int submark = -1;
		int sum = 0;
		int j;
		double tmp = 0;
		int submark4 = -1;
		base = Taplink[i].timepoint;
		for (j = i; sum <= threshold && j<Taplink.size(); j++)
			sum = Taplink[j].timepoint - base;
		if (j == Taplink.size() && Taplink[j - 1].timepoint - base <= threshold)
			j++;
		j--;
		for (int k = i; k < j; k++)
			tmp += Taplink[k].score;
		for (int l = j; l < Taplink.size(); l++)
		{
			int ssum = 0;
			int o;
			double ttmp = 0;
			bbase = Taplink[l].timepoint;
			for(o=l; ssum <= threshold && o<Taplink.size(); o++)
				ssum = Taplink[o].timepoint - bbase;
			if (o == Taplink.size() && Taplink[o - 1].timepoint - bbase <= threshold)
				o++;
			o--;
			for (int k = l; k < o; k++)
				ttmp += Taplink[k].score;
			if (ttmp > submax)
			{
				submax = ttmp;
				submark = l;
			    submark4 = o;
			}
		}
		MultiSort.push_back(MComboWithScore(getCombo(Taplink, i), getCombo(Taplink, submark), submax + tmp));
		if (submax + tmp > max)
		{
			max = submax + tmp;
			max1 = tmp;
			max2 = submax;
			mark1 = i;
			mark2 = submark;
			mark3 = j;
			mark4 = submark4;
		}
	}
	for (int k = 0; k < mark1; k++)
		combo1 += Taplink[k].len;
	for (int k = 0; k < mark2; k++)
		combo2 += Taplink[k].len;
	Solution tmp(combo1, combo2);
	//checkCunBao(Taplink, 5 * 64, tmp);
	tmp.max1 = &Taplink[mark1];
	tmp.max2 = &Taplink[mark2];
	tmp.maxscore1 = max1;
	tmp.maxscore2 = max2;
	tmp.mark1 = mark1;
	tmp.mark2 = mark3;
	tmp.mark3 = mark2;
	tmp.mark4 = mark4;
	return tmp;
}

void getFiles(string path, vector<string>& files)
{
	//文件句柄  
	intptr_t   hFile = 0;
	//文件信息，声明一个存储文件信息的结构体  
	struct _finddata_t fileinfo;
	string p;//字符串，存放路径
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)//若查找成功，则进入
	{
		do
		{
			//如果是目录,迭代之（即文件夹内还有文件夹）  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				//文件名不等于"."&&文件名不等于".."
				//.表示当前目录
				//..表示当前目录的父目录
				//判断时，两者都要忽略，不然就无限递归跳不出去了！
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			//如果不是,加入列表  
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		//_findclose函数结束查找
		_findclose(hFile);
	}
}
void correctCombo(TapLink &Taplink)
{
	int combocnt = 0;
	for (int i = 0; i < Taplink.size(); i++)
	{
		combocnt += Taplink[i].len;
		double weight = 0;
		if (combocnt < 20)
			weight = 1.0;
		else if (combocnt < 50)
			weight = 1.1;
		else if (combocnt < 100)
			weight = 1.15;
		else
			weight = 1.2;
		Taplink[i].score = int(Taplink[i].score * weight);
	}
}
string PrintType(OneTap *p)
{
	string rst;
	if (p->type == SHORT)
		rst = "SHORT";
	if (p->type == LONG)
		rst = "LONG";
	if (p->type == SLIP)
		rst = "SLIP";
	if (p->type == PinballLong)
		rst = "PinballLong";
	if (p->type == PinballSeries)
		rst = "PinballSeries";
	if (p->type == PinballSingle)
		rst = "PinballSingle";
	if (p->type == PinballSlip)
		rst = "PinballSlip";
	if (p->type == BubbleLong)
		rst = "BubbleLong";
	if (p->type == BubbleSingle)
		rst = "BubbleSingle";
	if (p->type == BubbleSlip)
		rst = "BubbleSlip";
	return rst;
}
void outPutTap(OneTap *p, ofstream &out)
{
	out << p->timepoint / 64 << ',' << p->timepoint % 64 << ',';
	string type = PrintType(p);
	out << type << endl;
}
TapLink convertLong(TapLink &Taplink)
{
	TapLink newLink;
	int restLen;
	for (int i = 0; i < Taplink.size(); i++)
	{
		restLen = int(Taplink[i].len);
		switch (Taplink[i].type)
		{
		case LONG:
			if (Taplink[i].inCombine)
			{
				if (!Taplink[i].cannotDivide)
				{
					for (int j = 1; j <= int(Taplink[i].len); j++)
						newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * j, Taplink[i].inCombine));
				}
				else
				{
					for (int j = 1; j < int(Taplink[i].len); j++)
						newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * j, Taplink[i].inCombine));
					newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * (Taplink[i].len - 1) + Taplink[i].cannotDivide, Taplink[i].inCombine));
				}
			}
			else
			{
				if (!Taplink[i].cannotDivide) 
				{
					for (int j = 0; j < int(Taplink[i].len); j++)
						newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * j, Taplink[i].inCombine));
				}
				else
				{
					for (int j = 0; j < int(Taplink[i].len) - 1; j++)
						newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * j, Taplink[i].inCombine));
					newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * (Taplink[i].len - 2) + Taplink[i].cannotDivide, Taplink[i].inCombine));
				}
			}
			break;
		case PinballLong:
		{
			if (!Taplink[i].cannotDivide)
			{
				for (int j = 0; j < int(Taplink[i].len); j++)
					newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * j, Taplink[i].inCombine));
			}
			else
			{
				for (int j = 0; j < int(Taplink[i].len) - 1; j++)
					newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * j, Taplink[i].inCombine));
				newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * (Taplink[i].len - 2) + Taplink[i].cannotDivide, Taplink[i].inCombine));
			}
			break;
		}
		case BubbleLong:
		{
			for (int j = 0; j < int(Taplink[i].len); j++)
				newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 8 * j, Taplink[i].inCombine));
			double rest = Taplink[i].len - double(int(Taplink[i].len));
			if (rest > 0.09)
				newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * (restLen - 1), Taplink[i].inCombine, rest));
			break;
		}
		case BubbleSlip:
		{
			for (int j = 0; j < int(Taplink[i].len); j++)
				newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 8 * j, Taplink[i].inCombine));
			double rest = Taplink[i].len - double(int(Taplink[i].len));
			if (rest > 0.09)
				newLink.push_back(OneTap(Taplink[i].type, Taplink[i].timepoint + 4 * (restLen - 1), Taplink[i].inCombine, rest));
			break;
		}
		default:
			newLink.push_back(Taplink[i]);
			break;
		}
	}
	return newLink;
}
pair<int, __int64> compareSkill(TapLink &Taplink, int start, int end)
{
	__int64 limit=0, fire=0;
	for (int i = 0; i < Taplink.size(); i++)
	{
		if (i >= start && i < end)
		{
			limit += int(Taplink[i].score * 1.11 * 1.5);
			fire += int(Taplink[i].score * 2.7);
		}
		else
		{
			limit += int(Taplink[i].score * 1.11);
			fire += Taplink[i].score;
		}
	}
	return pair<int, __int64>((limit > fire),(limit> fire)? limit: fire);
}
pair<int,int> ShuangcompareSkill(TapLink &Taplink, int start1, int end1, int start2, int end2)
{
	double limit = 0, bao = 0;
	for (int i = 0; i < Taplink.size(); i++)
	{
		if (i >= start1 && i < end1)
		{
			limit += Taplink[i].score * 1.11 * 1.5;
			bao += Taplink[i].score * 2.7;
		}
		else if (i >= start2 && i < end2)
		{
			limit += Taplink[i].score * 1.11 * 1.5;
			bao += Taplink[i].score * 2.7;
		}
		else
		{
			limit += Taplink[i].score * 1.11;
			bao += Taplink[i].score;
		}
	}
	return pair<int,int>(limit > bao, (limit>bao)?limit:bao);
}
twoNum getShuang(TapLink &Taplink, int start1, int end1, int start2, int end2)
{
	double limit = 0, bao = 0;
	for (int i = 0; i < Taplink.size(); i++)
	{
		if (i >= start1 && i < end1)
		{
			limit += Taplink[i].score * 1.11 * 1.5;
			bao += Taplink[i].score * 2.7;
		}
		else if (i >= start2 && i < end2)
		{
			limit += Taplink[i].score * 1.11 * 1.5;
			bao += Taplink[i].score * 2.7;
		}
		else
		{
			limit += Taplink[i].score * 1.11;
			bao += Taplink[i].score;
		}
	}
	return twoNum(limit, bao);
}
void dbgLinkPrint(TapLink &link)
{
	for (int i = 0; i < link.size(); i++)
		link[i].Print();
}

int FindNote(char *buffer, int size)
{
	for (int i = 0; i + 3 < size; i++)
	{
		if (buffer[i] == 'n'
			&& buffer[i + 1] == 'o'
			&& buffer[i + 2] == 't'
			&& buffer[i + 3] == 'e')
			return i;
	}
	return -1;
}
int FindPinEnd(char *buffer, int size)
{
	for (int i = 0; i + 1 < size; i++)
	{
		if (buffer[i] == 64
			&& buffer[i + 1] == 64)
			return i;
	}
	return -1;
}
void IdolBytes2XML(string filename)
{
	XML xml;
	vector<OneNoteIdol> NoteIdol;
	ifstream Bytesfile(filename, ios::in | ios::binary | ios::ate);
	ofstream out(string(filename.begin(), filename.begin() + filename.size() - 6));
	char *buffer;
	int i = 0;
	streampos fsize;
	UINT size;
	if (Bytesfile)
	{
		fsize = Bytesfile.tellg();
		size = (UINT)fsize;
		buffer = new char[size];
		Bytesfile.seekg(0, ios::beg);
		Bytesfile.read(buffer, size);
		Bytesfile.close();
	}
	else
	{
		MessageBox(NULL, "CANNOT READ FILE!", "ERROR", MB_OK);
		exit(1);
	}
	int showtimeI = getShowTime(buffer, size);
	unsigned int *q;
	q = (unsigned int *)(buffer + showtimeI);
	int showtime1 = *q;
	int showtimeII = getShowTime(buffer + showtimeI + 8, size);
	q = (unsigned int *)(buffer + showtimeI + 8 + showtimeII);
	int showtime2 = *q;
	int note1 = FindNote(buffer, size);
	int note2 = FindNote(buffer + note1 + 5, size) + note1 + 5;
	string Title = GetNextStr(buffer + 0x43, size - 0x43);
	i = 0x43 + Title.size() + 5;
	string Artist = GetNextStr(buffer + i, size - i);
	i += Artist.size() + 5;
	string FilePath = GetNextStr(buffer + i, size - i);
	int combinecnt = 0;
	int ID = 0;
	int track = 0;
	int endpos;
	while (i < size)
	{
		int Type;
		unsigned int *p;
		int LineOffset = FindNextIdol(buffer + i, size - i);		
		if (LineOffset == -1)
		{
			endpos = i;
			break;
		}
		i += LineOffset;		
		if (i >= size)
		{
			break;
		}
		idolWithType idolIndex = FindIdolType(buffer + i, size - i);
		if (!idolIndex.isValid())
		{
			string tmp = filename + "\r\n" + to_string(i) + '\t' + to_string(LineOffset) + '\t' + to_string(size);
			MessageBox(0, tmp.c_str(), "Error", MB_OK);
			exit(1);
		}
		Type = idolIndex.type;
		string from = GetNextStr(buffer + i, size - i);
		int targetI = FindNextIdol(buffer + i + from.size() + 5, size - i);
		string target = GetNextStr(buffer + i + from.size() + 5 + targetI, size - i);
		if (!track)
		{
			if (from == "Middle" || target == "Middle")
				track = 1;
		}
		if (Type == SLIP)
		{
			int startBar = Char2Uint(buffer + i - 17);
			int startPos = Char2Uint(buffer + i - 13);
			int inCombine = 0;
			if (combinecnt != 0)
			{
				inCombine = 1;
				combinecnt--;
			}
			else
			{
				int combineflag1 = Char2Uint(buffer + i - 25);
				if (combineflag1 == 1)
				{
					int combineflag2 = Char2Uint(buffer + i - 21);
					if (combineflag2 > 0 && combineflag2 < 30)
					{
						combinecnt = combineflag2 - 1;
					}
				}
			}
			NoteIdol.push_back(OneNoteIdol("slip", startBar, startPos, from, target, inCombine));
			//Taplink.push_back(OneTap(Type, TimePoint, inCombine));
			//if (filename.find("100168.xml") != -1)
				//cout << ID << '\t' << startBar << '\t' << startPos << '\t' << inCombine << '\t' << "slip" << endl;
		}
		else if (Type == LONG)
		{
			int startBar = Char2Uint(buffer + i - 12);
			int startPos = Char2Uint(buffer + i - 8);
			int endBar = Char2Uint(buffer + i + idolIndex.offset + 5);
			int endPos = Char2Uint(buffer + i + idolIndex.offset + 9);
			int inCombine = 0;
			if (combinecnt != 0)
			{
				inCombine = 1;
				combinecnt--;
			}
			else
			{
				int combineflag1 = Char2Uint(buffer + i - 20);
				if (combineflag1 == 1)
				{
					int combineflag2 = Char2Uint(buffer + i - 16);
					if (combineflag2 > 0 && combineflag2 < 30)
					{
						combinecnt = combineflag2 - 1;
					}
				}
			}
			NoteIdol.push_back(OneNoteIdol("long", startBar, startPos, from, target, inCombine, endBar, endPos));
			//if (filename.find("100168.xml") != -1)
				//cout << ID << '\t' << startBar << '\t' << startPos << '\t' << inCombine << '\t' << "long\t" << endBar << '\t' << endPos << endl;
		}
		else {
			int startBar = Char2Uint(buffer + i - 12);
			int startPos = Char2Uint(buffer + i - 8);
			NoteIdol.push_back(OneNoteIdol("short", startBar, startPos, from, target));
			//if (filename.find("100168.xml") != -1)
				//cout << ID << '\t' << startBar << '\t' << startPos << '\t' << '\t' << "short" << endl;
		}
		i += idolIndex.offset;
		ID++;
	}
	string Nlast = GetNextStr(buffer + endpos, size - endpos);
	endpos += Nlast.size() + 13;
	q = (unsigned int *)(buffer + endpos);
	int AcLen = *q;
	endpos += 4;
	vector<Action> ActionList;
	for (int j = 0; j < AcLen; j++)
	{
		ActionList.push_back(Action(*(unsigned int *)(buffer + endpos), *(unsigned int *)(buffer + endpos + 4), *(unsigned int *)(buffer + endpos + 8), *(unsigned int *)(buffer + endpos + 12)));
		endpos += 31;
	}
	int Astart = NoteIdol.front().Bar;
	int trackcnt = 4;
	if (track)
		trackcnt = 5;
	vector<int> levelinfo = { 180, 4, 16, 1550, 1, 166666, showtime2 + 3, Astart - 1, track, trackcnt, 4000 };
	xml.Levelinfo(levelinfo);
	vector<string> in = { Title, Artist, FilePath };
	xml.MusicInfo(in);
	vector<int> sectioninfo = { 1, Astart - 1, Astart, buffer[note1+9], showtime1, showtime1 + 3, buffer[note2+5], buffer[note2+9], showtime2, showtime2 + 3 };
	xml.SectionSeq(sectioninfo);
	xml.IndicatorReset();
	xml.NoteInfoIdol(NoteIdol);
	xml.ActionSeq(ActionList);
	xml.Camera(Astart, showtime2 - 1);
	xml.DancerSort(Astart, showtime2 - 1);
	xml.StageEffectSeq(Astart, showtime2 - 1);
	out << xml.xml;
	out.close();
	delete[]buffer;

}
void PinballBytes2XML(string filename)
{
	XML xml;
	vector<OneNotePinball> NotePinball;
	ifstream Bytesfile(filename, ios::in | ios::binary | ios::ate);
	ofstream out(string(filename.begin(), filename.begin() + filename.size() - 6));
	char *buffer;
	int i = 0;
	streampos fsize;
	UINT size;
	if (Bytesfile)
	{
		fsize = Bytesfile.tellg();
		size = (UINT)fsize;
		buffer = new char[size];
		Bytesfile.seekg(0, ios::beg);
		Bytesfile.read(buffer, size);
		Bytesfile.close();
	}
	else
	{
		MessageBox(NULL, "CANNOT READ FILE!", "ERROR", MB_OK);
		exit(1);
	}
	int showtimeI = getShowTime(buffer, size);
	unsigned int *q;
	q = (unsigned int *)(buffer + showtimeI);
	int showtime1 = *q;
	int showtimeII = getShowTime(buffer + showtimeI + 8, size);
	q = (unsigned int *)(buffer + showtimeI + 8 + showtimeII);
	int showtime2 = *q;
	int note1 = FindNote(buffer, size);
	int note2 = FindNote(buffer + note1 + 5, size) + note1 + 5;
	string Title = GetNextStr(buffer + 0x46, size - 0x46);
	i = 0x46 + Title.size() + 5;
	string Artist = GetNextStr(buffer + i, size - i);
	i += Artist.size() + 5;
	string FilePath = GetNextStr(buffer + i, size - i);
	int combinecnt = 0;
	int ID = 0;
	int track = 0;
	int endpos;
	while (i < size)
	{
		int Type;
		unsigned int *p;
		int LineOffset = FindNextPinball(buffer + i, size - i);
		if (LineOffset == -1)
		{
			endpos = i;
			break;
		}
		i += LineOffset;
		string type = GetNextStr(buffer + i, size - i);
		if (type.find("PinballSeries") != -1)
			Type = PinballSeries;
		else if (type.find("PinballSingle") != -1)
			Type = PinballSingle;
		else if (type.find("PinballSlip") != -1)
			Type = PinballSlip;
		else if (type.find("PinballLong") != -1)
			Type = PinballLong;
		else
			exit(1);
		p = (unsigned int *)(buffer + i - 8);
		unsigned int ID = *p;
		i += type.size();
		p = (unsigned int *)(buffer + 1 + i);
		UINT StartBar = *p;
		p = (unsigned int *)(buffer + 5 + i);
		UINT StartPos = *p;;
		int endArea1 = 0; 
		int endArea2 = 0; 
		p = (unsigned int *)(buffer + 5 + 0x10 + i);
		UINT son = *p;
		if (son < 3)
		{
			p = (unsigned int *)(buffer + 9 + 0x10 + i);
			endArea1 = *p + 1;
			p = (unsigned int *)(buffer + 13 + 0x10 + i);
			endArea2 = *p + 1;;
			if (endArea2 > 4)
				endArea2 = 0;
		}
		else
		{
			p = (unsigned int *)(buffer + 13 + 0x10 + i);
			endArea1 = *p + 1;
			p = (unsigned int *)(buffer + 17 + 0x10 + i);
			endArea2 = *p + 1;;
			if (endArea2 > 4)
				endArea2 = 0;
		}
		if (Type!= PinballLong)
		{
			if (son < 3)
				NotePinball.push_back(OneNotePinball(ID, type, StartBar, StartPos, endArea1, endArea2, 3));
			else
				NotePinball.push_back(OneNotePinball(ID, type, StartBar, StartPos, endArea1, endArea2, 3, son));
		}
		else
		{
			p = (unsigned int *)(buffer + i + 9);
			UINT EndBar = *p;
			p = (unsigned int *)(buffer + i + 13);
			UINT EndPos = *p;
			NotePinball.push_back(OneNotePinball(ID, type, StartBar, StartPos, endArea1, endArea2, 3, 0, EndBar, EndPos));
		}
		i += 17;
	}
	string Nlast = GetNextStr(buffer + endpos, size - endpos);
	endpos += Nlast.size();
	endpos += FindPinEnd(buffer + endpos, size - endpos) + 6;
	q = (unsigned int *)(buffer + endpos);
	int AcLen = *q;
	endpos += 4;
	vector<Action> ActionList;
	for (int j = 0; j < AcLen; j++)
	{
		ActionList.push_back(Action(*(unsigned int *)(buffer+endpos), *(unsigned int *)(buffer + endpos + 4), *(unsigned int *)(buffer + endpos + 8), *(unsigned int *)(buffer + endpos + 12)));
		endpos += 31;
	}
	
	int Astart = NotePinball.front().Bar;
	vector<int> levelinfo = { 180, 4, 16, 1550, 1, 166666, showtime2 + 3, Astart - 1, 0, 5, 4000 };
	xml.Levelinfo(levelinfo);
	vector<string> in = { Title, Artist, FilePath };
	xml.MusicInfo(in);
	vector<int> sectioninfo = { 1, Astart - 1, Astart, buffer[note1 + 9], showtime1, showtime1 + 3, buffer[note2 + 5], buffer[note2 + 9], showtime2, showtime2 + 3 };
	xml.SectionSeq(sectioninfo);
	xml.IndicatorReset();
	xml.NoteInfoPinball(NotePinball);
	xml.ActionSeq(ActionList);
	xml.Camera(Astart, showtime2 - 1);
	xml.DancerSort(Astart, showtime2 - 1);
	xml.StageEffectSeq(Astart, showtime2 - 1);
	out << xml.xml;
	out.close();
	delete[]buffer;

}
void BubbleBytes2XML(string filename)
{
	XML xml;
	vector<OneNoteSingleBubble> NoteBubble;
	ifstream Bytesfile(filename, ios::in | ios::binary | ios::ate);
	ofstream out(string(filename.begin(), filename.begin() + filename.size() - 6));
	char *buffer;
	int i = 0;
	streampos fsize;
	UINT size;
	if (Bytesfile)
	{
		fsize = Bytesfile.tellg();
		size = (UINT)fsize;
		buffer = new char[size];
		Bytesfile.seekg(0, ios::beg);
		Bytesfile.read(buffer, size);
		Bytesfile.close();
	}
	else
	{
		MessageBox(NULL, "CANNOT READ FILE!", "ERROR", MB_OK);
		exit(1);
	}
	int showtimeI = getShowTime(buffer, size);
	unsigned int *q;
	q = (unsigned int *)(buffer + showtimeI);
	int showtime1 = *q;
	int showtimeII = getShowTime(buffer + showtimeI + 8, size);
	q = (unsigned int *)(buffer + showtimeI + 8 + showtimeII);
	int showtime2 = *q;
	int note1 = FindNote(buffer, size);
	int note2 = FindNote(buffer + note1 + 5, size) + note1 + 5;
	string Title = GetNextStr(buffer + 0x45, size - 0x45);
	i = 0x45 + Title.size() + 5;
	string Artist = GetNextStr(buffer + i, size - i);
	i += Artist.size() + 5;
	string FilePath = GetNextStr(buffer + i, size - i);
	int endpos;
	int lastType = 0;
	int FirstTapLast = FindNextBubble((unsigned char *)(buffer + i), size - i);
	int start = BubbleGetStart(buffer, i + FirstTapLast);
	if (FirstTapLast == -1 || start == -1)
	{
		MessageBox(0, "Error", "Error", MB_OK);
		exit(1);
	}
	i = start;
	int ID = 0;
	while (i < size)
	{
		int Type;
		IndexWithTpye validPoint = getFirstMatchBubble(buffer + i, size - i);
		if (!validPoint.valid())
		{
			endpos = i;
			break;
		}
		i += validPoint.offset;
		int LineOffset = FindNextBubble((unsigned char *)(buffer + i), size - i);
		if (LineOffset == -1)
		{
			endpos = i - validPoint.offset;
			break;
		}
		if (validPoint.Type == 0)
		{
			bubbleSingleTap *q = (bubbleSingleTap *)(buffer + i);
			int *p = (int *)(buffer + i + 0x28);
			int X = *p;
			p = (int *)(buffer + i + 0x2C);
			int Y = *p;
			string des = GetNextStr(buffer + i + LineOffset, size - i - LineOffset);
			float *r = (float *)(buffer + i + LineOffset + des.size() + 1);
			//cout << ID << '\t' << q->startBar << '\t' << q->startPos << '\t' << q->type << '\t' << q->endBar << '\t' << q->endPos << endl;
			NoteBubble.push_back(OneNoteSingleBubble(q->startBar, q->startPos, q->track, q->type, q->endBar, q->endPos, X, Y, des, *r));
			lastType = 0;
			//Taplink.push_back(OneTap(Type, TimePoint, 0));
		}
		else
		{
			bubbleMulTap *q = (bubbleMulTap *)(buffer + i);
			//cout << ID << '\t' << q->startBar << '\t' << q->startPos << '\t' << q->type << '\t' << q->endBar << '\t' << q->endPos << endl;
			string des2 = GetNextStr(buffer + i + 0x24, size - i - 0x24);
			int pos = i + 0x24 + des2.size() + 1;
			float *rr = (float *)(buffer + pos);
			string des = GetNextStr(buffer + pos + 12, size - pos - 12);
			pos += des.size() + 13;
			float *r = (float *)(buffer + pos);
			int *p = (int *)(buffer + pos + 8);
			int X = *p;
			p = (int *)(buffer + pos + 12);
			int Y = *p;		
			OneNoteSingleBubble note(q->startBar, q->startPos, q->track, q->type, q->endBar, q->endPos, X, Y, des, *r);
			note.ID = q->ID;
			note.des2 = des2;
			note.degree2 = *rr;
			//cout << ID << '\t' << q->startBar << '\t' << q->startPos << '\t' << q->type << '\t' << q->endBar << '\t' << q->endPos << endl;
			NoteBubble.push_back(note);
			lastType = 1;
		}
		i += LineOffset;
	}
	string Nlast = GetNextStr(buffer + endpos, size - endpos);
	if(lastType == 0)
		endpos += Nlast.size() + 5;
	else
	{
		endpos += Nlast.size() + 25;
	}
	endpos += 4;
	q = (unsigned int *)(buffer + endpos);
	int AcLen = *q;
	endpos += 4;
	vector<Action> ActionList;
	for (int j = 0; j < AcLen; j++)
	{
		ActionList.push_back(Action(*(unsigned int *)(buffer + endpos), *(unsigned int *)(buffer + endpos + 4), *(unsigned int *)(buffer + endpos + 8), *(unsigned int *)(buffer + endpos + 12)));
		endpos += 31;
	}
	int indicator = showtimeI + 8 + showtimeII;
	q = (unsigned int *)(buffer + indicator + 1);
	while (*q != 64 || q[2] > 20)
	{
		q = (unsigned int *)(buffer + ++indicator);
	}
	indicator += 4;
	int *num = (int *)(buffer + indicator);
	vector<pair<int, int>> buin;
	indicator += 4;
	for (int j = 0; j < *num; j++)
	{
		int *qq = (int *)(buffer + indicator);
		int *qqq = (int *)(buffer + indicator + 4);
		buin.push_back(pair<int, int>{*qq, *qqq});
		indicator += 8;
	}
	int Astart = NoteBubble.front().startBar;
	vector<int> levelinfo = { 180, 4, 16, 1550, 2, 166666, showtime2 + 3, Astart - 1, 0, 5, 4000 };
	xml.Levelinfo(levelinfo);
	vector<string> in = { Title, Artist, FilePath };
	xml.MusicInfo(in);
	vector<int> sectioninfo = { 1, Astart - 1, Astart, buffer[note1 + 9], showtime1, showtime1 + 3, buffer[note2 + 5], buffer[note2 + 9], showtime2, showtime2 + 3 };
	xml.SectionSeq(sectioninfo);
	xml.BubbleReset(buin);
	xml.NoteInfoBubble(NoteBubble);
	xml.ActionSeq(ActionList);
	xml.Camera(Astart, showtime2 - 1);
	xml.DancerSort(Astart, showtime2 - 1);
	xml.StageEffectSeq(Astart, showtime2 - 1);
	out << xml.xml;
	out.close();
	delete[]buffer;

}
int FindIIndex(vector<pair<int, int>> &buin, TapLink &SB, int bar, int pos)
{
	pair<int, int> fp{ bar,pos };
	int ttime = 0;
	for (int i=0; i<buin.size(); i++)
	{
		auto &pp = buin[i];
		if (pp == fp)
			return 1;
		if (pp > fp)
		{
			ttime = buin[i-1].first * 64 + buin[i-1].second;
			break;
		}
	}
	int target = bar * 64 + pos;
	int pre = 0;
	int cnt = 0;
	for (auto &tap : SB)
	{
		int time = tap.timepoint;
		if (time < ttime)
			continue;
		else
		{
			if (pre != time)
			{
				pre = time;
				cnt++;
			}
			if (time >= target)
				return cnt;
		}
	}
	return cnt;
}
void MultiThreadProcess(Context ctx)
{
	vector<string> &filenamelist = *ctx.filenamelist;
	int start = ctx.start;
	int end = ctx.end;
	ofstream &out = *ctx.out;
	int type = idol;
	if (filenamelist[0].find("idol") != -1)
		type = idol;
	if (filenamelist[0].find("pinball") != -1)
		type = pinball;
	if (filenamelist[0].find("bubble") != -1)
		type = bubble;
	vector<thread> Write;
	for (int i = start; i < end; i++)
	{
		if (filenamelist[i].find(".bytes") == -1)
			continue;
		TapLink SB, newSB;
		vector<SComboWithScore> SingleSort;
		vector<MComboWithScore> MultiSort;
		vector<OneNoteIdol> NoteIdol;
		vector<MComboWithScore> CunSort;
		int cunFlag = 0;
		int showtime;
		tuple<int, string, string> ret;
		vector<pair<int, int>> buin;
		if (type == idol)
		{
			ret = IdolBytesParser(filenamelist[i], SB);
			showtime = get<0>(ret);
			Write.push_back(thread(IdolBytes2XML, filenamelist[i]));
		}
		else if (type == pinball)
		{
			ret = PinballBytesParser(filenamelist[i], SB);
			showtime = get<0>(ret);
			Write.push_back(thread(PinballBytes2XML, filenamelist[i]));
		}			
		else if (type == bubble)
		{
			ret = BubbleBytesParser(filenamelist[i], SB, buin);
			showtime = get<0>(ret);
			Write.push_back(thread(BubbleBytes2XML, filenamelist[i]));
			//std::sort(SB.begin(), SB.end(), comp);
		}
			
		if (showtime == -1)
		{
			MessageBox(0, "Parser fail", "Error", MB_OK);
			exit(1);
		}
		newSB = convertLong(SB);
		std::sort(newSB.begin(), newSB.end(), comp);
		correctCombo(newSB);
		Solution a = maxBao(newSB, showtime, SingleSort);
		pair<int, __int64> skill = compareSkill(newSB, a.mark1, a.mark2);
		Solution b = ShuangBao(newSB, showtime, MultiSort);
		Solution c = checkCunBao(newSB, threshold, showtime, MultiSort);
		std::sort(MultiSort.begin(), MultiSort.end(), MComboWithScoreScomp);
		std::sort(SingleSort.begin(), SingleSort.end(), SComboWithScoreScomp);
		if (b.maxscore1 + b.maxscore2 < c.maxscore1 + c.maxscore2 && c.maxscore1 / 2600 > 5.0)
			cunFlag = 1;
		pair<int, int> shuangSkill = ShuangcompareSkill(newSB, b.mark1, b.mark2, b.mark3, b.mark4);
		int bi1 = 0, bi2 = 0, bi3 = 0;
		if (type == bubble)
		{
			bi1 = FindIIndex(buin, SB, newSB[a.mark1].timepoint / 64, newSB[a.mark1].timepoint % 64);
			if (!cunFlag)
			{
				bi2 = FindIIndex(buin, SB, newSB[b.mark1].timepoint / 64, newSB[b.mark1].timepoint % 64);
				bi3 = FindIIndex(buin, SB, newSB[b.mark3].timepoint / 64, newSB[b.mark3].timepoint % 64);
			}
			else
			{
				bi2 = FindIIndex(buin, SB, newSB[c.mark1].timepoint / 64, newSB[c.mark1].timepoint % 64);
				bi3 = FindIIndex(buin, SB, newSB[c.mark3].timepoint / 64, newSB[c.mark3].timepoint % 64);
			}
		}
		
		out_mutex.lock();
		out << get<1>(ret) << ',' << get<2>(ret) << ',';
		out << filenamelist[i] << ',';
		if(!cunFlag)
			out << a.imax << /*',' << a.remax*/ ',' << b.imax << ',' << b.remax << endl;
		else
			out << a.imax << /*',' << a.remax*/ ',' << c.imax << ',' << c.remax << endl;
		if(type == bubble)
			out << "bubble index:" << ',' << bi1 << ',' << bi2 << ',' << bi3 << endl;
		//out << "Debug" << ',' << newSB[a.mark1].timepoint / 64 << ':' << newSB[a.mark1].timepoint % 64 << ',' << newSB[a.mark2].timepoint / 64 << ':' << newSB[a.mark2].timepoint % 64 << endl;
		
		outPutTap(a.max1, out);
		if (!cunFlag)
		{
			outPutTap(b.max1, out);
			outPutTap(b.max2, out);
			out << a.maxscore1 / 2600 << ',' << b.maxscore1 / 2600 << ',' << b.maxscore2 / 2600 << endl;
			if (skill.first == 0)
				out << "solo fire" << ',';
			else
				out << "solo limit" << ',';
			if (shuangSkill.first == 0)
				out << "dual fire" << endl;
			else
				out << "dual limit" << endl;
		}
		else
		{
			outPutTap(c.max1, out);
			outPutTap(c.max2, out);
			out << a.maxscore1 / 2600 <<  ',' << c.maxscore1 / 2600 << ',' << c.maxscore2 / 2600 << endl;
			pair<int, int> sskill = ShuangcompareSkill(newSB, c.mark1, c.mark2, c.mark3, c.mark4);
			if (skill.first == 0)
				out << "solo fire" << ',';
			else
				out << "solo limit" << ',';
			if (sskill.first)
				out << "store limit" << endl;
			else
				out << "store fire" << endl;

		}
				
		out << "solo rank" << endl;
		for (int o = 0; o < 5; o++)
			out << SingleSort[o].combo << ',' << SingleSort[o].score / 2600 << endl;
		out << "dual rank" << endl;
		for (int o = 0; o < 5; o++)
			out << MultiSort[o].combo1 << ',' << MultiSort[o].combo2 << ',' << MultiSort[o].score / 2600 << endl;
		out << endl;
		out_mutex.unlock();
		SB.clear();
		MultiSort.clear();
		SingleSort.clear();
	}
	for (auto &th : Write)
		th.join();
}
void getAll(string name, ofstream &out)
{
	vector<string>filenamelist;
	getFiles(name, filenamelist);
	vector<thread> worker;
	//out << "文件名"<< ',' << "曲目" << ',' << "单排首选" << ',' << "单排次选" << ',' << "双排一爆" << ',' << "双排二爆" << endl;
	//out << "bar" << ',' << "pos" << ',' << "type" << endl;
	//out << "爆气指数:" << endl << endl;
	int part = filenamelist.size() / 16;
	int res = filenamelist.size() - part * 16;
	if(part)
		for (int i = 0; i < 16; i++)
		{
			Context ctx(addressof<vector<string>>(filenamelist), i * part, (i + 1)*part, &out);
			worker.push_back(thread(MultiThreadProcess, ctx));
		}
	if (res)
	{
		Context ctx(addressof<vector<string>>(filenamelist), filenamelist.size() - res, filenamelist.size(), &out);
		MultiThreadProcess(ctx);
	}
	for (auto &p : worker)
		p.join();
	out.close();
}
void AutoUpdate()
{
	ifstream idol_new("星动模式.csv");
	ifstream pinball_new("弹珠模式.csv");
	string buf_idol = "";
	string buf_pinball = "";
	ifstream idol_select("idol_7-10.txt");
	ifstream pinball_select("pinball_7-10.txt");
	ofstream idol_out("idol_new_7-10.txt");
	ofstream pinball_out("pinball_new_7-10.txt");
	string line;
	while (getline(idol_new, line))
	{
		buf_idol += line + '\n';
	}
	while (getline(pinball_new, line))
	{
		buf_pinball += line + '\n';
	}
	while (getline(idol_select, line))
	{
		if (line.find(',') == -1)
			continue;
		int length = line.find(',');
		string Name = line.substr(0, length);
		int index = buf_idol.find(Name);
		string oneNew = "";
		while (buf_idol[index] != '\n' && index < buf_idol.size())
		{
			oneNew += buf_idol[index++];
		}
		idol_out << oneNew << endl << endl;
	}
	while (getline(pinball_select, line))
	{
		if (line.find(',') == -1)
			continue;
		int length = line.find(',');
		string Name = line.substr(0, length);
		int index = buf_pinball.find(Name);
		string oneNew = "";
		while (buf_pinball[index] != '\n' && index < buf_pinball.size())
		{
			oneNew += buf_pinball[index++];
		}
		pinball_out << oneNew << endl << endl;
	}
	pinball_out.close();
	idol_out.close();
	idol_new.close();
	pinball_new.close();
	idol_select.close();
	pinball_select.close();
}
twoNum AnalysisOne(string filename, int type, ofstream &out)
{
	/*TapLink SB, newSB;
	int showtime = XMLParse(filename, SB, type, out);
	newSB = convertLong(SB);
	sort(newSB.begin(), newSB.end(), comp);
	correctCombo(newSB);
	Solution &b = ShuangBao(newSB, showtime);
	Solution &c = checkCunBao(newSB, threshold, showtime);
	int cunFlag = 0;
	if (b.maxscore1 + b.maxscore2 < c.maxscore1 + c.maxscore2)
		cunFlag = 1;
	//AutoUpdate();
	if(cunFlag == 0)
		return getShuang(newSB, b.mark1, b.mark2, b.mark3, b.mark4);
	else
		return getShuang(newSB, c.mark1, c.mark2, c.mark3, c.mark4);*/
	return twoNum(-1, -1);
}