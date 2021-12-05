// Set game palette
#include "PaletteEditor/Palettes/Sand"
PaletteSetSand(FALSE);

#define DROPS_NUM 			0x60000

class Drop
{
	I32 id, x, y;
	I32 life, velocity;
	CColorROPU32 color;
	I64 has_been_updated;
} drops[DROPS_NUM];

I64	rain_amount = 2000;
I64 drop_count  = 0;
CDC *cur_dc;
I64 sc_1, sc_2, sc_3, sc_4, sc_5, sc_6, selection = 0;

U0 UpdateCurBase(CTask *task, CDC * dc)
{
	cur_dc->color = ROP_EQU;
	dc->flags |= DCF_NO_TRANSPARENTS;
	GrBlot(cur_dc, -task->pix_left - task->scroll_x, -task->pix_top - task->scroll_y, dc);
	dc->flags &= ~DCF_NO_TRANSPARENTS;
}



U0 UpdateSmoke(I64 i, I64 w, I64 h)
{
	I64 u, l, r, ul, ur, x0, x1, y0, y1;

	x0 = x1 = drops[i].x;
	y0 = y1 = drops[i].y;

	// down peek
	u = GrPeek(cur_dc, x0, y0 - 1);
	// if down is empty
	if (u == WHITE)
	{
		if (RandI16 >= 200)
			y1--;
		if (RandI16 >= 250)
			x1 += RandU32%10;
		else
			x1 -= RandU32%10;
	}
	else {		
		l  = GrPeek(cur_dc, x0 - 1, y0);
		r  = GrPeek(cur_dc, x0 + 1, y0);
		ul = GrPeek(cur_dc, x0 - 1, y0 - 1);
		ur = GrPeek(cur_dc, x0 + 1, y0 - 1);
		// if up-left empty
		if (ul == WHITE)
		{
			x1 -= RandU32%2;
			y1--;
		}	
		// if up-right empty
		else if (ur == WHITE)
		{	
			x1 += RandU32%2;
			y1--;
		}
		else if (l == WHITE)
			x1 -= RandU32%2;
		else if (r == WHITE)
			x1 += RandU32%2;
	}

	if (GrPeek(cur_dc, x0, y0) == drops[i].color)
	{
		cur_dc->color = WHITE;
		GrPlot(cur_dc, x0, y0);
	}

	cur_dc->color = drops[i].color;
	GrPlot(cur_dc, x1, y1);
	drops[i].x = x1;
	drops[i].y = y1;
}

U0 UpdateFire(I64 i, I64 w, I64 h)
{
	I64 c, l, r, d, u, ul, ur, dl, dr, x0, x1, y0, y1, v;

	x0 = x1 = drops[i].x;
	y0 = y1 = drops[i].y;

	// down peek
	c  = GrPeek(cur_dc, x0, y0);
	d  = GrPeek(cur_dc, x0, y0 + 1);
	l  = GrPeek(cur_dc, x0 - 1, y0);
	r  = GrPeek(cur_dc, x0 + 1, y0);
	dl = GrPeek(cur_dc, x0 - 1, y0 + 1);
	dr = GrPeek(cur_dc, x0 + 1, y0 + 1);

	if (d == WHITE)
	{
		if (RandI16 >= 500)
			y1++;
		if (RandI16 >= 50)
			x1 += RandU32%2;
		else
			x1 -= RandU32%2;
	}

	// if down is empty
	if (d == WHITE)
		y1++;
	// if down-left empty
	else if (dl == WHITE)
	{
		x1 -= RandU16%2;
		y1++;
	}	
	// if down-right empty
	else if (dr == WHITE)
	{	
		x1 += RandU16%2;
		y1++;
	}
	// clean up previous pos
	if (c == drops[i].color)
	{
		cur_dc->color = WHITE;
		GrPlot(cur_dc, x0, y0);
	}

	// if down is empty
	if (d == BROWN)
	{
		if(RandU16 <= 2000)
		{
			y1++;

			// if down-left empty
			if (dl == BROWN)
			{
				if(RandU16 <= 200)
					x1--;
					y1++;
			}	
			// if down-right empty
			else if (dr == BROWN)
			{	
				x1++;
				y1++;
			}

		}
	}
	if (c == BROWN)
	{
		cur_dc->color = WHITE;
		GrPlot(cur_dc, x0, y0);
	}

	cur_dc->color = drops[i].color;
	GrPlot(cur_dc, x1, y1);
	drops[i].x = x1;
	drops[i].y = y1;
}

