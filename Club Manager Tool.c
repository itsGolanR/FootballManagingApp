#include <utility.h>
#include "toolbox.h"
#include "Squad.h"
#include "Tactics.h"
#include "Fixtures.h"
#include <ansi_c.h>
#include "Club Manager Tool-LeagueTable.h"
#include <cvirte.h>		
#include <userint.h>
#include "Club Manager Tool.h"

#define PLAYER_X	492
#define PLAYER_SPACE 25
#define ICON_W		360
#define ICON_H		408
#define ICON_N	 	 48
#define FIELD_W		450
#define MAX_PLAYERS  25

typedef struct{
	int pos, played, pts;
	char name[20];
	int won, drawn, lost;
	int gf, ga, gd;
	char f1[2], f2[2], f3[2], f4[2], f5[2];
	int logo;
} Teams;

typedef struct {
	int match, hscore, ascore, pos, month;
	char date[20], gtime[10], monthstr[20];
	char loc[30], hteam[20], ateam[20], result[20], status[2];
} Fixtures;

typedef struct {
	char fname[20], lname[20], role[30], fullname[40], age[30];
	char shirtstr[3];
	int shirt, wage;
	
	//global stats
	char apps[40],goals[40],assists[40],yellowc[30],redc[30];
	
	//Goalkeeper Stats
	char cleansheet[30],saves[40];
	
	//Defender stats
	char tackles[40],successfult[50];
	
	//Midfield stats
	char passes[50], ppg[90];
	
	//Attacker stats
	char shots[40], sot[40]; 
	
	//initial position on canvas
	int xi, yi;
	
	
	int xf, yf;
	int w, h, shrink;
	int smlpic;
	int lncontrolid, wagecontrolid;
} Players;

static int menuPanel,leaguePanel,contPanel,mainPanel,fxtrPanel,tctPanel,sqdPanel,wagePanel,addPanel,iconPanel,aboutPanel,editPanel;
int menuWidth, currentPanel=0, currentButton, pcanvasHeight, pcanvasWidth;
int totalplayers=0, totalplayed, ind, ind_current_player;
int field_bitmap, tactic_bitmap, profile_bitmap, icon_bitmap;
int state=0, UndoCounter=1;
int dragflg=1,paintflg=0,eraserflg=0;
int profile_changed=0,icon_changed=0;

Teams teams[20];
Fixtures fix[38];
Players *players;

int WhoIsHigher(const void * a, const void * b){
	
	const Teams a1 = *(const Teams *)a;
    const Teams b1 = *(const Teams *)b;
	
	if(a1.pts>b1.pts)
		return -1;
	else if(a1.pts<b1.pts) 
		return 1;
	else{
		if(a1.gd>b1.gd)
			return -1;
		else if(a1.gd<b1.gd) 
			return 1;
		else
			return 0;
	}
}

void getPlayersData(){
	FILE *plp;
	char line[200],filename[50];
	int plcounter=0;
	int len=2;

	players = (Players *) malloc (sizeof(Players)*2);
	
	plp = fopen ("Files\\Players\\Players Data.txt", "r");
	
	fgets(line,199,plp);
	while(fgets(line,199,plp)!=NULL){
		if( plcounter == len){
			len*=2;
			players = (Players *) realloc (players, sizeof(Players)*len);					  
		}
		
		sscanf (line, "%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%d",
				&players[plcounter].shirt, players[plcounter].fname,players[plcounter].lname,players[plcounter].age,
				players[plcounter].role,players[plcounter].apps,players[plcounter].goals,players[plcounter].assists,
			    players[plcounter].yellowc,players[plcounter].redc,players[plcounter].cleansheet,players[plcounter].saves,
			    players[plcounter].tackles,players[plcounter].successfult,players[plcounter].passes,players[plcounter].ppg,
			    players[plcounter].shots,players[plcounter].sot,&players[plcounter].wage);
		
		players[plcounter].yi=plcounter*PLAYER_SPACE;
		players[plcounter].xi=PLAYER_X-40;
		players[plcounter].yf=plcounter*PLAYER_SPACE;
		players[plcounter].xf=PLAYER_X-40;
		
		players[plcounter].h=ICON_N;
		players[plcounter].w=ICON_W;
		players[plcounter].shrink=2;
		
		
		sprintf(players[plcounter].shirtstr,"%d",players[plcounter].shirt);
		sprintf(players[plcounter].fullname, "%s %s",players[plcounter].fname,players[plcounter].lname);
		
		
		sprintf (filename, "Images\\players images\\icons\\%d.png",players[plcounter].shirt);
		GetBitmapFromFileEx (filename, 1, &players[plcounter].smlpic);
		plcounter++; 
	}
	totalplayers=plcounter;
	fclose(plp);
}

void preloadCanvas(){
	
	CanvasClear (tctPanel, TCT_PANEL_PLAYERS_CANVAS, VAL_ENTIRE_OBJECT);
	GetCtrlAttribute (tctPanel, TCT_PANEL_PLAYERS_CANVAS, ATTR_HEIGHT,&pcanvasHeight);
	GetCtrlAttribute (tctPanel, TCT_PANEL_PLAYERS_CANVAS, ATTR_WIDTH,&pcanvasWidth);
	GetBitmapFromFile ("Images\\Field.png", &field_bitmap);
	
	CanvasDrawBitmap (tctPanel, TCT_PANEL_PLAYERS_CANVAS, field_bitmap, VAL_ENTIRE_OBJECT, MakeRect (0, 0, pcanvasHeight, FIELD_W));
	
	for(int i=0;i<totalplayers;i++){	//draw players
		CanvasDrawBitmap (tctPanel, TCT_PANEL_PLAYERS_CANVAS, players[i].smlpic, 
						  MakeRect (ICON_H-players[i].h, 0, players[i].h, players[i].w), 
						  MakeRect (i*PLAYER_SPACE, PLAYER_X-40, players[i].h/players[i].shrink, players[i].w/players[i].shrink));
	}
	
	
}

void SquadRingSetup(){
	
	ClearListCtrl (sqdPanel, SQD_PANEL_RING);
	for(int i=0;i<totalplayers;i++){
		InsertListItem (sqdPanel, SQD_PANEL_RING, i, players[i].fullname, players[i].shirt);
	}
	  
}

int init(){
	//defining all the panels
	if ((mainPanel = LoadPanel (0, "Club Manager Tool.uir", MAIN_PANEL)) < 0)
		return -1;
	if ((menuPanel = LoadPanel (mainPanel, "Club Manager Tool.uir", MENU_PANEL)) < 0)
		return -1;
	if ((contPanel = LoadPanel (mainPanel, "Club Manager Tool.uir", CONT_PANEL)) < 0)
		return -1;
	if ((leaguePanel = LoadPanel (contPanel, "Club Manager Tool-LeagueTable.uir", LEAG_TBPAN)) < 0)
		return -1;
	if ((fxtrPanel = LoadPanel (contPanel, "Fixtures.uir", FXTR_PANEL)) < 0)
		return -1;
	if ((tctPanel = LoadPanel (contPanel, "Tactics.uir", TCT_PANEL)) < 0)
		return -1;
	if ((sqdPanel = LoadPanel (contPanel, "Squad.uir", SQD_PANEL)) < 0)
		return -1;
	if ((wagePanel = LoadPanel (0, "Squad.uir", WAGE_PANEL)) < 0)
		return -1;
	if ((iconPanel = LoadPanel (0, "Squad.uir", ICON_PANEL)) < 0)
		return -1;
	if ((aboutPanel = LoadPanel (0, "Club Manager Tool.uir", ABT_PANEL)) < 0)
		return -1;
	
	//setting neccesary attributes
	GetPanelAttribute (menuPanel, ATTR_WIDTH, &menuWidth);
	SetPanelAttribute (menuPanel, ATTR_LEFT, 0);
	SetPanelAttribute (contPanel, ATTR_LEFT, menuWidth);
	SetPanelAttribute (menuPanel, ATTR_TOP, 0);
	SetPanelAttribute (contPanel, ATTR_TOP, 0);
	SetPanelAttribute (contPanel, ATTR_WIDTH, 1250-menuWidth);
	
	//Displaying neccesary panels
	DisplayPanel (mainPanel);
	DisplayPanel (contPanel);
	DisplayPanel (menuPanel);
	
	currentButton = MENU_PANEL_HOME_MBUTTON;
	
	//bold on menu
	SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 1);
	
	//getting players info
	getPlayersData();
	
	//inserting all the players in the ring
	SquadRingSetup();
	
	//Display Field and Players on Tactic Canvas
	preloadCanvas();
	
	SetCtrlAttribute (tctPanel, TCT_PANEL_PAINT_CANVAS, ATTR_PEN_WIDTH, 5);
	
	return 0;
	
}

