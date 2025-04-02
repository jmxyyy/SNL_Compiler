/***** �ô����ļ�������ͷ�ļ� *****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/* �궨�峣��TRUEΪ1 */
#ifndef TRUE
#define TRUE 1
#endif
/* �궨�峣��FALSEΪ0 */
#ifndef FALSE
#define FALSE 0
#endif


/***************** ���� *******************/

/* Ϊ���ͳ�����չ,ָ��洢����С,����Ϊ1024 */
#define   IADDR_SIZE  1024 
/* Ϊ���ͳ�����չ,���ݴ洢����С,����Ϊ1024 */
#define   DADDR_SIZE  1024 
/* �Ĵ�������,����Ϊ8 */
#define   NO_REGS 8
/* PC�Ĵ���,����Ϊ7 */
#define   PC_REG  7
/* Ŀ������д�С,����Ϊ121 */
#define   LINESIZE  121
/* �ִ�С,����Ϊ20 */
#define   WORDSIZE  20


/**************** ���� *******************/

/* ָ��Ѱַģʽ���� */
typedef enum
{
    opclRR,		/* �Ĵ���Ѱַģʽ����,������ʹ�üĴ���r,s,t */
    opclRM,		/* �Ĵ���-�ڴ�Ѱַģʽ����,������ʹ�üĴ���r,�ڴ��ַd+s */
    opclRA		/* �Ĵ���-������Ѱַģʽ����,������ʹ�üĴ���r,������ֵd+s */
} OPCLASS;

/* ����ָ���ʶ������ */
typedef enum opcode {

    /************* �Ĵ���Ѱַģʽָ���ʶ�� ***************/

    opHALT,	/* ָֹͣ��:��������ִ��,���Բ����� */

    opIN,	/* ����ָ��:���ⲿ��������Ĵ���,ʹ�õ�r���Ĵ���,����s,t���� */

    opOUT,	/* ���ָ��:���Ĵ�����ֵ���,ʹ�õ�r���Ĵ���,����s,t���� */

    opADD,   /* �ӷ�ָ��:�Ĵ���r��ֵ��Ϊ�Ĵ���s��ֵ��Ĵ���t��ֵ�ĺ� */

    opSUB,   /* ����ָ��:�Ĵ���r��ֵ��Ϊ�Ĵ���s��ֵ��Ĵ���t��ֵ�Ĳ� */

    opMUL,   /* �˷�ָ��:�Ĵ���r��ֵ��Ϊ�Ĵ���s��ֵ��Ĵ���t��ֵ�Ļ� */

    opDIV,   /* ����ָ��:�Ĵ���r��ֵ��Ϊ�Ĵ���s��ֵ��Ĵ���t��ֵ���� */

    /* �Ĵ���Ѱַģʽָ�����Ʊ�־,							*
     * ������ʶ��ö��ֵС��opRRLim��ָ���Ϊ�Ĵ���Ѱַģʽָ��	*/
    opRRLim,

    /************** �Ĵ���-�ڴ�Ѱַģʽָ���ʶ�� ****************/

    opLD,	/* ����ָ��:�Ĵ���r��ֵ��Ϊ��ַΪd+reg(s)���ڴ浥Ԫ��ֵ */

    opST,    /* ����ָ��:����ַΪd+reg(s)���ڴ浥Ԫֵ��Ϊ�Ĵ���r��ֵ */

    /* �Ĵ���-�ڴ�Ѱַģʽָ�����Ʊ�־										*
     * ������ʶ��ö��ֵС��opRMLim�Ҵ���opRRLim�ľ�Ϊ�Ĵ���-�ڴ�Ѱַģʽָ�� */
    opRMLim,


    /************* �Ĵ���-������Ѱַģʽָ���ʶ�� ****************/

    opLDA,	/* ����ָ��:���Ĵ���r��ֵ��Ϊ������d��Ĵ���s��ֵ�ĺ� */

    opLDC,   /* ����ָ��:���Ĵ���r��ֵ��Ϊ������d,����s������ */

    opJLT,   /* ����Ĵ���r��ֵС��0,����7���Ĵ�����ֵ��Ϊd+reg(s)	*
              * ��7���Ĵ���Ϊpc��������Ĵ���						*/

    opJLE,   /* ����Ĵ���r��ֵС�ڵ���0,��pc�Ĵ�����ֵ��Ϊd+reg(s) */

    opJGT,   /* ����Ĵ���r��ֵ����0,��pc�Ĵ�����ֵ��Ϊd+reg(s) */

    opJGE,   /* ����Ĵ���r��ֵ���ڵ���0,��pc�Ĵ�����ֵ��Ϊd+reg(s) */

    opJEQ,   /* ����Ĵ���r��ֵ����0,��pc�Ĵ�����ֵ��Ϊd+reg(s) */

    opJNE,   /* ����Ĵ���r��ֵ������0,��pc�Ĵ�����ֵ��Ϊd+reg(s) */

    /* �Ĵ���-������Ѱַģʽָ�����Ʊ�־
     * ������ʶ��ö��ֵС��opRALim�Ҵ���opRMLim�ľ�Ϊ�Ĵ���-������Ѱַģʽָ�� */
    opRALim

} OPCODE;

/************ ָ���ִ�н������ ************/
typedef enum {
    srOKAY,			/* ���� */

    srHALT,			/* ֹͣ */

    srIMEM_ERR,		/* ָ��洢�� */

    srDMEM_ERR,		/* ���ݴ洢�� */

    srZERODIVIDE		/* ����Ϊ��� */

} STEPRESULT;

