#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <tchar.h>
#include "HttpBean.hpp"
#include "printer.h"
#include "TimeUtils.h"

#define BUF_SIZE 524288
#define SMALL_BUF_SIZE 512

using namespace std;

//TCHAR smName[]=TEXT("Global\\cprinter_shared_mem");
TCHAR smName[]=TEXT("cprinter_shared_mem");

HANDLE H_Mutex = NULL;
HANDLE H_Event = NULL;
HANDLE shared_file = NULL;
LPVOID lpBUF = NULL;
HANDLE hFile = NULL;

HANDLE s_H_Mutex = NULL;
HANDLE s_H_Event = NULL;
HANDLE s_shared_file = NULL;
LPVOID s_lpBUF = NULL;
HANDLE s_hFile = NULL;


BOOL running = TRUE;

unsigned char char_to_hex( unsigned char x )   
{   
	return (unsigned char)(x > 9 ? x + 55: x + 48);   
}
int is_alpha_number_char( unsigned char c )   
{   
	if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') )   
	  return 1;   
	return 0;   
}
void urlencode( unsigned char * src, int  src_len, unsigned char * dest, int  dest_len )   
{   
	unsigned char ch;   
	int  len = 0;   
	  
	while (len < (dest_len - 4) && *src)   
	{   
	  ch = (unsigned char)*src;   
	  if (*src == ' ')   
	  {   
	   *dest++ = '+';   
	  }   
	  else if (is_alpha_number_char(ch) || strchr("=!~*'()", ch))   
	  {   
	   *dest++ = *src;   
	  }   
	  else   
	  {   
	   *dest++ = '%';   
	   *dest++ = char_to_hex( (unsigned char)(ch >> 4) );   
	   *dest++ = char_to_hex( (unsigned char)(ch % 16) );   
	  }    
	  ++src;   
	  ++len;   
	}   
	*dest = 0;   
	return ;   
}

unsigned char* urldecode(unsigned char* encd,unsigned char* decd)   
{   
    int j,i;   
    char *cd =(char*) encd;   
    char p[2];   
    unsigned int num;   
    j=0;   
  
    for( i = 0; i < strlen(cd); i++ )   
    {   
        memset( p, 0, 2 );   
        if( cd[i] != '%' )   
        {   
            decd[j++] = cd[i];   
            continue;   
        }   
    
		p[0] = cd[++i];   
        p[1] = cd[++i];   
  
        p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0) - ((p[0] >= 'a') ? 32 : 0);   
        p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0) - ((p[1] >= 'a') ? 32 : 0);   
        decd[j++] = (unsigned char)(p[0] * 16 + p[1]);   
    
    }   
    decd[j] = 0;   
  
    return decd;   
}

bool init_open_s_share_memory()
{
    s_shared_file = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"s_cprinter_shared_mem");
    if (s_shared_file==NULL)
    {
        printf("init_open_s_share_memory failed.\n");
        return false;
    }
    s_lpBUF = MapViewOfFile(s_shared_file, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (s_lpBUF == NULL)
    {
        CloseHandle(s_shared_file);
        return false;
    }
}

bool init_open_share_memory()
{
    shared_file = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"cprinter_shared_mem");
    if (shared_file==NULL)
    {
        printf("init_open_share_memory failed.\n");
        return false;
    }
    lpBUF = MapViewOfFile(shared_file, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (lpBUF == NULL)
    {
        CloseHandle(shared_file);
        return false;
    }
    // H_Mutex = OpenMutexW(NULL, FALSE, L"sm_mutex");
    // if (H_Mutex == NULL)
    // {
    //     UnmapViewOfFile(lpBUF);
    //     CloseHandle(shared_file);
    //     printf("init_open_share_memory OpenMutexW failed.\n");
    //     lpBUF = NULL;
    //     return false;
    // }
    // H_Event = OpenvEventW(NULL, FALSE, FALSE, _T("sm_event"));
    // if (H_Event == NULL)
    // {
    //     CloseHandle(H_Mutex);
    //     UnmapViewOfFile(lpBUF);
    //     CloseHandle(shared_file);
    //     return false;
    // }
}

