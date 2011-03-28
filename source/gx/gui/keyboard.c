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
  {NULL,Banner_top_png,IMAGE_VISIBLE|IMAGE_SLIDE_TOP,0,0,640,108,255},
  {NULL,Banner_bottom_png,IMAGE_VISIBLE|IMAGE_SLIDE_BOTTOM,0,380,640,100,255},
  {NULL,Main_logo_png,IMAGE_VISIBLE|IMAGE_SLIDE_TOP,466,40,152,44,255},
  {NULL,Frame_s1_png,IMAGE_VISIBLE,8,70,372,336,152},
};

/*****************************************************************************/
/*  Menu Items description                                                   */
/*****************************************************************************/
static gui_item items_keyboard[46] =
{
  {NULL,NULL,"","Current String",0,0,0,0},
  {NULL,NULL,"1",      "Add Character",      0,  0, 40, 40},
  {NULL,NULL,"2",      "Add Character",     46,  0, 40, 40},
  {NULL,NULL,"3",      "Add Character",     92,  0, 40, 40},
  {NULL,NULL,"4",      "Add Character",    138,  0, 40, 40},
  {NULL,NULL,"5",      "Add Character",    184,  0, 40, 40},
  {NULL,NULL,"6",      "Add Character",    230,  0, 40, 40},
  {NULL,NULL,"7",      "Add Character",    276,  0, 40, 40},
  {NULL,NULL,"8",      "Add Character",    322,  0, 40, 40},
  {NULL,NULL,"9",      "Add Character",    368,  0, 40, 40},
  {NULL,NULL,"0",      "Add Character",    414,  0, 40, 40},
  {NULL,NULL,"Back",   "Remove Character", 460,  0, 40, 40},
  {NULL,NULL,"q",      "Add Character",     52, 46, 40, 40},
  {NULL,NULL,"w",      "Add Character",     98, 46, 40, 40},
  {NULL,NULL,"e",      "Add Character",    144, 46, 40, 40},
  {NULL,NULL,"r",      "Add Character",    190, 46, 40, 40},
  {NULL,NULL,"t",      "Add Character",    236, 46, 40, 40},
  {NULL,NULL,"y",      "Add Character",    282, 46, 40, 40},
  {NULL,NULL,"u",      "Add Character",    328, 46, 40, 40},
  {NULL,NULL,"i",      "Add Character",    374, 46, 40, 40},
  {NULL,NULL,"o",      "Add Character",    420, 46, 40, 40},
  {NULL,NULL,"p",      "Add Character",    466, 46, 40, 40},
  {NULL,NULL,"-",      "Add Character",    512, 46, 40, 40},
  {NULL,NULL,"Caps",   "Caps Lock",         18, 92, 40, 40},
  {NULL,NULL,"a",      "Add Character",     64, 92, 40, 40},
  {NULL,NULL,"s",      "Add Character",    110, 92, 40, 40},
  {NULL,NULL,"d",      "Add Character",    156, 92, 40, 40},
  {NULL,NULL,"f",      "Add Character",    202, 92, 40, 40},
  {NULL,NULL,"g",      "Add Character",    248, 92, 40, 40},
  {NULL,NULL,"h",      "Add Character",    294, 92, 40, 40},
  {NULL,NULL,"j",      "Add Character",    340, 92, 40, 40},
  {NULL,NULL,"k",      "Add Character",    386, 92, 40, 40},
  {NULL,NULL,"l",      "Add Character",    432, 92, 40, 40},
  {NULL,NULL,";",      "Add Character",    478, 92, 40, 40},
  {NULL,NULL,"'",      "Add Character",    524, 92, 40, 40},
  {NULL,NULL,"Shift",  "Shift",             30,138, 40, 40},
  {NULL,NULL,"z",      "Add Character",     76,138, 40, 40},
  {NULL,NULL,"x",      "Add Character",    122,138, 40, 40},
  {NULL,NULL,"c",      "Add Character",    168,138, 40, 40},
  {NULL,NULL,"v",      "Add Character",    214,138, 40, 40},
  {NULL,NULL,"b",      "Add Character",    260,138, 40, 40},
  {NULL,NULL,"n",      "Add Character",    306,138, 40, 40},
  {NULL,NULL,"m",      "Add Character",    352,138, 40, 40},
  {NULL,NULL,",",      "Add Character",    398,138, 40, 40},
  {NULL,NULL,".",      "Add Character",    444,138, 40, 40},
  {NULL,NULL,"/",      "Add Character",    490,138, 40, 40},
};

/*****************************************************************************/
/*  Menu Buttons description                                                 */
/*****************************************************************************/
static gui_butn buttons_keyboard[46] =
{
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},0,0,0,0},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},  0,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1}, 46,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1}, 92,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},138,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},184,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},230,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},276,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},322,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},368,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},414,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},460,  0, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1}, 52, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1}, 98, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},144, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},190, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},236, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},282, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},328, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},374, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},420, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},466, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},512, 46, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1}, 18, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1}, 64, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},110, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},156, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},202, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},248, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},294, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},340, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},386, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},432, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},478, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},524, 92, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1}, 30,138, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1}, 76,138, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},122,138, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},168,138, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},214,138, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},260,138, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},306,138, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},352,138, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},398,138, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},444,138, 40, 40},
  {&button_digit_data  ,BUTTON_ACTIVE|BUTTON_SLIDE_RIGHT|BUTTON_OVER_SFX,{4,4,1,1},490,138, 40, 40},
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
  char temp[256];
  char *str = NULL;
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

  GUI_InitMenu(m);

  while (update != -1)
  {
    /* update arrows buttons */
		/*
    if (offset > 0) m->arrows[0]->state |= BUTTON_VISIBLE;
    else m->arrows[0]->state &= ~BUTTON_VISIBLE;
    if ((offset + 10) < (maxcheats + 1)) m->arrows[1]->state |= BUTTON_VISIBLE;
    else m->arrows[1]->state &= ~BUTTON_VISIBLE;
		*/

    /* draw menu */
    GUI_DrawMenu(m);

    /* update menu */
    update = GUI_UpdateMenu(m);

    /* handle pressed buttons */
    if (update > 0)
    {
      switch (m->selected)
      {
				default:
					;
      }
    }
    else if (update < 0)
    {
      if (m->bg_images[6].state & IMAGE_VISIBLE)
      {
        /* Restore old entry */
        strcpy(str, temp);
      }
    }
    else
    {
    }
  }
}

void keyboardmenu_cb(void)
{
}

void shift_keyboard(void)
{
}
