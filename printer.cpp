#include "printer.h"

int preNUm(unsigned char byte) {
    unsigned char mask = 0x80;
    int num = 0;
    for (int i = 0; i < 8; i++) {
        if ((byte & mask) == mask) {
            mask = mask >> 1;
            num++;
        } else {
            break;
        }
    }
    return num;
}
std::string format_job_status(const DWORD jobStatus)
{
    if (jobStatus == JOB_STATUS_PAUSED) {
        return "paused";
    } else if(jobStatus == JOB_STATUS_ERROR) {
        return "error";
    } else if(jobStatus == JOB_STATUS_DELETING) {
        return "deleting";
    } else if(jobStatus == JOB_STATUS_SPOOLING) {
        return "spooling";
    } else if(jobStatus == JOB_STATUS_PRINTING) {
        return "printing";
    } else if(jobStatus == JOB_STATUS_OFFLINE) {
        return "offline";
    } else if(jobStatus == JOB_STATUS_PAPEROUT) {
        return "paperout";
    } else if(jobStatus == JOB_STATUS_PRINTED) {
        return "printed";
    } else if(jobStatus == JOB_STATUS_DELETED) {
        return "deleted";
    } else if(jobStatus == JOB_STATUS_BLOCKED_DEVQ) {
        return "blocked";
    } else if(jobStatus == JOB_STATUS_USER_INTERVENTION) {
        return "intervention";
    } else if(jobStatus == JOB_STATUS_RESTART) {
        return "restart";
    } else if(jobStatus == JOB_STATUS_COMPLETE) {
        return "complete";
    }
    return "unknown";
}
bool printer_is_local(const DWORD attr)
{
    return (attr & PRINTER_ATTRIBUTE_LOCAL) == PRINTER_ATTRIBUTE_LOCAL;
}
bool printer_is_network(const DWORD attr)
{
    return (attr & PRINTER_ATTRIBUTE_NETWORK) == PRINTER_ATTRIBUTE_NETWORK;
}
bool printer_is_shared(const DWORD attr)
{
    return (attr & PRINTER_ATTRIBUTE_SHARED) == PRINTER_ATTRIBUTE_SHARED;
}
std::string format_printer_status(const DWORD printerStatus)
{
    if (printerStatus == PRINTER_STATUS_PAUSED) {
        return "paused";
    } else if(printerStatus == PRINTER_STATUS_ERROR) {
        return "error";
    } else if(printerStatus == PRINTER_STATUS_PENDING_DELETION) {
        return "pending_deletion";
    } else if(printerStatus == PRINTER_STATUS_PAPER_JAM) {
        return "paper_jam";
    } else if(printerStatus == PRINTER_STATUS_PAPER_OUT) {
        return "paper_out";
    } else if(printerStatus == PRINTER_STATUS_MANUAL_FEED) {
        return "manual_feed";
    } else if(printerStatus == PRINTER_STATUS_PAPER_PROBLEM) {
        return "paper_problem";
    } else if(printerStatus == PRINTER_STATUS_OFFLINE) {
        return "offline";
    } else if(printerStatus == PRINTER_STATUS_IO_ACTIVE) {
        return "io_active";
    } else if(printerStatus == PRINTER_STATUS_BUSY) {
        return "busy";
    } else if(printerStatus == PRINTER_STATUS_PRINTING) {
        return "printing";
    } else if(printerStatus == PRINTER_STATUS_OUTPUT_BIN_FULL) {
        return "output_bin_full";
    } else if(printerStatus == PRINTER_STATUS_NOT_AVAILABLE) {
        return "not_available";
    } else if(printerStatus == PRINTER_STATUS_WAITING) {
        return "waiting";
    } else if(printerStatus == PRINTER_STATUS_PROCESSING) {
        return "processing";
    } else if(printerStatus == PRINTER_STATUS_INITIALIZING) {
        return "initializing";
    } else if(printerStatus == PRINTER_STATUS_WARMING_UP) {
        return "warming_up";
    } else if(printerStatus == PRINTER_STATUS_TONER_LOW) {
        return "toner_low";
    } else if(printerStatus == PRINTER_STATUS_NO_TONER) {
        return "no_toner";
    } else if(printerStatus == PRINTER_STATUS_PAGE_PUNT) {
        return "page_punt";
    } else if(printerStatus == PRINTER_STATUS_USER_INTERVENTION) {
        return "user_intervention";
    } else if(printerStatus == PRINTER_STATUS_OUT_OF_MEMORY) {
        return "out_of_memory";
    } else if(printerStatus == PRINTER_STATUS_DOOR_OPEN) {
        return "door_open";
    } else if(printerStatus == PRINTER_STATUS_SERVER_UNKNOWN) {
        return "unknown";
    } else if(printerStatus == PRINTER_STATUS_POWER_SAVE) {
        return "power_save";
    }
    return "unknown";
}
std::string Printer::wstring_to_string(const std::wstring& input)
{		
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(input);
}
std::wstring Printer::string_to_wstring(const std::string& input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(input);
}
std::string lpwstr_to_string(const LPWSTR wstr)
{
    std::wstring documentName(wstr);
    return Printer::wstring_to_string(documentName);
}

