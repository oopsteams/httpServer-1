#ifndef  PRINTER_H
#define  PRINTER_H

#include <vector>
#include <map>
#include <string>
#include <set>
#include <stdio.h>
#include <locale>
#include <codecvt>
#include <windows.h>
#include <winspool.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/encodings.h"
#include "String.hpp"

// #include <ximage.h>
// extern "C" {
// #include <ximage.h>
// }
//  #include "jpeglib.h"
using namespace rapidjson;

struct PJob{
    uint32_t jobId;
    uint32_t size;
    uint32_t priority;
    uint32_t totalPages;
    uint32_t pagesPrinted;
    char status[32];
    char printStatus[64];
    char docName[200];
};
struct UniPrinterProb{
    uint32_t jobCount;
    uint8_t isLocal;
    uint8_t isShared;
    uint8_t isNetwork;
    uint8_t supportDuplex;
    uint8_t supportColor;
    // std::string status;
    char pStatus[32];
    char name[64];
    char pPort[64];
};

class Printer{
public:
    // , std::map<std::string, UniPrinterProb> printermap
    Printer(std::map<std::string, std::vector<PJob>*> printerJobs);
    
    int jsonSize;
    int errorCode;
    // void AddJobInfo(const std::string& key, const PJob& job);
    // int print(std::string printerName, std::string docName, LPBYTE lpData);
    // int print(std::string printerName, std::string docName, std::string imagePath, DWORD dwCount);
    void updatePrinterList(BOOL showJobs);
    BOOL updateTaskList(const std::wstring& printerName, DWORD jobCount, BOOL showJobs);
    // const char* printerListJson2();
    std::string printerListJson(BOOL showJobs);
    std::string printerListJsonNew(BOOL showJobs);
    int checkPrinter(std::wstring printerName);
    std::wstring getDefaultPrinterName();
    // void jpeg2bmp(std::string filePath);
    // BOOL GetJobs(
    // HANDLE hPrinter,        /* Handle to the printer. */
    // JOB_INFO_2W **ppJobInfo, /* Pointer to be filled.  */
    // int *pcJobs,            /* Count of jobs filled.  */
    // DWORD *pStatus);
    BOOL queryJobs(HANDLE hPrinter,/* Handle to the printer. */
    const std::string& printerName, DWORD jobCount);
    BOOL updateJobs(const std::string& printerName, const std::string& state, DWORD jobId);
    BOOL pauseJob(HANDLE hPrinter, DWORD jobId);
    BOOL resumeJob(HANDLE hPrinter, DWORD jobId);
    BOOL deleteJob(HANDLE hPrinter, DWORD jobId);
    BOOL printPostscript(const std::string& printerName, const std::string& psPath, const std::string& docName, std::vector<int>& range);
    void clearCache();
    virtual ~Printer(){clearCache();};

    static std::string wstring_to_string(const std::wstring& input);
    static std::wstring string_to_wstring(const std::string& input);
private:
    int ByteAlign( int nBits );
    // BOOL IsPrinterError(HANDLE hPrinter);
    std::vector<String> parseSections;
    // std::vector<UniPrinterProb> printerList;
    std::map<std::string, std::vector<PJob>*> printerJobs;
    std::map<std::string, UniPrinterProb> printermap;
    void UpdateJobsInfo(const std::string& key, std::vector<PJob>& jobs_temp_list);
    // void write_bmp_header(j_decompress_ptr cinfo, FILE *output_file);
    // void write_bmp_data(j_decompress_ptr cinfo,unsigned char *src_buff, FILE *output_file);
};

#endif /*PRINTER_H*/