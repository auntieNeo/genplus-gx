#include "shared.h"
#include "keyboard.h"
#include "font.h"
#include "gui.h"

#ifdef HW_RVL
#include <wiiuse/wpad.h>
#endif

#define BG_COLOR_1 {0x49,0x49,0x49,0xff}
#define BG_COLOR_2 {0x66,0x66,0x66,0xff}

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
static butn_data arrow_up_data =
{
  {NULL,NULL},
  {Button_up_png,Button_up_over_png}
};

static butn_data arrow_down_data =
{
  {NULL,NULL},
  {Button_down_png,Button_down_over_png}
};

static butn_data button_digit_data =
{
  {NULL,NULL},
  {Button_digit_png,Button_digit_over_png}
};

/*****************************************************************************/
/*  GUI Arrows button                                                        */
/*****************************************************************************/

static gui_butn arrow_up = {&arrow_up_data,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{0,0,0,0},14,76,360,32};
static gui_butn arrow_down = {&arrow_down_data,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{0,0,0,0},14,368,360,32};

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
/*  GUI Background images                                                    */
/*****************************************************************************/
static gui_image bg_keyboard[6] =
{
  {NULL,Bg_main_png,IMAGE_VISIBLE,374,140,284,288,255},
  {NULL,Bg_overlay_png,IMAGE_VISIBLE|IMAGE_REPEAT,0,0,640,480,255},
  {NULL,Banner_top_png,IMAGE_VISIBLE,0,0,640,108,255},
  {NULL,Banner_bottom_png,IMAGE_VISIBLE,0,380,640,100,255},
  {NULL,Main_logo_png,IMAGE_VISIBLE,466,40,152,44,255},
  {NULL,Frame_s1_png,IMAGE_VISIBLE,8,70,372,336,152},
};

/*****************************************************************************/
/*  Menu Items description                                                   */
/*****************************************************************************/
static gui_item items_keyboard[46] =
{
  {NULL,NULL,"","Current String",0,0,0,0},
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
static gui_butn buttons_keyboard[46] =
{
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},0,0,0,0},
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

/*****************************************************************************/
/*  Menu description                                                         */
/*****************************************************************************/
static gui_menu menu_keyboard =
{
  "Virtual Keyboard",
  0,0,
  46,46,6,0,
  items_keyboard,
  buttons_keyboard,
  bg_keyboard,
  {&action_cancel, &action_select},
  {&arrow_up,&arrow_down},
  keyboardmenu_cb
};

void KeyboardMenu(char *string, size_t size)
{
  int i, update = 0;
  int digit_cnt = 0;
  int max = 0;
	gui_menu *m = &menu_keyboard;

  /* background type */
  if (config.bg_type > 0)
  {
    bg_keyboard[0].state &= ~IMAGE_REPEAT;
    bg_keyboard[0].data = (config.bg_type > 1) ? Bg_main_png : Bg_main_2_png;
    bg_keyboard[0].x = 374;
    bg_keyboard[0].y = 140;
    bg_keyboard[0].w = 284;
    bg_keyboard[0].h = 288;
  }
  else
  {
    bg_keyboard[0].state |= IMAGE_REPEAT;
    bg_keyboard[0].data = Bg_layer_png;
    bg_keyboard[0].x = 0;
    bg_keyboard[0].y = 0;
    bg_keyboard[0].w = 640;
    bg_keyboard[0].h = 480;
  }

  /* background overlay */
  if (config.bg_overlay)
  {
    bg_keyboard[1].state |= IMAGE_VISIBLE;
  }
  else
  {
    bg_keyboard[1].state &= ~IMAGE_VISIBLE;
  }

  /* init menu */
  GUI_InitMenu(m);
  m->cb = keyboardmenu_cb;

  while (update != -1)
  {
    /* draw menu */
    GUI_DrawMenu(m);

    /* update menu */
    update = GUI_UpdateMenu(m);

    /* handle pressed buttons */
    if (update > 0)
    {
    }
    else if (update < 0)
    {
			// do something
    }
    else
    {
    }
  }

  /* leave menu */
  m->cb = NULL;
  GUI_DeleteMenu(m);
}

void keyboardmenu_cb(void)
{
}

void shift_keyboard(void)
{
}
