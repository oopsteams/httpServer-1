#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#include    "HttpSocket.h"
#include <stdio.h>
#include <windows.h>
#include <signal.h>
#include "printer.h"

#define _WIN32_WINNT 0x0601
#define SERVICE_PORT 18081

void sigBreak_handler(int sig)
{
	std::string basePath = Path::StartPath();
	String lockFile = basePath + "\\app.lock";
	File::Delete(lockFile);
	exit(2);
}

void modeset(SHORT w, SHORT h)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD s = {w, h};
	SMALL_RECT minr = {0,0,1,1};
	SetConsoleWindowInfo(hOut, true, &minr);

	SetConsoleScreenBufferSize(hOut, s);
	SetConsoleTitle("wvSpeeder");
	SMALL_RECT r = {1,1,w-1,h-1};
	SetConsoleWindowInfo(hOut, true, &r);
	ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
	CloseHandle(hOut);
	
}

void sendJson(Response& rp, String &json){
	rp.SetContent(json, "application/json;charset=UTF-8");
}

int main(int argc, char *argv[])
{
	Printer printer;
	std::string basePath = Path::StartPath();
	String lockFile = basePath + "\\app.lock";
	try{
		signal(SIGBREAK, &sigBreak_handler);
		if (File::Exists(lockFile)){
			int rs = Socket::checkPort(SERVICE_PORT);
			if (rs<0){
				MessageBox(0,"Speeder may be running!","Run error",MB_OK);
				return 0;
			}
		} else {
			File::Create(lockFile);
		}
		HttpSocket server;
		server.WebRoot = basePath;
		String dataPath =  server.WebRoot + "\\data";
		String outPath = server.WebRoot + "\\out";
		String logPath = server.WebRoot + "\\log";
		String sysLog = logPath + "\\sys.log";
		//printf("webroot:%s.",server.WebRoot.c_str());
		
		if (!Path::Exists(dataPath)){
			Path::Create(dataPath);
		}
		if (!Path::Exists(outPath)){
			Path::Create(outPath);
		}
		if (!Path::Exists(logPath)){
			Path::Create(logPath);
		}
		freopen(sysLog.c_str(), "w", stdout);
		server.dataDir = dataPath;
		server.outDir = outPath;
		server.logDir = logPath;
		server.Get("/api", [&](const Request& rq, Response& rp){
			String cmd = rq.GetParam("cmd");
			if(cmd.size()>0){
				printf("cmd:%s\n",cmd.c_str());
				if(cmd == "quit"){
					printf("will call Quit.\n");
					server.Quit();
				} else if(cmd == "render"){
					//String outf = rq.GetParam("o");
					//String infstr = rq.GetParam("i");
					String outf = server.render(rq, rp);
					rp.SetContent("{\"rs\":0,\"src\":"+outf.Replace("\\", "/")+"}", "application/json");
					return;
				} else if(cmd == "check"){
					
				} else if(cmd == "clean"){
					if (Path::Exists(server.dayDir)){
						Path::Delete(server.dayDir);
					}
					if (Path::Exists(server.outDayDir)){
						Path::Delete(server.outDayDir);
					}
					String outLog = server.logDir + "\\render.log";
					if (File::Exists(outLog)){
						File::Delete(outLog);
					}
					
				} else if(cmd == "tobmp"){
					std::string jf = dataPath+"\\test.jpg";
					if (File::Exists(jf)){
						printf("printer jpg path:%s.\n", jf.c_str());
						printer.jpeg2bmp(jf);
					}
					//
					
				} else if(cmd == "printerlist"){
					printer.updatePrinterList();
					std::string json = printer.printerListJson();
					// const char * json = printer.printerListJson2();
					
					// int size = printer.jsonSize;
					// int size = s.size();
					// char * c = new char[size+1];
					// c[size] = 0;
					// strcpy(c, s.c_str());
					printf("printer json:%s.\n", json.c_str());
					// rp.SetContent(json, size, "application/json;charset=UTF-8");

					rp.SetContent(json, "application/json;charset=UTF-8");
					return;
				}
			}
			rp.SetContent("{\"rs\":0}", "application/json");
		});
		server.Post("/api", [&](const Request& rq, Response& rp){
			String cmd = rq.GetParam("cmd");
			if(cmd == "upload"){
				//String fn = rq.GetParam("fn");
				//printf("fn:%s.\n", fn.c_str());
			}
			rp.SetContent("{\"rs\":0}", "application/json");
			//Path::Delete(server.WebRoot);
		});
		//modeset(64,32);
		system("mode con cols=80 lines=25");
		ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
		server.start(SERVICE_PORT);
		printf("server quit!");
	}catch(Error x)
	{
		x.showError();
	}
	File::Delete(lockFile);
	return 0;
}