bool isUtf8(const char* data, int len) {
    int num = 0;
    int i = 0;
    while (i < len) {
        if ((data[i] & 0x80) == 0x00) {
            // 0XXX_XXXX
            i++;
            continue;
        }
        else if ((num = preNUm(data[i])) > 2) {
        // 110X_XXXX 10XX_XXXX
        // 1110_XXXX 10XX_XXXX 10XX_XXXX
        // 1111_0XXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
        // 1111_10XX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
        // 1111_110X 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
        // preNUm() 返回首个字节8个bits中首�?0bit前面1bit的个数，该数量也是该字符所使用的字节数        
        i++;
        for(int j = 0; j < num - 1; j++) {
            //判断后面num - 1 个字节是不是都是10开
            if ((data[i] & 0xc0) != 0x80) {
                    return false;
                }
                i++;
        }
    } else {
        //其他情况说明不是utf-8
        return false;
    }
    } 
    return true;
}
bool isGBK(const char* data, int len)  {
    int i  = 0;
    while (i < len)  {
        if (data[i] <= 0x7f) {
            //编码小于等于127,只有一个字节的编码，兼容ASCII
            i++;
            continue;
        } else {
            //大于127的使用双字节编码
            if  (data[i] >= 0x81 &&
                data[i] <= 0xfe &&
                data[i + 1] >= 0x40 &&
                data[i + 1] <= 0xfe &&
                data[i + 1] != 0xf7) {
                i += 2;
                continue;
            } else {
                return false;
            }
        }
    }
    return true;
}
// LPWSTR stringToLPWSTR(std::string orig)
// {
// 	size_t origsize = orig.length() + 1;
// 	const size_t newsize = 100;
// 	size_t convertedChars = 0;
// 	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
// 	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
 
// 	return wcstring;
// }
// std::string WCharToMByte(LPWSTR lpcwszStr)
// {
// 	std::string str;
// 	DWORD dwMinSize = 0;
// 	LPSTR lpszStr = NULL;
// 	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
// 	if (0 == dwMinSize)
// 	{
// 		return FALSE;
// 	}
// 	lpszStr = new char[dwMinSize];
// 	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
// 	str = lpszStr;
// 	delete[] lpszStr;
// 	return str;
// }
// std::string UTF8_To_string(const std::string & str) 
// { 
// int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0); 

// wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
// memset(pwBuf, 0, nwLen * 2 + 2); 

// MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen); 

// int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL); 

// char * pBuf = new char[nLen + 1]; 
// memset(pBuf, 0, nLen + 1); 

// WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL); 

// std::string retStr = pBuf; 

// delete []pBuf; 
// delete []pwBuf; 

// pBuf = NULL; 
// pwBuf = NULL; 

// return retStr; 
// }
// std::string GBKToUTF8(const std::string& strGBK)
// {
// 	std::string strOutUTF8 = "";
// 	WCHAR * str1;
// 	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
// 	str1 = new WCHAR[n];
// 	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
// 	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
// 	char * str2 = new char[n];
// 	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
// 	strOutUTF8 = str2;
// 	delete[]str1;
// 	str1 = NULL;
// 	delete[]str2;
// 	str2 = NULL;
// 	return strOutUTF8;
// }

// std::string UTF8_To_string(const char* c, int csize) 
// { 
// int nwLen = MultiByteToWideChar(CP_UTF8, 0, c, -1, NULL, 0); 

// wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
// memset(pwBuf, 0, nwLen * 2 + 2); 

// MultiByteToWideChar(CP_UTF8, 0, c, csize, pwBuf, nwLen); 

// int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL); 

// char * pBuf = new char[nLen + 1]; 
// memset(pBuf, 0, nLen + 1); 

// WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL); 

// std::string retStr = pBuf; 

// delete []pBuf; 
// delete []pwBuf; 

// pBuf = NULL; 
// pwBuf = NULL; 

// return retStr; 
// }