int CheckWDL(char *state){
	
	if (state[0] == 'W')
		return VAL_DK_GREEN;
	else if (state[0] == 'D')
		return VAL_LT_GRAY;
	else if(state[0] == 'L')
		return VAL_RED;
	else
		return VAL_TRANSPARENT;
	
}

int CheckRole(char *role){
	if (role[0] == 'G')
		return MakeColor(211,111,16);
	else if (role[0] == 'D')
		return MakeColor(15,122,255);
	else if(role[0] == 'M')
		return MakeColor(119,183,56);
	else
		return MakeColor(195,23,23);
}

void GenerateLeagueTable(){
	FILE *tbp;
	char line[200],filename[50];
	int y=1,x=1;
	
	tbp = fopen ("Files\\Standings\\Table.csv", "r");
	fgets(line,199,tbp);
	while(fgets(line,199,tbp)!=NULL && y<=20){
		
		sscanf (line, "%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%[^,],%[^,],%[^,],%[^,],%s",teams[y-1].name,
				&teams[y-1].played,&teams[y-1].won,&teams[y-1].drawn,&teams[y-1].lost,&teams[y-1].gf,&teams[y-1].ga,
				&teams[y-1].gd,&teams[y-1].pts,teams[y-1].f1,teams[y-1].f2,teams[y-1].f3,teams[y-1].f4,teams[y-1].f5);
		
		sprintf (filename, "Images\\team logos\\%s.png", teams[y-1].name);
		GetBitmapFromFileEx (filename, 0, &teams[y-1].logo);
		
		y++;
	}
	
	qsort (teams, 20, sizeof(Teams), WhoIsHigher);
	
	for(y=1,x=1;y<=20;y++){
		
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), y);
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), teams[y-1].logo);
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), teams[y-1].name);
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), teams[y-1].played);
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), teams[y-1].won);
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), teams[y-1].drawn);
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), teams[y-1].lost);
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), teams[y-1].gf);
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), teams[y-1].ga);
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), teams[y-1].gd);
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), teams[y-1].pts);
		
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x,y), teams[y-1].f1);
		SetTableCellAttribute (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), ATTR_TEXT_BGCOLOR, CheckWDL(teams[y-1].f1));
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x,y), teams[y-1].f2);
		SetTableCellAttribute (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), ATTR_TEXT_BGCOLOR, CheckWDL(teams[y-1].f2));
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x,y), teams[y-1].f3);
		SetTableCellAttribute (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), ATTR_TEXT_BGCOLOR, CheckWDL(teams[y-1].f3));
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x,y), teams[y-1].f4);
		SetTableCellAttribute (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), ATTR_TEXT_BGCOLOR, CheckWDL(teams[y-1].f4));
		SetTableCellVal (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x,y), teams[y-1].f5);
		SetTableCellAttribute (leaguePanel, LEAG_TBPAN_LEAGUETABLE, MakePoint(x++,y), ATTR_TEXT_BGCOLOR, CheckWDL(teams[y-1].f5));
	
		x=1;
	}
	
	fclose (tbp);
	
}

void GetFixtureInfo(){
	FILE *fxp;
	char line[200];
	int y=0;
	
	fxp = fopen ("Files\\Fixtures\\Fixtures.csv", "r");
	fgets(line,199,fxp);
	while(fgets(line,199,fxp)!=NULL && y<38){
		sscanf (line, "%d,%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d,%[^,],%d,%[^,],%d",&fix[y].match,fix[y].date,fix[y].gtime,
				fix[y].loc,fix[y].hteam,fix[y].ateam,&fix[y].hscore,&fix[y].ascore,fix[y].status,&fix[y].pos,fix[y].monthstr
				,&fix[y].month);
		
		sprintf (fix[y].result,"%d - %d" ,fix[y].hscore,fix[y].ascore);
		
		if(fix[y].hscore != -1){
		totalplayed =  fix[y].match;
		}
		
		y++;

						
	}
}

