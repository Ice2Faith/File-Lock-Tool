//FileLocker
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#include<conio.h>
#include<string.h>
#include <direct.h>
#include <io.h>
#include <unistd.h>
#define MAXSIZE 1024
#define TRUE 1
#define FALSE 0
#define WORD char
#define FILEINFO "LoadFile.txt"
//对文件名“FileName”用“password”进行“mode”操作，mode为lock时进行加密，mode为unlock时进行解密,返回值1成功，0解密密码不正确，-1文件未加密不能执行解密
int FileLocker(char * FileName,char * password,char * mode,const char preworkdir[]);
//选择菜单
char SelectMain();
//控制分管中心
void ControlCenter(int num,char * string[]);
//返回当前时间信息，mode值0日期，1时间，2星期
int PresentDATE(int mode);
//快速设置程序窗口颜色标题，宽度，高度
void FaceInt(char * color,char * title,int cols);
//用户帮助或引导说明
void UserGuid();
//载入文件过多提示
void AceptMore();
//内部秘钥加密解密文本
void InLock(char * string);
//从文件读取信息
void FromFileReadInfo();
//从控制台获取信息
void FromConsoleReadInfo();
//文件夹加密，传入完全路径dirroute，文件夹名称dirname，当前工作路径preworkdir，密码password，用户选项usersel
void DirLock(const char dirroute[],const char dirname[],const char preworkdir[],const char password[],const char usersel);
//获得文件呢的名称和父目录路径，传入文件路径fileroute，保存文件名的filename，保存文件目录的fatherroute
int GetFileAttri(const char fileroute[],char filename[],char fatherroute[]);
//适配环境配置，传入保存用户路径的数组UserRoute
void PathEnvinoment(char UserRoute[]);