/* ָ��ṹ����:������,������1,������2,������3 */
typedef struct {
    int iop;
    int iarg1;
    int iarg2;
    int iarg3;
} INSTRUCTION;


/******** ���� ********/

int iloc = 0;			/* ָ��洢����ָ��,��ʼΪ0 */

int dloc = 0;			/* ���ݴ洢����ָ��,��ʼΪ0 */

int traceflag = FALSE;	/* ָ��ִ��׷�ٱ�־,��ʼΪFALSE */

int icountflag = FALSE;	/* ָ��ִ�м�����־,��ʼΪFALSE */

/* iMem����ָ��洢,Ϊ1024����ָ��ṹ���� */
INSTRUCTION iMem[IADDR_SIZE];

/* dMem�������ݴ洢,Ϊ1024���������������� */
int dMem[DADDR_SIZE];

/* reg���ڼĴ����洢,Ϊ8���������������� */
int reg[NO_REGS];


/* ָ��������,��ӦѰַģʽ��Ϊ���� */
char* opCodeTab[]= { "HALT","IN","OUT","ADD","SUB","MUL","DIV","????",
/* �Ĵ���Ѱַģʽָ������ */

 "LD","ST","????",
    /* �Ĵ���-�ڴ�Ѱַģʽָ������ */

     "LDA","LDC","JLT","JLE","JGT","JGE","JEQ","JNE","????"
    /* �Ĵ���-������Ѱַģʽָ������ */
};

/** ����ִ�н��״̬�� **/
char* stepResultTab[]

= { "OK","Halted","Instruction Memory Fault",
   "Data Memory Fault","Division by 0"
};

char pgmName[20];			/* ���ڴ洢�����ļ��� */

FILE* pgm;				/* �����ļ�ָ�� */

char in_Line[LINESIZE];	/* ���ڴ洢һ�д���,Ϊ121�����ַ����� */

int lineLen;				/* in_Line���н�β�ַ�λ�� */

int inCol;				/* ����ָ����in_Line�еĵ�ǰ�ַ�λ�� */

int num;					/* ���ڴ洢��ǰ������ֵ */

char word[WORDSIZE];		/* ���ڴ洢��ǰ����,Ϊ20�����ַ����� */

char ch;					/* ��ǰ�������е�ǰλ���ϵ��ַ� */

int done;



/****************************************************/
/* ������ opClass									*/
/* ��  �� ָ��Ѱַģʽ���ຯ��						*/
/* ˵  �� �ú����Ը�����ָ�������ö��ֵc���з���	*/
/*        ����ָ������Ѱַģʽ						*/
/****************************************************/
int opClass(int c)

{
    /* ���ö��ֵcС��opRRLim,��ָ��Ϊ�Ĵ���Ѱַģʽָ������ */
    if (c <= opRRLim) return (opclRR);

    /* ���ö��ֵcС��opRMLim,��ָ��Ϊ�Ĵ���-�ڴ�Ѱַģʽָ������ */
    else if (c <= opRMLim) return (opclRM);

    /* Ϊ�Ĵ���-������Ѱַģʽָ������ */
    else                    return (opclRA);

}

/********************************************************/
/* ������ writeInstruction								*/
/* ��  �� ָ���������									*/
/* ˵  �� �ú�����ָ��洢����ָ����ָ����ʽ�������Ļ	*/
/********************************************************/
void writeInstruction(int loc)

{
    /* locΪ��Ҫ�����ָ����ָ��洢���е�ַ,�������Ļ */
    printf("%5d: ", loc);

    /* ���ָ���ַloc��0-1023��Ч��ָ��洢����ַ��Χ֮�� */
    if ((loc >= 0) && (loc < IADDR_SIZE))

    {
        /* �����ַΪloc�ϵ�ָ�������ֵiMem[loc].iop�͵�һ������iMem[loc].iarg1 */
        printf("%6s%3d,", opCodeTab[iMem[loc].iop], iMem[loc].iarg1);

        /* ����ָ���Ѱַģʽ���ദ�� */
        switch (opClass(iMem[loc].iop))

        {
            /* ���ָ��Ϊ�Ĵ���Ѱַģʽָ��,�Ը�����ʽ���������2,������3 */
        case opclRR: printf("%1d,%1d", iMem[loc].iarg2, iMem[loc].iarg3);
            break;

            /* ���ָ��Ϊ�Ĵ���-������Ѱַģʽָ��,�ͼĴ���-�ڴ�Ѱַģʽָ��	*
             * �Ը�����ʽ���������2,������3									*/
        case opclRM:
        case opclRA: printf("%3d(%1d)", iMem[loc].iarg2, iMem[loc].iarg3);
            break;
        }
        /* ����Ļ������з� */
        printf("\n");
    }
} /* writeInstruction */

/****************************************************/
/* ������ getCh										*/
/* ��  �� �ַ���ȡ����								*/
/* ˵  �� �����ǰ�����ַ�δ����,�������ص�ǰ�ַ�	*/
/*		  ����,�������ؿո��ַ�						*/
/****************************************************/
void getCh(void)

{
    /* �ڵ�ǰ������in_Line��,��ǰ�ַ�����inColδ����������ʵ�ʳ���lineLen *
     * ȡ�õ�ǰ���е�ǰλ�õ��ַ�,����ch									*/
    if (++inCol < lineLen)
        ch = in_Line[inCol];

    /* ���inCol������ǰ�����г��ȷ�Χ,��ch��Ϊ�ո� */
    else ch = ' ';

} /* getCh */