void GenerateGraphFxtr(){
	int y=1;
	int plotH_line,plotH_dot;
	
	DeleteGraphPlot (fxtrPanel, FXTR_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
	SetAxisRange (fxtrPanel, FXTR_PANEL_GRAPH, VAL_MANUAL, 0, totalplayed+1, VAL_NO_CHANGE, 0.0, 1.0);
	SetCtrlAttribute (fxtrPanel, FXTR_PANEL_GRAPH, ATTR_XDIVISIONS, totalplayed+1);
	
	while(y<totalplayed){
		plotH_line = PlotLine (fxtrPanel, FXTR_PANEL_GRAPH, fix[y-1].match, fix[y-1].pos, fix[y].match, fix[y].pos, VAL_OFFWHITE);
		SetPlotAttribute (fxtrPanel, FXTR_PANEL_GRAPH, plotH_line, ATTR_PLOT_THICKNESS, 2);
		y++;
	}
	
	for(y=1;y<=totalplayed;y++){
		plotH_dot = PlotPoint (fxtrPanel, FXTR_PANEL_GRAPH, fix[y-1].match, fix[y-1].pos, VAL_DOTTED_SOLID_CIRCLE, CheckWDL(fix[y-1].status));
	}
}

void PlayerDraw(){
	
	CanvasStartBatchDraw (tctPanel, TCT_PANEL_PLAYERS_CANVAS);
	
	CanvasClear (tctPanel, TCT_PANEL_PLAYERS_CANVAS, VAL_ENTIRE_OBJECT);
	
	CanvasDrawBitmap (tctPanel, TCT_PANEL_PLAYERS_CANVAS, field_bitmap, VAL_ENTIRE_OBJECT, MakeRect (0, 0, pcanvasHeight, FIELD_W));

	for(int i=0; i<totalplayers;i++){
				CanvasDrawBitmap (tctPanel, TCT_PANEL_PLAYERS_CANVAS, players[i].smlpic, 
						  MakeRect (ICON_H-players[i].h, 0, players[i].h, players[i].w), 
						  MakeRect (players[i].yf, players[i].xf, players[i].h/players[i].shrink, players[i].w/players[i].shrink));
			}
	
	
	CanvasEndBatchDraw (tctPanel, TCT_PANEL_PLAYERS_CANVAS);
}
	   
void PlayerUpdate(){
	 int x,y,left;
	 
	 if(state == 1){
		
		GetRelativeMouseState (tctPanel, TCT_PANEL_PLAYERS_CANVAS, &x, &y, &left, 0, 0); 
		
		if(left == 1 && ind >-1){  //left mouse down
		
			if(x+90>=pcanvasWidth)
				x=pcanvasWidth-90;
			if(x-45<=0)
				x=45;
			if(x<=450){
				if(y-52 <=0)
					y=52;
				if(y+52 >= pcanvasHeight)
					y=pcanvasHeight-52;
			}
			else {
				if(y-12 <=0)
					y=12;
				if(y+12 >= pcanvasHeight)
					y=pcanvasHeight-12;
			}
			
			if(x<450){
					players[ind].h=ICON_H;
					players[ind].shrink=4;
					players[ind].yf=y-players[ind].h/8;
					players[ind].xf=x-players[ind].w/8;
				
			}
			else{
					players[ind].h=ICON_N;
					players[ind].shrink=2;
					players[ind].yf=y-players[ind].h/4;
					players[ind].xf=x-players[ind].w/4;
				
			}
		}
			//left mouse up
		else{
			if(x>450){	   // return to origianl pos
				players[ind].yf=players[ind].yi;
				players[ind].xf=players[ind].xi;
				
				players[ind].h=ICON_N;
				players[ind].shrink=2;
				
				state =0;
			}
			else{
				state =0;
			}
		}			 
		
	 }
}

void CreateWagePdfPage(){
	char wagestr[20];
	char wageDate[20],wageTime[20];
	int wagepdf;
	int wages[totalplayers];
	
	DeleteGraphPlot (wagePanel, WAGE_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
	SetAxisRange (wagePanel, WAGE_PANEL_GRAPH, VAL_MANUAL, -1, totalplayers, VAL_NO_CHANGE, 0.0, 1.0);
	ClearAxisItems (wagePanel, WAGE_PANEL_GRAPH, VAL_BOTTOM_XAXIS);
	
	sprintf(wageDate,"%s",DateStr());
	sprintf(wageTime,"%s",TimeStr());
	
	SetCtrlVal (wagePanel, WAGE_PANEL_DATE_STR, wageDate);
	SetCtrlVal (wagePanel, WAGE_PANEL_TIME_STR, wageTime);
	
	for(int i=0;i<totalplayers;i++){
		players[i].lncontrolid = NewCtrl (wagePanel, CTRL_TEXT_MSG, "", 90+i*20, 87);
		CreateMetaFont ("wagefont", "Consolas", 15, 0, 0, 0, 0);
		SetCtrlAttribute (wagePanel, players[i].lncontrolid, ATTR_TEXT_FONT, "wagefont");
		SetCtrlVal (wagePanel, players[i].lncontrolid, players[i].lname);
		
		players[i].wagecontrolid = NewCtrl (wagePanel, CTRL_TEXT_MSG, "", 90+i*20, 250);
		SetCtrlAttribute (wagePanel, players[i].wagecontrolid, ATTR_TEXT_FONT, "wagefont");
		sprintf (wagestr, "%d Pounds", players[i].wage);
		SetCtrlVal (wagePanel, players[i].wagecontrolid, wagestr);
		
		InsertAxisItem (wagePanel, WAGE_PANEL_GRAPH, VAL_BOTTOM_XAXIS, -1, players[i].lname, i);
		wages[i]=players[i].wage;
	}
	
	PlotY (wagePanel, WAGE_PANEL_GRAPH, wages, totalplayers, VAL_UNSIGNED_INTEGER, VAL_BASE_ZERO_VERTICAL_BAR, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_RED);
	GetPanelDisplayBitmap (wagePanel, VAL_FULL_PANEL, VAL_ENTIRE_OBJECT, &wagepdf);
	SaveBitmapToJPEGFile (wagepdf,"WageReport.jpg", 0, 100);
	LaunchExecutable ("jpeg2pdf WageReport.jpg -o WageReport.pdf");
	Delay (0.5);
	OpenDocumentInDefaultViewer ("WageReport.pdf", VAL_NO_ZOOM);
	DeleteFile ("WageReport.jpg");
	DiscardBitmap (wagepdf);
	
}

int Validate(int num){
	int shirt,role;
	char fname[21],lname[21];
	
	switch (num){
		case 1:		
			{
			GetCtrlVal (addPanel, ADD_PANEL_SHIRT_RING, &shirt);
			GetCtrlVal (addPanel, ADD_PANEL_ROLE_RING, &role);
			GetCtrlVal (addPanel, ADD_PANEL_FIRST_NAME, fname);
			GetCtrlVal (addPanel, ADD_PANEL_LAST_NAME, lname);
	
			if(shirt <= 0 || role <= 0 || fname[0]=='\0' || lname[0] =='\0')
				return -1;
			else
				return 1;
			}
		case 2:		
			{
			GetCtrlVal (editPanel, EDIT_PL_PN_SHIRT_RING, &shirt);
			GetCtrlVal (editPanel, EDIT_PL_PN_ROLE_RING, &role);
			GetCtrlVal (editPanel, EDIT_PL_PN_FIRST_NAME, fname);
			GetCtrlVal (editPanel, EDIT_PL_PN_LAST_NAME, lname);
	
			if(shirt <= 0 || role <= 0 || fname[0]=='\0' || lname[0] =='\0')
				return -1;
			else
				return 1;
			}
		default:
			return 0;
	
	}
		
	
}

void DrawFixturesOnCanvas(){
	int month, counter=0;
	int fcanvasWidth;
	int fontsize=20;
	int aLogo,hLogo;
	int i=0;
	char filename[50];
	

		GetCtrlVal (fxtrPanel, FXTR_PANEL_MONTHS_RING, &month);
		GetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_WIDTH, &fcanvasWidth);
		
		CanvasStartBatchDraw (fxtrPanel, FXTR_PANEL_CANVAS);
		CanvasClear (fxtrPanel, FXTR_PANEL_CANVAS, VAL_ENTIRE_OBJECT);
			
		for ( i=0 ; i<38 ; i++){
			if(fix[i].month==month){
				if(counter == 0){
					//1st rect
					SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_TRANSPARENT);
					CanvasDrawRect (fxtrPanel, FXTR_PANEL_CANVAS, MakeRect (0, 0, 30, fcanvasWidth), VAL_DRAW_INTERIOR);
	
					//headline
					CreateMetaFont ("Golans Font", "Gadugi", fontsize, 1, 0, 0, 0);
					SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_COLOR, VAL_LT_GRAY);
					CanvasDrawTextAtPoint (fxtrPanel, FXTR_PANEL_CANVAS, fix[i].monthstr, "Golans Font", MakePoint(15,15), VAL_CENTER_LEFT);
					}
				//2nd rect
				SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_LT_GRAY);
				CanvasDrawRect (fxtrPanel, FXTR_PANEL_CANVAS, MakeRect (30+counter*80, 0, 30, fcanvasWidth), VAL_DRAW_INTERIOR);
		
				//date
				fontsize=11;
				CreateMetaFont ("Golans Font", "Gadugi", fontsize, 1, 0, 0, 0);
				SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_COLOR, VAL_DK_GRAY);
				CanvasDrawTextAtPoint (fxtrPanel, FXTR_PANEL_CANVAS, fix[i].date, "Golans Font", MakePoint(5,45+counter*80), VAL_CENTER_LEFT);
		
				//seperator
				SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_DK_GRAY);
				CanvasDrawRect (fxtrPanel, FXTR_PANEL_CANVAS, MakeRect (35+counter*80, 70, 20, 2), VAL_DRAW_INTERIOR);
		
				//location
				SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_TRANSPARENT);
				CanvasDrawTextAtPoint (fxtrPanel, FXTR_PANEL_CANVAS, fix[i].loc, "Golans Font", MakePoint(85,45+counter*80), VAL_CENTER_LEFT);
		
				//3rd rect
				SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_DK_GRAY);
				CanvasDrawRect (fxtrPanel, FXTR_PANEL_CANVAS, MakeRect (60+counter*80, 0, 50, fcanvasWidth), VAL_DRAW_INTERIOR);
		
				//Home team
				fontsize=15;
				CreateMetaFont ("Golans Font", "Gadugi", fontsize, 1, 0, 0, 0);
				SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_COLOR, VAL_OFFWHITE);
				CanvasDrawTextAtPoint (fxtrPanel, FXTR_PANEL_CANVAS, fix[i].hteam, "Golans Font", MakePoint(290,85+counter*80), VAL_CENTER_RIGHT);
		
				//Home Team Logo
				sprintf (filename, "Images\\team logos\\%s.png", fix[i].hteam);
				GetBitmapFromFileEx (filename, 0, &hLogo);
				CanvasDrawBitmap (fxtrPanel, FXTR_PANEL_CANVAS, hLogo , VAL_ENTIRE_OBJECT, MakeRect (75+counter*80, 295, 20, 20));
		
				//Result
				SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_FILL_COLOR, MakeColor(40,40,40));
				CanvasDrawRect (fxtrPanel, FXTR_PANEL_CANVAS, MakeRect (75+counter*80, 325, 20, 40), VAL_DRAW_INTERIOR);
				fontsize=13;
				CreateMetaFont ("Golans Font", "Gadugi", fontsize, 1, 0, 0, 0);
				SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_COLOR, VAL_OFFWHITE);
				if(fix[i].hscore!=-1)
					CanvasDrawTextAtPoint (fxtrPanel, FXTR_PANEL_CANVAS, fix[i].result, "Golans Font", MakePoint(331,85+counter*80), VAL_CENTER_LEFT);
		
				//Away Team Logo
				sprintf (filename, "Images\\team logos\\%s.png", fix[i].ateam);
				GetBitmapFromFileEx (filename, 0, &aLogo);
				CanvasDrawBitmap (fxtrPanel, FXTR_PANEL_CANVAS, aLogo, VAL_ENTIRE_OBJECT, MakeRect (75+counter*80, 375, 20, 20));
		
				//Away Team
				fontsize=15;
				CreateMetaFont ("Golans Font", "Gadugi", fontsize, 1, 0, 0, 0);
				SetCtrlAttribute (fxtrPanel, FXTR_PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_DK_GRAY);
				CanvasDrawTextAtPoint (fxtrPanel, FXTR_PANEL_CANVAS, fix[i].ateam, "Golans Font", MakePoint(400,85+counter*80), VAL_CENTER_LEFT);
		
				counter++;
			}
		}
		CanvasEndBatchDraw (fxtrPanel, FXTR_PANEL_CANVAS);

}