int normalNewHeight(unsigned long width, unsigned long height){
    int wm,hm;
    wm = width % 4;
    if (wm == 0){
        return height;
    }
    wm = width % 2;
    if (wm == 0) {
        hm = height % 2;
        if (hm == 0){
            return height;
        } else {
            return (height+1)/2 * 2;
        }
    } else {
        hm = height % 4;
        if (hm == 0) {
            return height;
        }
        hm = height % 2;
        if (hm == 0){
            return (height+1)/2 * 2;
        } else {
            return (height+3)/4 * 4;
        }
    }
}
/*
void Printer::jpeg2bmp(std::string filePath){
    FILE * pfin = fopen(filePath.c_str(), "rb");
    fseek(pfin, 0L, SEEK_END);
    DWORD len = ftell(pfin);
    fseek(pfin, 0, SEEK_SET);
    std::string pwfName = filePath + ".bmp";
    FILE * pwf = fopen(pwfName.c_str(), "wb");
    CxImage img(pfin, CXIMAGE_FORMAT_JPG);
    img.Encode(pwf, CXIMAGE_FORMAT_BMP);
    
    fclose(pfin);
    fclose(pwf);
}
*/
/*
    struct jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    cinfo.err = jpeg_std_error(&jerr); // very important
    unsigned char *src_buff;
    unsigned char *point;
    unsigned int row_width;
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, pfin);

    jpeg_read_header(&cinfo, TRUE);
    printf("jpeg width:%d,height:%d,file len:%ld\n",cinfo.image_width, cinfo.image_height,len);

    jpeg_start_decompress(&cinfo);
    unsigned int width = cinfo.output_width;
    unsigned int height = cinfo.output_height;
    int depth = cinfo.output_components;
    printf("jpeg out width:%d,height:%d,depth:%d,X_density:%d,y_density:%d\n",width, height, depth, cinfo.X_density, cinfo.Y_density);
    
    write_bmp_header(&cinfo, pwf);            //写bmp文件头
    row_width = width * depth;
    printf("out_color_components:%d, out_color_space:%d\n", cinfo.out_color_components, cinfo.out_color_space);
    //src_buff=new unsigned char[width*height*depth];
    //memset(src_buff,0,sizeof(unsigned char)*width*height*depth);
    buffer=(*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo,JPOOL_IMAGE,row_width,1);
    //point = src_buff;
    while (cinfo.output_scanline < height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        //memcpy(point, *buffer, width * depth);
        //point += width * depth;
        fwrite(*buffer, row_width, 1, pwf);
    }
    fix_bmp_data(&cinfo, pwf);
    // write_bmp_data(&cinfo,src_buff, pwf);    //写bmp像素数据
    
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    //delete[] src_buff;
    */
/*
int Printer::print(std::string printerName, std::string docName, std::string imagePath, DWORD dwCount)
{
    DWORD level=1;
    DWORD dwBytesWritten, len;
    char *buff;
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    
    // char szprinter[80];
    // char *szDevice;
    // char *szDriver;
    // char *szOutput;
    // char *nextToken;
    // HDC hdcprint;
    
   //ALT::CImage img;

    //img.Load(imagePath);
    HBITMAP hBm;
    // LoadBitmapA(NULL, )
    FILE * pfin = fopen(imagePath.c_str(), "rb");
    fseek(pfin, 0L, SEEK_END);
    len = ftell(pfin);
    fseek(pfin, 0, SEEK_SET);
    
    // fread(&fileHeader,sizeof(BITMAPFILEHEADER),1,pfin);
    // fread(&infoHeader,sizeof(BITMAPINFOHEADER),1,pfin);
    // int height,width;
    // height=0;
    // width=0;
    // height=infoHeader.biHeight;
    // width=infoHeader.biWidth;
    // if (infoHeader.biBitCount >= 1) {
    //     int size = width * height;
    // }
    
    buff = new char[len];
    fread( buff, len, 1, pfin );
    //LPBYTE lpData;

    HANDLE hPrinter = NULL;
    DOC_INFO_1W docInfo;
    
    //LPWSTR
    DWORD ret = 0;
    char *pn = const_cast<char *>(printerName.c_str());
    if (OpenPrinterA(pn, &hPrinter, NULL)){
        docInfo.pDocName = (LPWSTR)docName.c_str();
        docInfo.pOutputFile = NULL;
        docInfo.pDatatype = stringToLPWSTR("RAW");

        ret = StartDocPrinterW(hPrinter, level, (LPBYTE)&docInfo);
        if (ret == 0) {
            errorCode = GetLastError();
            ClosePrinter(hPrinter);
            return FALSE;
        }
        ret = StartPagePrinter(hPrinter);
        if (!ret) {
            EndDocPrinter(hPrinter);
            ClosePrinter(hPrinter);
            errorCode = ret;
            return FALSE;
        }
        //if (!WritePrinter(hPrinter, lpData, dwCount, &dwBytesWritten)) {
        if (!WritePrinter(hPrinter, buff, dwCount, &dwBytesWritten)) {
            EndPagePrinter(hPrinter);
            EndDocPrinter(hPrinter);
            ClosePrinter(hPrinter);
            errorCode = ret;
            return FALSE;
        }
        if (!EndPagePrinter(hPrinter)) {
            EndDocPrinter(hPrinter);
            ClosePrinter(hPrinter);
            return FALSE;
        }
        if (!EndDocPrinter(hPrinter)) {
            ClosePrinter(hPrinter);
            return FALSE;
        }
        ClosePrinter(hPrinter);
        if (dwBytesWritten != dwCount) {
            errorCode = -9;
            return FALSE;
        }
        return TRUE;
    } else {
        errorCode = GetLastError();
        return FALSE;
    }
    fclose( pfin );
    //static DOCINFOW di={sizeof(DOCINFOW),"printer", NULL};
    // static DOCINFO di={sizeof(DOCINFO), "printer", NULL, "raw"};
    // GetProfileString("windows", "device", ",,,", szprinter, 80);
    // szDevice=strtok_s(szprinter, ",", &nextToken);
    // szDriver=strtok_s(NULL, ","strtok_s);
    // szOutput=strtok_s(NULL, ","strtok_s);
    // if(szDevice&&szDriver&&szOutput){
    //     if((hdcprint=CreateDC(szDriver, szDevice, szOutput, NULL))!=0){
    //         if(StartDoc(hdcprint, &di)>0){
    //             StartPage(hdcprint);
    //             SaveDC(hdcprint);
    //             TextOut(hdcprint,1,1,str,strlen(str));
    //             RestoreDC(hdcprint,-1);
    //             EndPage(hdcprint);
    //             EndDoc(hdcprint);
    //         }
    //         DeleteDC(hdcprint);
    //     }else {
    //         return 0;
    //     }
    // }
    int rs = ret;
    return rs;
}
*/
int support_duplex(const std::wstring& printerName, const std::string& portName)
{
    LPCWSTR pn = (LPCWSTR)printerName.c_str();
    LPCWSTR pPort = (LPCWSTR)Printer::string_to_wstring(portName).c_str();
    return DeviceCapabilitiesW(pn, pPort, DC_DUPLEX, NULL, NULL);
}
//DC_COLORDEVICE
int support_color(const std::wstring& printerName, const std::string& portName)
{
    LPCWSTR pn = (LPCWSTR)printerName.c_str();
    LPCWSTR pPort = (LPCWSTR)Printer::string_to_wstring(portName).c_str();
    return DeviceCapabilitiesW(pn, pPort, DC_COLORDEVICE, NULL, NULL);
}