/********************************************************/
/* ������ nonBlank										*/
/* ��  �� �ǿ��ַ���ȡ����								*/
/* ˵  �� ����ɹ��ӵ�ǰ����ȡ�÷ǿ��ַ�,��������TRUE	*/
/*		  ����,��������FALSE							*/
/********************************************************/
int nonBlank(void)

{
    /* �ڵ�ǰ������in_Line��,��ǰ�ַ�λ��inCol��Ϊ�ո��ַ�	*
     * �ڵ�ǰ������in_Line��,��ǰ�ַ�λ��inCol����,�Թ��ո�	*/
    while ((inCol < lineLen)
        && (in_Line[inCol] == ' '))
        inCol++;

    /* �ڵ�ǰ������in_Line��,�����ǿ��ַ� */
    if (inCol < lineLen)

    {
        /* ȡ��ǰ�ַ�λ��inCol�е��ַ�����ch,		*
         * ��������TRUE(�Ѷ���Ϊ1),ch�еõ��ǿ��ַ�	*/
        ch = in_Line[inCol];
        return TRUE;
    }

    /* ��ǰ�������Ѿ�����,����ǰ�ַ�ch ��Ϊ�ո�,	*
     * ��������FALSE(�Ѷ���Ϊ0),ch��Ϊ�ո��ַ�	*/
    else
    {
        ch = ' ';
        return FALSE;
    }
} /* nonBlank */


/****************************************************************/
/* ������ getNum												*/
/* ��  �� ��ֵ��ȡ����											*/
/* ˵  �� �����������������ֵ��мӼ��������term�ϲ�����,		*/
/*        ������ֵ����Ϊnum.����ɹ��õ���ֵ,��������TRUE;	*/
/*        ����,��������FALSE									*/
/****************************************************************/
int getNum(void)

{
    int sign;				/* �������� */

    int term;				/* ���ڼ�¼��ǰ¼��ľֲ���ֵ */

    int temp = FALSE;		/* ��¼��������ֵ,��ʼΪ�� */

    num = 0;				/* ���ڼ�¼���мӼ�������������ֵ��� */

    do
    {
        sign = 1;			/* �������ӳ�ʼΪ1 */

        /* ���ú���nonBlank()�Թ���ǰλ�õĿո��,			*
         * ���õ��ĵ�ǰ�ǿ��ַ�chΪ+��-.(+/-���������ִ���)	*/
        while (nonBlank() && ((ch == '+') || (ch == '-')))

        {
            temp = FALSE;

            /* ��ǰ�ַ�chΪ"-"ʱ,��������sign��Ϊ-1 */
            if (ch == '-')  sign = -sign;

            /* ȡ��ǰ����������һ�ַ�����ǰ�ַ�ch�� */
            getCh();
        }

        term = 0;		/* ��ǰ¼��ľֲ���ֵ��ʼΪ0 */

        nonBlank();		/* �Թ���ǰλ���ϵĿո� */

        /* ��ǰ�ַ�chΪ����,�ֲ���ֵ��ѭ������ */
        while (isdigit(ch))

        {
            temp = TRUE;		/* ��������ֵ��ΪTRUE,�ɹ��õ����� */

            /* ���ַ�����ת��Ϊ��ֵ��ʽ,���н�λ�ۼ� */
            term = term * 10 + (ch - '0');

            getCh();			/* ȡ��ǰ����������һ�ַ�����ǰ�ַ�ch�� */

        }
        /* ���ֲ���ֵ�������ۼ�,�õ�������ֵnum */
        num = num + (term * sign);

    } while ((nonBlank()) && ((ch == '+') || (ch == '-')));
    return temp;
} /* getNum */


/****************************************************/
/* ������ getWord									*/
/* ��  �� ���ʻ�ȡ����								*/
/* ˵  �� �����ӵ�ǰ�������л�ȡ����.����õ��ַ�,	*/
/*		  ��������TRUE;����,��������FALSE			*/
/****************************************************/
int getWord(void)

{

    int temp = FALSE;			/* ��������ֵ��ʼΪFALSE */

    int length = 0;			/* ���ʳ��ȳ�ʼΪ0 */

    /* �ڵ�ǰ�������гɹ���ȡ�ǿ��ַ�ch */
    if (nonBlank())

    {
        /* ��ǰ�ǿ��ַ�chΪ��ĸ������ */
        while (isalnum(ch))

        {
            /* ��ǰ����wordδ�����涨�ֳ�WORDSIZE-1(Ϊ���ʽ����ַ���һ��λ)	*
             * ����ǰ�ַ�ch���뵽����ĩβ										*/
            if (length < WORDSIZE - 1) word[length++] = ch;

            getCh();			/* ȡ��ǰ����������һ�ַ� */
        }

        /* ����ǰ����word��������ַ� */
        word[length] = '\0';

        /* ���ú�������ֵ,��������word�ǿյ�ʱ��ΪTRUE */
        temp = (length != 0);

    }
    return temp;
} /* getWord */


/************************************************************/
/* ������ skipCh											*/
/* ��  �� �ַ��չ�����										*/
/* ˵  �� �����ǰλ�����ַ�Ϊ����ָ�����ַ�,��չ����ַ�,	*/
/*		  ��������TRUE;����������FALSE					*/
/************************************************************/
int skipCh(char c)

