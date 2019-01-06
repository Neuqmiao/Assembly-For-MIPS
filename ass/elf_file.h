/*
	���ļ���װ�˱�׼ELF�ļ���ʽ��
*/

#ifndef _ELF_FILE_H
#define	_ELF_FILE_H
#define _ << " " <<
//elf�ļ��������ݽṹ�ͺ궨��

#include "elf.h"
#include <map>
//STLģ���
#include <ext/hash_map>
#include <vector>
#include <iostream>
#include <cstdio>

using namespace std;
using namespace __gnu_cxx;


struct lb_record//����������¼
{
  static int curAddr;//һ�����ڷ��ŵ�ƫ���ۼ���
  string segName;//�����ڵĶ��������֣�.text .data .bss
  string lbName;//������
  bool isEqu;//�Ƿ���L equ 1
  bool externed;//�Ƿ����ⲿ���ţ�������1��ʱ���ʾΪ�ⲿ�ģ���ʱcurAddr���ۼ�
  int addr;//���Ŷ�ƫ��
  int times;//�����ظ�����
  int len;//�������ͳ��ȣ�db-1 dw-2 dd-4
  int *cont;//������������
  int cont_len;//�������ݳ���
  lb_record(string n,bool ex);//L:���ߴ����ⲿ����(ex=true:L dd @e_esp)
  lb_record(string n,int a);//L equ 1
  lb_record(string n,int t,int l,int c[],int c_l);//L times 5 dw 1,"abc",L2 ���� L dd 23
  void write();//�����������
  ~lb_record();
};


// ��Ҫ�Լ�дhash����
struct string_hash
{
  size_t operator()(const string& str) const
  {
    return __stl_hash_string(str.c_str());
  }
};

//�ض�λ��Ϣ
struct RelInfo
{
	string tarSeg;//�ض�λĿ���
	int offset;//�ض�λλ�õ�ƫ��
	string lbName;//�ض�λ���ŵ�����
	int type;//�ض�λ����0-R_386_32��1-R_386_PC32
	RelInfo(string seg,int addr,string lb,int t);
};


//.rel .text segment
struct RelText
{
    int offset;     //offset in text segment
    int info;       //calculate by ELF32_R_INFO(sym, type), which is defined in elf.h, in which sym is the index of the symbol, type is R_MIPS_REL32(also in elf.h).
    int type;       //define in elf.h, R_MIPS_HI16, R_MIPS_LO16, R_MIPS_CALL16
    int symValue;   //in our program, always 0
    string symName; //__gnu_local_gp, .rodata, puts, scanf, printf
};

struct InstCode{
	int op;//������ 26-31
	int r1;//�Ĵ���1 21-25
	int r2;//16-20
	int tail;//0-15
	void print(){
        //printf("%-4x",CurrAddr);
		//cout << "#####" _ op _ r1 _ r2 _ tail <<endl;
	}
  int cal(){
    return (op<<26) + (r1<<21) + (r2<<16) + tail;
  }
};
extern vector<InstCode> program;//text segment

//elf�ļ��࣬����elf�ļ�����Ҫ���ݣ�����elf�ļ�
class ELF_FILE
{
public:

	vector<char> rodata;//rodata segment
	map<string,int> rodata_offset;//offset in .rodata
    vector<Elf32_Rel> reltext;  // relocate text
    vector<Elf32_Sym> symtab; // symbol table
    vector<char> strtab;//strtab segment
    map<string,int> strtab_offset;
    vector<char> shstrtab;// section header str table
    vector<Elf32_Shdr> shdrtab;// section header table
    Elf32_RegInfo reginfo;// register information
    Elf32_Ehdr ehdr;// elf header


public:
    ELF_FILE(){};
    ~ELF_FILE(){};
    /**
     * @Author    Wang_Chuhan    Wang_Zitai
     * @DateTime  2019-01-06
     * @copyright [Copyright (c)2019]
     * ��ӡ������Ϣ
     */
    void print(){
      cout << "-------------------------rodata--------------------" << endl;
      for(int i = 0; i<rodata.size(); i++){
            if(rodata[i]=='\0')
                putchar('_');
            else
                putchar(rodata[i]);
      }
      puts("");
      cout << "-------------------------reltext--------------------" << endl;
      cout << "offset" << " " << "info" << endl;
      for (int i = 0; i < reltext.size(); ++i)
      {
          Elf32_Rel now_reltext = reltext[i];
          printf("%x\t%x\n", now_reltext.r_offset, now_reltext.r_info);
      }
      cout << "-------------------------symtab--------------------" << endl;
      cout << "Num\tValue\tSize\tInfo\tVis\tNdx\tName\n";
      for (int i = 0; i < symtab.size(); ++i)
      {
            Elf32_Sym sym = symtab[i];
            printf("%d\t%d\t%d\t%x\t%x\t%x\t%d\n", i, sym.st_value, sym.st_size, sym.st_info, sym.st_other, sym.st_shndx, sym.st_name);
      }
      puts("");
      cout << "-------------------------strtab--------------------" << endl;
      for (int i = 0; i < strtab.size(); ++i)
      {
            if(strtab[i]=='\0')
                putchar('_');
            else
                putchar(strtab[i]);
      }
      puts("");

      cout << "------------------------Section Header---------------"<< endl;
      cout << "Nr\tName\tType\tAddr\toff\tSize\tES\tFlg\tLk\tInf  \tAl\n";
      for(int i = 0; i< shdrtab.size(); i++)
      {
          Elf32_Shdr t = shdrtab[i];
          printf("%d\t%d\t%x\t%x\t%x\t%x\t%x\t%x\t%d\t%d  \t%d\n",
                i, t.sh_name, t.sh_type, t.sh_addr, t.sh_offset,
                t.sh_size, t.sh_entsize, t.sh_flags,t.sh_link, t.sh_info, t.sh_addralign
            );
      }
      cout << "----------------Section Header String Table---------------"<< endl;
      for (int i = 0; i < shstrtab.size(); ++i)
      {
            if(shstrtab[i]=='\0')
                putchar('_');
            else
                putchar(shstrtab[i]);
      }
      puts("");
      cout << "-------------RegInfo----------------------" << endl;
      printf("%x ",reginfo.ri_gprmask);
      //cout << reginfo.ri_gprmask _ " ";
      for(int i = 0; i<4 ; i++)
        cout << reginfo.ri_cprmask[4] _ " ";
      cout << reginfo.ri_gp_value << endl;

      cout << "--------------program---------------------" << endl;
      cout << program.size() << endl;
    }


};


#endif //elf_file.h

