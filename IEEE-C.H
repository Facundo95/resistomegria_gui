/*
    Function declarations for "C" to IEEE-488 interface routines.

    Copyright (C) 1995, Capital Equipment Corp.
    Handles both 16-bit DOS & Windows, and 32-bit Windows libraries

    4/2/99   Added waitSRQDevice function for win32
    2/27/02  Added IEEEFIFO feature test
*/

#ifndef __IEEE_CEC488_H
#define __IEEE_CEC488_H


/******** WIN-32 VERSION *******/
#if  (defined(WIN32)||defined(__WIN32__)||defined(__WIN32))

/*
    Note: "long int" is used throughout instead of just "int" to
          avoid any problems with choice of default int size (16 or 32 bits).
          Source code written for 16-bit versions of C with "int" decl. should
          not need changes for 32-bit, as long as "int" =  "long int" in
          the 32-bit compiler.
*/

#ifdef __cplusplus
extern "C" {
#endif

extern void  _cdecl ieee488_initialize (long int,long int);
extern long int  _cdecl ieee488_transmit (char  *,unsigned,long int  *);
extern long int  _cdecl ieee488_receive (char  *,unsigned long,unsigned long  *,long int  *);
extern long int  _cdecl ieee488_send (long int,char  *,unsigned long,long int  *);
extern long int  _cdecl ieee488_enter (char  *,unsigned long,unsigned long  *,long int,long int  *);
extern long int  _cdecl ieee488_spoll (long int,char  *,long int  *);
extern void  _cdecl ieee488_ppoll (char  *);
extern long int  _cdecl ieee488_tarray (void  *,unsigned long,long int,long int  *);
extern long int  _cdecl ieee488_rarray (void  *,unsigned long,unsigned long  *,long int  *);

extern char  _cdecl ieee488_srq(void);

extern void  _cdecl ieee488_setport (long int,unsigned);

extern void  _cdecl ieee488_boardselect (long int);
extern void  _cdecl ieee488_dmachannel (long int);
extern void  _cdecl ieee488_settimeout (unsigned long int);
extern void  _cdecl ieee488_setoutputEOS (long int,long int);
extern void  _cdecl ieee488_setinputEOS (long int);

extern void  _cdecl ieee488_enable_488ex(long int);
extern void  _cdecl ieee488_enable_488sd(long int,long int);
extern char  _cdecl ieee488_listener_present(long int);
extern char  _cdecl ieee488_board_present(void);

extern long int _cdecl ieee488_feature (long int);

extern long int _cdecl ieee488_waitSRQDevice (long int,char *,long int *);

#ifdef __cplusplus
 }
#endif


/*
    "define"s to modify user's calls into direct calls to 488 support code
*/

#define initialize(addr,level)          ieee488_initialize(addr,level)
#define transmit(cmd,status)            ieee488_transmit((char  *) (cmd),0xFFFF,(long int  *) status)
#define receive(s,maxlen,len,status)    ieee488_receive((char  *) (s),maxlen,(unsigned long  *) len,(long int  *) status)
#define send(addr,s,status)             ieee488_send(addr,(char  *) (s),0xFFFF,(long int  *) status)
#define enter(s,maxlen,len,addr,status) ieee488_enter((char  *) (s),maxlen,(unsigned long  *) len,addr,(long int  *) status)
#define spoll(addr,poll,status)         ieee488_spoll(addr,(char  *) poll,(long int  *) status)
#define ppoll(poll)                     ieee488_ppoll((char  *) poll)
#define tarray(d,count,eoi,status)      ieee488_tarray((void  *) d,count,eoi,(long int  *) status)
#define rarray(d,count,len,status)      ieee488_rarray((void  *) d,count,(unsigned long  *) len,(long int  *) status)
#define srq                             ieee488_srq
#define setport(bd,io)                  ieee488_setport(bd,io)
#define boardselect(bd)                 ieee488_boardselect(bd)
#define dmachannel(c)                   ieee488_dmachannel(c)
#define settimeout(t)                   ieee488_settimeout(t)
#define setoutputEOS(e1,e2)             ieee488_setoutputEOS(e1,e2)
#define setinputEOS(e)                  ieee488_setinputEOS(e)
#define enable_488ex(e)                 ieee488_enable_488ex(e)
#define enable_488sd(e,t)               ieee488_enable_488sd(e,t)
#define listener_present(a)             ieee488_listener_present(a)
#define gpib_board_present              ieee488_board_present
#define gpib_feature                    ieee488_feature

