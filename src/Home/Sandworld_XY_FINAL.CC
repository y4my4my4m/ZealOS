// Set game palette
#include "PaletteEditor/Palettes/Sand"
PaletteSetSand(FALSE);

#define DROPS_NUM 			0x60000
#define GRAVITY				9.87
#define WIN_HEIGHT			GR_HEIGHT
#define WIN_WIDTH			GR_WIDTH

class Drop
{
	I32 id, x, y;
	I32 life, velocity;
	I32 velocity_x, velocity_y;
	CColorROPU32 color;
	Bool has_been_updated;
} drops[WIN_HEIGHT][WIN_WIDTH];

class Player
{
	I32 id, x, y;
	I32 life, velocity;
	I32 velocity_x, velocity_y;
	CColorROPU32 color;
	Bool has_been_updated;
	Bool grounded;
} players[0];


I64 debug_x;
I64 debug_y;

I64	rain_amount = 2000;
I64 drop_count  = 0;
CDC *cur_dc;
I64 sc_1, sc_2, sc_3, sc_4, sc_5, sc_6, sc_w, sc_a, sc_d, sc_s, selection = 0;

U0 PlayerController()
{

	if (Bt(kbd.down_bitmap, sc_a))
	{
		players[0].velocity_x -= 1;
	}	
	if (Bt(kbd.down_bitmap, sc_d))
	{	
		players[0].velocity_x += 1;
	}	
	if (Bt(kbd.down_bitmap, sc_w))
	{
		if(players[0].grounded)
		{
			players[0].grounded		= FALSE;
			players[0].velocity_y -= 10;
		}
	}
	if (Bt(kbd.down_bitmap, sc_s))
	{
		players[0].grounded = FALSE;
		players[0].y += 5;
		players[0].velocity_y = 5;
	}
}

U0 DrawMouse(CDC *dc, I64 x, I64 y)
{
	if(selection==0)
		dc->color=LTGRAY;
	else if(selection==1)
		dc->color=YELLOW;
	else if(selection==2)
		dc->color=BLUE;
	else if(selection==3)
		dc->color=BROWN;
	dc->thick = 3;
	dc->flags &= ~(DCF_TRANSFORMATION | DCF_SYMMETRY);
	GrRect(dc, x, y, 5, 5);
}

U0 UpdateCurBase(CTask *task, CDC * dc)
{
	// why cur_dc?
	cur_dc->color = ROP_EQU;
	dc->flags |= DCF_NO_TRANSPARENTS;
	GrBlot(cur_dc, -task->pix_left - task->scroll_x, -task->pix_top - task->scroll_y, dc);
	dc->flags &= ~DCF_NO_TRANSPARENTS;
}


// U0 UpdateSmoke(I64 i, I64 w, I64 h)
// {
// 	I64 u, l, r, ul, ur, x0, x1, y0, y1;

// 	x0 = x1 = drops[i].x;
// 	y0 = y1 = drops[i].y;

// 	// down peek
// 	u = GrPeek(cur_dc, x0, y0 - 1);
// 	// if down is empty
// 	if (u == WHITE)
// 	{
// 		if (RandI16 >= 200)
// 			y1--;
// 		if (RandI16 >= 250)
// 			x1 += RandU32%10;
// 		else
// 			x1 -= RandU32%10;
// 	}
// 	else {		
// 		l  = GrPeek(cur_dc, x0 - 1, y0);
// 		r  = GrPeek(cur_dc, x0 + 1, y0);
// 		ul = GrPeek(cur_dc, x0 - 1, y0 - 1);
// 		ur = GrPeek(cur_dc, x0 + 1, y0 - 1);
// 		// if up-left empty
// 		if (ul == WHITE)
// 		{
// 			x1 -= RandU32%2;
// 			y1--;
// 		}	
// 		// if up-right empty
// 		else if (ur == WHITE)
// 		{	
// 			x1 += RandU32%2;
// 			y1--;
// 		}
// 		else if (l == WHITE)
// 			x1 -= RandU32%2;
// 		else if (r == WHITE)
// 			x1 += RandU32%2;
// 	}

// 	if (GrPeek(cur_dc, x0, y0) == drops[i].color)
// 	{
// 		cur_dc->color = WHITE;
// 		GrPlot(cur_dc, x0, y0);
// 	}

// 	cur_dc->color = drops[i].color;
// 	GrPlot(cur_dc, x1, y1);
// 	drops[i].x = x1;
// 	drops[i].y = y1;
// }

