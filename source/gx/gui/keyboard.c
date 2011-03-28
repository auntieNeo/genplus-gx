#include "shared.h"
#include "keyboard.h"
#include "font.h"
#include "gui.h"

#ifdef HW_RVL
#include <wiiuse/wpad.h>
#endif

#define BG_COLOR_1 {0x49,0x49,0x49,0xff}
#define BG_COLOR_2 {0x66,0x66,0x66,0xff}

#define NUM_KEYBOARD_KEYS 45

#ifdef HW_RVL
extern const u8 Key_Minus_wii_png[];
extern const u8 Key_Plus_wii_png[];
#else
extern const u8 Key_L_gcn_png[];
extern const u8 Key_R_gcn_png[];
#endif
extern const u8 Key_DPAD_png[];

/*****************************************************************************/
/*  GUI Buttons data                                                         */
/*****************************************************************************/
static butn_data button_digit_data =
{
  {NULL,NULL},
  {Button_digit_png,Button_digit_over_png}
};

/*****************************************************************************/
/*  GUI helpers                                                              */
/*****************************************************************************/
static gui_item action_cancel =
{
  NULL,Key_B_png,"","Exit",10,422,28,28
};

static gui_item action_select =
{
  NULL,Key_A_png,"","Edit Entry",602,422,28,28
};

/*****************************************************************************/
/*  Menu Items description                                                   */
/*****************************************************************************/
static gui_item items_keyboard[NUM_KEYBOARD_KEYS] =
{
  {NULL,NULL,"1",      "Add Character",     70,150, 40, 40},
  {NULL,NULL,"2",      "Add Character",    116,150, 40, 40},
  {NULL,NULL,"3",      "Add Character",    162,150, 40, 40},
  {NULL,NULL,"4",      "Add Character",    208,150, 40, 40},
  {NULL,NULL,"5",      "Add Character",    254,150, 40, 40},
  {NULL,NULL,"6",      "Add Character",    300,150, 40, 40},
  {NULL,NULL,"7",      "Add Character",    346,150, 40, 40},
  {NULL,NULL,"8",      "Add Character",    392,150, 40, 40},
  {NULL,NULL,"9",      "Add Character",    438,150, 40, 40},
  {NULL,NULL,"0",      "Add Character",    484,150, 40, 40},
  {NULL,NULL,"Back",   "Remove Character", 530,150, 40, 40},
  {NULL,NULL,"q",      "Add Character",     82,196, 40, 40},
  {NULL,NULL,"w",      "Add Character",    128,196, 40, 40},
  {NULL,NULL,"e",      "Add Character",    174,196, 40, 40},
  {NULL,NULL,"r",      "Add Character",    220,196, 40, 40},
  {NULL,NULL,"t",      "Add Character",    266,196, 40, 40},
  {NULL,NULL,"y",      "Add Character",    312,196, 40, 40},
  {NULL,NULL,"u",      "Add Character",    358,196, 40, 40},
  {NULL,NULL,"i",      "Add Character",    404,196, 40, 40},
  {NULL,NULL,"o",      "Add Character",    450,196, 40, 40},
  {NULL,NULL,"p",      "Add Character",    496,196, 40, 40},
  {NULL,NULL,"-",      "Add Character",    542,196, 40, 40},
  {NULL,NULL,"Caps",   "Caps Lock",         48,242, 40, 40},
  {NULL,NULL,"a",      "Add Character",     94,242, 40, 40},
  {NULL,NULL,"s",      "Add Character",    140,242, 40, 40},
  {NULL,NULL,"d",      "Add Character",    186,242, 40, 40},
  {NULL,NULL,"f",      "Add Character",    232,242, 40, 40},
  {NULL,NULL,"g",      "Add Character",    278,242, 40, 40},
  {NULL,NULL,"h",      "Add Character",    324,242, 40, 40},
  {NULL,NULL,"j",      "Add Character",    370,242, 40, 40},
  {NULL,NULL,"k",      "Add Character",    416,242, 40, 40},
  {NULL,NULL,"l",      "Add Character",    462,242, 40, 40},
  {NULL,NULL,";",      "Add Character",    508,242, 40, 40},
  {NULL,NULL,"'",      "Add Character",    554,242, 40, 40},
  {NULL,NULL,"Shift",  "Shift",             60,288, 40, 40},
  {NULL,NULL,"z",      "Add Character",    106,288, 40, 40},
  {NULL,NULL,"x",      "Add Character",    152,288, 40, 40},
  {NULL,NULL,"c",      "Add Character",    198,288, 40, 40},
  {NULL,NULL,"v",      "Add Character",    244,288, 40, 40},
  {NULL,NULL,"b",      "Add Character",    290,288, 40, 40},
  {NULL,NULL,"n",      "Add Character",    336,288, 40, 40},
  {NULL,NULL,"m",      "Add Character",    382,288, 40, 40},
  {NULL,NULL,",",      "Add Character",    428,288, 40, 40},
  {NULL,NULL,".",      "Add Character",    474,288, 40, 40},
  {NULL,NULL,"/",      "Add Character",    520,288, 40, 40},
};

