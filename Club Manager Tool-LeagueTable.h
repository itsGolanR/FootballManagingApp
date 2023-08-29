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

#define  LEAG_TBPAN                       1       /* callback function: LeagueTablePanelFunc */
#define  LEAG_TBPAN_BG_PIC                2       /* control type: picture, callback function: (none) */
#define  LEAG_TBPAN_LEAGUETABLE           3       /* control type: table, callback function: (none) */
#define  LEAG_TBPAN_REFRESH_BUTTON        4       /* control type: command, callback function: RefreshTable */
#define  LEAG_TBPAN_EDIT_BUTTON           5       /* control type: command, callback function: OpenTableFile */
#define  LEAG_TBPAN_DECORATION            6       /* control type: deco, callback function: (none) */
#define  LEAG_TBPAN_TITLE                 7       /* control type: picture, callback function: (none) */
#define  LEAG_TBPAN_REFRESH_TXT           8       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK LeagueTablePanelFunc(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenTableFile(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RefreshTable(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