// U0 UpdateFire(I64 i, I64 w, I64 h)
// {
// 	I64 c, l, r, d, u, ul, ur, dl, dr, x0, x1, y0, y1, v;

// 	x0 = x1 = drops[i].x;
// 	y0 = y1 = drops[i].y;

// 	// down peek
// 	c  = GrPeek(cur_dc, x0, y0);
// 	d  = GrPeek(cur_dc, x0, y0 + 1);
// 	l  = GrPeek(cur_dc, x0 - 1, y0);
// 	r  = GrPeek(cur_dc, x0 + 1, y0);
// 	dl = GrPeek(cur_dc, x0 - 1, y0 + 1);
// 	dr = GrPeek(cur_dc, x0 + 1, y0 + 1);

// 	if (d == WHITE)
// 	{
// 		if (RandI16 >= 500)
// 			y1++;
// 		if (RandI16 >= 50)
// 			x1 += RandU32%2;
// 		else
// 			x1 -= RandU32%2;
// 	}

// 	// if down is empty
// 	if (d == WHITE)
// 		y1++;
// 	// if down-left empty
// 	else if (dl == WHITE)
// 	{
// 		x1 -= RandU16%2;
// 		y1++;
// 	}	
// 	// if down-right empty
// 	else if (dr == WHITE)
// 	{	
// 		x1 += RandU16%2;
// 		y1++;
// 	}
// 	// clean up previous pos
// 	if (c == drops[i].color)
// 	{
// 		cur_dc->color = WHITE;
// 		GrPlot(cur_dc, x0, y0);
// 	}

// 	// if down is empty
// 	if (d == BROWN)
// 	{
// 		if(RandU16 <= 2000)
// 		{
// 			y1++;

// 			// if down-left empty
// 			if (dl == BROWN)
// 			{
// 				if(RandU16 <= 200)
// 					x1--;
// 					y1++;
// 			}	
// 			// if down-right empty
// 			else if (dr == BROWN)
// 			{	
// 				x1++;
// 				y1++;
// 			}

// 		}
// 	}
// 	if (c == BROWN)
// 	{
// 		cur_dc->color = WHITE;
// 		GrPlot(cur_dc, x0, y0);
// 	}

// 	cur_dc->color = drops[i].color;
// 	GrPlot(cur_dc, x1, y1);
// 	drops[i].x = x1;
// 	drops[i].y = y1;
// }

U0 Erase(I64 x, I64 y)
{
	drops[y][x].id = NULL;
	drops[y][x].color = WHITE;
	drops[y][x].has_been_updated = TRUE;
}

U0 RenderDrop(I64 x1, I64 y1, I64 x, I64 y)
{
	drops[y1][x1].id 		 = drops[y][x].id;
	drops[y1][x1].color		 = drops[y][x].color;
	drops[y1][x1].velocity_x = drops[y][x].velocity_x;
	drops[y1][x1].velocity_y = drops[y][x].velocity_y;
	drops[y1][x1].has_been_updated	= TRUE;
	Erase(x,y);
}