/*****************************************************************************/
/*  Menu Buttons description                                                 */
/*****************************************************************************/
static gui_butn buttons_keyboard[NUM_KEYBOARD_KEYS] =
{
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 70,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},116,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},162,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},208,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},254,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},300,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},346,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},392,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},438,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},484,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},530,150, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 82,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},128,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},174,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},220,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},266,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},312,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},358,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},404,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},450,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},496,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},542,196, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 48,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 94,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},140,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},186,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},232,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},278,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},324,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},370,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},416,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},462,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},508,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},554,242, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 60,288, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},106,288, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},152,288, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},198,288, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},244,288, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},290,288, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},336,288, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},382,288, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},428,288, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},474,288, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},520,288, 40, 40},
};

void KeyboardMenu(gui_menu *parent, char *string, size_t size)
{
  int i, update = 0;
  int old, selected = 0;
	int done = 0;
  s16 p;
	gui_butn *button;
	gui_item *item;
#ifdef HW_RVL
  int x,y;
#endif

	/* initilize images */
	button_digit_data.texture[0] = gxTextureOpenPNG(button_digit_data.image[0],0);
	button_digit_data.texture[1] = gxTextureOpenPNG(button_digit_data.image[1],0);

	// TODO: slide in

  while (!done)
  {
		/* draw parent menu */
		GUI_DrawMenu(parent);

    /* draw key buttons and items */
		for (i = 0; i < NUM_KEYBOARD_KEYS; i++)
		{
			button = &buttons_keyboard[i];

			if (button->state & BUTTON_VISIBLE)
			{
				item = &items_keyboard[i];

				/* draw button + items */ 
				if (i == selected)
				{
					if (button->data)
						gxDrawTexture(button->data->texture[1],button->x-4,button->y-4,button->w+8,button->h+8,255);

					if (item)
					{
						if (item->texture)
						{
							gxDrawTexture(item->texture, item->x-4,item->y-4,item->w+8,item->h+8,255);
							FONT_writeCenter(item->text,18,button->x+4,item->x-4,button->y+(button->h - 36)/2+18,(GXColor)DARK_GREY);
						}
						else
						{
							FONT_writeCenter(item->text,18,item->x-4,item->x+item->w+4,button->y+(button->h-18)/2+18,(GXColor)DARK_GREY);
						}
					}
				}
				else
				{
					if (button->data)
						gxDrawTexture(button->data->texture[0],button->x,button->y,button->w, button->h,255);

					if (item)
					{
						if (item->texture)
						{
							gxDrawTexture(item->texture,item->x,item->y,item->w,item->h,255);
							FONT_writeCenter(item->text,16,button->x+8,item->x,button->y+(button->h - 32)/2+16,(GXColor)DARK_GREY);
						}
						else
						{
							FONT_writeCenter(item->text,16,item->x,item->x+item->w,button->y+(button->h - 16)/2+16,(GXColor)DARK_GREY);
						}
					}
				}
			}
		}

		old = selected;
    p = m_input.keys;

#ifdef HW_RVL
    if (Shutdown)
    {
			gxTextureClose(&button_digit_data.texture[0]);
			gxTextureClose(&button_digit_data.texture[1]);
      gxTextureClose(&w_pointer);
      GUI_DeleteMenu(parent);
      GUI_FadeOut();
      shutdown();
      SYS_ResetSystem(SYS_POWEROFF, 0, 0);
    }
    else if (m_input.ir.valid)
    {
      /* get cursor position */
      x = m_input.ir.x;
      y = m_input.ir.y;

      /* draw wiimote pointer */
      gxDrawTextureRotate(w_pointer, x-w_pointer->width/2, y-w_pointer->height/2, w_pointer->width, w_pointer->height,m_input.ir.angle,255);

      /* check for valid buttons */
      selected = -1;
      for (i = 0; i < NUM_KEYBOARD_KEYS; i++)
      {
				button = &buttons_keyboard[i];
				if ((button->state & BUTTON_ACTIVE) && (button->state & BUTTON_VISIBLE))
				{
					if((x>=button->x)&&(x<=(button->x+button->w))&&(y>=button->y)&&(y<=(button->y+button->h)))
					{
						selected = i;
						break;
					}
				}
      }
    }
    else
    {
      /* reinitialize selection */
      if (selected == -1)
        selected = 0;
    }
#endif

    /* update screen */
    gxSetScreen();

    /* update selection */
		// TODO: allow the user to navigate keyboard with pad
    if (p & PAD_BUTTON_UP)
    {
    }
    else if (p & PAD_BUTTON_DOWN)
    {
    }

    /* sound fx */
    if (selected != old)
    {
      if (selected >= 0)
      {
        ASND_SetVoice(ASND_GetFirstUnusedVoice(),VOICE_MONO_16BIT,22050,0,(u8 *)button_over_pcm,button_over_pcm_size,
                      ((int)config.sfx_volume * 255) / 100,((int)config.sfx_volume * 255) / 100,NULL);
      }
    }

    if (p & PAD_BUTTON_A)
    {
      if (selected >= 0)
      {
				// TODO: add a character
      }
    }
    else if (p & PAD_BUTTON_B)
    {
      done = 1;
    }
  }

	// TODO: slide out

  /* close textures */
	gxTextureClose(&button_digit_data.texture[0]);
	gxTextureClose(&button_digit_data.texture[1]);
}

void keyboardmenu_cb(void)
{
}

void shift_keyboard(void)
{
}
