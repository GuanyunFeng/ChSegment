#pragma once
#include<vector>
#include<unordered_map>
#include<stdio.h>
#include<assert.h>
using namespace std;

struct TrieNode {
	unordered_map <unsigned short, TrieNode* > childList;
	bool  wordTag;  //�Ƿ�����Ըý����Ϊ���ʽ�β
	int freq;       //��Ƶ
	char prop[5];     //����
	TrieNode() { childList.clear(); wordTag = false; freq = 0; }
};

typedef struct WordInfo {
	vector< unsigned short> vacab;      //����
	char prop[5];                 //����
	unsigned int freq = 0;        //��Ƶ
	WordInfo() { int freq = 0; vacab.clear(); };
}Info;                         //���ڱ�����ֵ�����ȡ����Ϣ

struct DAGInfo {
	int pos;
	int freq;       //��Ƶ
	char prop[5];     //����
};


class Trie
{
public:
	Trie();
	~Trie();
public:
	bool InsertVacab(Info word);
	bool DeletVacab(vector<unsigned short> vacab);
	void DeletNode(TrieNode* &node);
	WordInfo Search(vector<unsigned short> vacab);
	vector<vector<DAGInfo> > SearchDAG(vector<unsigned short> sentence);
	TrieNode * Getroot() { return this->root; }
	int GetNum() { return this->numb; }

	TrieNode * root = NULL;
	int numb = 0;
};