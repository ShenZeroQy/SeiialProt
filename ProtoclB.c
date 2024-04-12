#include<stdio.h>
#include<string.h>
#include <sys/time.h>


//kms
http://wind4.github.io/vlmcsd/

//中国国家授时中心, ntp1.nim.ac.cn,111.203.6.13
//查询指令:  linux:ntpq -p
          //windows:  w32tm /resync
#define NTP_OFFSET_MS (+15)

#define FRAME_SIZE 128
#define DATA_SIZE 96

#define U_SIZE DATA_SIZE

#define FRAME_ESCAPE 0x7D
#define FRAME_START 0xAA
#define FRAME_END 0x55

#define CHANNEL_DATA 0x70
#define CHANNEL_COMMAND 0x07

#define ESCAPE_TO(X) (0x0F&X|0x80)


unsigned char Send_data_Buffer[DATA_SIZE];
unsigned char Rec_data_Buffer[DATA_SIZE];
unsigned char Send_frame_Buffer[FRAME_SIZE];
unsigned char Rec_frame_Buffer[FRAME_SIZE];



//int 32bit精度,long 64bit,double 64bit

typedef struct 
{
  unsigned long Timestamp;
  unsigned char Channel;
  unsigned char Empty[3];
  int Posx;
  double Angx;
} DataFrame;

typedef struct 
{
  unsigned long Timestamp;
  unsigned char Channel;
  unsigned char Empty[3];
  int Delay;
} CommandFrame;

typedef union
{
  CommandFrame CF;
  DataFrame DF;
  unsigned char byte[U_SIZE];
  /* data */
} Frame;



//Print
void PrintMsg(char* data , int len ,char* fname)
{

    FILE *file;
    // 打开文件
    //\clear
    file = fopen(fname, "a");
    if (file == NULL) {
        printf("无法打开文件\n");
        return ;
    }
    // 写入数据到文件
    int i;
    unsigned char c;
    // printf('\n%s:',msg);
    for (i=0;i<len;i++)
    {  
        c=data[i];
        fprintf(file,"%2X;",c);
    }
    fprintf(file, "\n");
    // 关闭文件
    fclose(file);
    
    return;
}
// 转义字符定义


// encode_frame使用预定义frame数组
int encode_frame(unsigned char* data, int data_len,unsigned char* frame) {
  unsigned char checksum = 0;
  for (int i = 0; i < data_len; i++) {
    checksum ^= data[i];
  }
  frame[0] = FRAME_START;
  int j=2;
  for (int i = 0; i < data_len; i++) {
    if (data[i] == FRAME_ESCAPE) {
      frame[j++] = FRAME_ESCAPE;
      frame[j++] = ESCAPE_TO(FRAME_ESCAPE); 
    } else if (data[i] == FRAME_START) {
       frame[j++] = FRAME_ESCAPE;
       frame[j++] = ESCAPE_TO(FRAME_START);
    } else if (data[i] == FRAME_END) {
       frame[j++] = FRAME_ESCAPE;
       frame[j++] = ESCAPE_TO(FRAME_END);
    } else {        
       frame[j++] = data[i];
      }
    }
  frame[1] = j-2;
  // frame[1] = data_len;
  frame[j++] = checksum;
  frame[j++] = FRAME_END;
  int framelen= j;
  return framelen;
}

// decode_frame使用预定义data数组,自动识别帧长,自动转义恢复
int decode_frame(unsigned char* frame, unsigned char* data) 
{
  while(frame[0]!=FRAME_START)
  {
    frame++;
  }
  int frame_len=frame[1];

  if(frame[frame_len+3]!=FRAME_END)// end erroe
    return 0;
  int j=0;
  //decode de escape
  for (int i = 2; i < frame_len+2; i++) 
  {
    if(frame[i]==FRAME_ESCAPE)
        {
            i++;
            if(frame[i]==ESCAPE_TO(FRAME_ESCAPE))
            {
                data[j++]=FRAME_ESCAPE;
            }
            else {if(frame[i]==ESCAPE_TO(FRAME_START))
            {
                data[j++]=FRAME_START;
            }
            else {if(frame[i]==ESCAPE_TO(FRAME_END))
            {
                data[j++]=FRAME_END;
            }
            else
            {
              //illegal escape
                // data[j++]=0xFF;
                return -1;
            }}}
        }
        else
        {
            data[j++]=frame[i];
        }

  }
  int data_len=j;
  //check sum
  unsigned char checksum = 0;
  for( j=0;j<data_len;j++)
  {
    checksum ^= data[j];
  }

  if (checksum != frame[frame_len+2]) {
    //check sum error
    return -2;
  }

  return data_len;
}





