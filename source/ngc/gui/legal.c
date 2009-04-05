/****************************************************************************
 *  legal.c
 *
 *  legal informations screen
 *
 *  code by Softdev (2006), Eke-Eke (2007,2008)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ***************************************************************************/


#include "shared.h"
#include "font.h"
#include "Background_intro_c1.h"
#include "Background_intro_c2.h"
#include "Background_intro_c3.h"
#include "Background_intro_c4.h"

extern s16 ogc_input__getMenuButtons(u32 cnt);

/* 
 * This is the legal stuff - which must be shown at program startup 
 * Any derivative work MUST include the same textual output.
 *
 * In other words, play nice and give credit where it's due.
 */
void legal ()
{
  int ypos = 64;
  png_texture *texture;

  ClearScreen((GXColor)BLACK);

  WriteCentre (ypos, "Genesis Plus Sega Mega Drive Emulator (v1.2a)");
  ypos += fheight;
  WriteCentre (ypos, "(C) 1999 - 2003 Charles MacDonald");
  ypos += fheight;
  WriteCentre (ypos, "This is free software, and you are welcome to");
  ypos += fheight;
  WriteCentre (ypos, "redistribute it under the conditions of the");
  ypos += fheight;
  WriteCentre (ypos, "GNU GENERAL PUBLIC LICENSE Version 2");
  ypos += fheight;
  WriteCentre (ypos, "Additionally, the developers of this port");
  ypos += fheight;
  WriteCentre (ypos, "disclaims all copyright interests in the ");
  ypos += fheight;
  WriteCentre (ypos, "Nintendo Gamecube Porting code.");
  ypos += fheight;
  WriteCentre (ypos, "You are free to use it as you wish.");
  ypos += 2*fheight;

  texture= OpenTexturePNG(Background_intro_c4);
  if (texture)
  {
    DrawTexture(texture, (640-texture->width)/2, ypos, texture->width, texture->height);
    ypos += texture->height + 2 * fheight;
    if (texture->data) free(texture->data);
    free(texture);
  }

  SetScreen ();
  sleep (2);
  WriteCentre (ypos, "Press any button to skip intro");
  SetScreen ();
  int count = 100;
  while (count > 0)
  {
    count--;
    VIDEO_WaitVSync();
    if (ogc_input__getMenuButtons(0)) return;
  }

  ClearScreen((GXColor)BLACK);
  texture = OpenTexturePNG(Background_intro_c1);
  if (texture)
  {
    DrawTexture(texture, (640-texture->width)/2, (480-texture->height)/2,  texture->width, texture->height);
    if (texture->data) free(texture->data);
    free(texture);
  }

  SetScreen ();
  sleep (1);

  ClearScreen((GXColor)WHITE);
  texture = OpenTexturePNG(Background_intro_c2);
  if (texture)
  {
    DrawTexture(texture, (640-texture->width)/2, (480-texture->height)/2,  texture->width, texture->height);
    if (texture->data) free(texture->data);
    free(texture);
  }

  SetScreen ();
  sleep (1);

  ClearScreen((GXColor)BLACK);
  texture = OpenTexturePNG(Background_intro_c3);
  if (texture)
  {
    DrawTexture(texture, (640-texture->width)/2, (480-texture->height)/2,  texture->width, texture->height);
    if (texture->data) free(texture->data);
    free(texture);
  }

  SetScreen ();
  sleep (2);
}