{
    int temp = FALSE;

    /* ��ǰλ�����ַ�Ϊ����ָ���ַ�c */
    if (nonBlank() && (ch == c))

    {
        getCh();		/* �չ���ǰ�ַ�c,ȡ��һ�ַ� */

        temp = TRUE;	/* �չ�ָ���ַ�c,��������TRUE */
    }
    return temp;
} /* skipCh */


/************************************/
/* ������ atEOL						*/
/* ��  �� �н����жϺ���			*/
/* ˵  �� ��ǰ���Ƿ�������жϺ���	*/
/************************************/
int atEOL(void)

{
    return (!nonBlank());	/* �����ǰ����û�зǿ��ַ�,��������TRUE */
} /* atEOL */


/****************************************************/
/* ������ error										*/
/* ��  �� ��������								*/
/* ˵  �� ������������к�,ָ���ַ��źʹ�����Ϣ	*/
/****************************************************/
int error(char* msg, int lineNo, int instNo)

{
    /* ����������λ���к�lineNo */
    printf("Line %d", lineNo);

    /* �������ָ���ַ���instNo */
    if (instNo >= 0) printf(" (Instruction %d)", instNo);

    /* ���������Ϣmsg */
    printf("   %s\n", msg);

    return FALSE;
} /* error */


/********************************************************/
/* ������ readInstructions								*/
/* ��  �� ָ���ļ����뺯��								*/
/* ˵  �� ��ָ���ļ��е�ָ���������뵽ָ��洢��iMem	*/
/********************************************************/
int readInstructions(void)

{
    OPCODE op;				/* ��ǰָ������� */

    int arg1, arg2, arg3;		/* ��ǰָ������� */

    int loc, regNo, lineNo;

    /* ��8���Ĵ������ݳ�ʼ��Ϊ0 */
    for (regNo = 0; regNo < NO_REGS; regNo++)
        reg[regNo] = 0;

    /* dMemΪ���ݴ洢��,0��ַ��ԪdMem[0]��ֵ��Ϊ���ݴ洢���߶˵�ַ1023	*
     * ����ֵ����Ŀ���������ʱ�ɳ��������ָ����뵽mp�Ĵ�����			*/
    dMem[0] = DADDR_SIZE - 1;

    /* �����ݴ洢�����ڳ�0��ַ��Ԫ��ĸ���Ԫ��ʼ��Ϊ0 */
    for (loc = 1; loc < DADDR_SIZE; loc++)
        dMem[loc] = 0;

    /* ��ָ��洢���и���Ԫ��ʼ��Ϊָ��;HALT 0,0,0 */
    for (loc = 0; loc < IADDR_SIZE; loc++)
    {
        iMem[loc].iop = opHALT;
        iMem[loc].iarg1 = 0;
        iMem[loc].iarg2 = 0;
        iMem[loc].iarg3 = 0;
    }

    lineNo = 0;		/* lineNo���ڼ�¼��ǰ����ָ���к� */

    /* ����ָ���ļ�pgm���ļ���β */
    while (!feof(pgm))

    {
        /* ��ָ���ļ�pgm�ж���119���ַ�����ǰ������in_Line */
        fgets(in_Line, LINESIZE - 2, pgm);

        inCol = 0;		/* ��ǰ������in_Line�е�ǰ�ַ�λ��inCol��ʼΪ0 */

        lineNo++;		/* ��ǰ�������кż�1 */

        /* ��ǰ������in_Line�н�β�ַ�λ�ø���lineLen */
        lineLen = strlen(in_Line) - 1;

        /* ���Դ�н����ַ���'\n',������н����ַ�Ϊ'\0' */
        if (in_Line[lineLen] == '\n') in_Line[lineLen] = '\0';

        /* Դ��û���н����ַ�,���Ͻ����ַ�'\0' */
        else in_Line[++lineLen] = '\0';

        /* ��ǰ�ַ�����"*",������ע�����,Ӧ����ָ����� */
        if ((nonBlank()) && (in_Line[inCol] != '*'))

        {
            /* ��ǰ�ַ���������,����ַ��,�������к�lineNo */
            if (!getNum())

                return error("Bad location", lineNo, -1);

            /* ��������ֵ������ǰ�����ַ���loc */
            loc = num;

            /* �����ַ���loc����ָ��洢����ַIADDR_SIZE,���� */
            if (loc > IADDR_SIZE)
                return error("Location too large", lineNo, loc);

            /* �����ַ���loc����ȱ��ð��,��ȱ��ð�Ŵ� */
            if (!skipCh(':'))
                return error("Missing colon", lineNo, loc);

            /* ��ǰλ�ò��ǵ���,��ȱ��ָ�������� */
            if (!getWord())
                return error("Missing opcode", lineNo, loc);

            /* ��ʼ���ָ��op,opָ�����������,ֵΪopHALT */
            op = opHALT;

            /* ��������opCodeTab,ֻ�Ƚϵ�ǰ��word�е�ǰ�ĸ��ַ�	*
             * ��Ϊ���ж���Ĳ�������,�������HALT����ֻΪ��	*/
            while ((op < opRALim)
                && (strncmp(opCodeTab[op], word, 4) != 0))
                op = (enum opcode)(op + 1);

            /* ��ǰ����word��ָ���Ĳ����벻�ڲ������opCodeTab��,���Ƿ���������� */
            if (strncmp(opCodeTab[op], word, 4) != 0)
                return error("Illegal opcode", lineNo, loc);

            /* �Բ��õ��Ĳ�����ֵop��Ѱַģʽ,���з��ദ�� */
            switch (opClass(op))

            {
            case opclRR:			/* �Ĵ���Ѱַģʽ������ */

                /* ��һ�Ĵ�����������,��0-7֮������,		*
                 * ���������Ϣ,�к�lineNo,�����ַ���loc	*/
                if ((!getNum()) || (num < 0) || (num >= NO_REGS))
                    return error("Bad first register", lineNo, loc);

                /* ����һ������arg1��ֵΪ��ǰ��ֵnum */
                arg1 = num;

                /* ��һ��������©��","�ָ���,���� */
                if (!skipCh(','))
                    return error("Missing comma", lineNo, loc);

                /* �ڶ��Ĵ�����������,��0-7֮������,		*
                 * ���������Ϣ,�к�lineNo,�����ַ���loc	*/
                if ((!getNum()) || (num < 0) || (num >= NO_REGS))
                    return error("Bad second register", lineNo, loc);

                /* ���ڶ���������arg2��ֵΪ��ǰ��ֵnum */
                arg2 = num;

                /* �ڶ���������©��","�ָ���,���� */
                if (!skipCh(','))
                    return error("Missing comma", lineNo, loc);

                /* �����Ĵ�����������,��0-7֮������,���� */
                if ((!getNum()) || (num < 0) || (num >= NO_REGS))
                    return error("Bad third register", lineNo, loc);

                /* ������������arg3��ֵΪ��ǰ��ֵnum */
                arg3 = num;
                break;


                /* �Ĵ���-�ڴ�Ѱַģʽ		*
                 * �Ĵ���-������Ѱַģʽ	*/
            case opclRM:
            case opclRA:

                /* ��һ�Ĵ�����������,��0-7֮������,���� */
                if ((!getNum()) || (num < 0) || (num >= NO_REGS))
                    return error("Bad first register", lineNo, loc);

                /* ����һ������arg1��ֵΪ��ǰ��ֵnum */
                arg1 = num;

                /* ��һ����������©��","�ָ���,���� */
                if (!skipCh(','))
                    return error("Missing comma", lineNo, loc);

                /* �ڶ�ƫ�Ƶ�ַ����������,������ƫ�Ƶ�ַ,���� */
                if (!getNum())
                    return error("Bad displacement", lineNo, loc);

                /* ���ڶ�ƫ�Ƶ�ַ������arg2��ֵΪ��ǰ��ַnum */
                arg2 = num;

                /* �ڶ�ƫ�Ƶ�ַ��������©��"(",������","�ָ���,���� */
                if (!skipCh('(') && !skipCh(','))
                    return error("Missing LParen", lineNo, loc);

                /* �ڶ��Ĵ�����������,��0-7֮������,���� */
                if ((!getNum()) || (num < 0) || (num >= NO_REGS))
                    return error("Bad second register", lineNo, loc);

                /* ������������arg3��ֵΪ��ǰ��ֵnum */
                arg3 = num;
                break;
            }

            /* �������ַ���loc��ָ��洢��ָ��洢��iMem */
            iMem[loc].iop = op;
            iMem[loc].iarg1 = arg1;
            iMem[loc].iarg2 = arg2;
            iMem[loc].iarg3 = arg3;
        }
    }
    return TRUE;
} /* readInstructions */



