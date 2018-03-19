#include "Dict.h"

//Ĭ���ֵ�Ϊ��ǰĿ¼�µ�"dict.txt"
Dict::Dict(){
	char* curDirectory = (char*)malloc(80 * sizeof(char));
	curDirectory = _getcwd(curDirectory, 80 * sizeof(char));
	strcat(curDirectory, "\\dict");
	this->dictPath = curDirectory;
	free(curDirectory);
	//this->dictEncode = Decode::CheckEncode((char*)(this->dictPath).c_str());
}

Dict::Dict(char *path) {
	this->dictPath = path;
}


Dict::~Dict() {}


//�����ֵ�·��
bool Dict::SetDictPath(char *dictPath) {
	if (dictPath == NULL) return false; //���·���ַ���Ϊ�գ��򷵻�false
	else {
		this->dictPath = dictPath;
		return true;
	}
}


//���ı��ж���һ�����ʵ���Ϣ����װ��WordInfo����
Info Dict::ReadVacab(FILE* fp, Encode encoding) {
	Info info;
	if (encoding == ANSI || encoding == UTF_8 || encoding == UTF_8_BOM) {
		char str1[128], str2[33], str3[5];
		if (fscanf(fp, "%s%s%s", str1, str2, str3) == -1)
			return info;
		string tmpStr = str1;
		info.vacab = Dict::ToUnicode(tmpStr, encoding);
		for (int i = strlen(str2) - 1, j = 1; i >= 0; i--, j *= 10)
			info.freq += (str2[i] - 48) * j;
		strcpy(info.prop, str3);
	}
	else if (encoding == UCS2_BE ) {
		char tmp[20];
		int i = 0, j = 1;
		while (fread(tmp, 1, 2, fp) > 0) {
			if (tmp[0] == 0 && tmp[1] == 0x20) break;
			word high = tmp[0] & 0x00ff;;
			word low = tmp[1] & 0x00ff;;
			info.vacab.push_back((high << 8) | low);
		}
		while (fread(tmp, 1, 2, fp) > 0) {
			if (tmp[0] == 0 && tmp[1] == 0x20) break;
			info.freq += (tmp[1] - 48)*j;
			j *= 10;
		}
		while (fread(tmp, 1, 2, fp) > 0) {
			if (tmp[0] == 0 && tmp[1] == 0x0d) {
				fread(tmp, 1, 2, fp);
				break;
			}
			if (tmp[0] == 0 && tmp[1] == 0x0a) break;
			info.prop[i++] = tmp[1];
		}
		info.prop[i] = '\0';
	}
	else if (encoding == UCS2_LE) {
		char tmp[20];
		int i = 0, j = 1;
		while (fread(tmp, 1, 2, fp) > 0) {
			if (tmp[0] == 0 && tmp[1] == 0x20) break;
			word high = tmp[1] & 0x00ff;;
			word low = tmp[0] & 0x00ff;;
			info.vacab.push_back((high << 8) | low);
		}
		while (fread(tmp, 1, 2, fp) > 0) {
			if (tmp[1] == 0 && tmp[0] == 0x20) break;
			info.freq += (tmp[0] - 48)*j;
			j *= 10;
		}
		while (fread(tmp, 1, 2, fp) > 0) {
			if (tmp[1] == 0 && tmp[0] == 0x0d) {
				fread(tmp, 1, 2, fp);
				break;
			}
			if (tmp[1] == 0 && tmp[0] == 0x0a) break;
			info.prop[i++] = tmp[0];
		}
		info.prop[i] = '\0';
	}
	return info;
}
	

//�ֵ���ص�Trie����
bool Dict::LoadDict(Encode encoding) {
	Info info;
	FILE* fp = fopen(this->dictPath.c_str(), "rb");
	if (!fp) return false;

	//����ǰ����
	if (encoding == UTF_8_BOM)
		fseek(fp, 3L, SEEK_SET);
	else if ((encoding == UCS2_BE) || (encoding == UCS2_LE))
		fseek(fp, 2L, SEEK_SET);

	while ((info = this->ReadVacab(fp, encoding)).freq) {
		if (!this->tree.InsertVacab(info)) return false;
		this->total += info.freq;
	}
	fclose(fp);
	this->loaded = true;
	return true;
}


//��ӵ��ʵ�Trie���У�������ǡ��ڱ����ʱ����ֵ����д�롣
bool Dict::AddVacab(Info info){
	//Trie�����Ѿ�����
	if(!tree.InsertVacab(info)) return false;
	this->added.push_back(info);
	return true;
}


//��Trie����ɾ�����ʣ�������ǡ��ڱ����ʱ����ֵ�����޸ġ�
bool Dict::DelVacab(string str, Encode encoding) {
	vector<word> v1 = Dict::ToUnicode(str, encoding);
	if (!v1.size()) return false;
	if (!tree.DeletVacab(v1)) return false;
	this->deleted.push_back(str);
	return true;
}


//���棬���ֵ�����޸ġ�
bool Dict::SaveChange() {
	FILE* fp;
	char str[20];
	vector<int> vec;
	if ((fp = fopen(this->dictPath.c_str(), "a+")) == NULL)
		return false;

	if (this->added.size() > 0) {
		vector<Info>::iterator it = this->added.begin();
		while (it != this->added.end()) {
			strcpy(str, Dict::ToString(it->vacab, this->dictEncode).c_str());
			fputs(str, fp); fputc(' ', fp);
			for (int i = it->freq, j = 10; i; i = i / j, j *= 10)
				vec.push_back(i%j);
			for (int i = 0; i < vec.size(); i++)
				str[i] = (char)(vec[vec.size() - i - 1] + 48);
			str[vec.size()] = '\0';
			fputs(str, fp); fputc(' ', fp);
			strcpy(str, it->prop);
			fputs(str, fp); fputc('\r', fp); fputc('\n', fp);
		}
	}

	if (this->deleted.size() > 0) {
		fseek(fp, 0, SEEK_SET);
		//���鷳������ʵ����,��̤��û��ɾ�����Ӱ�ɾ������ɾ������ɾ
	}

	fclose(fp);
	return true;
}


