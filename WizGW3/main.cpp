//============================================================================
// Name        : WizGW.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
//============================================================================
// Name        : WizGW.cpp
// Author      : liyang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
// SingleL.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include "iostream"
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "STRING_Helper.h"
//#include "HTTP_Config.h"
#include "stdlib.h"
#include "time.h"
//#include "CDebug.h"
//#include "XmlBinProcess.h"
#include "WizBase.h"
//#include <pthread.h>
#include <string.h>
//#include "ThirdQQ.h"
//for WizBase
using namespace std;
#define CONFIGXML "./jawsconfig.ini"
//void test_bin_to_xml(void);
//XmlBinProcess * GB_XmlBin;

#ifdef THIRD_QQ
extern int QQ(void);
#endif

int main(int argc, char **argv)
{
    //loading parameters
    if (WIZBASE_RunGBParams(argc, argv) != 0) {
        cout << "Reading config params error!" << endl;
        return -1;
    }
    
    //0:multi thread solution
    pthread_t GB_Thread[WIZBASE_NUM_THREADS];
    //for (int i=1; i<GB_Ports_NUMBER && i< WIZBASE_NUM_THREADS; i++){
    for (int i=1; i< WIZBASE_NUM_THREADS; i++){//ly new pthread
        //创建子线程，使用pthread_create
        //evthread_use_pthreads();
        if (i==1){//ly new thread UDP
            int e=pthread_create(&(GB_Thread[i]),NULL,WIZBASE_RunUDP,(void *)&i);
            if(0!=e){
                printf("thread error thread num %d, error_n:%d",i,e);
                exit(-1);
            }
        }else{
            printf("thread error:only support two thread main and UDP!");
            exit(-1);
        }
        //      int e=pthread_create(&(GB_Thread[i]),NULL,WIZBASE_RunLibEvent,(void *)&i);
    }
    int j=0;
    WIZBASE_RunLibEvent(&j);
    
    // wait pthread exit
    //for (int i=1; i<GB_Ports_NUMBER && i< WIZBASE_NUM_THREADS; i++){
    for (int i=1; i< WIZBASE_NUM_THREADS; i++){//ly new pthread
        pthread_join (GB_Thread[i], NULL);
        cout << "pthread[" << i << "] quit!" <<endl;
    }
    
    return 0;
    
    //1: single process solution
    /*if (WIZBASE_RunGBParams(argc, argv) != 0) {
     cout << "Reading config params error!" << endl;
     return -1;
     }
     
     int i = 0;
     WIZBASE_RunLibEvent((void *)&i);
     return 1;*/
    
    //2: mult process pid solution;
    /*    pid_t pid;  s
     int num=0;
     pid=fork();
     if (pid==0){
     //run some addition action
     cout << "run addition system action" << endl;
     //sleep(3000);
     system("cd /www/web");
     system("node mother.js");
     }else
     return RunLibEvent(argc,argv);
     #ifdef THIRD_QQ
     ThirdQQ::Run(argc,argv);
     #endif
     return -1;
     */
}