U0 UpdateSand(I64 x, I64 y)
{
	I64 c, d, l, r, dl, dr, x0, x1, y0, y1, v;
	I64 vx, vy;

	x0 = x1 = x;
	y0 = y1 = y;

	// down peek
	c  = GrPeek(cur_dc, x0, y0); 
	d  = GrPeek(cur_dc, x0, y0 + 1);
	l  = GrPeek(cur_dc, x0 - 1, y0);
	r  = GrPeek(cur_dc, x0 + 1, y0);
	dl = GrPeek(cur_dc, x0 - 1, y0 + 1);
	dr = GrPeek(cur_dc, x0 + 1, y0 + 1);

//	c  = drops[y0][x0].color; 
//	d  = drops[y0][x0 + 1].color;
//	l  = drops[y0 - 1][x0].color;
//	r  = drops[y0 + 1][x0].color;
//	dl = drops[y0 - 1][x0 + 1].color;
//	dr = drops[y0 + 1][x0 + 1].color;
	
//	if (!drops[y][x].has_been_updated)
//	{
//		drops[y][x].velocity_y = Clamp(0.4 * mouse.speed, 0, 4);
//		
//		if (mouse.pos.x > mouse_last.pos.x)
//			drops[i].velocity_x = Clamp(0.4 * mouse.speed, 0, 2);
//		else if (mouse.pos.x < mouse_last.pos.x)
//			drops[i].velocity_x = - Clamp(0.4 * mouse.speed, 0, 2);
//		else
//			drops[i].velocity_x = 0;
//
//		if (mouse.pos.y > mouse_last.pos.y)
//			drops[i].velocity_y = Clamp(0.4 * mouse.speed, 0, 4);
//		else if (mouse.pos.y < mouse_last.pos.y)
//			drops[i].velocity_y = - Clamp(0.4 * mouse.speed, 0, 4);
//		else
//			drops[i].velocity_y = 0;
//	}

	//drops[i].has_been_updated = TRUE;

	//velocity x
	//if (Abs(drops[i].velocity_x) > 0)
	//{
		//x0 += drops[i].velocity_x;
		//drops[i].velocity_x -= 0.01;
	//}

	//velocity y
	//	if (Abs(drops[i].velocity_y) > 0)
	//	{
	//		y0 += drops[i].velocity_y;
	//		drops[i].velocity_y += 0.5;
	//	}

	// speedhack
//	for (v=2; v <= drops[y][x].velocity; v++)
//	{
//		if (drops[y0 + v][x0] == WHITE)
//			y1++;
//		//will move slower in water
//		//else if (GrPeek(cur_dc, x0, y0 + v) == BLUE)
//		//	y1++;
//	}

	// if down is empty
	if (d == WHITE)
		y1++;
	// if down-left empty
	else if (dl == WHITE)
	{
		x1--;
		y1++;
	}	
	// if down-right empty
	else if (dr == WHITE)
	{	
		x1++;
		y1++;
	}
	// clean up previous pos
//	if (c == drops[y][x].color)
//	{
//		cur_dc->color = WHITE;
//		GrPlot(cur_dc, x1, y1);
//	}
	// if down is empty
	if (d == BLUE)
		y1++;

	// if down-left empty
	else if (dl == BLUE)
	{
		x1--;
		y1++;
	}	
	// if down-right empty
	else if (dr == BLUE)
	{	
		x1++;
		y1++;
	}
	// makes sand fall in chunk
//	if (c == BLUE)
//	{
//		cur_dc->color = WHITE;
//		GrPlot(cur_dc, x1, y1);
//	}
//	cur_dc->color = drops[y1][x1].color;
//	GrPlot(cur_dc, x1, y1);

	RenderDrop(x1,y1,x,y);
	
	// this waits 60fps...
//	 Refresh;

//  cur_dc->color = drops[y][x].color;
//	GrPlot(cur_dc, x1, y1);
//	drops[i].x = x1;
//	drops[i].y = y1;
}

// U0 UpdateWater(I64 x, I64 y)
//{
//	I64 l, r, dl, dr, u, ul, ur, c1, c2, x0, x1, y0, y1, v;

// 	x0 = x1 = drops[i].x;
 //	y0 = y1 = drops[i].y;

// 	// speedhack	
// 	for (v=1; v <= drops[i].velocity; v++)
// 	{
// 		if (GrPeek(cur_dc, x0, y0 + v) == WHITE)
// 			y1++;
// 	}	

// 	if (GrPeek(cur_dc, x0, y0 + 1) == WHITE)
// 		y1++;

// 	else
// 	{
// 		l  = GrPeek(cur_dc, x0 - 1, y0);
// 		r  = GrPeek(cur_dc, x0 + 1, y0);		
// 		dl = GrPeek(cur_dc, x0 - 1, y0 + 1);
// 		dr = GrPeek(cur_dc, x0 + 1, y0 + 1);
// 		if (dl == WHITE)
// 		{
// 			y1++;
// 			x1--;	
// 		}
// 		else if (dr == WHITE)
// 		{
// 			y1++;
// 			x1++;
// 		}
// 		else if (l == WHITE)
// 			x1--;
// 		else if (r == WHITE)
// 			x1++;
// 	}

// 	// clean old position
// 	if (GrPeek(cur_dc, x0, y0) == drops[i].color)
// 	{
// 		cur_dc->color = WHITE;
// 		GrPlot(cur_dc, x0, y0);
// 	}

// 	cur_dc->color = drops[i].color;
// 	GrPlot(cur_dc, x1, y1);
// 	drops[i].x = x1;
// 	drops[i].y = y1;
// }


U0 DrawFrame(CTask *task, CDC *dc)
{
	I64 cx = WIN_WIDTH >> 1, cy = WIN_HEIGHT >> 1;
	dc->color = BLACK;
	GrPrint(dc, cx, cy, "Particles:%d", drop_count);
	GrPrint(dc, 10, 10, "PX:%d\nPY:%d\nG:%b\nPVX:%d\nPVY:%d", players[0].x, players[0].y, players[0].grounded, players[0].velocity_x, players[0].velocity_y);
	GrPrint(dc, cx, cy+50, "MX:%d\nMY:%d", mouse.pos.x, mouse.pos.y);
	GrPrint(dc, cx, cy+100, "DX:%d\nDY:%d", debug_x, debug_y);
}