U0 UpdateSand(I64 i, I64 w, I64 h)
{
	I64 c, d, l, r, dl, dr, x0, x1, y0, y1, v;

	x0 = x1 = drops[i].x;
	y0 = y1 = drops[i].y;
	
	// speedhack	
	for (v=2; v <= drops[i].velocity; v++)
	{
		if (GrPeek(cur_dc, x0, y0 + v) == WHITE)
			y1++;
		//will move slower in water
		//else if (GrPeek(cur_dc, x0, y0 + v) == BLUE)
		//	y1++;
	}

	// down peek
	c  = GrPeek(cur_dc, x0, y0); 
	d  = GrPeek(cur_dc, x0, y0 + 1);
	l  = GrPeek(cur_dc, x0 - 1, y0);
	r  = GrPeek(cur_dc, x0 + 1, y0);
	dl = GrPeek(cur_dc, x0 - 1, y0 + 1);
	dr = GrPeek(cur_dc, x0 + 1, y0 + 1);
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
	if (c == drops[i].color)
	{
		cur_dc->color = WHITE;
		GrPlot(cur_dc, x0, y0);
	}
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
	if (c == BLUE)
	{
		cur_dc->color = WHITE;
		GrPlot(cur_dc, x0, y0);
	}
	cur_dc->color = drops[i].color;
	GrPlot(cur_dc, x1, y1);
	drops[i].x = x1;
	drops[i].y = y1;
}

U0 UpdateWater(I64 i, I64 w, I64 h)
{
	I64 l, r, dl, dr, u, ul, ur, c1, c2, x0, x1, y0, y1, v;

	x0 = x1 = drops[i].x;
	y0 = y1 = drops[i].y;

	// speedhack	
	for (v=1; v <= drops[i].velocity; v++)
	{
		if (GrPeek(cur_dc, x0, y0 + v) == WHITE)
			y1++;
	}	

	if (GrPeek(cur_dc, x0, y0 + 1) == WHITE)
		y1++;

	else
	{
		l  = GrPeek(cur_dc, x0 - 1, y0);
		r  = GrPeek(cur_dc, x0 + 1, y0);		
		dl = GrPeek(cur_dc, x0 - 1, y0 + 1);
		dr = GrPeek(cur_dc, x0 + 1, y0 + 1);
		if (dl == WHITE)
		{
			y1++;
			x1--;	
		}
		else if (dr == WHITE)
		{
			y1++;
			x1++;
		}
		else if (l == WHITE)
			x1--;
		else if (r == WHITE)
			x1++;
	}

	// clean old position
	if (GrPeek(cur_dc, x0, y0) == drops[i].color)
	{
		cur_dc->color = WHITE;
		GrPlot(cur_dc, x0, y0);
	}

	cur_dc->color = drops[i].color;
	GrPlot(cur_dc, x1, y1);
	drops[i].x = x1;
	drops[i].y = y1;
}


U0 DrawFrame(CTask *task, CDC *dc)
{
	I64 w = task->pix_width, h = task->pix_height, cx = w >> 1, cy = h >> 1;
	dc->color = BLACK;
	GrPrint(dc, cx, cy, "Particles:%d", drop_count);
}

