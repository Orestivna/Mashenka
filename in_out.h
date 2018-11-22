#if ! defined(_IN_OUT_KUDUSHTEEV_H_)
#define _IN_OUT_KUDUSHTEEV_H_
#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif
#define MAX_COLS   160
#define MAX_ROWS   12
#define CELL_SIZE  40
#define CELL_PSIZE 80
#define CELL_MID   20
#define CELL_EDGE  4


//класс ввода из файла
class input_file {
private:
	HANDLE fp;
	DWORD  num;
	BOOL   res;
public:
	input_file(void);
	~input_file();

	input_file(const input_file&);
	input_file& operator = (const input_file&);
public:
	bool  open(LPCSTR filename, bool seq_scan = true);
	bool  read(BYTE& ch);
	bool  read(DWORD& val);
	DWORD read(char* s, DWORD n);
	void  close(void);
	DWORD seek(LONG dist, DWORD pos);

	bool isEof(void) const {
		return (num == 0) && (res == TRUE);
	}

	static DWORD read_dword(LPCSTR filename);
};


//класс вывода из файла
class output_file {
private:
	HANDLE fp;
public:
	output_file(void);
	~output_file();

	output_file(const output_file&);
	output_file& operator = (const output_file&);
public:
	bool open(LPCSTR filename);
	bool write(DWORD val);
	void flush(void);
	void close(void);
	static bool write_dword(LPCSTR filename, DWORD val);
};


//ресурсы
class input_res {
private:
	HGLOBAL data;
	LPBYTE  buf;
	DWORD   size;
public:
	input_res(void):data(NULL), buf(NULL), size(0) {}
	~input_res(){ destroy(); }

	input_res(const input_res&);
	input_res& operator = (const input_res&);
public:
	//загрузка
	bool load(HINSTANCE hinst, LPCSTR res_name, LPCSTR res_type){
		destroy();
		HRSRC res = FindResourceA((HMODULE)hinst, res_name, res_type);
		if(res == NULL)
			return false;

		data = LoadResource((HMODULE)hinst, res);
		if(data == NULL)
			return false;

		buf = reinterpret_cast<LPBYTE>(LockResource(data));
		if(buf == NULL){
			destroy();
			return false;
		}

		size = SizeofResource((HMODULE)hinst, res);
		return true;
	}

	LPBYTE getData(void) const {
		return buf;
	}

	DWORD getSize(void) const {
		return size;
	}

	//уничтожение
	void destroy(void){
		if(buf != NULL){
			UnlockResource(data);
			buf = NULL;
		}

		if(data != NULL)
			FreeResource(data);
		data = NULL;
	}
};

//читатель
class sreader {
private:
	LPBYTE pbuf;
	DWORD  pos;
	DWORD  size;
public:
	sreader(void):pbuf(NULL), pos(0), size(0){}
	~sreader(){destroy();}
public:
	void put(const input_res& rd){
		pos  = 0;
		size = rd.getSize();
		pbuf = rd.getData();
	}

	DWORD read(LPVOID buf, DWORD len){
		if((len = from_size(len)) > 0){
			memcpy(buf, &pbuf[pos], len);
			pos += len;
		}
		return len;
	}

	LPBYTE getCurrent(void) {
		return pbuf + pos;
	}

	bool isEof(void) const {
		return (pos >= size);
	}

	void ignore(DWORD n){
		pos += n;
	}

	void seek(DWORD off){
		pos = off;
	}

	void destroy(void){
		pbuf = NULL;
		size = pos = 0;
	}

private:

	DWORD from_size(DWORD len){
		if(pos >= size)
			return 0;
		else if(len > size)
			len = size;
		else if((pos + len) > size)
			len -= (pos + len) - size;
		return len;
	}
};


struct q_level {
	q_level* next;
	BYTE     mat[MAX_ROWS * MAX_COLS];
};

//уровни игры
class q_levels {
private:
	q_level* head;
	q_level* tail;
	DWORD    cnt;
public:
	q_levels(void);
	~q_levels();

	q_levels(const q_levels&);
	q_levels& operator = (const q_levels&);
public:
	bool load(HINSTANCE hinst, LPCSTR res_name, LPCSTR res_type);
	void clear(void);
	q_level* getAt(DWORD pos);

	DWORD getCount(void) const {
		return cnt;
	}

private:
	q_level* new_level(void);
	void     copy_m(BYTE* dst, const BYTE* src);
};


#endif
