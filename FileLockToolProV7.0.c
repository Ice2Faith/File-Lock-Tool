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
//���ļ�����FileName���á�password�����С�mode��������modeΪlockʱ���м��ܣ�modeΪunlockʱ���н���,����ֵ1�ɹ���0�������벻��ȷ��-1�ļ�δ���ܲ���ִ�н���
int FileLocker(char * FileName,char * password,char * mode);
//ѡ��˵�
char SelectMain();
//���Ʒֹ�����
void ControlCenter(int num,char * string[]);
//���ص�ǰʱ����Ϣ��modeֵ0���ڣ�1ʱ�䣬2����
int PresentDATE(int mode);
//�������ó��򴰿���ɫ���⣬��ȣ��߶�
void FaceInt(char * color,char * title,int cols,int lines);
//�û�����������˵��
void UserGuid();
//�����ļ�������ʾ
void AceptMore();
//�ڲ���Կ���ܽ����ı�
void InLock(char * string);
//���ļ���ȡ��Ϣ
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
        printf("�����Ҫ��������/����506���������� \'+\' �����򲻲�����\n>/  ");
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
        printf("\n\tʹ��������˵��:\n\n");
        printf("\t�뽫������/�����ļ��϶�������ͼ��������\t( ^_^)��\n");
        printf("\t������Ҳ���԰Ѵ������ļ�·����Ϣ�ŵ��ļ���: \"%s\"\n",FILEINFO);
        printf("\tע�⣬·������˫���Ŷ��У����ǲ�Ҫ������Ķ�����\n");
        printf("\t���ң����׵�һ�������#�Ž��ᱻ������������\n");
        printf("\n\tTips:\t�벻Ҫ�޸ı��������ƣ����ⷢ������\n\t�ɹ������Ὣ�ļ�����Ϣ������ \"FileLockToolPro\\01A_FileLockToolInfo.txt\"\n");
        printf("\t\t�ɹ���������ļ����ᱣ��������Ŀ¼ \"FileLockToolPro\\\"\n");
        printf("\t\t˫�����нű�����ɾ��Դ�ļ���\"01A_DeleteSourceFile.bat\"\n");
        printf("\t�ر�ע�⣺\n");
        printf("\t\t�������Ŀ¼�´���ͬ���ļ�����ô���ļ����ᱻ���ǣ���\n");
        printf("\n\tName:\tFileLockToolProV7.0\n\n\tDev:\t Ice2Faith\n");
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
            printf("��ò��û����������Ŷ�������ԣ�\t\t(-_-) zzz\n");
            Sleep(800);
        }
    }
    while(!strlen(password));
    printf("���� \'*\' ����ɾ��Դ�ļ��ű���01A_DeleteSourceFile.bat����������\n>/ ");
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
    printf("���� \'+\' ������ϸ���ȣ�����Ĭ��");
    char cre=getch();
    system("cls");
    printf("����/�����У����Ժ�..................\n");
    printf("������ʼ��...........................\n");
    printf("�����ļ���: FileLockToolPro\\..........\n");
    if(del=='*')
        printf("����ɾ���ű�..........\n");
    if(cre=='+')
    {
        system("mode con lines=10 cols=80");
        printf("\n\n������ȣ�\n");
    }
    else
        system("mode con lines=7 cols=40");
    int pre=0,be=0,poi=0;
    for(int i=1,j; i<num; i++)
    {
        //�����������
        for(int b=0; b<400; b++)
        {
            if(name[b]!='\0')
                name[b]='\0';
        }
        strcpy(File,string[i]);
        int len,dsp;
        //ͳ��·������
        len=0;
        while(File[len]!='\0')
            len++;
        //�����ļ��б�ʶ��λ��
        dsp=0;
        while(File[len-dsp]!='\\')
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
        if(strcmp(name,"01A_FileLockToolInfo.txt")==0)
        {
            if(del=='*')
            {
                sprintf(mind,"del -f -q \"%s\"\n\0",File);
                fputs(mind,Bat);
            }
            continue;
        }
        //����Ƿ����ļ���
        if(fopen(File,"r")==NULL)
        {
            continue;
        }
        //��ʼ���м��ܲ���
        //ɾ�����ļ�
        sprintf(mind,"del -f -q \"FileLockToolPro\\%s\"\0",name);
        system(mind);
        //������ģʽѡ��
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
                printf("\n������ȣ�");
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