U0 DrawDrops(CTask *, CDC *dc)
{
	I64 i, k;

	for (i = 0; i < DROPS_NUM; i++)
	{
		dc->color = drops[i].color;
		if (drops[i].y >= 0) //&& GrPeek(dc, drops[i].x, drops[i].y) == WHITE)
		{
			switch (drops[i].color)
			{
				//rock
				case DKGRAY:	
					GrFillCircle(dc, drops[i].x, drops[i].y, , 12);	
					break;
				//sand
				case YELLOW:
					GrPlot(dc, drops[i].x, drops[i].y);
					break;
				//water
				case BLUE:
					GrPlot(dc, drops[i].x, drops[i].y);
					break;
				//wood
				case BROWN:
					GrRect(dc, drops[i].x, drops[i].y, 10, 10);
					break;
				//fire
				case RED:
					GrPlot(dc, drops[i].x, drops[i].y);
					break;
				//smoke
				case LTGRAY:
					GrPlot(dc, drops[i].x, drops[i].y);
					break;
				//eraser
				case WHITE:
					GrFillCircle(dc, drops[i].x, drops[i].y, ,20);
					break;
			}
		}
	}
}
U0 DrawIt(CTask *task, CDC *dc)
{
	U32 rx, ry;
	DrawFrame(task, dc);
	DrawDrops(task, dc);
	UpdateCurBase(task, dc);

	if (Bt(kbd.down_bitmap, sc_1))
		selection = 0;
	if (Bt(kbd.down_bitmap, sc_2))
		selection = 1;
	if (Bt(kbd.down_bitmap, sc_3))
		selection = 2;
	if (Bt(kbd.down_bitmap, sc_4))
		selection = 3;
	if (Bt(kbd.down_bitmap, sc_5))
		selection = 4;
	if (Bt(kbd.down_bitmap, sc_6))
		selection = 5;
	
	if(mouse.lb)
	{

		// add rock
		if(selection == 0)
		{
			drops[drop_count].velocity = 0;
			drops[drop_count].color = DKGRAY;
			rx = 1;
			ry = 1;
		}
		// add sand
		if(selection == 1)
		{
	
			// drop 20 at a time
			I64 i;
			for(i=0; i < 20; i++)
			{
				rx = RandU32%10;
				ry = RandU32%10;

				drops[drop_count].id = drop_count;
				drops[drop_count].velocity = 5;
				drops[drop_count].color = YELLOW;
				drops[drop_count].x = mouse.pos.x - rx/2 + i*RandU32%30;
				drops[drop_count].y = mouse.pos.y + i + ry/4;
				drop_count++;
			}
		}
		// add water
		if(selection == 2)
		{
			// drop 20 at a time
			I64 j;
			for(j=0; j < 20; j++)
			{	
				rx = RandU32%50;
				ry = RandU32%50;

				drops[drop_count].id = drop_count;
				drops[drop_count].velocity = 3;
				drops[drop_count].color = BLUE;	
				drops[drop_count].x = mouse.pos.x - rx/2 + j*RandU32%60;
				drops[drop_count].y = mouse.pos.y + j + ry/4;
				drop_count++;
			}
		}
		// add wood
		if(selection == 3)
		{
			drops[drop_count].velocity = 0;
			drops[drop_count].color = BROWN;
			rx = 1;
			ry = 1;
		}
		// add fire
		if(selection == 4)
		{
			drops[drop_count].velocity = 5;
			drops[drop_count].color = RED;
			rx = RandU32%5;
			ry = RandU32%5;
		}
		// add smoke
		if(selection == 5)
		{
			// drop 20 at a time
			I64 k;
			for(k=0; k < 20; k++)
			{	
				rx = RandU32%8;
				ry = RandU32%8;

				drops[drop_count].id = drop_count;	
				drops[drop_count].velocity = 5;
				drops[drop_count].color = LTGRAY;
				drops[drop_count].x = mouse.pos.x - rx/2 + k*RandU32%60;
				drops[drop_count].y = mouse.pos.y - k - ry/4;
				drop_count++;
			}
		}
		
		drops[drop_count].id = drop_count;
		drops[drop_count].x = mouse.pos.x + rx/4;
		drops[drop_count].y = mouse.pos.y + ry/4;
		drop_count++;
	}
	// eraser
	else if (mouse.rb)
	{
		drops[drop_count].velocity = 0;
		drops[drop_count].color = WHITE;
		drops[drop_count].x = mouse.pos.x;
		drops[drop_count].y = mouse.pos.y;
		// really should be deleting those it touches
		drop_count++;
	}
}

U0 AnimateTask(I64)
{
	I64 i, k, w=Fs->parent_task->pix_width, h=Fs->parent_task->pix_height;

	while (TRUE)
	{
	
		for (i = 0; i < DROPS_NUM; i++)
		{
			cur_dc->color = drops[i].color;
			if (drops[i].y >= 0) //&& GrPeek(dc, drops[i].x, drops[i].y) == WHITE)
			{
				switch (drops[i].color)
				{
					//rock
					case DKGRAY:	
						break;
					//sand
					case YELLOW:
						UpdateSand(i, w, h);
						break;
					//water
					case BLUE:
						UpdateWater(i, w, h);
						break;
					//wood
					case BROWN:
						break;
					//fire
					case RED:
						UpdateFire(i, w, h);
						break;
					//smoke
					case LTGRAY:
						UpdateSmoke(i, w, h);
						break;
					//eraser
					case WHITE:
						break;
				}
			}
		}
	Sleep(8);
	}
}


U0 Init()
{
	drop_count  = 0;
	MemSet(drops,  0, sizeof(drops));
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
	cur_dc = DCNew(GR_WIDTH, GR_HEIGHT);
	sc_1 = Char2ScanCode('1');
	sc_2 = Char2ScanCode('2');
	sc_3 = Char2ScanCode('3');
	sc_4 = Char2ScanCode('4');
	sc_5 = Char2ScanCode('5');
	sc_6 = Char2ScanCode('6');

	Init;
	//Fs->animate_task = Spawn(&AnimateSand, NULL, "Animate",, Fs);
	Fs->animate_task = Spawn(&AnimateTask, NULL, "Animate",, Fs);
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
	SettingsPop;
	MenuPop;
	DCDel(cur_dc);
}

SandWorld;