void SetDefaultPlayer(int num){
	char filename[50];

		SetCtrlVal (sqdPanel, SQD_PANEL_RING, players[num].shirt);
	
		SetCtrlVal (sqdPanel, SQD_PANEL_SHIRT, players[num].shirtstr);
		SetCtrlVal (sqdPanel, SQD_PANEL_FIRSTNAME, players[num].fname);
		SetCtrlVal (sqdPanel, SQD_PANEL_LASTNAME, players[num].lname);
		SetCtrlVal (sqdPanel, SQD_PANEL_AGE_NUM, players[num].age);
		SetCtrlVal (sqdPanel, SQD_PANEL_GOALS_NUM, players[num].goals);
		SetCtrlVal (sqdPanel, SQD_PANEL_ASSISTS_NUM, players[num].assists);
		SetCtrlVal (sqdPanel, SQD_PANEL_APP_NUM, players[num].apps);
		SetCtrlVal (sqdPanel, SQD_PANEL_YC_NUM, players[num].yellowc);
		SetCtrlVal (sqdPanel, SQD_PANEL_RC_NUM, players[num].redc);
		SetCtrlVal (sqdPanel, SQD_PANEL_POS_TXT, players[num].role);
	
		sprintf (filename, "Images\\players images\\%d.png",players[num].shirt);
		DisplayImageFile (sqdPanel, SQD_PANEL_PLAYER_PIC, filename);
	
		if(strcmp("GK",players[num].role)==0){
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_LBL, "CLEAN SHEETS");
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_NUM, players[num].cleansheet);
		
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_LBL, "SAVES");
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_NUM, players[num].saves);
		
		}
	
		if(strcmp("DF",players[num].role)==0){
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_LBL, "TACKLES");
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_NUM, players[num].tackles);
		
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_LBL, "SUCCESSFUL TACKLES");
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_NUM, players[num].successfult);		
		}
	
		if(strcmp("MF",players[num].role)==0){
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_LBL, "PASSES");
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_NUM, players[num].passes);
		
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_LBL, "PASSES PER GAME");
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_NUM, players[num].ppg);	
		}
	
		if(strcmp("FW",players[num].role)==0){
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_LBL, "SHOTS");
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_NUM, players[num].shots);
		
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_LBL, "SHOTS ON TARGET");
			SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_NUM, players[num].sot);	
		}

}

void DiscardAllBitmaps(){
	
	if(field_bitmap != 0)
		DiscardBitmap(field_bitmap);
	
	if(tactic_bitmap != 0)
		DiscardBitmap(tactic_bitmap);
	
	if(profile_bitmap != 0)
		DiscardBitmap(profile_bitmap);
	
	if(icon_bitmap != 0)
		DiscardBitmap(icon_bitmap);
	
	for(int i=0; i<20;i++){
		if(teams[i].logo != 0)
			DiscardBitmap(teams[i].logo);
	}
	
	for(int i=0; i<totalplayers;i++){
		if(players[i].smlpic != 0)
			DiscardBitmap(players[i].smlpic);
	}
}

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	
	if (init() == -1) 
		return -1;
	
	RunUserInterface ();
	DiscardAllBitmaps();
	DiscardPanel (mainPanel);
	DiscardPanel (aboutPanel);
	
	
	return 0;
}

int CVICALLBACK MainPanelFunc (int panel, int event, void *callbackData,
							   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK LeagueTablePanelFunc (int panel, int event, void *callbackData,
									  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}

int CVICALLBACK DisplayHome (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//unbold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 0);
			
			currentButton = MENU_PANEL_HOME_MBUTTON;
			 
			
			//bold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 1);
			
			if (currentPanel!=0)
			HidePanel(currentPanel);
			
			currentPanel = 0;
			
			break;
	}
	return 0;
}

int CVICALLBACK SalaryPanelDisplay (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

int CVICALLBACK DisplayTacticalPanel (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//unbold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 0);
			
			if(currentPanel != tctPanel){
				
				if (currentPanel!=0)
					HidePanel(currentPanel);
				
				//default mode
				dragflg=1;
				paintflg=0;
				eraserflg=0;
				SetCtrlAttribute (tctPanel, TCT_PANEL_ERASER_BUTTON, ATTR_DIMMED, 1);
				
				//timer enabled
				SetCtrlAttribute (tctPanel, TCT_PANEL_DRAG_TIMER, ATTR_ENABLED, 1);
				
				
				currentPanel =  tctPanel;
				currentButton =  MENU_PANEL_TACTICAL_MBUTTON;
			}
			
			//bold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 1);
			
			
			//setting the panel so it fits perfectly inside container panel
			SetPanelAttribute (tctPanel, ATTR_LEFT, 0);
			SetPanelAttribute (tctPanel, ATTR_TOP, 0);
			SetPanelAttribute (tctPanel, ATTR_WIDTH, 1250-menuWidth);  //width of main panel minus menu 
		
			DisplayPanel (tctPanel);
			break;
	}
	return 0;
}

int CVICALLBACK DisplayFixturesPanel (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//unbold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 0);
			
			if(currentPanel != fxtrPanel){
				
				if (currentPanel!=0)
				HidePanel(currentPanel);
				
				GetFixtureInfo();
				GenerateGraphFxtr();
				currentPanel =  fxtrPanel;
				currentButton =  MENU_PANEL_FIXTURES_MBUTTON;
			}
			
			//bold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 1);
			
			
			//setting the panel so it fits perfectly inside container panel
			SetPanelAttribute (fxtrPanel, ATTR_LEFT, 0);
			SetPanelAttribute (fxtrPanel, ATTR_TOP, 0);
			SetPanelAttribute (fxtrPanel, ATTR_WIDTH, 1250-menuWidth);  //width of main panel minus menu 
		
			DisplayPanel (fxtrPanel);
			break;
	}
	return 0;
}

int CVICALLBACK DisplayLeagueTablePanel (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//unbold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 0);
			
			if(currentPanel != leaguePanel){
				
				if (currentPanel!=0)
				HidePanel(currentPanel);
				
				GenerateLeagueTable();
				currentPanel =  leaguePanel;
				currentButton =  MENU_PANEL_LEAGUE_TABLE_MBUTTON;
			}
			
			//bold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 1);
			
			
			//setting the panel so it fits perfectly inside container panel
			SetPanelAttribute (leaguePanel, ATTR_LEFT, 0);
			SetPanelAttribute (leaguePanel, ATTR_TOP, 0);
			SetPanelAttribute (leaguePanel, ATTR_WIDTH, 1250-menuWidth);  //width of main panel minus menu 
			
			//seeting the table so it shows only the number needed of rows and cols
			SetCtrlAttribute (leaguePanel, LEAG_TBPAN_LEAGUETABLE, ATTR_NUM_VISIBLE_ROWS, 20);
			SetCtrlAttribute (leaguePanel, LEAG_TBPAN_LEAGUETABLE, ATTR_NUM_VISIBLE_COLUMNS, 16);
			
			//Transparent Frame
			SetCtrlAttribute (leaguePanel, LEAG_TBPAN_LEAGUETABLE, ATTR_FRAME_COLOR, VAL_TRANSPARENT);
			
			DisplayPanel (leaguePanel);
			break;
	}
	return 0;
}

int CVICALLBACK DisplaySquadPanel (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//unbold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 0);
			
			if(currentPanel != sqdPanel){
				
				if (currentPanel!=0)
					HidePanel(currentPanel);
				
				if(totalplayers>0)
					SetDefaultPlayer(0);
				
				currentPanel =  sqdPanel;
				currentButton =  MENU_PANEL_SQUAD_MBUTTON;
			}
			
			//bold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 1);
			
			
			//setting the panel so it fits perfectly inside container panel
			SetPanelAttribute (sqdPanel, ATTR_LEFT, 0);
			SetPanelAttribute (sqdPanel, ATTR_TOP, 0);
			SetPanelAttribute (sqdPanel, ATTR_WIDTH, 1250-menuWidth);  //width of main panel minus menu 
		
			DisplayPanel (sqdPanel);
			break;
	}
	return 0;
}

int CVICALLBACK menuFunc (int panel, int event, void *callbackData,
						  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:
			DisplayPanel (menuPanel);
			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}

int CVICALLBACK HomePanelFunc (int panel, int event, void *callbackData,
							   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}

int CVICALLBACK FixturesPanelFunc (int panel, int event, void *callbackData,
								   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}

int CVICALLBACK TacticsPanelFunc (int panel, int event, void *callbackData,
								  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}

