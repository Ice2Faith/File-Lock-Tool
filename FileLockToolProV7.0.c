//FileLocker
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#include<conio.h>
#include<string.h>
#define MAXSIZE 1024
#define TRUE 1
#define FALSE 0
#define WORD char
#define FILEINFO "LoadFile.txt"
//对文件名“FileName”用“password”进行“mode”操作，mode为lock时进行加密，mode为unlock时进行解密,返回值1成功，0解密密码不正确，-1文件未加密不能执行解密
int FileLocker(char * FileName,char * password,char * mode);
//选择菜单
char SelectMain();
//控制分管中心
void ControlCenter(int num,char * string[]);
//返回当前时间信息，mode值0日期，1时间，2星期
int PresentDATE(int mode);
//快速设置程序窗口颜色标题，宽度，高度
void FaceInt(char * color,char * title,int cols,int lines);
//用户帮助或引导说明
void UserGuid();
//载入文件过多提示
void AceptMore();
//内部秘钥加密解密文本
void InLock(char * string);
//从文件读取信息
void FromFileReadInfo();

int main(int num,char * string[])
{
    FaceInt("f5","FileLockToolProV7.0 Dev: Ice2Faith Ver: 7.0",85,25);
    if(num==1)
    {
        UserGuid();
    }
    else if(num>507)
    {
        AceptMore();
        printf("如果需要继续加密/解密506个，请输入 \'+\' ，否则不操作：\n>/  ");
        char con=getch();
        if(con=='+')
            ControlCenter(507,&string[0]);
    }
    else
        ControlCenter(num,&string[0]);

    return 0;
}
void InLock(char * string)
{
    char InKey[MAXSIZE]= {"Dev:Ice2Faith-20181122\0"};

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
int FileLocker(char * FileName,char * password,char * mode)
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
        Lock=fopen("FileLockToolPro\\LockFile.tmp","wb");
    }
    else if(strcmp("lock",mode)==0)
    {
        char contact='_';
        //unlock file to lock
        //reopen file
        fclose(Soure);
        Soure=NULL;
        Soure=fopen(FileName,"rb");
        Lock=fopen("FileLockToolPro\\LockFile.tmp","wb");
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
    printf("\n\t\tFileLockToolProV7.0\n\n");
    printf("\n\tVer: 7.0\n\n");
    printf("\n\tDev: Ice2Faith\n\n");
    printf("\n\n\tClick \"1\" can load file info from \"%s\",else jump to help:\n\n\t>/ ",FILEINFO);
    if(getch()=='1')
    {
        FromFileReadInfo();
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
        printf("\n\tTips:\t请不要修改本程序名称，以免发生错误\n\t成功操作会将文件等信息保存至 \"FileLockToolPro\\01A_FileLockToolInfo.txt\"\n");
        printf("\t\t成功操作后的文件将会保存至工作目录 \"FileLockToolPro\\\"\n");
        printf("\t\t双击运行脚本即可删除源文件：\"01A_DeleteSourceFile.bat\"\n");
        printf("\t特别注意：\n");
        printf("\t\t如果工作目录下存在同名文件，那么旧文件将会被覆盖！！\n");
        printf("\n\tName:\tFileLockToolProV7.0\n\n\tDev:\t Ice2Faith\n");
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
    printf("\n\tName:\tFileLockToolProV7.0\n\n\tDev:\t Ice2Faith\n\n");
}
void FaceInt(char * color,char * title,int cols,int lines)
{
    char mind[100];
    sprintf(mind,"color %s\0",color);
    system(mind);
    sprintf(mind,"title %s\0",title);
    system(mind);
    sprintf(mind,"mode con cols=%d lines=%d\0",cols,lines);
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
void ControlCenter(int num,char * string[])
{
    int success=1;
    char usersel=SelectMain();
    if(usersel=='0')
        return;
    system("md \"FileLockToolPro\\\"");
    char File[4096]= {'\0'};
    char mind[2048]= {'\0'};
    char name[400]= {'\0'};
    char password[400]= {'\0'};
    char rename[4096];
    char title[20]= {"Ice2Faith\0"};
    char del='2';
    FILE * Password;
    Password=fopen("FileLockToolPro\\01A_FileLockToolInfo.txt","a");
    system("cls");
    do
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
    FILE * Bat;
    if(del=='*')
    {
        Bat=fopen("01A_DeleteSourceFile.bat","w");
        fputs("@echo off\n",Bat);
        fputs("echo Builder: FileLockToolProV7.0\n",Bat);
        fputs("echo Ver: 7.0\n",Bat);
        fputs("echo Dev: Ice2Faith\n",Bat);
        fputs("title DeleteSourceFile\n",Bat);
        fputs("color f5\n",Bat);
        fputs("echo Dev: Ice2Faith\n",Bat);
    }
    fputs(">>>>>>>>>>>>\tIce2Faith FileLockToolProV7.0 informations\n\n",Password);
    fprintf(Password,"Ver: \t\t7.0\n\n",Password);
    fprintf(Password,"Date: \t\t%d:%d\n\n",PresentDATE(0),PresentDATE(1));
    fputs("Password:\t",Password);
    fputs(password,Password);
    fputs("\n\n",Password);
    fputs("Operation log:\n\n",Password);
    strcat(password,title);
    printf("输入 \'+\' 启用详细进度，否则默认");
    char cre=getch();
    system("cls");
    printf("加密/解密中，请稍后..................\n");
    printf("环境初始化...........................\n");
    printf("建立文件夹: FileLockToolPro\\..........\n");
    if(del=='*')
        printf("创建删除脚本..........\n");
    if(cre=='+')
    {
        system("mode con lines=10 cols=80");
        printf("\n\n处理进度：\n");
    }
    else
        system("mode con lines=7 cols=40");
    int pre=0,be=0,poi=0;
    for(int i=1,j; i<num; i++)
    {
        //清空数组内容
        for(int b=0; b<400; b++)
        {
            if(name[b]!='\0')
                name[b]='\0';
        }
        strcpy(File,string[i]);
        int len,dsp;
        //统计路径长度
        len=0;
        while(File[len]!='\0')
            len++;
        //计算文件夹标识符位置
        dsp=0;
        while(File[len-dsp]!='\\')
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
        if(strcmp(name,"01A_FileLockToolInfo.txt")==0)
        {
            if(del=='*')
            {
                sprintf(mind,"del -f -q \"%s\"\n\0",File);
                fputs(mind,Bat);
            }
            continue;
        }
        //检测是否是文件夹
        if(fopen(File,"r")==NULL)
        {
            continue;
        }
        //开始进行加密操作
        //删除旧文件
        sprintf(mind,"del -f -q \"FileLockToolPro\\%s\"\0",name);
        system(mind);
        //进度条模式选择
        if(cre=='+')
        {
            system("cls");
            printf("Processing:\n\n>/ %s\n\n\t%.2lf %%\n",File,(((double)i/(double)num)*100));
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
        if(usersel=='1')
            success=FileLocker(File,password,"lock"); //lock
        else
            success=FileLocker(File,password,"unlock"); //unlock
        if(success==1)
        {
            fputs("Success\t: >> ",Password);
            fputs(File,Password);
            fputs("\n",Password);
            sprintf(rename,"ren \"FileLockToolPro\\LockFile.tmp\" \"%s\"\0\0",name);
            system(rename);
            if(del=='*')
            {
                sprintf(mind,"del -f -q \"%s\"\n\0",File);
                fputs(mind,Bat);
            }
        }
        else
        {
            if(success==0)
                fputs("Error!Key is not true\t: >> ",Password);
            if(success==-1)
                fputs("Error!File not lock\t: >> ",Password);
            fputs(File,Password);
            fputs("\n",Password);
            system("color e1");
            Sleep(30);
            system("color f5");
        }
    }
    if(del=='*')
    {
        fputs("del -f -q 01A_DeleteSourceFile.bat\n",Bat);
        fclose(Bat);
    }
    fputs("\n\n",Password);
    fclose(Password);
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