string read_share_memory(DWORD *cnt)
{
    // DWORD time_out = 10*1000;
   
    // WaitForSingleObject(H_Event, time_out);
    // char datas[BUF_SIZE+1];
    if (lpBUF != NULL){
        // WaitForSingleObject(H_Mutex, time_out);
        printf("read_share_memory will read file.\n");
        // info = (char*)lpBUF;
        string _s((char*)lpBUF);
        *cnt = _s.size();
        // info = const_cast<char *>(_s.c_str());
        // memcpy(datas, lpBUF, BUF_SIZE);
        // const wstring datas((wchar_t *)lpBUF);
        // ReleaseMutex(H_Mutex);
        return _s;
    } else {
        printf("read_share_memory lpBUF is NULL.\n");
    }
    *cnt=0;
    return "";
}
string read_small_share_memory(DWORD* cnt)
{
    if(s_lpBUF!=NULL)
    {
        string info((char*)s_lpBUF);
        *cnt = info.size();
        return info;
    } else {
        printf("read_small_share_memory s_lpBUF is NULL.\n");
    }
    *cnt = 0;
    return "";
}
bool init_s_share_memory()
{
    // s_hFile = CreateFileW(L"s_cprinter_shared_mem", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 
    // NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // if (s_hFile == INVALID_HANDLE_VALUE){
    //     printf("init_s_share_memory create file failed.\n");
    //     return false;
    // }
    // INVALID_HANDLE_VALUE or hFile
    s_shared_file = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SMALL_BUF_SIZE, L"s_cprinter_shared_mem");
    if (s_shared_file==NULL)
    {
        printf("init_s_share_memory failed.\n");
        return false;
    }
    s_lpBUF = MapViewOfFile(s_shared_file, FILE_MAP_ALL_ACCESS, 0, 0, SMALL_BUF_SIZE);
    if (s_lpBUF == NULL)
    {
        CloseHandle(s_shared_file);
        return false;
    }
	/*
    s_H_Mutex = CreateMutexW(NULL, FALSE, L"s_sm_mutex");
    if (s_H_Mutex == NULL)
    {
        UnmapViewOfFile(s_lpBUF);
        CloseHandle(s_shared_file);
        s_lpBUF = NULL;
        return false;
    }
	*/
    return true;
}
bool init_share_memory()
{
    // hFile = CreateFileW(L"cprinter_shared_mem", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 
    // NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // if (hFile == INVALID_HANDLE_VALUE){
    //     printf("init_share_memory create file failed.\n");
    //     return false;
    // }
    // INVALID_HANDLE_VALUE or hFile
    shared_file = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE, L"cprinter_shared_mem");
    if (shared_file==NULL)
    {
        printf("init_share_memory failed.\n");
        return false;
    }
    lpBUF = MapViewOfFile(shared_file, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
    if (lpBUF == NULL)
    {
        CloseHandle(shared_file);
        return false;
    }
	/*
    H_Mutex = CreateMutexW(NULL, FALSE, L"sm_mutex");
    if (H_Mutex == NULL)
    {
        UnmapViewOfFile(lpBUF);
        CloseHandle(shared_file);
        lpBUF = NULL;
        return false;
    }
	*/
    // H_Event = CreateEventW(NULL, FALSE, FALSE, _T("sm_event"));
    // if (H_Event == NULL)
    // {
    //     CloseHandle(H_Mutex);
    //     UnmapViewOfFile(lpBUF);
    //     CloseHandle(shared_file);
    //     return false;
    // }
    return true;
}

bool release_share_memory()
{
    
    if (H_Mutex!=NULL){
        CloseHandle(H_Mutex);
    }
    // CloseHandle(H_Event);
    UnmapViewOfFile(lpBUF);
    CloseHandle(shared_file);
    if(hFile!=NULL){
        CloseHandle(hFile);
    }
    if (s_H_Mutex!=NULL){
        CloseHandle(H_Mutex);
    }
    if (s_lpBUF!=NULL){
       UnmapViewOfFile(s_lpBUF);
    }
    if (s_shared_file!=NULL){
       UnmapViewOfFile(s_shared_file);
    }
    if (s_hFile!=NULL){
       CloseHandle(s_hFile);
    }
}

