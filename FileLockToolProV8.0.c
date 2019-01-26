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
//���ļ�����FileName���á�password�����С�mode��������modeΪlockʱ���м��ܣ�modeΪunlockʱ���н���,����ֵ1�ɹ���0�������벻��ȷ��-1�ļ�δ���ܲ���ִ�н���
int FileLocker(char * FileName,char * password,char * mode,const char preworkdir[]);
//ѡ��˵�
char SelectMain();
//���Ʒֹ�����
void ControlCenter(int num,char * string[]);
//���ص�ǰʱ����Ϣ��modeֵ0���ڣ�1ʱ�䣬2����
int PresentDATE(int mode);
//�������ó��򴰿���ɫ���⣬��ȣ��߶�
void FaceInt(char * color,char * title,int cols);
//�û�����������˵��
void UserGuid();
//�����ļ�������ʾ
void AceptMore();
//�ڲ���Կ���ܽ����ı�
void InLock(char * string);
//���ļ���ȡ��Ϣ
void FromFileReadInfo();
//�ӿ���̨��ȡ��Ϣ
void FromConsoleReadInfo();
//�ļ��м��ܣ�������ȫ·��dirroute���ļ�������dirname����ǰ����·��preworkdir������password���û�ѡ��usersel
void DirLock(const char dirroute[],const char dirname[],const char preworkdir[],const char password[],const char usersel);
//����ļ��ص����ƺ͸�Ŀ¼·���������ļ�·��fileroute�������ļ�����filename�������ļ�Ŀ¼��fatherroute
int GetFileAttri(const char fileroute[],char filename[],char fatherroute[]);
//���价�����ã����뱣���û�·��������UserRoute
void PathEnvinoment(char UserRoute[]);

