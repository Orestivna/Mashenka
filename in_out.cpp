#include "headers.h"


input_file::input_file(void):fp(INVALID_HANDLE_VALUE), num(0), res(FALSE){}

input_file::~input_file(){
	close();
}


//открыть
bool input_file::open(LPCSTR filename, bool seq_scan){
	close();
	DWORD attr = FILE_ATTRIBUTE_NORMAL;

	if(seq_scan)
		attr |= FILE_FLAG_SEQUENTIAL_SCAN;

	fp = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, attr, NULL);
	return (fp != INVALID_HANDLE_VALUE);
}


//читать
bool input_file::read(BYTE& ch){
	res = ReadFile(fp, &ch, sizeof(ch), &num, NULL);
	return (res == TRUE) && (num == sizeof(ch));
}

bool input_file::read(DWORD& val){
	res = ReadFile(fp, &val, sizeof(val), &num, NULL);
	return (res == TRUE) && (num == sizeof(val));
}

DWORD input_file::read(char* s, DWORD n){
	res = ReadFile(fp, s, n, &num, NULL);
	return num;
}


//позиция
DWORD input_file::seek(LONG dist, DWORD pos){
	return SetFilePointer(fp, dist, NULL, pos);
}


//закрыть
void input_file::close(void){
	if(fp != INVALID_HANDLE_VALUE)
		CloseHandle(fp);
	fp = INVALID_HANDLE_VALUE;

	num = 0;
	res = FALSE;
}


DWORD input_file::read_dword(LPCSTR filename){
	DWORD val = 0;
	input_file fp;
	if(!fp.open(filename))
		return 0;

	fp.read(val);
	fp.close();
	return val;
}


//----------------------------------------------------------------------------------------------------------


output_file::output_file(void):fp(INVALID_HANDLE_VALUE){}

output_file::~output_file(){
	close();
}

bool output_file::open(LPCSTR filename){
	close();
	fp = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	return (fp != INVALID_HANDLE_VALUE);
}
	
bool output_file::write(DWORD val){
	DWORD  n = 0;
	BOOL res = WriteFile(fp, &val, sizeof(val), &n, NULL);
	return (res && (n == sizeof(val)));
}
	
void output_file::flush(void){
	FlushFileBuffers(fp);
}
	
void output_file::close(void){
	if(fp != INVALID_HANDLE_VALUE)
		CloseHandle(fp);
	fp = INVALID_HANDLE_VALUE;
}


bool output_file::write_dword(LPCSTR filename, DWORD val){
	output_file fp;
	if(!fp.open(filename))
		return false;

	bool res = fp.write(val);
	fp.close();
	return res;
}

//----------------------------------------------------------------------------------------------------------


q_levels::q_levels(void):head(NULL), tail(NULL), cnt(0){}

q_levels::~q_levels(){
	clear();
}

//загрузить
bool q_levels::load(HINSTANCE hinst, LPCSTR res_name, LPCSTR res_type){
	input_res res;
	if(! res.load(hinst, res_name, res_type))
		return false;

	sreader rd;
	rd.put(res);

	BYTE mat[MAX_ROWS * MAX_COLS];

	while(!rd.isEof()){
		for(DWORD i = 0; i < MAX_ROWS; ++i){
			rd.read(&mat[i * MAX_COLS], MAX_COLS * sizeof(BYTE));
			rd.ignore(2);
		}

		if(!rd.isEof())
			rd.ignore(2);

		q_level* p = new_level();
		copy_m(p->mat, mat);

		if(head == NULL)
			head = tail = p;
		else {
			tail->next = p;
			tail = p;
		}
		++cnt;
	}
	res.destroy();
	return true;
}


//очистить
void q_levels::clear(void){
	q_level* p;
	while(head != NULL){
		p    = head;
		head = head->next;
		delete p;
	}
	tail = NULL;
	cnt  = 0;
}


//получить
q_level* q_levels::getAt(DWORD pos){
	q_level* p = head;
	for(DWORD i = 0; i < pos; ++i)
		p = p->next;
	return p;
}

//новый уровень
q_level* q_levels::new_level(void){
	q_level* p = new q_level();
	p->next    = NULL;
	return p;
}

//копирование
void  q_levels::copy_m(BYTE* dst, const BYTE* src){
	const BYTE* e = src + (MAX_ROWS * MAX_COLS);
	while(src != e)
		*dst++ = *src++;
}