int CVICALLBACK CanvasTacticsFunc (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
		int mx,my;
		int flag=1, i;
	
		switch (event)
	{
		case EVENT_LEFT_CLICK:
			
			if(dragflg){
			
				GetRelativeMouseState (tctPanel, TCT_PANEL_PLAYERS_CANVAS, &mx, &my, 0, 0, 0);
	
			
				ind=-1;
				for(i=0;i<totalplayers && flag!=0; i++){
					if(mx<450){
						if (mx>players[i].xf && mx<players[i].xf+(90) &&
							my>=players[i].yf && my<=players[i].yf+(102)){
							
								flag=0;
								ind=i;
						}
					}
					else{
						if (mx>players[i].xf && mx<players[i].xf+(180) &&
							my>=players[i].yf && my<=players[i].yf+(24)){
						
								flag=0;
								ind=i;
							}
						}
				}
			
				if (ind != -1){
					if(mx<450){
						if (mx>players[ind].xf && mx<players[ind].xf+(90) &&
							my>=players[ind].yf && my<=players[ind].yf+(102)) //player was pressed
						{
						state=1;
						}
					}	
					else{
						if (mx>players[ind].xf && mx<players[ind].xf+(180) &&
							my>=players[ind].yf && my<=players[ind].yf+(24)) //player was pressed
						{
							state=1;
				
						}
					}
				}
			}
		}
	return 0;
}

int CVICALLBACK DragTimerFunc (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			SetCtrlAttribute (tctPanel, TCT_PANEL_UNDO_BUTTON, ATTR_DIMMED, 1);
			PlayerDraw();
			PlayerUpdate();
			break;
	}
	return 0;
}

int CVICALLBACK DrawFixtures (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			DrawFixturesOnCanvas();
			break;
	}
	return 0;
}

int CVICALLBACK SquadPanelFunc (int panel, int event, void *callbackData,
								int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}

int CVICALLBACK ShowPlayers (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	int plshirt;
	char filename[50];
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (sqdPanel, SQD_PANEL_RING, &plshirt);
			
			for(int i=0; i<totalplayers; i++){
				if(plshirt == players[i].shirt){
					
					SetCtrlVal (sqdPanel, SQD_PANEL_SHIRT, players[i].shirtstr);
					SetCtrlVal (sqdPanel, SQD_PANEL_FIRSTNAME, players[i].fname);
					SetCtrlVal (sqdPanel, SQD_PANEL_LASTNAME, players[i].lname);
					SetCtrlVal (sqdPanel, SQD_PANEL_AGE_NUM, players[i].age);
					SetCtrlVal (sqdPanel, SQD_PANEL_GOALS_NUM, players[i].goals);
					SetCtrlVal (sqdPanel, SQD_PANEL_ASSISTS_NUM, players[i].assists);
					SetCtrlVal (sqdPanel, SQD_PANEL_APP_NUM, players[i].apps);
					SetCtrlVal (sqdPanel, SQD_PANEL_YC_NUM, players[i].yellowc);
					SetCtrlVal (sqdPanel, SQD_PANEL_RC_NUM, players[i].redc);
					SetCtrlVal (sqdPanel, SQD_PANEL_POS_TXT, players[i].role);
					
					
					
					sprintf (filename, "Images\\players images\\%d.png",players[i].shirt);
					DisplayImageFile (sqdPanel, SQD_PANEL_PLAYER_PIC, filename);
					
					if(strcmp("GK",players[i].role)==0){
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_LBL, "CLEAN SHEETS");
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_NUM, players[i].cleansheet);
						
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_LBL, "SAVES");
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_NUM, players[i].saves);
						
					}
					
					if(strcmp("DF",players[i].role)==0){
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_LBL, "TACKLES");
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_NUM, players[i].tackles);
						
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_LBL, "SUCCESSFUL TACKLES");
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_NUM, players[i].successfult);		
					}
					
					if(strcmp("MF",players[i].role)==0){
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_LBL, "PASSES");
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_NUM, players[i].passes);
						
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_LBL, "PASSES PER GAME");
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_NUM, players[i].ppg);	
					}
					
					if(strcmp("FW",players[i].role)==0){
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_LBL, "SHOTS");
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_1_NUM, players[i].shots);
						
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_LBL, "SHOTS ON TARGET");
						SetCtrlVal (sqdPanel, SQD_PANEL_ROLE_STAT_2_NUM, players[i].sot);	
					}
				}
			}
			break;
	}
	return 0;
}

int CVICALLBACK wagepanelFunc (int panel, int event, void *callbackData,
							   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			
			break;
	}
	return 0;
}

int CVICALLBACK GetWagePDF (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			CreateWagePdfPage();
			
			break;
	}
	return 0;
}

int CVICALLBACK OpenFixtureFile (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			OpenDocumentInDefaultViewer ("Files\\Fixtures\\Fixtures.csv", VAL_NO_ZOOM);
			break;
	}
	return 0;
}

int CVICALLBACK OpenTableFile (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			OpenDocumentInDefaultViewer ("Files\\Standings\\Table.csv", VAL_NO_ZOOM);
			break;
	}
	return 0;
}

int CVICALLBACK AddPlayerPanelFunc (int panel, int event, void *callbackData,
									int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			DiscardPanel (addPanel);
			break;
	}
	return 0;
}

int CVICALLBACK UploadIconPic (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	int status;
	char filename[1000];
	switch(event)
	{
		case EVENT_LEFT_CLICK:
		/*	switch(panel)
			{
				case addPanel: */
					status =  FileSelectPopupEx ("", "*.png", "*.png", "Select an image", VAL_LOAD_BUTTON, 0, 1, filename);
			
					if(status >=1){
						DisplayImageFile (addPanel, ADD_PANEL_ICONPIC, filename);
						DisplayImageFile (iconPanel, ICON_PANEL_ICONPIC, filename);
					}
				//	break;
					
			/*	case editPanel: 
					status =  FileSelectPopupEx ("", "*.png", "*.png", "Select an image", VAL_LOAD_BUTTON, 0, 1, filename);
			
					if(status >=1){
						DisplayImageFile (editPanel, EDIT_PL_PN_ICONPIC, filename);
						DisplayImageFile (iconPanel, ICON_PANEL_ICONPIC, filename);
					}
					break;  */
			//}
			break;
	}
	return 0;
}

int CVICALLBACK UploadProfilePic (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	int status;
	char filename[1000];
	switch(event)
	{
		case EVENT_LEFT_CLICK:
		/*	switch(panel)
			{
				case addPanel:*/
					status =  FileSelectPopupEx ("", "*.png", "*.png", "Select an image", VAL_LOAD_BUTTON, 0, 1, filename);
			
					if(status >=1){
						DisplayImageFile (addPanel, ADD_PANEL_PROFILEPIC, filename);
					}
				//break;
				
	/*			case editPanel:
					status =  FileSelectPopupEx ("", "*.png", "*.png", "Select an image", VAL_LOAD_BUTTON, 0, 1, filename);
			
					if(status >=1){
						DisplayImageFile (editPanel, EDIT_PL_PN_PROFILEPIC, filename);
					}
				break;
			}*/
			break;
			
	}
	return 0;
}

