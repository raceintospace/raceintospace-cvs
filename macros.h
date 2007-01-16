
#ifndef __MACROS_H__
#define __MACROS_H__

#define IRBox(a,b,c,d,e)           {InBox((a),(b),(c),(d));RectFill((a)+1,(b)+1,(c)-1,(d)-1,(e));}
#define ORBox(a,b,c,d,e)           {OutBox((a),(b),(c),(d));RectFill((a)+1,(b)+1,(c)-1,(d)-1,(e));}


#define pButton(a,b,c,d,e,f,g) if ((x>=(a) && y>=(b) && x<=(c) && y<=(d) && mousebuttons==1) || ((f)==(g)))\
                            { \
                            InBox((a),(b),(c),(d)); \
                            while(1)  { GetMouse();if (mousebuttons==0) break;} \
                            (e);\
               		    if((f)>0) delay(150); \
                            OutBox((a),(b),(c),(d));\
                            };

#define Button2(a,b,c,d,e,f,g) if ((x>=(a) && y>=(b) && x<=(c) && y<=(d) && mousebuttons==1) || ((f)==(g)))\
                            { \
                            while(1)  { GetMouse();if (mousebuttons==0) break;} \
                            (e); \
                            };

#define poz(a) (((a)<0)?0:(a))



#endif