U0 DrawDrops(CTask *, CDC *dc)
{
	I64 x, y;
    for (y = 0; y < WIN_HEIGHT; ++y)
    {
	    for (x = 0; x < WIN_WIDTH; ++x)
    	{
			dc->color = drops[y][x].color;
			switch (drops[y][x].color)
            {
				//rock
				case DKGRAY:	
					GrFillCircle(dc, x, y, , 12);	
					break;
				//sand
				case YELLOW:
					GrPlot(dc, x, y);
					break;
				//water
				case BLUE:
					GrPlot(dc, x, y);
					break;
				//wood
				case BROWN:
					GrRect(dc, x, y, 10, 10);
					break;
				//fire
				case RED:
					GrPlot(dc, x, y);
					break;
				//smoke
				case LTGRAY:
					GrPlot(dc, x, y);
					break;
				//eraser
				case WHITE:
					//GrFillCircle(dc, x,  y, ,20);
					GrPlot(dc, x, y);
					break;
			}
		}
	}
}

U0 DrawPlayer(CTask *, CDC *dc)
{
	if (players)
	{
		GrRect(dc, players[0].x, players[0].y, 4, 10);
	}
		
}

U0 InputController()
{

	I64 x, y, rx, ry;

	if (Bt(kbd.down_bitmap, sc_1))
		selection = 0;
	if (Bt(kbd.down_bitmap, sc_2))
		selection = 1;
	if (Bt(kbd.down_bitmap, sc_3))
		selection = 2;

	if(mouse.lb)
	{
		x = mouse.pos.x;
		y = mouse.pos.y;

		// add rock
		if (selection == 0)
		{
			drops[y][x].velocity = 0;
			drops[y][x].color	 = DKGRAY;
		}
		// add sand
		if (selection == 1)
		{
			drops[y][x].velocity = 5;
			drops[y][x].color	 = YELLOW;
		}
		// add water
		if (selection == 2)
		{
			drops[y][x].velocity = 3;
			drops[y][x].color	 = BLUE;
		}

		drops[y][x].id = drop_count;
		drops[y][x].x  = x;
		drops[y][x].y  = y;
		drop_count++;
	}
}

I64 loop_count = 0;

U0 AnimateTask(I64)
{
	I64 x, y;

	while (TRUE)
	{
		for (y = 0; y < WIN_HEIGHT; ++y)
		{
			for (x = 0; x < WIN_WIDTH; ++x)
			{   
				drops[y][x].has_been_updated = FALSE;
			}
		}

		for (y = 0; y < WIN_HEIGHT; ++y)
		{
			for (x = 0; x < WIN_WIDTH; ++x)
			{   
				if (drops[y][x].has_been_updated == FALSE)
				{
        		debug_x = x;
				debug_y = y;
		        switch (drops[y][x].color)
                {
                    //rock
                    case DKGRAY:	
                        break;
                    //sand
                    case YELLOW:
		                //cur_dc->color = drops[y][x].color;
                        UpdateSand(x, y);
                        break;
                    //water
                    case BLUE:
                        //UpdateWater(x, y);
                        break;
                    //wood
                    case BROWN:
                        break;
                    //fire
                    case RED:
                        // UpdateFire(x, y);
                        break;
                    //smoke
                    case LTGRAY:
                        // UpdateSmoke(x, y);
                        break;
                    //eraser
                    case WHITE:
                        break;
                }
				}
				//gr.dc->color = loop_count & 15;
				//GrPlot(gr.dc, x, y);
				//Sleep(1);
            }
			//Sleep(1);
			//Refresh;
		}
		//loop_count++;
		//Sleep(1);
		//Refresh;
	}
	Refresh;
}