int CVICALLBACK AddPlayerFunc (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{   
	int validated=0;
	FILE *plp;
	Players newPlayer;
	char filename[100], shirtnum[3];
	int profile_bitmap,icon_bitmap;
	int age,role;

	switch (event)
	{
		case EVENT_COMMIT:
			validated = Validate(1);
			
			
			if(validated == 1){
				if(sizeof(players)/sizeof(Players) <totalplayers)
					players = (Players*) realloc (players, sizeof(Players)*totalplayers*2);
				
				GetCtrlVal (addPanel, ADD_PANEL_FIRST_NAME, newPlayer.fname);
				GetCtrlVal (addPanel, ADD_PANEL_LAST_NAME, newPlayer.lname);
				GetCtrlVal (addPanel, ADD_PANEL_SHIRT_RING, &newPlayer.shirt);
				GetCtrlVal (addPanel, ADD_PANEL_AGE, &age);
				GetCtrlVal (addPanel, ADD_PANEL_ROLE_RING, &role);
				GetCtrlVal (addPanel, ADD_PANEL_WAGE, &newPlayer.wage);
				
				switch(role)
				{
					case 1:
						sprintf(newPlayer.role,"GK");
						break;
					case 2:
						sprintf(newPlayer.role,"DF");
						break;
					case 3:
						sprintf(newPlayer.role,"MF");
						break;
					case 4:
						sprintf(newPlayer.role,"FW");
						break;
				}
				
				sprintf(filename, "Images\\players images\\%d.png",newPlayer.shirt);
				GetCtrlDisplayBitmap (addPanel, ADD_PANEL_PROFILEPIC, 0, &profile_bitmap);
				SaveBitmapToPNGFile (profile_bitmap, filename);
				
				//suiting data on icon
				SetCtrlVal (iconPanel, ICON_PANEL_ROLE, newPlayer.role);
				SetCtrlAttribute (iconPanel, ICON_PANEL_ROLE, ATTR_TEXT_COLOR, CheckRole(newPlayer.role));
				SetCtrlVal (iconPanel, ICON_PANEL_NAME, newPlayer.fname);
				sprintf(shirtnum,"%d",newPlayer.shirt);
				SetCtrlVal (iconPanel, ICON_PANEL_SHIRT, shirtnum);
			
				GetPanelDisplayBitmap (iconPanel, VAL_VISIBLE_AREA,MakeRect (28, 106, ICON_H, ICON_W) , &icon_bitmap);
				sprintf(filename, "Images\\players images\\icons\\%d.png",newPlayer.shirt);
				SaveBitmapToPNGFile (icon_bitmap, filename);
				
				plp = fopen ("Files\\Players\\Players Data.txt", "a");
				fprintf (plp,"%d,%s,%s,%d,%s,0,0,0,0,0,0,0,0,0,0,0,0,0,%d\n",
						 newPlayer.shirt,newPlayer.fname,newPlayer.lname,age,newPlayer.role,newPlayer.wage);
				
				fclose(plp);
				getPlayersData();
				SquadRingSetup();
				MessagePopup ("Message", "Player Added Successfully!");
				
				SetCtrlAttribute (sqdPanel, SQD_PANEL_DELETE_PLAYER, ATTR_DIMMED, 0);
				SetCtrlAttribute (sqdPanel, SQD_PANEL_EDIT_BUTTON, ATTR_DIMMED, 0);
				SetCtrlAttribute (sqdPanel, SQD_PANEL_WAGE_PDF, ATTR_DIMMED, 0);
				SetCtrlAttribute (sqdPanel, SQD_PANEL_RING, ATTR_DIMMED, 0);
				
				SetDefaultPlayer(totalplayers-1);
				DiscardPanel(addPanel);
				DisplayImageFile (iconPanel, ICON_PANEL_ICONPIC, "Images\\DefaultPlayerIcon.png");
			}
			else
				MessagePopup ("Message", "Please fill out all the areas");
			
			break;
	}
	return 0;
}

int CVICALLBACK OpenAddPanel (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	int freeShirts[99] ={0};
	int shirtCounter=0;
	int taken = 0;
	char shirtstr[3];
	switch (event)
	{
		case EVENT_COMMIT:
			if(totalplayers<= MAX_PLAYERS){
			
			//filling an array of all available shirts
			for(int i=0;i<99;i++){
				for( int j=0;j<totalplayers;j++){
					if(players[j].shirt==(i+1)){
						taken = 1;	
					}
				}
				if(taken == 0){
					freeShirts[shirtCounter]=i+1;
					shirtCounter++;
				}
				taken = 0;
				
			}
			
			if ((addPanel = LoadPanel (0, "Squad.uir", ADD_PANEL)) < 0)
				return -1;
			
			//filling available shirt ring
			ClearListCtrl (addPanel, ADD_PANEL_SHIRT_RING);
			InsertListItem (addPanel, ADD_PANEL_SHIRT_RING, -1, "- - PICK SHIRT # - -", 0); 
			
			for(int i=0;i<shirtCounter;i++){
				sprintf(shirtstr,"%d",freeShirts[i]);
				InsertListItem (addPanel, ADD_PANEL_SHIRT_RING, -1, shirtstr, freeShirts[i]);
			}
			
			DisplayPanel (addPanel);
			}
			else
				MessagePopup ("Message", "Max Players Limit reached (25)");
			break;
	}
	return 0;
}

int CVICALLBACK IconPanelFunc (int panel, int event, void *callbackData,
							   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}

int CVICALLBACK DeletePlayer (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	int response, i=0, delete;
	FILE* plp,*cpp;
	char line[200], icon[100], profile[100];
	
	switch (event)
	{
		case EVENT_COMMIT:
			if(totalplayers>1){
			response = ConfirmPopup ("Confirmation", "Are you sure you wanna delete this player?");
			
			if(response){		 //copying to temp file
				plp = fopen ("Files\\Players\\Players Data.txt", "r");
				cpp = fopen ("Files\\Players\\temp.txt", "w");
				GetCtrlVal (sqdPanel, SQD_PANEL_RING, &delete);
				
				fgets(line,199,plp);
				fputs (line, cpp);
				while(fgets(line,199,plp)!=NULL){
					if(delete == players[i].shirt){
						sprintf(icon,"Images\\players images\\icons\\%d.png", players[i].shirt);
						sprintf(profile,"Images\\players images\\%d.png", players[i].shirt);
						i++;
					}
					else{
						fputs (line, cpp);
						i++;
					}
				}
				
				fclose(plp);
				fclose(cpp);
				
				DeleteFile (icon);
				DeleteFile (profile);
				DeleteFile ("Files\\Players\\Players Data.txt");
				
				Delay(0.5);
				rename ("Files\\Players\\temp.txt", "Files\\Players\\Players Data.txt");
				
				totalplayers--;
				
				getPlayersData();
				SquadRingSetup();
				
				MessagePopup ("Message", "Player Deleted!!");
				
				if(totalplayers>0)
					SetDefaultPlayer(0);
			}
			}
			else{
				MessagePopup ("Message", "there must be at least 1 player");
			}
			break;
	}
	return 0;
}

int CVICALLBACK ResetCanvas (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(dragflg){
				for(int i=0;i<totalplayers;i++){
					players[i].h=ICON_N;
					players[i].shrink=2;
					players[i].xf=players[i].xi;
					players[i].yf=players[i].yi;
				}
				preloadCanvas();
			}
			if(paintflg==1 || eraserflg==1){
			CanvasClear (tctPanel, TCT_PANEL_PAINT_CANVAS, VAL_ENTIRE_OBJECT);
			SetCtrlAttribute (tctPanel, TCT_PANEL_UNDO_BUTTON, ATTR_DIMMED, 1);
			UndoCounter=1;
			}
			break;
	}
	return 0;
}

int CVICALLBACK SwitchtoDrag (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			dragflg=1;
			paintflg=0;
			eraserflg=0;
			
			if(UndoCounter >1)
				DeleteFile ("tacticstate");
			
			UndoCounter=1;
			
			SetCtrlAttribute (tctPanel, TCT_PANEL_DRAG_TIMER, ATTR_ENABLED, 1);
			//paint hidden
			SetCtrlAttribute (tctPanel, TCT_PANEL_PAINT_CANVAS, ATTR_VISIBLE, 0);
			
			SetCtrlAttribute (tctPanel, TCT_PANEL_ERASER_BUTTON, ATTR_DIMMED, 1); 
			
			break;
	}
	return 0;
}

int CVICALLBACK SwitchtoPaint (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			dragflg=0;
			paintflg=1;
			eraserflg=0;
			SetCtrlAttribute (tctPanel, TCT_PANEL_DRAG_TIMER, ATTR_ENABLED, 0);
			//paint visible
			SetCtrlAttribute (tctPanel, TCT_PANEL_PAINT_CANVAS, ATTR_VISIBLE, 1);
			
			SetCtrlAttribute (tctPanel, TCT_PANEL_ERASER_BUTTON, ATTR_DIMMED, 0);
			
			if(UndoCounter-1>0){
				SetCtrlAttribute (tctPanel, TCT_PANEL_UNDO_BUTTON, ATTR_DIMMED, 0);
			}
			break;
	}
	return 0;
}

int CVICALLBACK UndoFunc (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(UndoCounter-1 > 0){
				RecallPanelState (tctPanel, "tacticstate", UndoCounter-1);
				UndoCounter--;
			}
			break;
		case EVENT_LEFT_CLICK_UP:
				if(UndoCounter-1 <= 1)
					SetCtrlAttribute (tctPanel, TCT_PANEL_UNDO_BUTTON, ATTR_DIMMED, 1);
			break;
	}
	return 0;
}

int CVICALLBACK SizeAndColor (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	int size, color;
	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlVal (tctPanel, TCT_PANEL_PEN_SIZE, &size);
			GetCtrlVal (tctPanel, TCT_PANEL_PICK_COLOR, &color);
			
			SetCtrlAttribute (tctPanel, TCT_PANEL_PAINT_CANVAS, ATTR_PEN_WIDTH, size);
			SetCtrlAttribute (tctPanel, TCT_PANEL_PAINT_CANVAS, ATTR_PEN_COLOR, color);
			break;
	}
	return 0;
}

