#include "dprintf.h"

#include <stdlib.h>

#include <proto/alib.h>

#include <proto/exec.h>
#include <inline/exec.h>

#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/icclass.h>
#include <clib/intuition_protos.h>
#include <inline/intuition.h>

#include <dos/dos.h>
#include <inline/dos.h>

#include <utility/tagitem.h>
#include <inline/utility.h>

#define PROPGADGET_ID       1L
#define PROPGADGETWIDTH     10L
#define PROPGADGETHEIGHT    80L
#define VISIBLE             10L
#define TOTAL               100L
#define INITIALVAL          25L
#define MINWINDOWWIDTH      80
#define MINWINDOWHEIGHT     (PROPGADGETHEIGHT + 70)
#define MAXCHARS            3L

int __nocommandline=1; /* Disable commandline parsing  */
int __initlibraries=0; /* Disable auto-library-opening */

extern struct WBStartup *_WBenchMsg;

struct DosLibrary    *DOSBase         = NULL;
struct IntuitionBase *IntuitionBase   = NULL;
struct UtilityBase   *UtilityBase     = NULL;

struct Window        *w               = NULL;
struct IntuiMessage  *msg;
struct Gadget        *prop            = NULL;

static BPTR           astdout         = 0;
static struct IClass *mybutton_class  = NULL;

struct mybutton_data
{
		UWORD gad_x;
		UWORD gad_y;
		UWORD gad_w;
		UWORD gad_h;
//		struct button button[4];
		WORD active_button;
};

void _debug_putc (char c)
{
    if (!astdout)
        return;

    char s[2] = {c, 0};

    Write (astdout, s, 1);
}

void cleanexit(char *msg)
{
    if (msg)
        DPRINTF (msg);
    if (prop)
        DisposeObject(prop);
    if (w)
        CloseWindow(w);

    if (DOSBase)
        CloseLibrary((struct Library*)DOSBase);
    if (IntuitionBase)
        CloseLibrary((struct Library*)IntuitionBase);

    exit(0);
}

static __saveds ULONG _mybutton_dispatcher (struct IClass *cl, struct Gadget *o, Msg msg)
{
	ULONG retval = 0;

	DPRINTF ("_mybutton_dispatcher: msg->MethodID=%d\n", msg->MethodID);

	switch (msg->MethodID)
	{
#if 0
	case OM_NEW:
		retval = gui_new (cl,(Object *)o,(struct opSet *)msg);
		break;
	case OM_DISPOSE:
		retval = gui_dispose (cl,o,msg);
		break;
	case OM_SET:
	case OM_UPDATE:
		retval = gui_set (cl,o,(struct opSet *)msg,FALSE);
		break;
	case OM_GET:
		retval = gui_get (cl,o,(struct opGet *)msg);
		break;
	case GM_LAYOUT:
		retval = gui_layout (cl,o,(struct gpLayout *)msg);
		break;
	case GM_RENDER:
		retval = gui_render (cl,o,(struct gpRender *)msg);
		break;
	case GM_HITTEST:
		retval = gui_hittest (cl,o,(struct gpHitTest *)msg);
		break;
	case GM_GOACTIVE:
		retval = gui_active (cl,o,(struct gpInput *)msg);
		break;
	case GM_HANDLEINPUT:
		retval = gui_input (cl,o,(struct gpInput *)msg);
		break;
	case GM_GOINACTIVE:
		retval = gui_inactive (cl,o,(struct gpGoInactive *)msg);
		break;
#endif
	default:
		retval = DoSuperMethodA (cl, (Object *)o, msg);
	}

	return (retval);
}

struct IClass *init_mybutton_class (void)
{
	struct IClass *cl = MakeClass (/*ClassID      =*/ NULL,
						           /*SuperClassID =*/ (STRPTR) "gadgetclass",
								   /*SuperClassPtr=*/ NULL,
								   /*InstanceSize =*/ sizeof(struct mybutton_data),
								   /*Flags        =*/ 0);

	DPRINTF ("init_mybutton_class: MakeClass() returned 0x%08lx\n", cl);

	if (cl)
	{
		cl->cl_Dispatcher.h_Entry    = (APTR)HookEntry;
		cl->cl_Dispatcher.h_SubEntry = (APTR)_mybutton_dispatcher;
	}

	return cl;
}

int main(void)
{
    if ( !(DOSBase=(struct DosLibrary *)OpenLibrary((STRPTR)"dos.library", 37)) )
        cleanexit ("failed to open dos.library!\n");
    astdout = Output();
    DPRINTF ("dos.library opened.\n");

    if (! (IntuitionBase = (struct IntuitionBase*)OpenLibrary((STRPTR)"intuition.library", 37)))
        cleanexit("failed to open intuition.library\n");
    DPRINTF ("intuition.library opened\n");

    if (! (UtilityBase = (struct UtilityBase*) OpenLibrary((STRPTR)"utility.library", 37)))
        cleanexit("failed to open utility.library\n");
    DPRINTF ("utility.library opened\n");

	mybutton_class = init_mybutton_class();
	if (!mybutton_class)
        cleanexit("failed to init mybutton class\n");

    w = OpenWindowTags(NULL,
                       WA_Flags,       WFLG_DEPTHGADGET | WFLG_DRAGBAR |
                                       WFLG_CLOSEGADGET | WFLG_SIZEGADGET,
                       WA_IDCMP,       IDCMP_CLOSEWINDOW,
                       WA_MinWidth,    MINWINDOWWIDTH,
                       WA_MinHeight,   MINWINDOWHEIGHT,
                       TAG_END);
    if (!w)
        cleanexit("failed to open the window\n");
    DPRINTF ("window opened.");

    prop = (struct Gadget *)NewObject(NULL, (STRPTR)"propgclass",
                                      GA_ID,     PROPGADGET_ID,
                                      GA_Top,    (w->BorderTop) + 5L,
                                      GA_Left,   (w->BorderLeft) + 5L,
                                      GA_Width,  PROPGADGETWIDTH,
                                      GA_Height, PROPGADGETHEIGHT,
                                      PGA_Total,     TOTAL,
                                      PGA_Top,       INITIALVAL,
                                      PGA_Visible,   VISIBLE,
                                      PGA_NewLook,   TRUE,
                                      TAG_END);
    if (!prop)
        cleanexit ("failed to create proportional gadget object\n");
    DPRINTF ("proportional gadget object created.\n");

	struct Gadget *myb = NewObject (mybutton_class,NULL,
									GA_Top,         (w->BorderTop) + 5L,
									GA_Left,        (w->BorderLeft) + 45L,
                                    GA_Width,       150,
                                    GA_Height,      23,
									GA_ID,          42,
									GA_RelVerify,   TRUE,
									GA_Previous,    (LONG) prop,
									TAG_END);

	if (!myb)
        cleanexit ("failed to create mybutton gadget object\n");
    DPRINTF ("proportional mybutton object created.\n");

    AddGList(w, prop, -1, -1, NULL);
    RefreshGList(prop, w, NULL, -1);

    BOOL done = FALSE;

    while (done == FALSE)
    {
        WaitPort((struct MsgPort *)w->UserPort);
        while ( (msg = (struct IntuiMessage *) GetMsg((struct MsgPort *)w->UserPort)) )
        {
            if (msg->Class == IDCMP_CLOSEWINDOW)
                done = TRUE;
            ReplyMsg((struct Message *)msg);
        }
    }
    RemoveGList(w, prop, -1);
    cleanexit("all done. goodbye.\n");
}
