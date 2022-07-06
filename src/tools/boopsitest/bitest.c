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

struct DosLibrary    *DOSBase       = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct UtilityBase   *UtilityBase   = NULL;

struct Window        *w             = NULL;
struct IntuiMessage  *msg;
struct Gadget        *prop=NULL;

static BPTR           astdout       = 0;

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