bool write_share_memory(const string &info)
{
    
    DWORD time_out = 10*1000;
    if(lpBUF!=NULL)
    {
        //WaitForSingleObject(H_Mutex, time_out);
        DWORD size = info.size();
        const char* c = info.c_str();
        // DWORD n = size*sizeof(TCHAR);
        DWORD n = size;
        memset(lpBUF, 0, n+1);
        memcpy(lpBUF, c, n);
        // printf("write_share_memory info size:%d!\n", size*sizeof(TCHAR));
        //ReleaseMutex(H_Mutex);
        // SetEvent(H_Event);
    }
    FlushViewOfFile(lpBUF, BUF_SIZE);

}
bool write_small_share_memory(const string &info)
{
    
    DWORD time_out = 10*1000;
    if(s_lpBUF!=NULL)
    {
        //WaitForSingleObject(s_H_Mutex, time_out);
        DWORD size = info.size();
        const char* c = info.c_str();
        DWORD n = size*sizeof(TCHAR);
        memset(s_lpBUF, 0, n+1);
        memcpy(s_lpBUF, c, n);
        //ReleaseMutex(s_H_Mutex);
        // SetEvent(H_Event);
    }
    FlushViewOfFile(s_lpBUF, SMALL_BUF_SIZE);

}
DWORD CleanProcessIDByName(const char* targetName, DWORD skipPid)
{
    HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);
    DWORD pids[10];
    // std::vector<DWORD> pids;
    int pos = 0;
    BOOL ret=Process32FirstW(hSnapshot, &pe);
    for (; ret; ret = Process32NextW(hSnapshot, &pe)) {
        std::wstring pn = std::wstring(pe.szExeFile);
        std::string pName = Printer::wstring_to_string(pn);
        if (strcmp(pName.c_str(), targetName) == 0) {
            pids[pos] = pe.th32ProcessID;
            pos++;
            if (pos>=10){
                break;
            }
        }
    }
    CloseHandle(hSnapshot);
    //printf("cprinter CleanProcessIDByName pids pos:%d.\n", pos);
    for(int i=0;i<pos;i++){
        if(i>=10){
            break;
        }
        DWORD pid = pids[i];
        if(pid != skipPid){
            printf("cprinter CleanProcessIDByName will kill pid:%ld.\n", pid);
            TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE,pid), 0);
        }
    }
    return 0;
}
bool processExist(DWORD pid)
{
    bool bExist = false;
    HANDLE hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == INVALID_HANDLE_VALUE)
    {
        return bExist;
    }
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);
    if (Process32FirstW(hSnapShot, &pe))
    {
        do
        {
            if (pe.th32ProcessID == pid) {
                bExist = true;
                break;
            }
        } while (Process32NextW(hSnapShot, &pe));
        
    }
    CloseHandle(hSnapShot);
    return bExist;
}
void update_printer_state(Printer *printer)
{
    printer->updatePrinterList(TRUE);
    const std::string json = printer->printerListJson(TRUE);
    write_share_memory(json);
}
void loop_run(Printer *printer)
{
    int count = 5;
    int period = 1;
    DWORD char_count;
    //count 12;
    while(running)
    {
        utils::TimeUtils::sleepSec(period);
        count++;
        // update one time / 5 seconds
        if (count>5) {
            count = 0;
            // printer->updatePrinterList(FALSE);
            update_printer_state(printer);
            // printer->updatePrinterList(TRUE);
            // const std::string json = printer->printerListJson(TRUE);
            // printf("cprinter service printerListJson json len:%d.\n", json.size());
            // write_share_memory(json);
        }
        string cmd = read_small_share_memory(&char_count);
        if (char_count>0) {
            
            if ("quit" == cmd){
                running = FALSE;
                printf("cprinter service will quit.\n");
            } else if("start" == cmd){
                //printf("cprinter service started.\n");
            } else {
                // printf("printer receive cmd:%s.\n", cmd.c_str());
                vector<String> secs;
                String s(cmd);
                s.Split(",", secs);
                int pos=0;
				String tag = secs[0];
				if ("updatejob" == tag)
				{
					String jobIdStr = secs[1];
					int jobid = atoi(jobIdStr.c_str());
					string pName(secs[2]);
					string tState(secs[3]);
					// printf("cprinter will call printer updateJobs....\n");
					printer->updateJobs(pName, tState, jobid);
					// printf("cprinter update job id:%d,pname:%s,state:%s.\n", jobid,pName.c_str(),tState.c_str());

                    update_printer_state(printer);
				}
				/*
                for (auto it = secs.begin();it!=secs.end();it++)
                {
                    String sec = *it;
                    printf("cprinter service received cmd[%d]:%s.\n", pos, sec.c_str());
                    pos++;
                }
				*/
				write_small_share_memory("start");
            }
        }
        
    }
    
}