int CVICALLBACK PaintCanvasFunc (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{   	int pmx,pmy;
		static int lastx,lasty;
		int size;
		int leftpressed;
	
		switch (event)
	{
		case EVENT_LEFT_CLICK_UP:
			
			//undimming the undo button
			SetCtrlAttribute (tctPanel, TCT_PANEL_UNDO_BUTTON, ATTR_DIMMED, 0);
			
			break;
			
		case EVENT_LEFT_CLICK:
			
			SavePanelState (tctPanel, "tacticstate", UndoCounter);
			UndoCounter++;
			
			break;
			
		case EVENT_MOUSE_POINTER_MOVE:
			
			if(paintflg){
				
				GetRelativeMouseState (tctPanel, TCT_PANEL_PAINT_CANVAS, &pmx, &pmy, &leftpressed, 0, 0);
				
				if (leftpressed)
				{
				CanvasDrawPoint (tctPanel, TCT_PANEL_PAINT_CANVAS, MakePoint(pmx,pmy));
				lastx=pmx;
				lasty=pmy;
				}
				
			}
			
			if(eraserflg){
				
				GetCtrlVal (tctPanel, TCT_PANEL_PEN_SIZE, &size);
			 	GetRelativeMouseState (tctPanel, TCT_PANEL_PAINT_CANVAS, &pmx, &pmy, &leftpressed, 0, 0);
				
				if (leftpressed){
				CanvasClear (tctPanel, TCT_PANEL_PAINT_CANVAS, MakeRect (pmy,pmx ,size ,size ));
				}
			}
			
			break;
	}
	return 0;
}

int CVICALLBACK SwitchtoEraser (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			eraserflg=1;
			paintflg=0;
			dragflg=0;
			
			SetCtrlAttribute (tctPanel, TCT_PANEL_DRAG_TIMER, ATTR_ENABLED, 0);
			//paint visible
			SetCtrlAttribute (tctPanel, TCT_PANEL_PAINT_CANVAS, ATTR_VISIBLE, 1);
			break;
	}
	return 0;
}

int CVICALLBACK SaveTacticsFunc (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	int filestatus;
	char filename[100], executable[100], savename[100];
	switch (event)
	{
		case EVENT_COMMIT:
			
			filestatus = PromptPopup ("Save as:", "Enter the name of the file:", filename, 20);
			
			if(filestatus == 0)
			{
				GetPanelDisplayBitmap (tctPanel, VAL_VISIBLE_AREA, MakeRect (4, 440, 638, 450), &tactic_bitmap);
				sprintf(savename,"%s.jpg",filename);
				SaveBitmapToJPEGFile (tactic_bitmap, savename, 0, 100);
				sprintf(executable,"jpeg2pdf %s.jpg -o %s.pdf",filename,filename);
				system (executable);
				DeleteFile (savename);
				
			}
			break;
	}
	return 0;
}

int CVICALLBACK OpenPdfTactics (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	int filestatus;
	char filename[1000];
	switch (event)
	{
		case EVENT_COMMIT:
			
			filestatus = FileSelectPopup ("", "*.pdf", "*.pdf", "Load Tactic", VAL_LOAD_BUTTON, 0, 0, 1, 0, filename);
			
			if(filestatus >= 1)
			{
			OpenDocumentInDefaultViewer (filename, VAL_NO_ZOOM);
			}
			break;
	}
	return 0;
}

int CVICALLBACK RefreshFxtrPage (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetFixtureInfo();
			GenerateGraphFxtr();
			DrawFixturesOnCanvas();
			break;
	}
	return 0;
}

int CVICALLBACK RefreshTable (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlAttribute (leaguePanel, LEAG_TBPAN_REFRESH_TXT, ATTR_VISIBLE, 1);
			SetCtrlAttribute (leaguePanel, LEAG_TBPAN_EDIT_BUTTON, ATTR_DIMMED, 1);
			SetCtrlAttribute (leaguePanel, LEAG_TBPAN_REFRESH_BUTTON, ATTR_DIMMED, 1);
			GenerateLeagueTable();
			SetCtrlAttribute (leaguePanel, LEAG_TBPAN_REFRESH_BUTTON, ATTR_DIMMED, 0);
			SetCtrlAttribute (leaguePanel, LEAG_TBPAN_EDIT_BUTTON, ATTR_DIMMED, 0);
			SetCtrlAttribute (leaguePanel, LEAG_TBPAN_REFRESH_TXT, ATTR_VISIBLE, 0);
			break;
	}
	return 0;
}

int CVICALLBACK OpenEditPanel (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	int freeShirts[99] ={0};
	int shirtCounter=0;
	int taken = 0;
	int currentPlayer, role=0, i;
	char shirtstr[3],filename1[40],filename2[40], stacklesstr[4];
	
	int age,apps,goals,assists,yc,rc,clns,saves,tackles,stackles,passes,shots,sot;
	double ppg;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			if ((editPanel = LoadPanel (0, "Squad.uir", EDIT_PL_PN)) < 0)
				return -1;
			
			GetCtrlVal (sqdPanel, SQD_PANEL_RING, &currentPlayer);
			
			
			//filling an array of all available shirts including the current player's shirt
			for(i=0;i<99;i++){
				for( int j=0;j<totalplayers;j++){
					if(players[j].shirt==(i+1) && players[j].shirt != currentPlayer){
						taken = 1;	
					}
				}
				if(taken == 0){
					freeShirts[shirtCounter]=i+1;
					shirtCounter++;
				}
				taken = 0;
				
			}
			
			//filling available shirt ring
			ClearListCtrl (editPanel, EDIT_PL_PN_SHIRT_RING);
			InsertListItem (editPanel, EDIT_PL_PN_SHIRT_RING, -1, "- - PICK SHIRT # - -", 0); 
			
			for(i=0;i<shirtCounter;i++){
				sprintf(shirtstr,"%d",freeShirts[i]);
				InsertListItem (editPanel, EDIT_PL_PN_SHIRT_RING, -1, shirtstr, freeShirts[i]);
			}
			
			// finding the index of the current player
			for(i=0; players[i].shirt != currentPlayer; i++);
			ind_current_player = i;
			
			//checking the role of the current player
			if(players[i].role[0]=='G')
				role=1;
			if(players[i].role[0]=='D')
				role=2;
			if(players[i].role[0]=='M')
				role=3;
			if(players[i].role[0]=='F')
				role=4;
				
			
			//getting players pictures locations
			sprintf(filename1,"Images\\players images\\icons\\%d.png",currentPlayer);
			sprintf(filename2,"Images\\players images\\%d.png",currentPlayer);
			
			
			//convert from string to int
			sscanf (players[i].age, "%d", &age);
			sscanf (players[i].apps, "%d", &apps);	
			sscanf (players[i].goals, "%d", &goals);
			sscanf (players[i].assists, "%d", &assists);
			sscanf (players[i].yellowc, "%d", &yc);
			sscanf (players[i].redc, "%d", &rc);
			sscanf (players[i].cleansheet, "%d", &clns);
			sscanf (players[i].saves, "%d", &saves);
			sscanf (players[i].tackles, "%d", &tackles);
			sscanf (players[i].successfult, "%[^%,]", stacklesstr);
			sscanf (stacklesstr, "%d", &stackles);
			sscanf (players[i].passes, "%d", &passes);
			sscanf (players[i].ppg, "%lf", &ppg);
			sscanf (players[i].shots, "%d", &shots);
			sscanf (players[i].sot, "%d", &sot);
			
			
			//setting current values on panel
			SetCtrlVal (editPanel, EDIT_PL_PN_LAST_NAME, players[i].lname);
			SetCtrlVal (editPanel, EDIT_PL_PN_FIRST_NAME, players[i].fname);
			SetCtrlVal (editPanel, EDIT_PL_PN_WAGE, players[i].wage);
			SetCtrlVal (editPanel, EDIT_PL_PN_RED, rc);
			SetCtrlVal (editPanel, EDIT_PL_PN_YELLOW, yc);
			SetCtrlVal (editPanel, EDIT_PL_PN_SHOTS_ON_TARGET, sot);
			SetCtrlVal (editPanel, EDIT_PL_PN_SHOTS, shots);
			SetCtrlVal (editPanel, EDIT_PL_PN_PASSES_PER_MATCH, ppg);
			SetCtrlVal (editPanel, EDIT_PL_PN_PASSES, passes);
			SetCtrlVal (editPanel, EDIT_PL_PN_TACKLE_SUCCESS, stackles);
			SetCtrlVal (editPanel, EDIT_PL_PN_TACKLES, tackles);
			SetCtrlVal (editPanel, EDIT_PL_PN_CLEAN_SHEET, clns);
			SetCtrlVal (editPanel, EDIT_PL_PN_SAVES, saves);
			SetCtrlVal (editPanel, EDIT_PL_PN_GOALS, goals);
			SetCtrlVal (editPanel, EDIT_PL_PN_ASSISTS, assists);
			SetCtrlVal (editPanel, EDIT_PL_PN_APPS, apps);
			SetCtrlVal (editPanel, EDIT_PL_PN_AGE, age);
			SetCtrlVal (editPanel, EDIT_PL_PN_ROLE_RING, role);
			SetCtrlVal (editPanel, EDIT_PL_PN_SHIRT_RING, currentPlayer);
			DisplayImageFile (editPanel, EDIT_PL_PN_ICONPIC, filename1);
			DisplayImageFile (editPanel, EDIT_PL_PN_PROFILEPIC, filename2);
			
			DisplayPanel (editPanel);
			break;
	}
	return 0;
}