//���ҵ��ʣ�����Info��Ϣ��δ�ҵ���Info.freq=0��
Info Dict::SearchDict(string str, Encode encoding){
	vector<word> vacab = Dict::ToUnicode(str, encoding);
	return this->tree.Search(vacab);
}


//�����ֵ���뷽ʽ
bool Dict::SetEncode(Encode encoding){
	if (encoding<0 || encoding>4)
		return false;
	this->dictEncode = encoding;
	return true;
}

//���ַ���ת��Unicode���룬�洢��vector�з���
vector<word> Dict::ToUnicode(string str, Encode encoding) {
	vector<word> tmpUnico;
	tmpUnico.clear();

	//�ַ���ΪUTF-8����
	if (encoding == UTF_8 || encoding == UTF_8_BOM) {
		for (unsigned i = 0; i < str.size(); ) {
			//��һ���ֽ�С��127(���ֽ���0��ͷ),ΪASCII�ַ�
			if (!(str[i] & 0x80)) tmpUnico.push_back(str[i++]);
			//��һ���ֽ���ʽΪ0x110*****,��ôUTF8����ռ2�ֽ�
			else if ((byte)str[i] < 0xDF && i + 1 < str.size()) {
				word tmpL = (str[i] & 0x1F); tmpL = tmpL << 6;
				word tmpR = str[i + 1] & 0x3F;
				tmpUnico.push_back(tmpL | tmpR);
				i += 2;
			}
			//��һ���ֽ���ʽΪ0x1110****,��ôUTF8����ռ3���ֽ�
			else if ((byte)str[i] < 0xEF && i + 2 < str.size()) {
				word tmpL = (str[i] & 0x0F); tmpL = tmpL << 12;
				word tmpM = (str[i + 1] & 0x3F); tmpM = tmpM << 6;
				word tmpR = str[i + 2] & 0x3F;
				tmpUnico.push_back(tmpL | tmpM | tmpR);
				i += 3;
			}
			//������Unicode������Ӧ�ֽ�(����UCS-4 ���룬���¹淶����Щ�ֽ�ֵ�޷������ںϷ� UTF-8������)
			else if ((byte)str[i] <= 0xF7)	i += 4;
			else if ((byte)str[i] <= 0xFB) i += 5;
			else if ((byte)str[i] <= 0xFD) i += 6;
			else return tmpUnico; //exit(-1);
		}
	}

	//�ַ���ΪUnicode_little_endian����
	if (encoding == UNICODE_LITTLE_ENDIAN) {
		for (unsigned i = 0; i < str.size(); i += 2) {
			word high = str[i + 1];
			high = (high & 0x00FF) << 8;
			word low = str[i];
			low = low & 0x00FF;
			tmpUnico.push_back(high | low);
		}
	}

	//�ַ���ΪUnicode_big_endian����
	if (encoding == UNICODE_BIG_ENDIAN) {
		for (unsigned i = 0; i < str.size(); i += 2) {
			word high = str[i];
			high = (high & 0x00FF) << 8;
			word low = str[i + 1];
			low = low & 0x00FF;
			tmpUnico.push_back(high | low);
		}
	}

	//ANSI���롣
	if (encoding == ANSI)
		tmpUnico = Decode::AnsiToVec(str.c_str());

	return tmpUnico;
}


//��vector�д洢�Ĵ�ת������Ӧ������ַ�������
string Dict::ToString(vector<word> vacab, Encode encoding){
	string str;
	char tmpStr[20];

	//UTF-8����
	if (encoding == UTF_8 || encoding == UTF_8_BOM) {
		unsigned j = 0;
		for (unsigned i = 0; i < vacab.size(); i++) {
			if (vacab[i] <= 0x0080) tmpStr[j++] = (char)(vacab[i] & 0x7F);
			else if (vacab[i] <= 0x0800) {
				tmpStr[j] = (char)((vacab[i] >> 6) | 0xC0);
				tmpStr[j + 1] = (char)((vacab[i] & 0x3F) | 0x80);
				j += 2;
			}
			else {
				tmpStr[j] = (char)((vacab[i] >> 12) | 0xE0);
				tmpStr[j + 1] = (char)(((vacab[i] >> 6) & 0x3F) | 0x80);
				tmpStr[j + 2] = (char)((vacab[i] & 0x3F) | 0x80);
				j += 3;
			}
		}
		tmpStr[j] = '\0';
		str = tmpStr;
	}

	//Unicode_little_endian����
	else if (encoding == UNICODE_LITTLE_ENDIAN) {
		for (int i = 0; i < vacab.size(); i++) {
			tmpStr[2 * i] = (char)(vacab[i] & 0x00FF);
			tmpStr[2 * i + 1] = (char)(vacab[i] >> 6);
		}
		tmpStr[2 * vacab.size()] = '\0';
		str = tmpStr;
	}

	//Unicode_big_endian����
	else if (encoding == UNICODE_BIG_ENDIAN) {
		for (int i = 0; i < vacab.size(); i++) {
			tmpStr[2 * i] = (char)(vacab[i] >> 6);
			tmpStr[2 * i + 1] = (char)(vacab[i] & 0x00FF);
		}
		tmpStr[2 * vacab.size()] = '\0';
		str = tmpStr;
	}

	//ANSI
	else if (encoding == ANSI)
		str = Decode::VecToAnsi(vacab);

	else;
	return str;
}