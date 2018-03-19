#include "Trie.h"



Trie::Trie(){
	this->root = new TrieNode();
}


Trie::~Trie(){
	Trie::DeletNode(this->root);
	//delete this->root;
}

//��һ��������ӵ�����vacab�ǵ����Լ���Ƶ������Ϣ
bool Trie::InsertVacab(Info info) {
	//�ѵ�����ӵ�Trie��
	TrieNode *pTmp = this->root;
	for (unsigned int i = 0; i < info.vacab.size(); i++) {
		assert(pTmp);
		unordered_map<unsigned short, TrieNode*>::iterator ite = pTmp->childList.find(info.vacab[i]);
		if (ite == pTmp->childList.end()) {
			TrieNode *tmpNode = new TrieNode();
			if (!tmpNode) return false;
			pTmp->childList[info.vacab[i]] = tmpNode;
			pTmp = pTmp->childList[info.vacab[i]] = tmpNode;
			tmpNode = NULL;
		}
		else if (!pTmp->childList[info.vacab[i]]) {
			TrieNode *tmpNode = new TrieNode();
			if (!tmpNode) return false;
			pTmp->childList[info.vacab[i]] = tmpNode;
			pTmp = pTmp->childList[info.vacab[i]] = tmpNode;
			tmpNode = NULL;
		}
		else pTmp = pTmp->childList[info.vacab[i]];

	}
	//if (pTmp->wordTag) return false;
	pTmp->wordTag = true;
	pTmp->freq = info.freq;
	strcpy(pTmp->prop, info.prop);
	this->numb++;
	return true;
}


//��ĳһ�����ʴ�Tire����ɾ����vacab�ǵ���
bool Trie::DeletVacab(vector<unsigned short> vacab) {
	TrieNode *pTmp = this->root, *pTmp3 = pTmp;
	unsigned short tmpWord = vacab[0];
	for (unsigned int i = 0; i < vacab.size(); i++) {
		unordered_map<unsigned short, TrieNode*>::iterator it = pTmp->childList.find(vacab[i]);
		if (it == pTmp->childList.end()) return false;
		if (pTmp->wordTag) {
			pTmp3 = pTmp;
			tmpWord = vacab[i];
		}
		pTmp = pTmp->childList[vacab[i]];
	}
	//�������������ֻ��ȥ���ɴʱ��
	if (!pTmp->childList.empty()) {
		pTmp->wordTag = false;
		pTmp->freq = 0;
	}
	//����ɾ����һ�����ԳɴʵĽ������
	else 
		Trie::DeletNode(pTmp3->childList[tmpWord]);
	return true;
}


//�ѽ��node�Լ�����������ɾ��
void Trie::DeletNode(TrieNode* &node) {
	if (!node) return;
	/*//���Գɴ�
    if (node->wordTag) {
		node->wordTag = false;
		node->freq = 0;
	}*/
	//Ҷ�ڵ�
	if (node->childList.empty()) {
		delete node; 
		node = NULL;
	}
	//��Ҷ�ڵ�
	else {
		unordered_map<unsigned short, TrieNode*>::iterator it = node->childList.begin();
		for (; it != node->childList.end(); it++)
			this->DeletNode(it->second);
	}
}


//�ڼ��ص�Trie���в��ҵ���str
WordInfo Trie::Search(vector<unsigned short> vacab) {
	Info info;
	TrieNode* pTmp = this->root;
	if (!pTmp) return info; //info.freq == 0����Ϊ�Ƿ���ȷ�ҵ����ʵ��ж�����
	for (unsigned int i = 0; i < vacab.size(); i++) {
		unordered_map<unsigned short, TrieNode*>::iterator it = pTmp->childList.find(vacab[i]);
		//���Trie����û�иõ���
		if (it == pTmp->childList.end()) return info;
		pTmp = pTmp->childList[vacab[i]];
		if (!pTmp) return info;
	}
	//���True�����и�·������������Ϊһ������
	if (!pTmp->wordTag) return info;
	else {
		info.vacab = vacab;
		strcpy(info.prop, pTmp->prop);
		info.freq = pTmp->freq;
		return info;
	}
}

vector<vector<DAGInfo>> Trie::SearchDAG(vector<unsigned short> sentence)
{
	vector<vector<DAGInfo>> dag;
	vector<DAGInfo> tmp;
	DAGInfo tmpInfo;
	dag.clear(); tmp.clear();
	TrieNode* pTmp = this->root;
	assert(pTmp);
	for (unsigned int i = 0; i < sentence.size(); i++) {
		pTmp = this->root;
		tmp.clear();
		for (unsigned j = i; j < sentence.size(); j++) {
			unordered_map<unsigned short, TrieNode*>::iterator it = pTmp->childList.find(sentence[j]);
			if (it == pTmp->childList.end()) break;
			else if (it->second->wordTag) {
				tmpInfo.freq = it->second->freq;
				strcpy(tmpInfo.prop, it->second->prop);
				tmpInfo.pos = j;
				tmp.push_back(tmpInfo);
			}
			pTmp = pTmp->childList[sentence[j]];
		}
		if (tmp.empty()) {
			tmpInfo.pos = i;
			tmpInfo.freq = 1;
			strcpy(tmpInfo.prop, "*");
			tmp.push_back(tmpInfo);
		}
		dag.push_back(tmp);
	}
	return dag;
}