Printer::Printer(std::map<std::string, std::vector<PJob>*> printerJobs, std::vector<PrinterProb> printerList)
:printerJobs(printerJobs),printerList(printerList)
{
}
std::wstring Printer::getDefaultPrinterName() {
    DWORD size = 0;
    std::wstring printerName;
    // pPrinterInfo = (PRINTER_INFO_2W *)malloc(cByteNeeded);
    GetDefaultPrinterW(NULL, &size);
    if (size) {
        // char* buffer = new char[size];
        LPWSTR buffer = new WCHAR[size];
        GetDefaultPrinterW(buffer, &size);
        printerName = buffer;
        delete []buffer;
    }
    return printerName;
}

BOOL Printer::updateTaskList(const std::wstring& printerName, DWORD jobCount, BOOL showJobs){
    HANDLE hPrinter = NULL;
    if(OpenPrinterW((LPWSTR)printerName.c_str(), &hPrinter, NULL)){
        std::string pName = Printer::wstring_to_string(printerName);
        if(showJobs){
            queryJobs(hPrinter, pName, jobCount);
        }
        
        ClosePrinter(hPrinter);
        return TRUE;
    }
    return FALSE;
}
void Printer::clearCache()
{
    printerList.clear();
    for(auto& it : printerJobs){
        it.second->clear();
        free(it.second);
    }
    printerJobs.clear();
}

void Printer::updatePrinterList(BOOL showJobs){
    int ret = 0;
    DWORD dwBufferLen = 0;
    DWORD level=2;
    DWORD dwNeed;
    DWORD dwRet;
    PRINTER_INFO_2W pInfo;
    BYTE *pBuffer;
    BYTE *pTmpBuffer;
    
    clearCache();
    EnumPrintersW(PRINTER_ENUM_LOCAL, NULL, level, NULL, 0, &dwNeed, &dwRet);
    pBuffer = new BYTE[dwNeed];
    if (pBuffer == NULL){
        ret = -1;
    } else {
        dwBufferLen = dwNeed;
    }
    if (ret == 0){
        ret = EnumPrintersW(PRINTER_ENUM_LOCAL, NULL, level, pBuffer, dwBufferLen, &dwNeed, &dwRet);
        if (ret == 0){
            ret = -1;
        } else {
            ret = 0;
        }
    }
    if(ret == 0) {
        pTmpBuffer = pBuffer;
        
        for(int i=0;i<(int)dwRet;i++){
            memcpy(&pInfo,pTmpBuffer,sizeof(pInfo));
            std::wstring spPrinterName(pInfo.pPrinterName);
            
            std::string pName = Printer::wstring_to_string(spPrinterName);
            std::string pStatus = format_printer_status(pInfo.Status);
            DWORD jobCount = pInfo.cJobs;
            DWORD attr = pInfo.Attributes;
            
            PrinterProb p;
            
            String ppn(Printer::wstring_to_string(pInfo.pPortName));
            p.supportColor = 0;
			
            if (ppn.size()>0){
                std::vector<String> pns;
                ppn.Split(",", pns);
                if(pns.size()>0){
                    auto it = pns.begin();
                    String firstPortName = *it;
                    std::string portName(firstPortName.c_str());
                    int support_cl = support_color(spPrinterName, portName);
                    //printf("updatePrinterList printer:%s, firstPortName:%s,support_cl:%d\n", pName.c_str(), portName.c_str(), support_cl);
                    
                    p.supportColor = support_cl==1?1:0;
                }
            }
            
            //printf("updatePrinterList printer attr:%ld\n", attr);
            
            
            p.isLocal = printer_is_local(attr)?1:0;
            p.isShared = printer_is_shared(attr)?1:0;
            p.isNetwork = printer_is_network(attr)?1:0;
            
            // p.name = pName;
            DWORD n = pName.size();
            memset(p.name, 0, n+1);
            memcpy(p.name, pName.c_str(), n);
            // p.status = pStatus;
            n = pStatus.size();
            // printf("updatePrinterList printer pStatus size:%d\n", n);
            memset(p.pStatus, 0, n+1);
            memcpy(p.pStatus, pStatus.c_str(), n);
            p.jobCount = jobCount;

            printerList.emplace(printerList.begin(), p);
            /**/
            // std::string v = wstring_to_string(spPrinterName);
            // jobCount = pPrinterInfo->cJobs;
            // std::wstring printName(pPrinterInfo->pPrinterName);// = pPrinterInfo->pPrinterName;
            // p.name = Printer::wstring_to_string(printName);
            // p.status = format_printer_status(pPrinterInfo->Status);
            
            if (i>=0){
                updateTaskList(spPrinterName, jobCount, showJobs);
            }
            // printerList.insert(v);
            pTmpBuffer += sizeof(pInfo);
        }
    }
    delete []pBuffer;
}

