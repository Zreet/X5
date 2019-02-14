#pragma once
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include "stdafx.h"
using namespace std;
class OneNoteIdol
{
public:
	string type;
	int Bar;
	int Pos;
	int endBar;
	int endPos;
	int inCombine;
	string from;
	string target;
	OneNoteIdol(string type, int Bar, int Pos, string &from, string &target, int inCombine = 0, int endBar = 0, int endPos = 0)
		: type(type), Bar(Bar), Pos(Pos), from(from), target(target), inCombine(inCombine), endBar(endBar), endPos(endPos){}
};
class OneNotePinball
{
public:
	int ID;
	string type;
	int Bar;
	int Pos;
	int endBar;
	int endPos;
	int son;
	int EndArea1;
	int EndArea2;
	int moveTime;
	OneNotePinball(int ID, string type, int Bar, int Pos, int area1, int area2, int movetime, int son = 0, int endBar = 0, int endPos = 0)
		: ID(ID), type(type), Bar(Bar), Pos(Pos), EndArea1(area1), EndArea2(area2), moveTime(movetime), son(son), endBar(endBar), endPos(endPos){}
};
class OneNoteSingleBubble
{
public:
	UINT startBar;
	UINT startPos;
	UINT track;
	UINT type;
	UINT endBar;
	UINT endPos;
	int X;
	int Y;
	string des;
	float degree;
	string des2;
	float degree2;
	int ID;
	OneNoteSingleBubble(UINT startBar, UINT startPos, UINT track, UINT type, UINT endBar, UINT endPos, int X, int Y, string des, float degree)
		: startBar(startBar), startPos(startPos), track(track), type(type), endBar(endBar), endPos(endPos), X(X), Y(Y), des(des), degree(degree){}

};
class Action 
{
public:
	int start_bar;
	int dance_len;
	int seq_len;
	int level;
	Action(int start_bar, int dance_len, int seq_len, int level) : start_bar(start_bar), dance_len(dance_len), seq_len(seq_len), level(level){}
};
class XML
{
public:
	string xml;
	double pps;
	XML()
	{
		xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Level>\n";
	}
	void Levelinfo(vector<int> &in)
	{
		xml += "  <LevelInfo>\n";
		AddItemL("BPM", in[0]);
		pps = (in[0]/60)*16;
		AddItemL("BeatPerBar", in[1]);
		AddItemL("BeatLen", in[2]);
		AddItemL("EnterTimeAdjust", in[3]);
		AddItemL("NotePreShow", in[4]);
		AddItemL("LevelTime", in[5]);
		AddItemL("BarAmount", in[6]);
		AddItemL("BeginBarLen", in[7]);
		if(in[8])
			AddItemM("IsFourTrack", "True");
		else
			AddItemM("IsFourTrack", "False");
		AddItemL("TrackCount", in[9]);
		AddItemL("LevelPreTime", in[10]);
		xml += "  </LevelInfo>\n";
	}
	void ActionSeq(vector<Action> &in)
	{
		xml += "  <ActionSeq type=\"1\">\n";
		for (auto &i : in)
		{
			xml += "    <ActionList start_bar=\"" + to_string(i.start_bar) + "\" dance_len=\"" + to_string(i.dance_len) + "\" seq_len=\"" + to_string(i.seq_len) + "\" level=\"" + to_string(i.level) + "\" type=\"\" />\n";
		}
		//xml += "    <ActionList start_bar=\"" + to_string(start) + "\" dance_len=\"2\" seq_len=\"" + to_string(end - start) + "\" level=\"2\" type=\"\" />\n";
		xml += "  </ActionSeq>\n";
	}
	void Camera(int start, int end)
	{
		xml += "  <CameraSeq>\n";
		xml += "    <Camera name=\"SC\" bar=\"" + to_string(start) + "\" pos=\"0\" end_bar=\"" + to_string(end) + "\" end_pos=\"0\" />\n";
		xml += "  </CameraSeq>";
	}
	void DancerSort(int start, int end)
	{
		xml += "  <DancerSort>\n";
		for (int i = start + 10; i < end - 2; i += 2)
		{
			xml += "    <Bar>" + to_string(i) + "</Bar>\n";
		}
		xml += "  </DancerSort>\n";
	}
	void StageEffectSeq(int start, int end)
	{
		xml += "  <StageEffectSeq>\n";
		xml += "    <effect name=\"wutai_scene_global_script\" bar=\"-1\" length=\"-1\" />\n";
		xml += "    <effect name=\"wutai_scene_note_biaozhun_01\" bar=\"" + to_string(start) + "\" length=\"" + to_string(end - start) + "\" />\n";
		xml += "    <effect name=\"wutai_scene_note_biaozhun_01\" bar=\"" + to_string(end) + "\" length=\"-1\" />\n";
		xml += "  </StageEffectSeq>\n";
		xml += "</Level>";
	}
	void NoteInfoBubble(vector<OneNoteSingleBubble> &in)
	{
		xml += "  <NoteInfo>\n";
		xml += "    <Normal PosNum=\"64\">\n";
		for (auto &p : in)
		{
			if (p.type == 0)
			{
				stringstream stream;
				stream << setprecision(7) << p.degree;
				xml += "      <Note Bar=\"" + to_string(p.startBar) + "\" BeatPos=\"" + to_string(p.startPos) + "\" Track=\"" + to_string(p.track) + "\" Type=\"" + to_string(p.type) + "\">\n";
				xml += "        <ScreenPos x=\"" + to_string(p.X) + "\" y=\"" + to_string(p.Y) + "\">\n";
				xml += "          <FlyTrack name=\"" + p.des + "\" degree=\"" + stream.str() +"\" />\n";
				xml += "        </ScreenPos>\n";
				xml += "      </Note>\n";
			}
			else
			{
				stringstream stream, s2;
				stream << setprecision(7) << p.degree;
				s2 << setprecision(7) << p.degree2;
				xml += "      <Note Bar=\"" + to_string(p.startBar) + "\" BeatPos=\"" + to_string(p.startPos) + "\" Track=\"" + to_string(p.track) + "\" Type=\"" + to_string(p.type) + "\" EndBar=\"" + to_string(p.endBar) + "\" EndPos=\"" + to_string(p.endPos) + "\" ID=\"" + to_string(p.ID) + "\">\n";
				xml += "        <MoveTrack name=\"" + p.des2 + "\" degree=\"" + s2.str() +"\" />\n";
				xml += "        <FlyTrack name=\"" + p.des + "\" degree=\"" + stream.str() + "\" />\n";
				xml += "        <ScreenPos x=\"" + to_string(p.X) + "\" y=\"" + to_string(p.Y) + "\" />\n";
				xml += "      </Note>\n";
			}
		}
		xml += "    </Normal>\n";
		xml += "  </NoteInfo>\n";
	}
	void NoteInfoPinball(vector<OneNotePinball> &in)
	{
		xml += "  <NoteInfo>\n";
		xml += "    <Normal>\n";
		for (auto &p : in)
		{
			if (p.type == "PinballLong")
			{
				xml += "      <Note ID=\"" + to_string(p.ID) + "\" note_type=\"" + p.type + "\" Bar=\"" + to_string(p.Bar) + "\" Pos=\"" + to_string(p.Pos) + "\" EndBar=\"" + to_string(p.endBar) + "\" EndPos=\"" + to_string(p.endPos) + "\" Son=\"\" EndArea=\"" + to_string(p.EndArea1);
				if (p.EndArea2)
					xml += '|' + to_string(p.EndArea2);
			}
			else
			{
				if (p.son)
				{
					xml += "      <Note ID=\"" + to_string(p.ID) + "\" note_type=\"" + p.type + "\" Bar=\"" + to_string(p.Bar) + "\" Pos=\"" + to_string(p.Pos) + "\" Son=\"" + to_string(p.son) + "\" EndArea=\"" + to_string(p.EndArea1);
					if (p.EndArea2)
						xml += '|' + to_string(p.EndArea2);
				}
				else
				{
					xml += "      <Note ID=\"" + to_string(p.ID) + "\" note_type=\"" + p.type + "\" Bar=\"" + to_string(p.Bar) + "\" Pos=\"" + to_string(p.Pos) + "\" Son=\"\" EndArea=\"" + to_string(p.EndArea1);
					if (p.EndArea2)
						xml += '|' + to_string(p.EndArea2);
				}
			}
			xml += "\" MoveTime=\"3\" ";
			//xml += "Second=\"" + to_string((p.Bar * 64 + p.Pos) / pps) + "\" ";
			xml += "/>\n";
		}
		xml += "    </Normal>\n";
		xml += "  </NoteInfo>\n";
	}
	void NoteInfoIdol(vector<OneNoteIdol> &in)
	{
		xml += "  <NoteInfo>\n";
		xml += "    <Normal>\n";
		int flag = 0;
		for (int i=0; i<in.size(); i++)
		{
			auto &note = in[i];
			if (flag == 0 && i+1<in.size() && in[i+1].inCombine)
			{
				flag = 1;
				xml += "      <CombineNote>\n";
			}
			if (flag)
				xml += "  ";
			if (note.type == "long")
			{
				xml += "      <Note Bar=\"" + to_string(note.Bar) + "\" Pos=\"" + to_string(note.Pos) + "\" from_track=\"" + note.from + "\" target_track=\"" + note.target + "\" note_type =\"" + note.type + "\" EndBar=\"" + to_string(note.endBar) + "\" EndPos=\"" + to_string(note.endPos) + "\" />\n";
			}
			else if (note.type == "slip")
			{
				xml += "      <Note Bar=\"" + to_string(note.Bar) + "\" Pos=\"" + to_string(note.Pos) + "\" target_track=\"" + note.target + "\" end_track=\"" + note.from + "\" note_type=\"slip\" />\n";
			}
			else
			{
				xml += "      <Note Bar=\"" + to_string(note.Bar) + "\" Pos=\"" + to_string(note.Pos) + "\" from_track=\"" + note.from + "\" target_track=\"" + note.target + "\" note_type =\"" + note.type + "\" />\n";
			}
			if (flag && i + 1 < in.size() && !in[i + 1].inCombine)
			{
				flag = 0;
				xml += "      </CombineNote>\n";
			}
		}
		if(flag)
			xml += "      </CombineNote>\n";
		xml += "    </Normal>\n";
		xml += "  </NoteInfo>\n";
	}
	void MusicInfo(vector<string> &in)
	{
		xml += "  <MusicInfo>\n";
		AddItemM("Title", in[0]);
		AddItemM("Artist", in[1]);
		AddItemM("FilePath", in[2]);
		xml += "  </MusicInfo>\n";
	}
	void SectionSeq(vector<int> &in)
	{
		xml += "  <SectionSeq>\n";
		xml += "    <Section type=\"previous\" endbar=\"0\" mark=\"\" param1=\"wutai_scene_pre_script_2\" />\n";
		xml += "    <Section type=\"begin\" startbar=\"" + to_string(in[0]) + "\" endbar=\"" + to_string(in[1]) + "\" mark=\"\" param1=\"wutai_scene_begin_script_2\" />\n";
		xml += "    <Section type=\"note\" startbar=\"" + to_string(in[2]) + "\" endbar=\"" + to_string(in[3]) + "\" mark=\"\" param1=\"\" />\n";
		xml += "    <Section type=\"showtime\" startbar=\"" + to_string(in[4]) + "\" endbar=\"" + to_string(in[5]) + "\" mark=\"dance\" param1=\"\" />\n";
		xml += "    <Section type=\"note\" startbar=\"" + to_string(in[6]) + "\" endbar=\"" + to_string(in[7]) + "\" mark=\"\" param1=\"\" />\n";
		xml += "    <Section type=\"showtime\" startbar=\"" + to_string(in[8]) + "\" endbar=\"" + to_string(in[9]) + "\" mark=\"pose_xuanku\" param1=\"\" />\n";
		xml += "  </SectionSeq>\n";
	}
	void IndicatorReset()
	{
		xml += "  <IndicatorResetPos PosNum=\"64\" />\n";
	}
	void BubbleReset(vector<pair<int, int>> &in)
	{
		xml += "  <IndicatorResetPos PosNum=\"64\">\n";
		for (auto &p : in)
		{
			xml += "    <Pos Bar=\"" + to_string(p.first) + "\" BeatPos=\"" + to_string(p.second) + "\" />\n";
		}
		xml += "  </IndicatorResetPos>\n";
	}
	void AddItemM(string info, string val)
	{
		xml += "    <" + info + '>' + val + "</" + info + ">\n";
	}
	void AddItemL(string info, int val)
	{
		xml += "    <" + info + '>' + to_string(val) + "</" + info + ">\n";
	}
};