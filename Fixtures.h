/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  FXTR_PANEL                       1       /* callback function: FixturesPanelFunc */
#define  FXTR_PANEL_GRAPH                 2       /* control type: graph, callback function: (none) */
#define  FXTR_PANEL_MONTHS_RING           3       /* control type: ring, callback function: DrawFixtures */
#define  FXTR_PANEL_BG_PIC                4       /* control type: picture, callback function: (none) */
#define  FXTR_PANEL_CANVAS                5       /* control type: canvas, callback function: (none) */
#define  FXTR_PANEL_EDIT_BUTTON           6       /* control type: command, callback function: OpenFixtureFile */
#define  FXTR_PANEL_REFRESH_BUTTON        7       /* control type: command, callback function: RefreshFxtrPage */
#define  FXTR_PANEL_PICTURE               8       /* control type: picture, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK DrawFixtures(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FixturesPanelFunc(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenFixtureFile(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RefreshFxtrPage(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