std::string Printer::printerListJson(BOOL showJobs) {
    Document d;
    d.SetObject();
    Document::AllocatorType& allocator = d.GetAllocator();
    Value printers(kArrayType);
    for(PrinterProb elem : printerList){
        // Value pn(kStringType);
        // pn.SetString(StringRef(elem.c_str()),allocator);
        // const std::wstring o = elem;
        // std::string v = wstring_to_string(o)
        
        Value printer(kObjectType);

        std::string printerName(elem.name);
        // std::string printerStatus = elem.status;
        
        // uint16_t isLocal = elem.isLocal;
        // uint16_t isShared = elem.isShared;
        // uint16_t isNetwork = elem.isNetwork;

        // std::string portName = elem.portName;

        printer.AddMember("isLocal", elem.isLocal, allocator);
        printer.AddMember("isShared", elem.isShared, allocator);
        printer.AddMember("isNetwork", elem.isNetwork, allocator);
        printer.AddMember("supportColor", elem.supportColor, allocator);
        const char* key = printerName.c_str();
        // const char* _status = printerStatus.c_str();
        // const char* _portName = portName.c_str();
        
        Value pName(elem.name, allocator);
        printer.AddMember(StringRef("name"), pName.Move(), allocator);
        //printf("printer status:%s\n", elem.pStatus);
        Value pStatus(elem.pStatus, allocator);
        printer.AddMember(StringRef("status"), pStatus.Move(), allocator);

        // Value pPort(_portName, allocator);
        // printer.AddMember(StringRef("portName"), pPort.Move(), allocator);
        /**/
        if (showJobs) 
        {
            std::map<std::string, std::vector<PJob>*>::iterator _iter = printerJobs.find(key);
            if (_iter != printerJobs.end())
            {
                // printf("find job objects in......key:%s\n", key);
                
                Value json_jobs(kArrayType);
                
                std::vector<PJob>* jobs = _iter->second;
                auto iter=jobs->begin();
                for(iter;iter!=jobs->end();++iter)
                {
                    PJob job = *iter;
                    //printf("find job objects jobId:%d\n", job.jobId);
                    
                    Value json_job(kObjectType);
                    
                    // uint64_t _jobId = job.jobId;
                    json_job.AddMember("id", job.jobId, allocator);
                    //Value doc(job.docName.c_str(), allocator);
					Value doc(job.docName, allocator);
                    json_job.AddMember(StringRef("doc"), doc.Move(), allocator);
                    //Value _status(job.status.c_str(), allocator);
					Value _status(job.status, allocator);
                    json_job.AddMember(StringRef("status"), _status.Move(), allocator);
                    // uint64_t _size = job.size;
                    json_job.AddMember("size", job.size, allocator);
                    // uint64_t _priority = job.priority;
                    json_job.AddMember("priority", job.priority, allocator);
                    // uint64_t _pagesPrinted = job.pagesPrinted;
                    json_job.AddMember("pagesPrinted", job.pagesPrinted, allocator);
                    // uint64_t _totalPages = job.totalPages;
                    json_job.AddMember("totalPages", job.totalPages, allocator);
                    
                    json_jobs.PushBack(json_job.Move(),allocator);
                    
                }
                
                printer.AddMember(StringRef("jobs"), json_jobs.Move(), allocator);
                
            }
        }
        printers.PushBack(printer.Move(), allocator);
        
    }
    d.AddMember("printers", printers.Move(), allocator);
    StringBuffer buffer;
    // Writer<StringBuffer, ANSI, UTF8, CrtAllocator, kWriteDefaultFlags> writer(buffer);
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
    // jsonSize = buffer.GetSize();
    // jsonSize = buffer.GetLength();

    // std::string rs = UTF8_To_string(buffer.GetString(), buffer.GetLength());
    // return rs;
    // printf("printerListJson GetString:%s\n", buffer.GetString());
    return std::string(buffer.GetString());
}