/************************************************/
/* ������ stepTM								*/
/* ��  �� TM������ִ�к���						*/
/* ˵  �� ����Ϊһ��ָ�����ִ��,���ָ���.	*/
/************************************************/
STEPRESULT stepTM(void)

{
    /* currentinstruction ���ڴ洢��ǰ��ִ�е�ָ�� */
    INSTRUCTION currentinstruction;

    int pc;			/* ��������� */

    int r, s, t, m;	/* ָ������� */

    int ok;

    /* pc����Ϊ��7���Ĵ���reg[7]��ֵ,Ϊ��������� */
    pc = reg[PC_REG];

    if ((pc < 0) || (pc > IADDR_SIZE))
        /* pc��ֵ����ָ��洢������Ч��ַ,��ָ��洢��,��������srIMEM_ERR */
        return srIMEM_ERR;

    /* pc��ֵΪ��Чָ���ַ,�����������reg[PC_REG]��ֵ��1 */
    reg[PC_REG] = pc + 1;

    /* ��ָ��洢��iMem֮��ȡ����ǰָ�� */
    currentinstruction = iMem[pc];

    /* ��ȡ����ָ���Ѱַģʽ���ദ��,��ʼ������ָ����������� */
    switch (opClass(currentinstruction.iop))

    {
    case opclRR:		/* �Ĵ���Ѱַģʽ */

        r = currentinstruction.iarg1;
        s = currentinstruction.iarg2;
        t = currentinstruction.iarg3;
        break;

    case opclRM:		/* �Ĵ���-�ڴ�Ѱַģʽ */

        r = currentinstruction.iarg1;
        s = currentinstruction.iarg3;
        m = currentinstruction.iarg2 + reg[s];

        /* ������m�����ݴ洢����Ч��ַ,�����ݴ洢��,��������srDMEM_ERR */
        if ((m < 0) || (m > DADDR_SIZE))
            return srDMEM_ERR;
        break;

    case opclRA:		/* �Ĵ���-������Ѱַģʽ */

        r = currentinstruction.iarg1;
        s = currentinstruction.iarg3;
        m = currentinstruction.iarg2 + reg[s];
        break;
    } /* case */

    /* �Խ�ִ��ָ��Ĳ�����ֵ���з��ദ��,���ָ����Ϣ,	*
     * ���ָ���,������Ӧ���״̬						*/
    switch (currentinstruction.iop)

    {
        /******************** RRָ�� ******************/

        /* ��ʽ����Ļ��ʾHALT(ֹͣ)ָ��,����״̬srHALT(ֹͣ) */
    case opHALT:
        printf("HALT: %1d,%1d,%1d\n", r, s, t);
        return srHALT;
        /**********************************************/

    case opIN:
        do
        {
            /* ��Ļ��ʾ�û���ʾ��Ϣ,��ʾ�û�Ϊָ��������ֵ */
            printf("Enter value for IN instruction: ");

            /* ˢ�±�׼����stdin���ͱ�׼���stdout�� */
            fflush(stdin);
            fflush(stdout);

            /* �ӱ�׼������stdinȡ���û��������ֵ,���뻺����in_Line */
            gets(in_Line);

            /* ȡ���û�������ֵ��ʵ�ʳ���lineLen */
            lineLen = strlen(in_Line);

            inCol = 0;

            /* ��ӻ�����in_Line�гɹ�ȡ������,��okΪTRUE */
            ok = getNum();

            /* �û�����Ĳ�������,���Ƿ�INָ��������� */
            if (!ok) printf("Illegal value\n");

            /* ���������ֵnum����INָ�������rָ���ļĴ���reg[r] */
            else reg[r] = num;
        }

        /* ����û����벻�Ϸ�,�ظ�Ҫ���û�����,ֱ������Ϸ���ֵ */
        while (!ok);
        break;
        /**********************************************/

        /* ��Ļ��ʾOUTָ��ִ�еĽ����Ϣ */
    case opOUT:
        printf("OUT instruction prints: %d\n", reg[r]);
        break;
        /**********************************************/

        /* ���ADDָ����� */
    case opADD:  reg[r] = reg[s] + reg[t];  break;
        /**********************************************/

        /* ���SUBָ����� */
    case opSUB:  reg[r] = reg[s] - reg[t];  break;
        /**********************************************/

        /* ���MULָ����� */
    case opMUL:  reg[r] = reg[s] * reg[t];  break;
        /**********************************************/

    case opDIV:

        /* ���ڳ���ָ��,������Ϊ0,�򱨳������, *
         * ������srZERODIVIDE;����,��ɳ������� */
        if (reg[t] != 0) reg[r] = reg[s] / reg[t];
        else return srZERODIVIDE;
        break;

        /***************** RM ָ�� ********************/

        /* �����ݴ洢��dMem�е��������뵽�Ĵ���reg[r] */
    case opLD:    reg[r] = dMem[m];  break;

        /**********************************************/

        /* ���Ĵ���reg[r]�е�����д�뵽���ݴ洢��dMem */
    case opST:    dMem[m] = reg[r];  break;


        /***************** RA ָ�� ********************/

        /* ���Ĵ���reg[r]��ֵΪ������m��ֵ */
    case opLDA:    reg[r] = m; break;
        /**********************************************/

        /* ���Ĵ���reg[r]��ֵΪ��ǰָ��ĵڶ���������ֵ */
    case opLDC:    reg[r] = currentinstruction.iarg2;   break;

        /**********************************************/

        /* ����Ĵ���reg[r]��ֵС��0,�򽫳��������reg[PC_REG]��ֵ	*
         * ��ֵΪ������m,����С��������ת							*/
    case opJLT:    if (reg[r] < 0) reg[PC_REG] = m; break;

        /**********************************************/

        /* ����Ĵ���reg[r]��ֵС�ڵ���0,�򽫳��������reg[PC_REG]��ֵ	*
         * ��ֵΪ������m,����С�ڵ���������ת							*/
    case opJLE:    if (reg[r] <= 0) reg[PC_REG] = m; break;

        /**********************************************/

        /* ����Ĵ���reg[r]��ֵ����0,�򽫳��������reg[PC_REG]��ֵ	*
         * ��ֵΪ������m,��������������ת							*/
    case opJGT:    if (reg[r] > 0) reg[PC_REG] = m; break;

        /**********************************************/

        /* ����Ĵ���reg[r]��ֵ���ڵ���0,�򽫳��������reg[PC_REG]��ֵ	*
         * ��ֵΪ������m,�������ڵ�����ת								*/
    case opJGE:    if (reg[r] >= 0) reg[PC_REG] = m; break;

        /**********************************************/

        /* ����Ĵ���reg[r]��ֵ����0,�򽫳��������reg[PC_REG]��ֵ	*
         * ��ֵΪ������m,��������������ת							*/
    case opJEQ:    if (reg[r] == 0) reg[PC_REG] = m; break;

        /**********************************************/

        /* ����Ĵ���reg[r]��ֵ������0,�򽫳��������reg[PC_REG]��ֵ	*
         * ��ֵΪ������m,����������������ת								*/
    case opJNE:    if (reg[r] != 0) reg[PC_REG] = m; break;
    }
    /* case */

    /* ������������ָ��,�����������״̬ */
    return srOKAY;

} /* stepTM */