#define waitSRQDevice(addr,poll,status)  ieee488_waitSRQDevice(addr,(char *)poll,(long *)status)

#else
/************* DOS or Win 3.x VERSION *******/

/*
    function declarations
*/

#ifdef __cplusplus
  extern "C" {
#endif

void far pascal ieee488_initialize (int,int);
int far pascal ieee488_transmit (char far *,unsigned,int far *);
int far pascal ieee488_receive (char far *,unsigned,unsigned far *,int far *);
int far pascal ieee488_send (int,char far *,unsigned,int far *);
int far pascal ieee488_enter (char far *,unsigned,unsigned far *,int,int far *);
int far pascal ieee488_spoll (int,char far *,int far *);
void far pascal ieee488_ppoll (char far *);
int far pascal ieee488_tarray (void far *,unsigned,int,int far *);
int far pascal ieee488_rarray (void far *,unsigned,unsigned far *,int far *);

char far pascal ieee488_srq(void);

void far pascal ieee488_setport (int,unsigned);

void far pascal ieee488_boardselect (int);
void far pascal ieee488_dmachannel (int);
void far pascal ieee488_settimeout (unsigned);
void far pascal ieee488_setoutputEOS (int,int);
void far pascal ieee488_setinputEOS (int);

void far pascal ieee488_enable_488ex(int);
void far pascal ieee488_enable_488sd(int,int);
char far pascal ieee488_listener_present(int);
char far pascal ieee488_board_present(void);

int far pascal ieee488_feature(int);

#ifdef __cplusplus
 }
#endif

/*
    "define"s to modify user's calls into direct calls to 488 support code
*/

/*** handle bug in Turbo C 3.0 handling of DOS extenders ***/
#if defined(__TURBOC__) && (__TURBOC__ < 0x0410) && !defined(_Windows)
#ifdef __cplusplus
extern "C"
#endif
void far pascal ieee488_protcheck(int);
#define initialize(addr,level)          ieee488_protcheck(0); ieee488_initialize(addr,level)
#else
#define initialize(addr,level)          ieee488_initialize(addr,level)
#endif

#define transmit(cmd,status)            ieee488_transmit((char far *) (cmd),0xFFFF,(int far *) status)
#define receive(s,maxlen,len,status)    ieee488_receive((char far *) (s),maxlen,(unsigned int far *) len,(int far *) status)
#define send(addr,s,status)             ieee488_send(addr,(char far *) (s),0xFFFF,(int far *) status)
#define enter(s,maxlen,len,addr,status) ieee488_enter((char far *) (s),maxlen,(unsigned int far *) len,addr,(int far *) status)
#define spoll(addr,poll,status)         ieee488_spoll(addr,(char far *) poll,(int far *) status)
#define ppoll(poll)                     ieee488_ppoll((char far *) poll)
#define tarray(d,count,eoi,status)      ieee488_tarray((void far *) d,count,eoi,(int far *) status)
#define rarray(d,count,len,status)      ieee488_rarray((void far *) d,count,(unsigned int far *) len,(int far *) status)
#define srq                             ieee488_srq
#define setport(bd,io)                  ieee488_setport(bd,io)
#define boardselect(bd)                 ieee488_boardselect(bd)
#define dmachannel(c)                   ieee488_dmachannel(c)
#define settimeout(t)                   ieee488_settimeout(t)
#define setoutputEOS(e1,e2)             ieee488_setoutputEOS(e1,e2)
#define setinputEOS(e)                  ieee488_setinputEOS(e)
#define enable_488ex(e)                 ieee488_enable_488ex(e)
#define enable_488sd(e,t)               ieee488_enable_488sd(e,t)
#define listener_present(a)             ieee488_listener_present(a)
#define gpib_board_present              ieee488_board_present
#define gpib_feature                    ieee488_feature

#endif


/*
    Feature codes for CEC488 board feature inquiry routine.
*/

/* features of the hardware */
#define IEEEListener               0
#define IEEE488SD                  1
#define IEEEDMA                    2
#define IEEEFIFO                   3

/* hardware settings */
#define IEEEIOBASE                100 

/* software settings */
#define IEEETIMEOUT               200
#define IEEEINPUTEOS              201
#define IEEEOUTPUTEOS1            202
#define IEEEOUTPUTEOS2            203
#define IEEEBOARDSELECT           204
#define IEEEDMACHANNEL            205


#endif