int Printer::checkPrinter(std::wstring printerName) {
    int ret = 0;
    DWORD dwBufferLen = 0;
    DWORD level=2;
    DWORD dwNeed;
    DWORD dwRet;
    PRINTER_INFO_2W pInfo;
    BYTE *pBuffer;
    BYTE *pTmpBuffer;
    EnumPrintersW(PRINTER_ENUM_LOCAL, NULL, level, NULL, 0, &dwNeed, &dwRet);
    pBuffer = new BYTE[dwNeed];
    if (pBuffer == NULL){
        ret = -1;
    } else {
        dwBufferLen = dwNeed;
    }
    if (ret == 0){
        ret = EnumPrintersW(PRINTER_ENUM_LOCAL, NULL, level, pBuffer, dwBufferLen, &dwNeed, &dwRet);
        if (ret == 0){
            ret = -1;
        } else {
            ret = 0;
        }
    }
    if(ret == 0) {
        pTmpBuffer = pBuffer;
        for(int i=0;i<(int)dwRet;i++){
            memcpy(&pInfo,pTmpBuffer,sizeof(pInfo));
            //std::string spPrinterName = pInfo.pPrinterName;
            std::wstring spPrinterName(pInfo.pPrinterName);
            // std::string spPrinterName = WCharToMByte(pInfo.pPrinterName);
            if (spPrinterName == printerName){
                ret = 0;
                break;
            } else {
                ret = -1;
            }
            pTmpBuffer += sizeof(pInfo);
        }
    }
    return ret;
}

