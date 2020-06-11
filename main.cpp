#include <QtWidgets/QApplication>
#include <QProcess>
#include <iostream>
#include "AWQueue.h"
#include "AWGBCheckTool.h"
#include "ConfigureGuide.h"
#include "main.h"
//xml解析头文件
#include <iconv.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

using namespace std;

void testXml()
{

    char * pszPayload = "<bookstore> \
            <book category=\"cooking\"> \
               <title lang=\"en\">Everyday Italian</title>\
               <author>Giada De Laurentiis</author>\
               <year>2005</year>\
               <price>30.00</price>\
            </book>\
            <book category=\"children\" title=\"Harry Potter\" author=\"J K. Rowling\" year=\"2005\" price=\"$29.9\"/>\
   </bookstore>";

    int nXmlSize = 0;
    nXmlSize = strlen(pszPayload);
    xmlDocPtr pstXmlDoc = NULL;
    xmlNodePtr pstXmlNode = NULL;
    xmlNodePtr pstXmlSubNode = NULL;
    char *pszXmlNodeContent = NULL;
    pstXmlDoc = xmlParseMemory(pszPayload, nXmlSize);
    pstXmlNode = xmlDocGetRootElement(pstXmlDoc);
    pstXmlSubNode = pstXmlNode->children;
    while(pstXmlSubNode)
    {
        pszXmlNodeContent = (char *)xmlNodeGetContent(pstXmlSubNode);
        cout << pszXmlNodeContent << endl;
//        cout << (char)pszXmlNodeContent->name << endl;
        pstXmlSubNode = pstXmlSubNode->next;
    }
}

using namespace std;
int main(int argc, char *argv[])
{
    pGBStart_s GB = (pGBStart_s)calloc(1, sizeof(GBStart_s));
    QProcess* pro = new QProcess();
    int handle;
    int ret;
    pro->start("beanstalkd.exe -l 127.0.0.1 -p 11300");
    ret = AW_BSQueue_Init(&handle, nullptr, 0, (unsigned char *)"server",(unsigned char *)"server");
    if(ret == 0)
        cout << "init OK" << endl;
	QApplication a(argc, argv);
    ConfigureGuide w(NULL, GB, handle);
    //char * buf = (char *) calloc(32, sizeof(char));
    //AW_BSQueue_PutBuffer(handle, (unsigned char*)"test", 4);
    testXml();

	w.show();
    a.exec();
    pro->kill();
    AW_BSQueue_Destroy(handle);
    cout << "process killed" << endl;
    return 0;
}
