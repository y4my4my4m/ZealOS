public CBGR24 gr_palette_sand[COLORS_NUM] = {
0xE3E3E3, 0x167BBB, 0x73A255, 0x297582, 0xC20000, 0x8A52C3, 0xB47B29, 0x626262, 0x322F2B, 0x94BFDE, 0xA1CE97, 0x6DB4BE, 0xE88E88, 0xCA94E8, 0xD4B475, 0x222222
};

public U0 PaletteSetSand(Bool persistent=TRUE)
{
	GrPaletteSet(gr_palette_sand);
	LFBFlush;
	if (persistent)
		fp_set_std_palette = &PaletteSetSand;
} 