/****************************************************/
/* ������ doCommand									*/
/* ��  �� TM�������������						*/
/* ˵  �� ���������û������TM��������,�����Ӧ����	*/
/****************************************************/
int doCommand(void)

{
    char cmd;				/* �û����������� */

    int stepcnt = 0, i;
    int printcnt;
    int stepResult;
    int regNo, loc;
    do
    {
        /* ��Ļ��ʾ��ʾ��Ϣ,��ʾ�û�����TM���� */
        printf("Enter command: ");

        /* ˢ�±�׼��������� */
        fflush(stdin);
        fflush(stdout);

        /* �ӱ�׼��������ȡ���û���������� */
        gets(in_Line);

        lineLen = strlen(in_Line);
        inCol = 0;
    }
    /* �ظ������û�����������,ֱ���õ��������� */
    while (!getWord());

    cmd = word[0];		/* ȡ�����������еĵ�һ���ַ���cmd */

    switch (cmd)
    {
        /* ��������������ָ��ִ��׷�ٱ�־,׷��ָ��ִ�� */
    case 't':

        traceflag = !traceflag;		/* ȡ������׷�ٱ�־traceflag */

        /* ���TM��t����ִ�н����Ϣ */
        printf("Tracing now ");
        if (traceflag) printf("on.\n"); else printf("off.\n");
        break;
        /**************************************************************/

        /* ���������������Ϣ�б�,��ʾ��������书�� */
    case 'h':

        printf("Commands are:\n");

        /* ����ִ��(step)����:������"s(tep <n>"��ִ��,	*
         * ��ִ��n(Ĭ��Ϊ1)��tmָ��.						*/
        printf("   s(tep <n>      "\
            "Execute n (default 1) TM instructions\n");

        /* ִ�е�����(go)����:������"g(o"��ִ��,	*
         * ˳��ִ��tmָ��ֱ������HALTָ��			*/
        printf("   g(o            "\
            "Execute TM instructions until HALT\n");

        /* ��ʾ�Ĵ���(regs)����:������"r(egs"��ִ��,	*
         * ��ʾ���Ĵ���������							*/
        printf("   r(egs          "\
            "Print the contents of the registers\n");

        /* ���ָ��(iMem)����:������"i(Mem <b<n>>"��ִ��,	*
         * �ӵ�ַb�����n��ָ��							*/
        printf("   i(Mem <b <n>>  "\
            "Print n iMem locations starting at b\n");

        /* �������(dMem)����:������"d(Mem<b<n>>"��ִ��,	*
         * �ӵ�ַb�����n������							*/
        printf("   d(Mem <b <n>>  "\
            "Print n dMem locations starting at b\n");

        /* ����(trace)����:������"t(race"��ִ��,		*
         * ����׷�ٱ�־traceflag,���traceflagΪTRUE,	*
         * ��ִ��ÿ��ָ��ʱ����ʾָ��					*/
        printf("   t(race         "\
            "Toggle instruction trace\n");

        /* ��ʾִ��ָ������(print)����:������"p(rint)"��ִ��,	*
         * ����׷�ٱ�־icountflag,���icountflagΪTRUE,		*
         * ����ʾ�Ѿ�ִ�й���ָ������.ֻ��ִ��"go"����ʱ��Ч	*/
        printf("   p(rint         "\
            "Toggle print of total instructions executed"\
            " ('go' only)\n");

        /* ����tm����(clear)����:������"c(lear"��ִ��,	*
         * ��������tm�����,����ִ���µĳ���.				*/
        printf("   c(lear         "\
            "Reset simulator for new execution of program\n");

        /* ����(help)����:������"h(elp"��ִ��,��ʾ�����б� */
        printf("   h(elp          "\
            "Cause this list of commands to be printed\n");

        /* ��ֹ(quit)����,������"q(uit"��ִ��,�����������ִ�� */
        printf("   q(uit          "\
            "Terminate the simulation\n");

        break;
        /**************************************************************/

        /* ������ʾ����ִ�й�ָ���p���� */
    case 'p':

        icountflag = !icountflag;		/* ����ִ��ָ�������־ */

        /* ���p����ִ�еĽ����Ϣ */
        printf("Printing instruction count now ");
        if (icountflag) printf("on.\n"); else printf("off.\n");
        break;
        /**************************************************************/

        /* ����ִ��s���� */
    case 's':

        /* ȱʡ������ģʽ,�����������,����ִ�� */
        if (atEOL())  stepcnt = 1;

        /* �����������������ģʽ,ȡ�ò���stepcnt */
        else if (getNum())  stepcnt = abs(num);

        /* ���δ֪����ִ�в�����Ϣ */
        else   printf("Step count?\n");
        break;
        /**************************************************************/


        /* ִ�е�����g���� */
    case 'g':   stepcnt = 1;     break;
        /**************************************************************/

        /* ��ʾ�Ĵ�������(regs)���� */
    case 'r':

        /* ��ʽ����ʾ���мĴ������� */
        for (i = 0; i < NO_REGS; i++)
        {
            printf("%1d: %4d    ", i, reg[i]);
            if ((i % 4) == 3) printf("\n");
        }
        break;
        /**************************************************************/

        /* ���ָ��洢��iMem��ָ���i���� */
    case 'i':

        /* ��ʼ�����ָ����printcntΪ1 */
        printcnt = 1;

        if (getNum())
        {
            /* �õ�����ĵ�һ��ִ�в���,ilocָ�����ָ��Ŀ�ʼ��ַ */
            iloc = num;

            /* �õ�����ĵڶ���ִ�в���,printcntָ�����ָ������� */
            if (getNum()) printcnt = num;
        }

        /* δ����ָ�ʼ��ַ�����ָ������ */
        if (!atEOL())
            printf("Instruction locations?\n");

        else
        {
            /* ָ���ַiloc��ָ��洢��iMem��ַ��Χ��,						*
             * ��ָ���������printcnt����0,��ilocָ����ַ���ָ������ָ��	*/
            while ((iloc >= 0) && (iloc < IADDR_SIZE)
                && (printcnt > 0))
            {
                writeInstruction(iloc);
                iloc++;
                printcnt--;
            }
        }
        break;
        /**************************************************************/

        /* ������ݴ洢��dMem�е����ݵ�d���� */
    case 'd':

        printcnt = 1;
        if (getNum())
        {
            /* ȡ������ĵ�һִ�в���,���ݴ洢�Ŀ�ʼ��ַdloc */
            dloc = num;

            /* ȡ������ĵڶ�ִ�в���,������ݵ�����printcnt */
            if (getNum()) printcnt = num;
        }

        /* δ�������ݴ洢���е����ݿ�ʼ��ַ������ */
        if (!atEOL())
            printf("Data locations?\n");

        else
        {
            /* �������ݵ�ַdloc�����ݴ洢��dMen��ַ��Χ��,					*
             * �������������printcnt����0,��dlocָ����ַ���ָ������������ */
            while ((dloc >= 0) && (dloc < DADDR_SIZE)
                && (printcnt > 0))
            {
                printf("%5d: %5d\n", dloc, dMem[dloc]);
                dloc++;
                printcnt--;
            }
        }
        break;
        /**************************************************************/

        /* ����tm������ִ���µĳ���(clear)ָ�� */
    case 'c':

        iloc = 0;			/* ָ��洢��ַ,��ʼΪ0 */

        dloc = 0;			/* ���ݴ洢��ַ,��ʼΪ0 */

        stepcnt = 0;		/* ָ��ִ�в���,��ʼΪ0 */

        /* ��ʼ�����Ĵ���reg[]Ϊ0 */
        for (regNo = 0; regNo < NO_REGS; regNo++)
            reg[regNo] = 0;

        /* ���ݴ洢��0��ַ��Ԫ���ڼ�¼���ݴ洢��dMem�ĸ߶˵�ַ */
        dMem[0] = DADDR_SIZE - 1;

        /* ��ʼ���������ݴ洢����ԪΪ0 */
        for (loc = 1; loc < DADDR_SIZE; loc++)
            dMem[loc] = 0;
        break;
        /**************************************************************/

    case 'q': return FALSE;		/* ִֹͣ�в��˳����� */
        /**************************************************************/

        /* ����δ��������,���������Ϣ */
    default: printf("Command %c unknown.\n", cmd); break;

    }  /* case */

    /******************** ����ĺ������� **********************/

    stepResult = srOKAY;		/* ����ִ�н��ΪsrOKAY */

    if (stepcnt > 0)
    {
        if (cmd == 'g')
        {
            stepcnt = 0;			/* �˴�stepcnt��Ϊ�Ѿ�ִ�й���ָ����Ŀ */

            while (stepResult == srOKAY)
            {
                /* ����ִ��ָ��׷�ٱ�־traceflag,����ǰ��ַiloc��ָ���������Ļ */
                iloc = reg[PC_REG];
                if (traceflag) writeInstruction(iloc);

                /* ����ִ�е�ǰָ��,�������stepResult */
                stepResult = stepTM();

                /* ִ�й�ָ�����stepcnt��1 */
                stepcnt++;

            }

            /* ����ִ��ִ������׷�ٱ�־icountflag,��ʾ�Ѿ�ִ�й���ָ������ */
            if (icountflag)
                printf("Number of instructions executed = %d\n", stepcnt);
        }

        else
        {
            /* ������������stepcnt��Ϊ��ִ��,�����ָ������ݵ����� */
            while ((stepcnt > 0) && (stepResult == srOKAY))

            {
                /* ȡ�ó��������reg[PC_REG]�е�ǰָ���ַ */
                iloc = reg[PC_REG];

                /* ����ִ��ָ��׷�ٱ�־traceflag,����ǰָ���ַiloc��ָ���������Ļ */
                if (traceflag) writeInstruction(iloc);

                /* ִ�е�ǰָ��,�������stepResult */
                stepResult = stepTM();

                /* stepcnt��ʱ���ڼ�¼��Ҫִ��,�����ָ������ݵ�����,�Լ� */
                stepcnt--;
            }
        }
        /* ����ִ�н����ö��ֵ,��ִ�н��״̬��,��ʾ���״̬ */
        printf("%s\n", stepResultTab[stepResult]);
    }
    return TRUE;
} /* doCommand */