BOOL Printer::queryJobs(HANDLE hPrinter,/* Handle to the printer. */
    const std::string& printerName,         /* Print Queue status.*/
    DWORD jobCount
    ){
    
    DWORD cByteNeeded, nReturned, cByteUsed;
    DWORD priority, totalPages, size, pagesPrinted, jobStatus;
    // LPWSTR pStat;
    JOB_INFO_2W *_pJobStorage = NULL;
    // PRINTER_INFO_2W *pPrinterInfo = NULL;
    BYTE *pBuffer;
    int i;
    // if (!GetPrinterW(hPrinter, 2, NULL, 0, &cByteNeeded))
    // {
    //     if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    //         return FALSE;
    // }
    // pPrinterInfo = (PRINTER_INFO_2W *)malloc(cByteNeeded);
    // if (!pPrinterInfo){
    //     // Failure to allocate memory.
    //     return FALSE;
    // }
    // if (!GetPrinterW(hPrinter, 2, (LPBYTE)pPrinterInfo, cByteNeeded, &cByteUsed))
    // {
    //     // Failure to access the printer.
    //     free(pPrinterInfo);
    //     pPrinterInfo = NULL;
    //     return FALSE;
    // }
    // jobCount = pPrinterInfo->cJobs;
    // *printerJobCount = jobCount;
    // std::wstring printName(pPrinterInfo->pPrinterName);// = pPrinterInfo->pPrinterName;
    // std::string printerName = Printer::wstring_to_string(printName);
    // bool isgbk = isGBK(v.c_str(), v.size());
    // bool isurf8 = isUtf8(v.c_str(), v.size());
    // if (isgbk && isurf8) {
    //     v = GBKToUTF8(v);
    // }
    // printf("queryJobs pPrinterInfo.pPrinterName :%s\n,isgbk:%d\n,isurf8:%d\n", v.c_str(), isgbk, isurf8);
    // printf("queryJobs pPrinterInfo.pPrinterName:%s\n, jobs:%d\n", printerName.c_str(), jobCount);

    //EnumPrintersW(PRINTER_ENUM_LOCAL, NULL, level, NULL, 0, &dwNeed, &dwRet);
    
    if (!EnumJobsW(hPrinter, 0, jobCount, 2, NULL, 0, &cByteNeeded, &nReturned))
    {
        //unsigned long err = GetLastError();
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            return FALSE;
        }
        
    }
    // printf("queryJobs EnumJobsW need bytes count:%d\n", cByteNeeded);
    _pJobStorage = (JOB_INFO_2W *)malloc(cByteNeeded);
    if (!_pJobStorage)
    {
        return FALSE;
    }
    ZeroMemory(_pJobStorage, cByteNeeded);
    if (!EnumJobsW(hPrinter, 0, jobCount, 2, (LPBYTE)_pJobStorage, cByteNeeded, &cByteUsed, &nReturned))
    {
        free(_pJobStorage);
        _pJobStorage = NULL;
        return FALSE;
    }
    JOB_INFO_2W jobInfo;
    JOB_INFO_2W *pJobStorage = _pJobStorage;
    for (i=0; i < jobCount; i++)
    {
        memcpy(&jobInfo,pJobStorage,sizeof(jobInfo));
        //std::string spPrinterName = pInfo.pPrinterName;
        std::string doc = lpwstr_to_string(jobInfo.pDocument);
        // printf("queryJobs pDocument:%s\n", doc.c_str());
        // pStat = jobInfo.pStatus;
        // if (pStat) {
        //     std::string stat = lpwstr_to_string(pStat);
        //     printf("queryJobs stat:%s\n", stat.c_str());
        // }
        // std::string stat = lpwstr_to_string(jobInfo.pStatus);
        // printf("queryJobs stat:%s\n", stat.c_str());
        jobStatus = jobInfo.Status;
        std::string jobStatusStr = format_job_status(jobStatus);
        priority = jobInfo.Priority;
        totalPages = jobInfo.TotalPages;
        size = jobInfo.Size;
        pagesPrinted = jobInfo.PagesPrinted;
        PJob job;
        job.jobId = jobInfo.JobId;
        job.size = size;
        job.priority = priority;
        job.totalPages = totalPages;
        job.pagesPrinted = pagesPrinted;
        //job.status = jobStatusStr;
		DWORD n = jobStatusStr.size();
		memset(job.status, 0, n+1);
		memcpy(job.status, jobStatusStr.c_str(), n);
		n = doc.size();
        //job.docName = doc;
		memset(job.docName, 0, n+1);
		memcpy(job.docName, doc.c_str(), n);
        AddJobInfo(printerName, job);
        // printf("queryJobs jobStatus:%d\n,priority:%d\n,totalPages:%ld\n,size:%ld\n,pagesPrinted:%d\n", jobStatus, priority, totalPages, size, pagesPrinted);
        // printf("queryJobs jobStatusStr:%s\n", jobStatusStr.c_str());

        pJobStorage += 1;
    }
    if (_pJobStorage!=NULL){
        free(_pJobStorage);
    }
    
    // if (!pPrinterInfo == NULL) {
    //     *pStatus = pPrinterInfo->Status;
    //     free(pPrinterInfo);
    // }
    return TRUE;
}
BOOL Printer::updateJobs(const std::string& printerName, const std::string& state, DWORD jobId)
{
    HANDLE hPrinter = NULL;
    std::wstring pn = Printer::string_to_wstring(printerName);
    if(OpenPrinterW((LPWSTR)pn.c_str(), &hPrinter, NULL)){
		printf("updateJobs OpenPrinterW ok,state:%s.\n", state.c_str());
        
        if("pause"==state){
            return pauseJob(hPrinter, jobId);
        } else if("resume"==state){
            return resumeJob(hPrinter, jobId);
        } else if("delete"==state){
            return deleteJob(hPrinter, jobId);
        }
        ClosePrinter(hPrinter);
    } else {
		printf("updateJobs can not  OpenPrinterW %s.\n", printerName.c_str());
	}
    return FALSE;
}
BOOL Printer::pauseJob(HANDLE hPrinter, DWORD jobId)
{
    JOB_INFO_2W* _pJobStorage=NULL;
    DWORD cByteNeeded, cByteUsed;
    if (!GetJobW(hPrinter, jobId, 2, NULL, 0, &cByteNeeded))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            return FALSE;
        }
    }
    _pJobStorage = (JOB_INFO_2W *)malloc(cByteNeeded);
    if(!GetJobW(hPrinter, jobId, 2, (LPBYTE)_pJobStorage, cByteNeeded, &cByteUsed)){
        free(_pJobStorage);
        return FALSE;
    }
    if (!SetJobW(hPrinter, jobId, 2, (LPBYTE)_pJobStorage, JOB_CONTROL_PAUSE))
    {
        free(_pJobStorage);
        return FALSE;
    }
    
    if(_pJobStorage!=NULL){
        free(_pJobStorage);
    }
    return TRUE;
}
BOOL Printer::resumeJob(HANDLE hPrinter, DWORD jobId)
{
    JOB_INFO_2W* _pJobStorage=NULL;
    DWORD cByteNeeded, cByteUsed;
    if (!GetJobW(hPrinter, jobId, 2, NULL, 0, &cByteNeeded))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            return FALSE;
        }
    }
    _pJobStorage = (JOB_INFO_2W *)malloc(cByteNeeded);
    if(!GetJobW(hPrinter, jobId, 2, (LPBYTE)_pJobStorage, cByteNeeded, &cByteUsed)){
        free(_pJobStorage);
        return FALSE;
    }
    if (!SetJobW(hPrinter, jobId, 2, (LPBYTE)_pJobStorage, JOB_CONTROL_RESUME))
    {
        free(_pJobStorage);
        return FALSE;
    }
    
    if(_pJobStorage!=NULL){
        free(_pJobStorage);
    }
    return TRUE;
}
BOOL Printer::deleteJob(HANDLE hPrinter, DWORD jobId)
{
    JOB_INFO_2W* _pJobStorage=NULL;
    DWORD cByteNeeded, cByteUsed;
    if (!GetJobW(hPrinter, jobId, 2, NULL, 0, &cByteNeeded))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            return FALSE;
        }
    }
    _pJobStorage = (JOB_INFO_2W *)malloc(cByteNeeded);
    if(!GetJobW(hPrinter, jobId, 2, (LPBYTE)_pJobStorage, cByteNeeded, &cByteUsed)){
        free(_pJobStorage);
        return FALSE;
    }
    if (!SetJobW(hPrinter, jobId, 2, (LPBYTE)_pJobStorage, JOB_CONTROL_DELETE))
    {
        free(_pJobStorage);
        return FALSE;
    }
    
    if(_pJobStorage!=NULL){
        free(_pJobStorage);
    }
    return TRUE;
}
void Printer::AddJobInfo(const std::string& key, const PJob& job){
    bool exist = false;
    
    // for(auto& it : printerJobs){
    //     if(it !=NULL && it.first == key){
    //         it.second = job.docName;
    //         exist = true;
    //         break;
    //     }
    // }
    // std::map<std::string, PJob>::iterator iter = printerJobs.find(key.c_str());
    // std::map<std::string, std::string>::iterator iter = printerJobs.find(key.c_str());
    const char* _key = key.c_str();
    PJob _job = job;
    std::map<std::string, std::vector<PJob>*>::iterator iter = printerJobs.find(_key);
    if (iter != printerJobs.end())
    {
        std::vector<PJob>* jobs = iter->second;
        // iter->second = job;
        // auto job = iter->second;
        jobs->push_back(job);
        // printf("printerListJson job=> docName:%s\n", job.docName.c_str());
    } else {
        std::vector<PJob> *_jobs = new std::vector<PJob>();
        
        _jobs->push_back(job);
        printerJobs.emplace(std::pair<const std::string, std::vector<PJob>*>(key, _jobs));
    }

    // printf("AddJobInfo key:%s\n", _key);
    
    // if (!exist) {
    //     printerJobs.emplace(std::pair<const std::string, PJob>(key, job));
    //     // printerJobs.emplace(key, job);
    // }
}
// BOOL Printer::IsPrinterError(HANDLE hPrinter)
// {
//     JOB_INFO_2W  *pJobs;
//     int i;
//     DWORD cJobs;
//     DWORD dwPrinterStatus;