int data_frame(unsigned long time, int PosX,double AngX,unsigned char* send_frame_ptr)
{
      //小端存储

    Frame F;
    F.DF.Timestamp=time;
    F.DF.Channel=CHANNEL_DATA;
    F.DF.Posx=PosX;
    F.DF.Angx=AngX;
    int dlen=sizeof(DataFrame);
    for (int i=0;i<sizeof(DataFrame);i++)
      Send_data_Buffer[i]=F.byte[i];
    int flen=encode_frame(Send_data_Buffer, dlen,Send_frame_Buffer);
    if(send_frame_ptr!=NULL)//Copy out
      for (int i=0;i<flen;i++)
        send_frame_ptr[i]=Send_frame_Buffer[i];
    return flen;

}

int Read_data_frame(unsigned long* time,int* PosX,double* AngX,unsigned char* rec_frame_ptr)
{
    int j;
    if(rec_frame_ptr!=NULL)//Copy read in .from start to end
    {
        j=0;
        while(rec_frame_ptr[j]!=FRAME_START)
          j++;
        int i=0;
        while(rec_frame_ptr[j]!=FRAME_END)
         {
          Rec_frame_Buffer[i++]=rec_frame_ptr[j++];
         }
         Rec_frame_Buffer[i++]=FRAME_END;
         j++;
    }
    else
    {
        j=1;
    }

  ////////////////////////////////////////////////////
    int dlen=decode_frame(Rec_frame_Buffer,Rec_data_Buffer);
    // PrintMsg(Rec_data_Buffer,dlen,'debug.txt');
    unsigned long Rtime=0;
    int RPosX=0;
    double RAngX=0;
    if(dlen<=0)//解码错误
      return-1;
    if(dlen%4!=0)//没有对齐
      return -2;
    Frame F;
    for (int i=0;i<dlen;i++)
      F.byte[i]=Rec_data_Buffer[i];
    // check channel
    if(F.DF.Channel!=CHANNEL_DATA)// 信道错误
      return -3;
    
    *time=F.DF.Timestamp;
    *PosX=F.DF.Posx;
    *AngX=F.DF.Angx;
    return j; //Success

}

void local_send(void)
{
  for (int i=0;i<FRAME_SIZE;i++)
    Rec_frame_Buffer[i]=Send_frame_Buffer[i];
  return;
}
void judge_ending(void)
{
  unsigned long value = 1;  // 一个整数值，最低有效字节非零
    unsigned char *ptr = (unsigned char *)&value;

    if (*ptr == 1) {
        printf("Your computer is little-endian.\n");
    } else {
        printf("Your computer is big-endian.\n");
    }
}

// 获取当前毫秒时间的函数
unsigned long getMilliseconds_64(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL); // 获取当前时间
    unsigned long long milliseconds = (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
    unsigned long Ms64=(milliseconds+NTP_OFFSET_MS)&0xFFFFFFFF;
    return Ms64; // 返回毫秒时间
}



int main(){


  //timer

   unsigned long currentTimeMillis = getMilliseconds_64();\
  //  unsigned long ModcurrentTime32=currentTimeMillis &0xFFFFFFFF;
    printf("当前毫秒时间：%llu\n", currentTimeMillis);
    judge_ending();
    FILE *file;
    // 打开文件
    //\clear
    file = fopen("debug.txt", "w");
    fclose(file);
    file = fopen("debug.txt", "a");
    if (file == NULL) {
        printf("无法打开文件\n");
        return 1;
    }
    // 写入数据到文件
    fprintf(file, "Hello, World!\n");
    // 关闭文件
    fclose(file);

    unsigned char sent[]={0xaa,0xbb,0xb1,0x11,0x55,0x7D,0x7D,0xAA};
    PrintMsg(sent,8,"debug.txt");




////////////////////////////////////////////////透明传输数据
    unsigned long Pt=25125;
    int Pi=-5;
    double Pf=-0.56;
    unsigned long Rt;
    int Ri;
    double Rf;

    unsigned char S[60];
    unsigned char* s=S;
    int sl=data_frame(Pt,Pi,Pf,s);

    PrintMsg(Send_frame_Buffer,sl,"debug.txt");
    // local_send();//本地回环
    int Res=Read_data_frame(&Rt,&Ri,&Rf,s);
    PrintMsg(Rec_data_Buffer,32,"debug.txt");
/////////////////////////////////////////////////////
    return 0;
}







