#include <stdio.h>
#include <string.h>
#include "header.h"
typedef float_type word;

word __atof(char* s)
{
    char u[80]="";
    char v[80]="";

    word m = 0;//尾数
    word e = 0x7F;//阶码


    int i,n,c,t,p,flag=0;
    n=t=p=0;

    i=(s[0]=='-')?1:0;
    while(s[i]&&s[i]!='.'){
      u[t]=s[i]-'0';
      t++;
      i++;
    }
   // printf("%d\n",t);
    while(1){
        c=0;
        if(u[p]==0){
            if(p==t-1)
                break;
            else p++;
        }
        for(i=0; i<=t-1; i++){
            u[i]=u[i]+c*10;
            c=u[i]&1;
            u[i]>>=1;
        }
 //       printf("c=%d\n",c);
        m=(m>>1)|(c<<31);
 //       printf("m=%X\n",m);
        e++;
    }
    e--;
//    printf("\n%X\n",m);
    if(e<=23+0x7F)
        m>>=31-(e-0x7F);
    else m>>=8;
//    printf("\n%X\n",m);
//    printf("%X\n",e);
    if(s[i]=='.'){
        i++;
        strcpy(v, s+i);
        n = strlen(v);
//        printf("%d\n",n);
    }
    i = 0;
//    printf("%s\n",v);
    while(v[i]){
        if(!flag&&v[i]!='0')
            flag=1;
        v[i]-='0';
        i++;
    }//把v转化为BCD码数组

    if(!m&&!flag) return 0;//0的情况

    while(!(m&1<<23)){
        c = 0;
        for(i=n-1; i>=0; i--){
            v[i]=(v[i]<<1)+c;
            if(v[i]>=10){
                v[i]-=10;
                c = 1;
            }
            else c = 0;
        }
        m = (m<<1)+c;
        if(e<0x7F&&!m)
            e--;
    }//模拟笔算的方法对小数部分乘2取整
//    printf("%X\n",e);
//    printf("%X\n",m);
    while(!(m&0x800000))m<<=1;
    m<<=9;
    m>>=9;
    m = m|((e&0xFF)<<23);
    if(s[0]=='-')
        m = m|0x80000000;
//    printf("%X\n",m);

    return m;
}

void __ftoa(word w, char* s)
{
    char a[80];
    char tmp,ch;
    word m, e;//阶码是加0x7F
    int i,j,k,l,c,len,llen;
    j=0;
    i=(w&0x80000000)?1:0;
    if(i)s[0]='-';
    m=(w&0x00FFFFFF)|0x00800000;
    e=(w>>23)&0xFF;
    for(l=0;l<80;l++)
        a[l]=0;
 //printf("%X %X %X\n",w,m,e);
    if(e==0xFF&&m==0xFFFFFF){
        if(i)s="-INF";
        else s="INF";
    }
    else if(w==0)s="0";
// 先把m当做24位2进制整数将其转化为BCD数组
    else{
        e-=23;//尾数都转化为整数部分
        do{
            a[j]=m%10;
            m/=10;
            j++;
        }while(m);
        llen=len=j;
 //       printf("%d\n",len);
        l=0;
        if(e<0x7F)
            j--;
        else j=79;//e为正，放到字符串最后
        for(;l<j;l++,j--)
            if(a[l]!=a[j]){
                tmp = a[l];
                a[l] = a[j];
                a[j] = tmp;
        }//将整数部分转化为码
        if(e>=0x7F){
            while(e>0x7F){
              c = 0;
                for(j=79; j>=0; j--){
                    a[j]=(a[j]<<1)+c;
                    if(a[j]>=10){
                        a[j]-=10;
                        c = 1;
                    }
                    else c = 0;
                }
              e--;
            }
            j=0;
            while(!a[j])j++;
            for(k=j;k<80;k++)
                a[k-1]=a[k]+'0';
                a[k]=0;
            strcpy(s+i,a+j-1);
       //     k=79-j;
        //    for(j=i;j<=k;j++)
         //       s[j]+='0';
        }
        else{
             ch=a[0];//标记
             while(e<0x7F){
                c=0;
                for(j=0; j<80; j++){
                    a[j]=a[j]+c*10;
                    c=a[j]&1;
                    a[j]>>=1;
                }
                e++;
            }

            j=0;
            while(!a[j])j++;
            if(a[j]<ch)len++;

            for(k=0;k<=j+len-1;k++)
                a[k]+='0';
            for(;k<79;k++)a[k]=0;
            k=79;
            while(k>llen){//小数点
                a[k]=a[k-1];
                k--;
            }
            a[llen]='.';
            k=0;
            while(a[k]=='0'&&a[k+1]!='.')k++;
            strcpy(s+i,a+k);
        }
    }
}