/********************************************/
/* ������ tmain								*/
/* ��  �� tm����ִ�к���					*/
/* ˵  �� �������tm���������,			*/
/*		  ������ִ��Ŀ��ָ��				*/
/********************************************/
int main()
{
    const char* codefile = "./compiler/resources/goal_code.txt";
    //char pgmName[120];
    /*��ʾ����Դ�����ļ�Ŀ¼��*/
    //printf("input program names:\n");   
    /*�洢�ļ�Ŀ¼����pgm��*/
    //scanf("%s",pgmName);  

    pgm = fopen(codefile/*pgmName*/, "r");

    /* δ�ܳɹ���Ŀ������ļ�pgmName,���������Ϣ */
    if (pgm == NULL)
    {
        printf("file '%s' not found\n", codefile);
        exit(1);
    }

    /* ����ָ��:��ָ��洢��iMem��ղ���ָ�����ļ���д��ָ������ */
    if (!readInstructions())
        exit(1);

    /* ׼��ִ��TM���������,�����ʾ��Ϣ */
    printf("TM  simulation (enter h for help)...\n");

    /* ����ִ��,�����û������TM����,���Ѿ����뵽iMem�е�ָ����в��� */
    do
        done = !doCommand();
    while (!done);

    /* ���������ִ����� */
    printf("Simulation done.\n");
    return 0;
}
