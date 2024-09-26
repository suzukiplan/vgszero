void init_palette(void)
{
    vgs0_palette_set(0, 0, 0, 0, 0);
    vgs0_palette_set(0, 1, 0, 0, 21);
    vgs0_palette_set(0, 2, 0, 21, 0);
    vgs0_palette_set(0, 3, 0, 21, 21);
    vgs0_palette_set(0, 4, 21, 0, 0);
    vgs0_palette_set(0, 5, 21, 0, 21);
    vgs0_palette_set(0, 6, 21, 10, 0);
    vgs0_palette_set(0, 7, 21, 21, 21);
    vgs0_palette_set(0, 8, 10, 10, 10);
    vgs0_palette_set(0, 9, 10, 10, 31);
    vgs0_palette_set(0, 10, 10, 31, 10);
    vgs0_palette_set(0, 11, 10, 31, 31);
    vgs0_palette_set(0, 12, 31, 10, 10);
    vgs0_palette_set(0, 13, 31, 10, 31);
    vgs0_palette_set(0, 14, 31, 31, 10);
    vgs0_palette_set(0, 15, 31, 31, 31);
}