int main(int num,char * string[])
{
    FaceInt("f5","FileLockToolProV8.0 Dev: Ice2Faith Ver: 8.0",85);
    char UserRoute[40]; //用来初始化运行工环境的数组，只保证传参没问题
    PathEnvinoment(UserRoute);  //初始化环境
    if(num==1)  //未拖入文件启动进入引导
    {
        UserGuid();
    }
    else if(num>507)    //如果拖入文件项目过多则执行
    {
        AceptMore();
        printf("如果需要继续加密/解密506个，请输入 \'+\' ，否则不操作：\n>/  ");
        char con=getch();
        if(con=='+')
            ControlCenter(507,&string[0]);
    }
    else    //常规进行处理
        ControlCenter(num,&string[0]);
    fflush(stdin);
    printf("已完成，按任意键结束\n"); //处理完成提示
    getch();
    return 0;
}
void InLock(char * string)
{
    char InKey[MAXSIZE]= {"Dev:Ice2Faith-20181122\0"};  //内部加密秘钥，不得更改，否则不能兼容以前版本

    int Incount=0;
    int Inkeylen=strlen(InKey);
    int i=0;
    while(*(string+i))
    {
        *(string+i)^=(WORD)(InKey[Incount%Inkeylen]);
        Incount=(Incount++)%Inkeylen;
        i++;
    }
}
int FileLocker(char * FileName,char * password,char * mode,const char preworkdir[])
{
    //file define format:
    //Ice2Faith_FileLockToolPro__int_key_body
    //design :
    //get length of "Ice2Faith_FileLockToolPro_" char to save
    //compare get chars and std chars
    //if == then ,the file is locked else unlock
    //if locked then get int and key then locked
    //if unlock ,reopen file to lock
    FILE * Soure,* Lock;
    Soure=NULL;
    Lock=NULL;
    char Flag[MAXSIZE]= {"Ice2Faith_FileLockToolPro_\0"};
    char ReadFlag[MAXSIZE]= {"\0"};
    char Key[MAXSIZE]= {"\0"};
    char PreWorkDir[MAXSIZE]= {0};
    strcpy(PreWorkDir,preworkdir);
    //printf("Locking:>>>%s\n",PreWorkDir);
    strcat(PreWorkDir,"\\LockFile.tmp\0");
    int KeyLen=0;
    //open file
    Soure=fopen(FileName,"rb");
    //get flag
    fread(ReadFlag,sizeof(WORD),strlen(Flag),Soure);
    //unlock read flag
    InLock(&ReadFlag[0]);
    if(strcmp("unlock",mode)==0)
    {
        if(strcmp(Flag,ReadFlag)!=0)
            return -1;
        char tpch;
        //Locked file to unlock
        //get key length
        fread(&KeyLen,sizeof(int),1,Soure);
        //get contact char
        fread(&tpch,sizeof(WORD),1,Soure);
        //get key of file
        fread(Key,sizeof(WORD),KeyLen,Soure);
        //get contact char
        fread(&tpch,sizeof(WORD),1,Soure);
        //recovery key for cmp password
        InLock(&Key[0]);
        if(strcmp(Key,password)!=0)
        {
            fclose(Soure);
            return 0;
        }
        //recovery key for unlock
        InLock(&Key[0]);
        Lock=fopen(PreWorkDir,"wb");
    }
    else if(strcmp("lock",mode)==0)
    {
        char contact='_';
        //unlock file to lock
        //reopen file
        fclose(Soure);
        Soure=NULL;
        Soure=fopen(FileName,"rb");
        Lock=fopen(PreWorkDir,"wb");
        //write info
        //write flag
        //lock read flag
        InLock(&Flag[0]);
        fwrite(Flag,sizeof(WORD),strlen(Flag),Lock);
        //write key length
        strcpy(Key,password);
        KeyLen=strlen(Key);
        fwrite(&KeyLen,sizeof(int),1,Lock);
        //write contact char
        fwrite(&contact,sizeof(WORD),1,Lock);
        //write key of file
        InLock(&Key[0]);
        fwrite(Key,sizeof(WORD),KeyLen,Lock);
        //write contact char
        fwrite(&contact,sizeof(WORD),1,Lock);
        //start lock
    }
    char * temp=NULL;
    temp=(char *)malloc(sizeof(WORD));
    int counter=0;
    while(!feof(Soure))
    {
        if(fread(temp,sizeof(WORD),1,Soure)==0)
            break;
        *temp^=(WORD)(Key[counter%KeyLen]);
        fwrite(temp,sizeof(WORD),1,Lock);
        counter=(counter++)%KeyLen;
    }
    fclose(Soure);
    fclose(Lock);
    return 1;

}
char SelectMain()
{
    char result='6';
    do
    {
        printf("----------------------------------\n\n");
        printf("\t1.文件加密\n\n");
        printf("\t2.文件解密\n\n");
        printf("\t0.退出程序\n\n");
        printf("----------------------------------\n\n");
        printf("请选择：\n\n>/ ");
        result=getch();
        if(result>='0' && result<='2')
            printf("%c\n",result);
    }
    while(result<'0' || result>'2' );
    return result;
}
void UserGuid()
{
    system("color ef");
    Sleep(30);
    system("color f5");
    system("cls");
    printf("\n\t\tFileLockToolProV8.0\n\n");
    printf("\n\tVer: 8.0\n\n");
    printf("\n\tDev: Ice2Faith\n\n");
    printf("\tPress any key continue\n\n\t>/ ");
    getch();
    system("cls");
    printf("\n\n");
    printf("\t1.\tload file info from \"%s\"\n\n",FILEINFO);
    printf("\t2.\tInput file info\n\n");
    printf("\tElse:\tView help\n\n");
    printf("\tplease select:\n\n\t>/ ");
    char sel='9';
    sel=getch();
    system("cls");
    if(sel=='1')
    {
        FromFileReadInfo();
        return;
    }
    else if(sel=='2')
    {
        FromConsoleReadInfo();
        return;
    }

    for(int i=0; i<5; i++)
    {
        system("cls");
        printf("\n\t使用引导及说明:\n\n");
        printf("\t请将待加密/解密文件拖动到程序图标上启动\t( ^_^)／\n");
        printf("\t或者您也可以把待处理文件路径信息放到文件中: \"%s\"\n",FILEINFO);
        printf("\t注意，路径有无双引号都行，但是不要加另外的东西。\n");
        printf("\t而且，行首第一个如果是#号将会被放弃，不处理。\n");
        printf("\t支持相对路径和绝对路径哦！\n");
        printf("\n\tTips:\t成功操作会将文件等信息保存至 \"C:\\FileLockToolPro\\01A_FileLockToolInfo.txt\"\n");
        printf("\t\t成功操作后的文件将会保存至工作目录 \"C:\\FileLockToolPro\\\"\n");
        printf("\t\t双击运行脚本即可删除源文件：\"01A_DeleteSourceFile.bat\"\n");
        printf("\t特别注意：\n");
        printf("\t\t如果工作目录下存在同名文件，那么旧文件将会被覆盖！！\n");
        printf("\n\tName:\tFileLockToolProV8.0\n\n\tDev:\t Ice2Faith\n");
        printf("\n\t%d秒后将会关闭程序\t\tヾ(￣▽￣)Bye~Bye~\n\n",5-i);
        Sleep(1000);
    }
}
void AceptMore()
{
    system("color cf");
    Sleep(30);
    system("color f5");
    printf("\n\t糟了个糕的\t\t<(ToT)>\n\n\t文件数量超过 506 个\t((((；\"A\"))))\n\n");
    printf("\n\t别问我为什么是 506 个\t\t（＃－.－）\n\n\t鬼知道我经历了什么\t(TwT') \n\n");
    printf("\n\tName:\tFileLockToolProV8.0\n\n\tDev:\t Ice2Faith\n\n");
}
void FaceInt(char * color,char * title,int cols)
{
    char mind[100];
    sprintf(mind,"color %s\0",color);
    system(mind);
    sprintf(mind,"title %s\0",title);
    system(mind);
    sprintf(mind,"mode con cols=%d\0",cols);
    system(mind);
}
int PresentDATE(int mode)
{
    time_t PresentTime;
    PresentTime=time(NULL);
    struct tm * PresentDate;
    PresentDate=localtime(&PresentTime);
    int pdate,ptime;
    pdate=(PresentDate->tm_year+1900)*10000+(PresentDate->tm_mon+1)*100+PresentDate->tm_mday;
    ptime=(PresentDate->tm_hour)*10000+(PresentDate->tm_min)*100+PresentDate->tm_sec;
    if(mode==0)
        return pdate;
    else if(mode==1)
        return ptime;
    else if(mode==2)
        return PresentDate->tm_wday;
    else
        return -1;
}
void DirLock(const char dirroute[],const char dirname[],const char preworkdir[],const char password[],const char usersel)
{
    char mind[2048]= {'\0'};
    char workdir[2048]= {0};
    sprintf(workdir,"%s\\%s",preworkdir,dirname);   //建立新的工作路径
    if(access(workdir,F_OK)!=0) //如果文件夹不存在再创建
    {
        sprintf(mind,"md \"%s\"\0",workdir);
        system(mind);
    }
    struct _finddata_t predir;  //遍历文件夹用
    long handle;    //句柄
    char pdirname[2048]= {0};
    strcpy(pdirname,dirroute);
    strcat(pdirname,"\\");  //跟踪当前文件夹，用来处理遍历
    char tmpname[2048]= {0};
    strcpy(tmpname,pdirname);
    if((handle=_findfirst(strcat(tmpname,"*"),&predir))==-1L)
        return; //vacant dir
    char filename[4096]= {0};
    char fatherroute[4096]= {0};    //保存父路径，文件名，重命名脚本
    char rename[4096];
    do
    {
        //遍历显示，隐藏，只读，子目录，系统文件，不读取空目录和父目录
        if((predir.attrib==_A_ARCH||_A_HIDDEN||_A_RDONLY||_A_SUBDIR||_A_SYSTEM)&&(strcmp(predir.name,".")!=0)&&(strcmp(predir.name,"..")!=0))
        {

            strcpy(tmpname,pdirname);
            strcat(tmpname,predir.name);
            FILE * pf=fopen(tmpname,"r");
            GetFileAttri(tmpname,filename,fatherroute);
            if(!pf) //如果是文件夹则递归操作
            {
                fclose(pf);
                DirLock(tmpname,filename,workdir,password,usersel);
            }
            else    //是文件则执行操作
            {
                fclose(pf);
                int success=0;
                if(usersel=='1')
                    success=FileLocker(tmpname,password,"lock",workdir); //lock
                else
                    success=FileLocker(tmpname,password,"unlock",workdir); //unlock
                if(success==1)
                {
                    if(access(workdir,F_OK)==0) //如果工作路径下文件已经存在则先删除
                    {
                        char remov[2048]= {0};
                        sprintf(remov,"%s\\%s\0",workdir,filename);
                        remove(remov);
                    }
                    sprintf(rename,"ren \"%s\\LockFile.tmp\" \"%s\"\0\0",workdir,filename); //重命名临时文件
                    system(rename);
                }
                else
                {
                    //操作失败提示
                    system("color e1");
                    Sleep(30);
                    system("color f5");
                }
            }

        }
    }
    while(_findnext(handle,&predir)==0);    //直到遍历结束退出
    _findclose(handle); //关闭文件遍历访问
}
void PathEnvinoment(char UserRoute[])
{
    char pathinfo[10][2048]=
    {
        {"FileLockTool Path\n"},
        {"C:FileLockToolPro\n"},    //配置默认工作路径，以免发生错误
        {"You can modify the path to second line.\n"},
        {"Form here down you can free modify it.\n"},
        {"-----------------------------------------\n"},
    };
    FILE * Path=fopen("C:\\FileLockToolPro\\UserPath\\Path.xml","r");   //写入配置文件，如果不存在则建立，存在则读取
    if(!Path)
    {
        fclose(Path);
        system("md C:\\FileLockToolPro\\UserPath");
        Path=fopen("C:\\FileLockToolPro\\UserPath\\Path.xml","w");
        for(int i=0; pathinfo[i][0]; i++)
            fputs(pathinfo[i],Path);
    }
    else
    {
        for(int i=0; i<5; i++)
            fgets(pathinfo[i],2048,Path);
    }
    pathinfo[1][strlen(pathinfo[1])-1]='\0';
    strcpy(UserRoute,pathinfo[1]);  //返回得到的配置路径
    fclose(Path);
}
void ControlCenter(int num,char * string[])
{
    char UserRoute[2048]= {"C:FileLockToolPro"};
    PathEnvinoment(UserRoute);  //获取工作路径
    char usersel=SelectMain();  //获取用户选择
    if(usersel=='0')    //选择退出则退出
        return;
    char workdir[2048]= {0};
    if(usersel=='1')    //选择不同则建立不同的工作路径
    {
        sprintf(workdir,"%s\\MyLock\\\0",UserRoute);
    }
    if(usersel=='2')
    {
        sprintf(workdir,"%s\\MyUnlock\\\0",UserRoute);
    }
    char mind[2048]= {'\0'};
    if(access(workdir,F_OK)!=0) //检查工作路径是否存在，不存在则建立
    {
        sprintf(mind,"md \"%s\"\0",workdir);
        system(mind);
    }
    int isfile=0;   //检查是否是文件
    char filename[4096]= {0};
    char fatherroute[4096]= {0};

    int success=1;

    char password[400]= {'\0'};
    char rename[4096];
    char title[20]= {"Ice2Faith\0"};
    char del='2';   //删除脚本需要标志
    FILE * Password;    //信息脚本输出定义
    char routeinfo[2048]= {0};
    sprintf(routeinfo,"%s01A_FileLockToolInfo.txt\0",workdir);
    Password=fopen(routeinfo,"a");
    system("cls");
    do  //直到获取到合法的密码
    {
        system("cls");
        printf("Please input your key:\n>/ ");
        gets(password);
        if(!strlen(password))
        {
            printf("您貌似没有输入密码哦！请重试！\t\t(-_-) zzz\n");
            Sleep(800);
        }
    }
    while(!strlen(password));
    printf("输入 \'*\' 生成删除源文件脚本：01A_DeleteSourceFile.bat，否则不生成\n>/ ");
    del=getch();
    FILE * Bat;     //建立删除脚本
    if(del=='*')
    {
        //输出必要的提示和版本信息
        char deldir[2048]= {0};
        sprintf(deldir,"%s01A_DeleteSourceFile.bat",workdir);
        Bat=fopen(deldir,"w");
        fputs("@echo off\n",Bat);
        fputs("echo Builder: FileLockToolProV8.0\n",Bat);
        fputs("echo Ver: 8.0\n",Bat);
        fputs("echo Dev: Ice2Faith\n",Bat);
        fputs("title DeleteSourceFile\n",Bat);
        fputs("color f5\n",Bat);
        fputs("echo Dev: Ice2Faith\n",Bat);
    }   //输出必要信息文件说明
    fputs(">>>>>>>>>>>>\tIce2Faith FileLockToolProV8.0 informations\n\n",Password);
    fprintf(Password,"Ver: \t\t8.0\n\n",Password);
    fprintf(Password,"Date: \t\t%d:%d\n\n",PresentDATE(0),PresentDATE(1));
    fputs("Password:\t",Password);
    fputs(password,Password);
    fputs("\n\n",Password);
    fputs("Operation log:\n\n",Password);
    strcat(password,title); //强化密码
    printf("输入 \'+\' 启用详细进度，否则默认");
    char cre=getch();   //进度展示模式标记
    system("cls");
    printf("加密/解密中，请稍后..................\n");
    printf("环境初始化...........................\n");
    printf("建立文件夹: %sFileLockToolPro\\..........\n",workdir);
    if(del=='*')
        printf("创建删除脚本..........\n");
    if(cre=='+')
    {
        system("mode con lines=10 cols=80");
        printf("\n\n处理进度：\n");
    }
    else
        system("mode con lines=7 cols=40");
    int pre=0,be=0,poi=0;   //进度百分比控制变量
    for(int i=1; i<num; i++)
    {
        isfile=GetFileAttri(string[i],filename,fatherroute);
        //判断文件路径性质，提示用户
        if(access(workdir,F_OK)!=0)
        {
            printf("\n文件/文件夹不存在正在跳过...\n");
            continue;
        }
        else
        {
            if(isfile)
                printf("\n文件处理中，请不要关闭程序...\n");
            else
                printf("\n文件夹处理中，请不要关闭程序...\n");
        }

        if(!isfile)
        {
            //文件夹处理加密
            DirLock(string[i],filename,workdir,password,usersel);
            fputs("Success Dir\t: >> ",Password);
            fputs(string[i],Password);
            fputs("\n",Password);
            if(del=='*')
            {
                sprintf(mind,"rmdir -s -q -y \"%s\"\n\0",string[i]);
                fputs(mind,Bat);
            }
        }
        else
        {
            //文件加密
            if(usersel=='1')
                success=FileLocker(string[i],password,"lock",workdir); //lock
            else
                success=FileLocker(string[i],password,"unlock",workdir); //unlock
            if(success==1)
            {
                fputs("Success File\t: >> ",Password);
                fputs(string[i],Password);
                fputs("\n",Password);
                if(access(workdir,F_OK)==0)
                {
                    char remov[2048]= {0};
                    sprintf(remov,"%s\\%s\0",workdir,filename);
                    remove(remov);
                }
                sprintf(rename,"ren \"%s\\LockFile.tmp\" \"%s\"\0\0",workdir,filename);
                system(rename);
                if(del=='*')
                {
                    sprintf(mind,"del -f -q \"%s\"\n\0",string[i]);
                    fputs(mind,Bat);
                }
            }
            else
            {
                if(success==0)
                    fputs("Error!Key is not true\t: >> ",Password);
                if(success==-1)
                    fputs("Error!File not lock\t: >> ",Password);
                fputs(string[i],Password);
                fputs("\n",Password);
                system("color e1");
                Sleep(30);
                system("color f5");
            }
        }
        //进度条模式选择
        if(cre=='+')
        {
            system("cls");
            printf("Processing:\n\n>/ %s\n\n\t%.2lf %%\n",string[i],(((double)(i+1)/(double)num)*100));
        }
        else
        {
            pre=(int)(((double)(i+1)/(double)num)*100);
            if(pre>100)
                pre=100;
            poi=pre/5;
            if(pre!=be)
            {
                system("cls");
                printf("\n处理进度：");
                printf("\n\t--------------------\n\t");
                for(int t=0; t<poi; t++)
                    printf(">");
                printf("\t%d %%\n",pre);
                printf("\t--------------------\n");
                be=pre;
            }
        }
    }
    //处理配置文件、脚本善后
    if(del=='*')
    {
        char tep[2048]= {0};
        sprintf(tep,"del -f -q \"%s01A_DeleteSourceFile.bat\"\n",workdir);
        fputs(tep,Bat);
        fclose(Bat);
    }
    fputs("\n\n",Password);
    fclose(Password);
}
int GetFileAttri(const char fileroute[],char filename[],char fatherroute[])
{
    char File[4096]= {'\0'};
    char name[400]= {'\0'};
    char faroute[4096]= {0};
    int i=1,j;

    strcpy(File,fileroute);
    int len,dsp;
    //统计路径长度
    len=0;
    while(File[len]!='\0')
        len++;
    //计算文件夹标识符位置
    dsp=0;
    while(File[len-dsp]!='\\' && len-dsp >=0)
    {
        dsp++;
    }
    dsp=len-dsp+1;
    //复制文件夹标识符以后的后缀进去name
    j=0;
    while(File[dsp+j]!='\0')
    {
        name[j]=File[dsp+j];
        j++;
    }
    name[j]='\0';
    name[++j]='\0';
    //复制父路径到faroute
    j=2;
    faroute[dsp+1]='\0';
    while(dsp-j>=0)
    {
        faroute[dsp-j]=File[dsp-j];
        j++;
    }
    strcpy(filename,name);
    strcpy(fatherroute,faroute);
    //检测是否是文件夹
    FILE * p;
    if((p=fopen(File,"r"))==NULL)
    {
        fclose(p);
        return 0;
    }
    else
        return 1;
}
void FromFileReadInfo()
{
    static char data[MAXSIZE][MAXSIZE]= {'\0'};
    static char * Data[MAXSIZE]= {NULL};
    for(int i=0; i<MAXSIZE; i++)    //vacant Data[0] instead of data[0].
    {
        Data[i+1]=data[i];          //build mapping
    }
    static int count=1;
    FILE * SF=fopen(FILEINFO,"r");
    if(SF)                          //if FILEINFO exist,read,else build it.
    {
        while(!feof(SF))
        {
            do
            {
                fgets(data[count-1],MAXSIZE-1,SF);  //if route first is # will be trash
                if(strlen(data[count-1])<2)
                {
                    printf("Not exist path or file not correct.\n\t");
                    system("pause");
                    return;
                }
            }
            while(data[count-1][0]=='#');
            int j=0;
            while(data[count-1][j])         //detail got \n and eof
            {
                if(data[count-1][j]=='\n'||data[count-1][j]==EOF)
                    data[count-1][j]='\0';
                j++;
            }
            if(data[count-1][0]=='\"')      //remove  donate ""
            {
                int k=0;
                while(data[count-1][k])
                {
                    data[count-1][k]=data[count-1][k+1];
                    k++;
                }
                data[count-1][k-2]='\0';
            }
            count++;
        }
        system("cls");
        printf("Load this File(display most 10 info):\n");
        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        for (int i=0; i<(count<10?count-1:10); i++)
            printf("File%3d:   %s\n",i+1,data[i]);
        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        system("pause");
        system("cls");
        ControlCenter(count,&Data[0]);
    }
    else
    {
        SF=fopen(FILEINFO,"w");
        printf("Not exist the file \"%s\",auto created for you\n\t",FILEINFO);
        system("pause");
        system("cls");
    }
    fclose(SF);
}
void FromConsoleReadInfo()
{
    static char data[MAXSIZE][MAXSIZE]= {'\0'};
    static char * Data[MAXSIZE]= {NULL};
    for(int i=0; i<MAXSIZE; i++)    //vacant Data[0] instead of data[0].
    {
        Data[i+1]=data[i];          //build mapping
    }
    static int count=1;

    printf("Please input file path,end of #:\n\n");
    while(data[count-2][0]!='#')        //get the path first is # end until
    {
        while(!strlen(data[count-1])||data[count-1][0]==' ')   //get the path when length is zero or first is vacant
        {
            printf("Path %d: >/ ",count);
            gets(data[count-1]);
        }
        count++;
    }
    count--;
    system("cls");
    printf("Load this File(display most 10 info):\n");
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    for (int i=0; i<(count<10?count-1:10); i++)
        printf("File%3d:   %s\n",i+1,data[i]);
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    system("pause");
    system("cls");
    ControlCenter(count,&Data[0]);
}