U0 AnimatePlayer(I64)
{

	I64 vx, vy, down, up, left, right, vx0, vy0,
			grounded = players[0].grounded,
			x0   = players[0].x,
			y0   = players[0].y,	
			w	 = 0, // wind
			x	 = 0,
			y	 = 0,
			time = 0.0;

	while (TRUE)
	{
		x0	= players[0].x;
		y0	= players[0].y;
		vx0 = players[0].velocity_x;
		vy0 = players[0].velocity_y;

		down  = GrPeek(cur_dc, players[0].x, 	 players[0].y + 1);
		up	  = GrPeek(cur_dc, players[0].x, 	 players[0].y - 1);
		left  = GrPeek(cur_dc, players[0].x - 1, players[0].y);
		right = GrPeek(cur_dc, players[0].x + 1, players[0].y);


		x	= x0 + vx0 + w * 0.5;
		vx	= vx0 + w;
		
		if(!players[0].grounded)
		{
			y  = y0  + vy0 + w * 0.5;
			vy = vy0 + w;
		}
			
//		if(players[0].y < WIN_HEIGHT && !players[0].grounded)
//		if(players[0].velocity_x > 0)
//			vx -= 1;

//		if(players[0].y > WIN_HEIGHT)
//		{
//			players[0].grounded = TRUE;
//			players[0].y = WIN_HEIGHT-1;
//		}

		if(down != WHITE && players[0].y >= WIN_HEIGHT)
		{
			players[0].grounded = TRUE;
			players[0].y = WIN_HEIGHT - 18;
			vy = 0;
		}
		else if (players[0].grounded != TRUE)
		{
			players[0].grounded = FALSE;
			vy++;
		//	players[0].y += vy;
		//	players[0].velocity_y += vy;
		}
		else if(up == YELLOW)
		{
			players[0].grounded = TRUE;	
			players[0].y -= 1;
//			vy = 0;
		}

		// clamping dedides max speed
		players[0].velocity_x = Clamp(vx, -1, 1);
		players[0].velocity_y = Clamp(vy, -6, 6);

		// this is a bit ugly
		// adding the non clamped value but
		// every cycle picks up players[0].velocity_x
		// so in the end it does get clamped
		// somehow smoother/faster
		players[0].x += vx;
		players[0].y += vy;
//
		Refresh;
		time += 1.0 / 60.0;
		
		//Sleep(9);

	}
}

U0 DrawIt(CTask *task, CDC *dc)
{
	InputController();
	PlayerController();

	DrawFrame(task, dc);
	DrawDrops(task, dc);
	DrawPlayer(task, dc);
	UpdateCurBase(task, dc);
}

U0 Init()
{
	drop_count  = 0;
	MemSet(drops, 0, sizeof(drops));
	MemSet(players, 0, sizeof(players));
	players[0].x = WIN_WIDTH/2;
	players[0].y = WIN_HEIGHT/2;
	players[0].velocity_x = 0;
	players[0].velocity_y = 0;
	players[0].grounded = FALSE;
}

U0 SandWorld()
{
	I64 ch, sc;

	MenuPush(	"File {"
				"  Abort(,CH_SHIFT_ESC);"
				"  Exit(,CH_ESC);"
				"}"
				"Play {"
				"  Restart(,'\n');"
				"  OpenUpper(,'1');"
				"  OpenLower(,'2');"
				"}"
				);
	SettingsPush; //See $LK,"SettingsPush",A="MN:SettingsPush"$
	AutoComplete;
	WinBorder;
	WinMax;
	DocCursor;
	DocClear;
	cur_dc = DCNew(WIN_WIDTH, WIN_HEIGHT);
	sc_1 = Char2ScanCode('1');
	sc_2 = Char2ScanCode('2');
	sc_3 = Char2ScanCode('3');
	sc_4 = Char2ScanCode('4');
	sc_5 = Char2ScanCode('5');
	sc_6 = Char2ScanCode('6');
	sc_w = Char2ScanCode('w');
	sc_a = Char2ScanCode('a');
	sc_d = Char2ScanCode('d');
	sc_s = Char2ScanCode('s');

	Init;
	//Fs->animate_task = Spawn(&AnimateSand, NULL, "Animate",, Fs);
	
	gr.fp_draw_mouse = &DrawMouse;
	// TODO
	// currently manually setting multicores... not good
	Fs->animate_task = Spawn(&AnimateTask, NULL, "Animate", 2, Fs);
	Fs->animate_task = Spawn(&AnimatePlayer, NULL, "AnimatePlayer", 3, Fs);
	Fs->draw_it		 = &DrawIt;


	if(sizeof(drops) >= DROPS_NUM)
	{
		//drop_count--;
		Init;
	}

	try
	{
		do
		{
			switch (ch = KeyGet(&sc))
			{
				case '\n':
					Init;
					break;
			}
		}
		while (ch != CH_ESC && ch != CH_SHIFT_ESC);
	}
	catch
		PutExcept;
	Init;
	SettingsPop;
	MenuPop;
	DCDel(cur_dc);
}

SandWorld;