int main(int argc, char *argv[])
{
    DWORD _pid;
    DWORD pid = GetCurrentProcessId();
    string param_cmd = "start";
    if (argc > 1)
    {
		char * args = argv[1];
        // printf("args:%s.\n", args);
		int n = strlen(args);
		unsigned char decd[n]={0};
		unsigned char* param_str = (unsigned char*)(args);
		char * ps = (char*)urldecode(param_str, decd);
        param_cmd = string(ps);
    }
    // printf("param_cmd:%s.\n", param_cmd.c_str());
    DWORD param_len = param_cmd.size();
    char temp[64];
    ifstream readPidFile("pid.bin", ios::in);
    if (!readPidFile){
        printf("pid.bin not exist.\n");
        if ("quit" == param_cmd)
        {
            return 0;
        }
    } else {
        readPidFile >> temp;
        _pid = atol(temp);
        if (_pid>0) {
            bool pexist = processExist(_pid);
            if ("quit" == param_cmd && pexist){
                init_open_s_share_memory();
                //printf("write_small_share_memory will write quit command.\n");
                const string _cmd("quit");
                write_small_share_memory(_cmd);
                release_share_memory();
                return 0;
            } else if("datas" == param_cmd && pexist){
                DWORD count;
                // read shared memory datas.
                init_open_share_memory();
                string rs = read_share_memory(&count);
                //printf("read_share_memory count:%d.\n", count);
                if(count>0){
                    printf("<_datas>%s</_datas>\n", rs.c_str());
                }
                release_share_memory();
                return 0;
            } else if("check" == param_cmd && pexist){
                DWORD char_count;
                init_open_s_share_memory();
                string st = read_small_share_memory(&char_count);
                if(char_count>0){
                    printf("<_state>%s</_state>\n", st.c_str());
                }
                release_share_memory();
                return 0;
            } else if(param_len>0 && pexist){
                init_open_s_share_memory();
                write_small_share_memory(param_cmd);
                release_share_memory();
                return 0;
            } else if (pexist){
                DWORD count;
                // read shared memory datas.
                init_open_share_memory();
                string rs = read_share_memory(&count);
                //printf("read_share_memory count:%d.\n", count);
                if(count>0){
                    printf("datas:%s.\n", rs.c_str());
                }
                release_share_memory();
                return 0;
            }
        }
        if ("quit" == param_cmd)
        {
            return 0;
        }
        printf("pid.bin _pid:%ld.\n", _pid);
    }
    ofstream writePidFile("pid.bin", ios::out);
    if (!writePidFile){
        return 0;
    }
    ltoa(pid, temp, 10);
    printf("pid.bin new pid:%ld.\n", pid);
    writePidFile << temp << std::endl;
    writePidFile.close();


    std::vector<PrinterProb> printerList;
    std::map<std::string, std::vector<PJob>*> printerJobs;
    Printer printer(printerJobs, printerList);

    // printer->updatePrinterList(TRUE);
    // std::string json = printer->printerListJson(TRUE);

    init_share_memory();
    init_s_share_memory();
    write_small_share_memory("start");

    std::string basePath = Path::StartPath();
    String resultFile = basePath + "\\cpp.bin";
    freopen(resultFile.c_str(), "w", stdout);
    printf("check printer argc:%d.\n", argc);
    
    long ct = utils::TimeUtils::getCurrentTime();
    printf("cprinter service run start at:%ld.\n", ct);

    loop_run(&printer);
    CleanProcessIDByName("cprinter.exe", pid);
    // std::string json = printer.printerListJson(FALSE);
    
    // for(auto& it : printerJobs){
    //     it.second->clear();
    //     free(it.second);
    // }
    // printerJobs.clear();
    release_share_memory();

    printf("cprinter service end at:%ld.\n", utils::TimeUtils::getCurrentTime());
    //taskkill -PID 333 -F
    printf("cprinter service quit ok.\n");
    //cprinter.exe updatejob%2C2%2CEPSON%20WF-C5290%20Series%20(%E5%89%AF%E6%9C%AC%201)%2Cresume
    return 0;
}