int CVICALLBACK EditPlayerPanelFunc (int panel, int event, void *callbackData,
									 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			DiscardPanel(editPanel);
			break;
	}
	return 0;
}

int CVICALLBACK EditPlayerData (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	int validated=0 ,i,oldshirt;
	int age,apps,goals,assists,yc,rc,clns,saves,tackles,stackles,passes,shots,sot,shirt,role;
	double ppg;
	char filename[100],filename2[100];
	
	FILE* plp;
	
	switch (event)
	{
		case EVENT_COMMIT:
			validated = Validate(2);
			i = ind_current_player;
			oldshirt=players[i].shirt;
			
			if(validated == 1){
				GetCtrlVal (editPanel, EDIT_PL_PN_LAST_NAME, players[i].lname);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_FIRST_NAME, players[i].fname);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_WAGE, &players[i].wage);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_RED, &rc);
				sprintf(players[i].redc,"%d",rc);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_YELLOW, &yc);
				sprintf(players[i].yellowc,"%d",yc); 
				
				GetCtrlVal (editPanel, EDIT_PL_PN_SHOTS_ON_TARGET, &sot);
				sprintf(players[i].sot,"%d",sot);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_SHOTS, &shots);
				sprintf(players[i].shots,"%d",shots);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_PASSES_PER_MATCH, &ppg);
				sprintf(players[i].ppg,"%lf",ppg);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_PASSES, &passes);
				sprintf(players[i].passes,"%d",passes);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_TACKLE_SUCCESS, &stackles);
				sprintf(players[i].successfult,"%d%%",stackles);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_TACKLES, &tackles);
				sprintf(players[i].tackles,"%d",tackles);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_CLEAN_SHEET, &clns);
				sprintf(players[i].cleansheet,"%d",clns);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_SAVES, &saves);
				sprintf(players[i].saves,"%d",saves);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_GOALS, &goals);
				sprintf(players[i].goals,"%d",goals);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_ASSISTS, &assists);
				sprintf(players[i].assists,"%d",assists);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_APPS, &apps);
				sprintf(players[i].apps,"%d",apps);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_AGE, &age);
				sprintf(players[i].age,"%d",age);
				
				GetCtrlVal (editPanel, EDIT_PL_PN_ROLE_RING, &role);
				switch(role){
					case 1:
						sprintf(players[i].role,"GK");
						break;
					case 2:
						sprintf(players[i].role,"DF");
						break;
					case 3:
						sprintf(players[i].role,"MF");
						break;
					case 4:
						sprintf(players[i].role,"FW");
						break;
				}
				
				GetCtrlVal (editPanel, EDIT_PL_PN_SHIRT_RING, &shirt);
				sprintf(players[i].shirtstr,"%d",shirt);
				GetCtrlVal (editPanel, EDIT_PL_PN_SHIRT_RING, &players[i].shirt);
				
				if(profile_changed == 1){
					sprintf(filename2, "Images\\players images\\%d.png",oldshirt);
					DeleteFile(filename2);
					sprintf(filename, "Images\\players images\\%d.png",players[i].shirt);
					GetCtrlDisplayBitmap (addPanel, ADD_PANEL_PROFILEPIC, 0, &profile_bitmap);
					SaveBitmapToPNGFile (profile_bitmap, filename);
				}
				else{
					sprintf(filename2, "Images\\players images\\%d.png",oldshirt);
					sprintf(filename, "Images\\players images\\%d.png",players[i].shirt);
					rename(filename2,filename);		
				}
				
				//suiting data on icon
				 
					SetCtrlVal (iconPanel, ICON_PANEL_ROLE, players[i].role);
					SetCtrlAttribute (iconPanel, ICON_PANEL_ROLE, ATTR_TEXT_COLOR, CheckRole(players[i].role));
					SetCtrlVal (iconPanel, ICON_PANEL_NAME, players[i].fname);
					SetCtrlVal (iconPanel, ICON_PANEL_SHIRT, players[i].shirtstr);
					
				if(icon_changed == 1){
					GetPanelDisplayBitmap (iconPanel, VAL_VISIBLE_AREA,MakeRect (28, 106, ICON_H, ICON_W) , &icon_bitmap);
					sprintf(filename2, "Images\\players images\\icons\\%d.png",oldshirt);
					DeleteFile(filename2);
					sprintf(filename, "Images\\players images\\icons\\%d.png",players[i].shirt);
					SaveBitmapToPNGFile (icon_bitmap, filename);
				}
				else{
					sprintf(filename2, "Images\\players images\\icons\\%d.png",oldshirt);
					sprintf(filename, "Images\\players images\\icons\\%d.png",players[i].shirt);
					rename(filename2,filename);
				}
				
			plp = fopen ("Files\\Players\\Players Data.txt", "w");
			
			fprintf(plp,"shirt,first name,last name,age,role,app,goals,assists,yellow,red,clean sheets,saves,tackles,tackles success,Passes,Passes p.m,shots,shots on target,Weekly wage\n");
			
			for(int j=0; j<totalplayers; j++){
				fprintf (plp,"%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n",
						 players[j].shirt,players[j].fname,players[j].lname,players[j].age,players[j].role,players[j].apps
						 ,players[j].goals,players[j].assists,players[j].yellowc,players[j].redc,players[j].cleansheet
						 ,players[j].saves,players[j].tackles,players[j].successfult,players[j].passes,players[j].ppg
						 ,players[j].shots,players[j].sot,players[j].wage);
				}
				
			fclose(plp);
			getPlayersData();
			SquadRingSetup();
			MessagePopup ("Message", "Edit Finsihed");
			SetDefaultPlayer(i);
			DiscardPanel(editPanel);
			icon_changed=0; 
			profile_changed=0;
			DisplayImageFile (iconPanel, ICON_PANEL_ICONPIC, "Images\\DefaultPlayerIcon.png");
				
			}
			else{
			MessagePopup ("Message", "Please fill out all the areas");	
			}
			break;
	}
	return 0;
}
	
int CVICALLBACK DisplayAboutPanel (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//unbold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 0);
			
			if(currentPanel != aboutPanel){
				
				if (currentPanel!=0)
					HidePanel(currentPanel);
			
				currentPanel =  aboutPanel;
				currentButton =  MENU_PANEL_ABOUT_MBUTTON;
			}
			
			//bold
			SetCtrlAttribute (menuPanel, currentButton, ATTR_LABEL_BOLD, 1);
		
			DisplayPanel (aboutPanel);
			break;
	}
	return 0;
}

int CVICALLBACK AboutPanelfumc (int panel, int event, void *callbackData,
								int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			HidePanel(aboutPanel);
			break;
	}
	return 0;
}

int CVICALLBACK E_UploadIconPic (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	int status;
	char filename[1000];
	switch(event)
	{
		case EVENT_LEFT_CLICK:
			status =  FileSelectPopupEx ("", "*.png", "*.png", "Select an image", VAL_LOAD_BUTTON, 0, 1, filename);
	
			if(status >=1){
				DisplayImageFile (editPanel, EDIT_PL_PN_ICONPIC, filename);
				DisplayImageFile (iconPanel, ICON_PANEL_ICONPIC, filename);
				icon_changed=1;
			}
			break;
	}
	return 0;
}

int CVICALLBACK E_UploadProfilePic (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	int status;
	char filename[1000];
	switch(event)
	{
		case EVENT_LEFT_CLICK:
			status =  FileSelectPopupEx ("", "*.png", "*.png", "Select an image", VAL_LOAD_BUTTON, 0, 1, filename);
	
			if(status >=1){
				DisplayImageFile (editPanel, EDIT_PL_PN_PROFILEPIC, filename);
				profile_changed=1;
			}
			break;
			
	}
	return 0;
}

int CVICALLBACK OpenVideo (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			OpenDocumentInDefaultViewer ("MU Club Manager tool.mp4", VAL_NO_ZOOM);
			break;
	}
	return 0;
}

int CVICALLBACK OpenHelpDoc (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			OpenDocumentInDefaultViewer ("help.pdf", VAL_NO_ZOOM);
			break;
	}
	return 0;
}
