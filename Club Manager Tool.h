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

#define  ABT_PANEL                        1       /* callback function: AboutPanelfumc */
#define  ABT_PANEL_NAME                   2       /* control type: textMsg, callback function: (none) */
#define  ABT_PANEL_NICK_NAME              3       /* control type: textMsg, callback function: (none) */
#define  ABT_PANEL_ID                     4       /* control type: textMsg, callback function: (none) */
#define  ABT_PANEL_AGE                    5       /* control type: textMsg, callback function: (none) */
#define  ABT_PANEL_COURSE                 6       /* control type: textMsg, callback function: (none) */
#define  ABT_PANEL_INSTITUTE              7       /* control type: textMsg, callback function: (none) */
#define  ABT_PANEL_TITLE                  8       /* control type: textMsg, callback function: (none) */
#define  ABT_PANEL_SIGNATURE              9       /* control type: picture, callback function: (none) */
#define  ABT_PANEL_PIC                    10      /* control type: picture, callback function: (none) */

#define  CONT_PANEL                       2
#define  CONT_PANEL_TITLE                 2       /* control type: picture, callback function: (none) */
#define  CONT_PANEL_TACTIC_BUTTON         3       /* control type: pictButton, callback function: DisplayTacticalPanel */
#define  CONT_PANEL_SQUAD_BUTTON          4       /* control type: pictButton, callback function: DisplaySquadPanel */
#define  CONT_PANEL_FIXTURES_BUTTON       5       /* control type: pictButton, callback function: DisplayFixturesPanel */
#define  CONT_PANEL_TABLE_BUTTON          6       /* control type: pictButton, callback function: DisplayLeagueTablePanel */
#define  CONT_PANEL_BG_PIC                7       /* control type: picture, callback function: (none) */

#define  MAIN_PANEL                       3       /* callback function: MainPanelFunc */

#define  MENU_PANEL                       4       /* callback function: menuFunc */
#define  MENU_PANEL_HELP_MBUTTON          2       /* control type: command, callback function: OpenHelpDoc */
#define  MENU_PANEL_VIDEO_MBUTTON         3       /* control type: command, callback function: OpenVideo */
#define  MENU_PANEL_ABOUT_MBUTTON         4       /* control type: command, callback function: DisplayAboutPanel */
#define  MENU_PANEL_TACTICAL_MBUTTON      5       /* control type: command, callback function: DisplayTacticalPanel */
#define  MENU_PANEL_FIXTURES_MBUTTON      6       /* control type: command, callback function: DisplayFixturesPanel */
#define  MENU_PANEL_LEAGUE_TABLE_MBUTTON  7       /* control type: command, callback function: DisplayLeagueTablePanel */
#define  MENU_PANEL_SQUAD_MBUTTON         8       /* control type: command, callback function: DisplaySquadPanel */
#define  MENU_PANEL_HOME_MBUTTON          9       /* control type: command, callback function: DisplayHome */
#define  MENU_PANEL_SPLITTER_LINE         10      /* control type: picture, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK AboutPanelfumc(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DisplayAboutPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DisplayFixturesPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DisplayHome(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DisplayLeagueTablePanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DisplaySquadPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DisplayTacticalPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainPanelFunc(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK menuFunc(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenHelpDoc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OpenVideo(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