int main(int num,char * string[])
{
    FaceInt("f5","FileLockToolProV8.0 Dev: Ice2Faith Ver: 8.0",85);
    char UserRoute[40]; //������ʼ�����й����������飬ֻ��֤����û����
    PathEnvinoment(UserRoute);  //��ʼ������
    if(num==1)  //δ�����ļ�������������
    {
        UserGuid();
    }
    else if(num>507)    //��������ļ���Ŀ������ִ��
    {
        AceptMore();
        printf("�����Ҫ��������/����506���������� \'+\' �����򲻲�����\n>/  ");
        char con=getch();
        if(con=='+')
            ControlCenter(507,&string[0]);
    }
    else    //������д���
        ControlCenter(num,&string[0]);
    fflush(stdin);
    printf("����ɣ������������\n"); //���������ʾ
    getch();
    return 0;
}
void InLock(char * string)
{
    char InKey[MAXSIZE]= {"Dev:Ice2Faith-20181122\0"};  //�ڲ�������Կ�����ø��ģ������ܼ�����ǰ�汾

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
        printf("\t1.�ļ�����\n\n");
        printf("\t2.�ļ�����\n\n");
        printf("\t0.�˳�����\n\n");
        printf("----------------------------------\n\n");
        printf("��ѡ��\n\n>/ ");
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
        printf("\n\tʹ��������˵��:\n\n");
        printf("\t�뽫������/�����ļ��϶�������ͼ��������\t( ^_^)��\n");
        printf("\t������Ҳ���԰Ѵ������ļ�·����Ϣ�ŵ��ļ���: \"%s\"\n",FILEINFO);
        printf("\tע�⣬·������˫���Ŷ��У����ǲ�Ҫ������Ķ�����\n");
        printf("\t���ң����׵�һ�������#�Ž��ᱻ������������\n");
        printf("\t֧�����·���;���·��Ŷ��\n");
        printf("\n\tTips:\t�ɹ������Ὣ�ļ�����Ϣ������ \"C:\\FileLockToolPro\\01A_FileLockToolInfo.txt\"\n");
        printf("\t\t�ɹ���������ļ����ᱣ��������Ŀ¼ \"C:\\FileLockToolPro\\\"\n");
        printf("\t\t˫�����нű�����ɾ��Դ�ļ���\"01A_DeleteSourceFile.bat\"\n");
        printf("\t�ر�ע�⣺\n");
        printf("\t\t�������Ŀ¼�´���ͬ���ļ�����ô���ļ����ᱻ���ǣ���\n");
        printf("\n\tName:\tFileLockToolProV8.0\n\n\tDev:\t Ice2Faith\n");
        printf("\n\t%d��󽫻�رճ���\t\t�d(������)Bye~Bye~\n\n",5-i);
        Sleep(1000);
    }
}
void AceptMore()
{
    system("color cf");
    Sleep(30);
    system("color f5");
    printf("\n\t���˸����\t\t<(ToT)>\n\n\t�ļ��������� 506 ��\t((((��\"A\"))))\n\n");
    printf("\n\t������Ϊʲô�� 506 ��\t\t������.����\n\n\t��֪���Ҿ�����ʲô\t(TwT') \n\n");
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
    sprintf(workdir,"%s\\%s",preworkdir,dirname);   //�����µĹ���·��
    if(access(workdir,F_OK)!=0) //����ļ��в������ٴ���
    {
        sprintf(mind,"md \"%s\"\0",workdir);
        system(mind);
    }
    struct _finddata_t predir;  //�����ļ�����
    long handle;    //���
    char pdirname[2048]= {0};
    strcpy(pdirname,dirroute);
    strcat(pdirname,"\\");  //���ٵ�ǰ�ļ��У������������
    char tmpname[2048]= {0};
    strcpy(tmpname,pdirname);
    if((handle=_findfirst(strcat(tmpname,"*"),&predir))==-1L)
        return; //vacant dir
    char filename[4096]= {0};
    char fatherroute[4096]= {0};    //���游·�����ļ������������ű�
    char rename[4096];
    do
    {
        //������ʾ�����أ�ֻ������Ŀ¼��ϵͳ�ļ�������ȡ��Ŀ¼�͸�Ŀ¼
        if((predir.attrib==_A_ARCH||_A_HIDDEN||_A_RDONLY||_A_SUBDIR||_A_SYSTEM)&&(strcmp(predir.name,".")!=0)&&(strcmp(predir.name,"..")!=0))
        {

            strcpy(tmpname,pdirname);
            strcat(tmpname,predir.name);
            FILE * pf=fopen(tmpname,"r");
            GetFileAttri(tmpname,filename,fatherroute);
            if(!pf) //������ļ�����ݹ����
            {
                fclose(pf);
                DirLock(tmpname,filename,workdir,password,usersel);
            }
            else    //���ļ���ִ�в���
            {
                fclose(pf);
                int success=0;
                if(usersel=='1')
                    success=FileLocker(tmpname,password,"lock",workdir); //lock
                else
                    success=FileLocker(tmpname,password,"unlock",workdir); //unlock
                if(success==1)
                {
                    if(access(workdir,F_OK)==0) //�������·�����ļ��Ѿ���������ɾ��
                    {
                        char remov[2048]= {0};
                        sprintf(remov,"%s\\%s\0",workdir,filename);
                        remove(remov);
                    }
                    sprintf(rename,"ren \"%s\\LockFile.tmp\" \"%s\"\0\0",workdir,filename); //��������ʱ�ļ�
                    system(rename);
                }
                else
                {
                    //����ʧ����ʾ
                    system("color e1");
                    Sleep(30);
                    system("color f5");
                }
            }

        }
    }
    while(_findnext(handle,&predir)==0);    //ֱ�����������˳�
    _findclose(handle); //�ر��ļ���������
}
void PathEnvinoment(char UserRoute[])
{
    char pathinfo[10][2048]=
    {
        {"FileLockTool Path\n"},
        {"C:FileLockToolPro\n"},    //����Ĭ�Ϲ���·�������ⷢ������
        {"You can modify the path to second line.\n"},
        {"Form here down you can free modify it.\n"},
        {"-----------------------------------------\n"},
    };
    FILE * Path=fopen("C:\\FileLockToolPro\\UserPath\\Path.xml","r");   //д�������ļ�������������������������ȡ
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
    strcpy(UserRoute,pathinfo[1]);  //���صõ�������·��
    fclose(Path);
}
void ControlCenter(int num,char * string[])
{
    char UserRoute[2048]= {"C:FileLockToolPro"};
    PathEnvinoment(UserRoute);  //��ȡ����·��
    char usersel=SelectMain();  //��ȡ�û�ѡ��
    if(usersel=='0')    //ѡ���˳����˳�
        return;
    char workdir[2048]= {0};
    if(usersel=='1')    //ѡ��ͬ������ͬ�Ĺ���·��
    {
        sprintf(workdir,"%s\\MyLock\\\0",UserRoute);
    }
    if(usersel=='2')
    {
        sprintf(workdir,"%s\\MyUnlock\\\0",UserRoute);
    }
    char mind[2048]= {'\0'};
    if(access(workdir,F_OK)!=0) //��鹤��·���Ƿ���ڣ�����������
    {
        sprintf(mind,"md \"%s\"\0",workdir);
        system(mind);
    }
    int isfile=0;   //����Ƿ����ļ�
    char filename[4096]= {0};
    char fatherroute[4096]= {0};

    int success=1;

    char password[400]= {'\0'};
    char rename[4096];
    char title[20]= {"Ice2Faith\0"};
    char del='2';   //ɾ���ű���Ҫ��־
    FILE * Password;    //��Ϣ�ű��������
    char routeinfo[2048]= {0};
    sprintf(routeinfo,"%s01A_FileLockToolInfo.txt\0",workdir);
    Password=fopen(routeinfo,"a");
    system("cls");
    do  //ֱ����ȡ���Ϸ�������
    {
        system("cls");
        printf("Please input your key:\n>/ ");
        gets(password);
        if(!strlen(password))
        {
            printf("��ò��û����������Ŷ�������ԣ�\t\t(-_-) zzz\n");
            Sleep(800);
        }
    }
    while(!strlen(password));
    printf("���� \'*\' ����ɾ��Դ�ļ��ű���01A_DeleteSourceFile.bat����������\n>/ ");
    del=getch();
    FILE * Bat;     //����ɾ���ű�
    if(del=='*')
    {
        //�����Ҫ����ʾ�Ͱ汾��Ϣ
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
    }   //�����Ҫ��Ϣ�ļ�˵��
    fputs(">>>>>>>>>>>>\tIce2Faith FileLockToolProV8.0 informations\n\n",Password);
    fprintf(Password,"Ver: \t\t8.0\n\n",Password);
    fprintf(Password,"Date: \t\t%d:%d\n\n",PresentDATE(0),PresentDATE(1));
    fputs("Password:\t",Password);
    fputs(password,Password);
    fputs("\n\n",Password);
    fputs("Operation log:\n\n",Password);
    strcat(password,title); //ǿ������
    printf("���� \'+\' ������ϸ���ȣ�����Ĭ��");
    char cre=getch();   //����չʾģʽ���
    system("cls");
    printf("����/�����У����Ժ�..................\n");
    printf("������ʼ��...........................\n");
    printf("�����ļ���: %sFileLockToolPro\\..........\n",workdir);
    if(del=='*')
        printf("����ɾ���ű�..........\n");
    if(cre=='+')
    {
        system("mode con lines=10 cols=80");
        printf("\n\n������ȣ�\n");
    }
    else
        system("mode con lines=7 cols=40");
    int pre=0,be=0,poi=0;   //���Ȱٷֱȿ��Ʊ���
    for(int i=1; i<num; i++)
    {
        isfile=GetFileAttri(string[i],filename,fatherroute);
        //�ж��ļ�·�����ʣ���ʾ�û�
        if(access(workdir,F_OK)!=0)
        {
            printf("\n�ļ�/�ļ��в�������������...\n");
            continue;
        }
        else
        {
            if(isfile)
                printf("\n�ļ������У��벻Ҫ�رճ���...\n");
            else
                printf("\n�ļ��д����У��벻Ҫ�رճ���...\n");
        }

        if(!isfile)
        {
            //�ļ��д������
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
            //�ļ�����
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
        //������ģʽѡ��
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
                printf("\n������ȣ�");
                printf("\n\t--------------------\n\t");
                for(int t=0; t<poi; t++)
                    printf(">");
                printf("\t%d %%\n",pre);
                printf("\t--------------------\n");
                be=pre;
            }
        }
    }
    //���������ļ����ű��ƺ�
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
    //ͳ��·������
    len=0;
    while(File[len]!='\0')
        len++;
    //�����ļ��б�ʶ��λ��
    dsp=0;
    while(File[len-dsp]!='\\' && len-dsp >=0)
    {
        dsp++;
    }
    dsp=len-dsp+1;
    //�����ļ��б�ʶ���Ժ�ĺ�׺��ȥname
    j=0;
    while(File[dsp+j]!='\0')
    {
        name[j]=File[dsp+j];
        j++;
    }
    name[j]='\0';
    name[++j]='\0';
    //���Ƹ�·����faroute
    j=2;
    faroute[dsp+1]='\0';
    while(dsp-j>=0)
    {
        faroute[dsp-j]=File[dsp-j];
        j++;
    }
    strcpy(filename,name);
    strcpy(fatherroute,faroute);
    //����Ƿ����ļ���
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