//     /*
//     *  Get the state information for the Printer Queue and
//     *  the jobs in the Printer Queue.
//     */
//     if (!queryJobs(hPrinter, &dwPrinterStatus, &cJobs, FALSE))
//         return FALSE;

//     /*
//     *  If the Printer reports an error, believe it.
//     */
//     if (dwPrinterStatus &
//         (PRINTER_STATUS_ERROR |
//         PRINTER_STATUS_PAPER_JAM |
//         PRINTER_STATUS_PAPER_OUT |
//         PRINTER_STATUS_PAPER_PROBLEM |
//         PRINTER_STATUS_OUTPUT_BIN_FULL |
//         PRINTER_STATUS_NOT_AVAILABLE |
//         PRINTER_STATUS_NO_TONER |
//         PRINTER_STATUS_OUT_OF_MEMORY |
//         PRINTER_STATUS_OFFLINE |
//         PRINTER_STATUS_DOOR_OPEN))
//     {
//         free( pJobs );
//         return TRUE;
//     }

//     /*
//     *  Find the Job in the Queue that is printing.
//     */
//     // for (i=0; i < cJobs; i++)
//     // {
//     //     if (pJobs[i].Status & JOB_STATUS_PRINTING)
//     //     {
//     //         /*
//     //         *  If the job is in an error state,
//     //         *  report an error for the printer.
//     //         *  Code could be inserted here to
//     //         *  attempt an interpretation of the
//     //         *  pStatus member as well.
//     //         */
//     //         if (pJobs[i].Status &
//     //             (JOB_STATUS_ERROR |
//     //             JOB_STATUS_OFFLINE |
//     //             JOB_STATUS_PAPEROUT |
//     //             JOB_STATUS_BLOCKED_DEVQ))
//     //         {
//     //             free( pJobs );
//     //             return TRUE;
//     //         }
//     //     } else {
//     //         ///pJobs[i].pDocument;
//     //     }
//     // }

//     /*
//     *  No error condition.
//     */
//     free( pJobs );
//     return FALSE;
// }
// int main()
// {
//     input_file=fopen("lena.jpg","rb");
//     output_file=fopen("lena.bmp","wb");

//     analyse_jpeg();

//     fclose(input_file);
//     fclose(output_file);

//     cout<<"good job."<<endl;
//     cin.get();
//     return 0;
// }