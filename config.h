/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
/* appearance */
static const unsigned int borderpx = 1; /*窗口边框像素*/
static const unsigned int gappx = 6;    /* gaps between windows */
static const unsigned int snap = 32; /*snap是窗口差多少像素与屏幕边缘正好卡住,
在平铺模式下也是窗口拖动多少像素变成浮动状态*/
static const int showbar = 1; /* 是否显示bar，0为不显示*/
static const int topbar = 1;  /* bar是否在顶部，0为底部*/
static const int vertpad = 5; /* vertical padding of bar */
static const int sidepad = 5; /* horizontal padding of bar */
static const char *fonts[] = {"FiraCode Nerd Font Mono:size=16"}; /*dwm字体*/
static const char dmenufont[] = "FiraCode Nerd Font:size=18"; /*dmenu字体*/
static const char col_gray1[] = "#222222";
static const char col_gray2[] = "#444444";
static const char col_gray3[] = "#bbbbbb";
static const char col_gray4[] = "#eeeeee";
static const char col_purp1[] = "#1d1f21";
static const char col_cyan[] = "#005577";
static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3] = {
    /*               fg         bg         border   */
    [SchemeNorm] =
        {col_gray3, col_gray1,
         col_gray2}, /*普通状态下的颜色方案，分别是前景色，背景色，窗口边框颜色*/
    [SchemeSel] = {col_gray4, col_purp1, col_cyan}, /*选中状态下的颜色方案*/
};
static const unsigned int alphas[][3] = {
    /*               fg      bg        border     */
    [SchemeNorm] = {OPAQUE, baralpha, borderalpha},
    [SchemeSel] = {OPAQUE, baralpha, borderalpha},
};

/* tagging */
static const char *tags[] = {"󱢇", "󰊯", "", "",
                             "󰕷", "󰙝", "󰘳"}; /*显示在bar上的tag文本*/

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = titlejj
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    {"Gimp", NULL, NULL, 0, 1, -1},
    {"Google Chrome", NULL, NULL, 1 << 1, 0, -1},
}; /*程序规则，决定程序如何运行，其中class和instance和title可以通过xprop工具获取，
 tags mask决定其显示在哪个标签，0意味着当前标签，
 1 << 8 表示二进制1 0000 0000也就是9，isfloating为1是浮动。*/

/* layout(s) */
static const float mfact = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;   /* number of clients in master area */
static const int resizehints =
    1; /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen =
    1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
    /* symbol     arrange function */
    {"", tile},    /* 平铺*/
    {"", NULL},    /* NULL意味着浮动*/
    {"", monocle}, /*单窗口聚焦*/
}; /*布局，这里可以修改显示的文字。*/

/* key definitions */
#define MODKEY                                                                 \
    Mod4Mask /*modkey默认为alt，也就是这里定义的Mod1Mask，        \
Mod2Mask为NumLock，Mod4Mask为super键，也就是windows键*/
#define TAGKEYS(KEY, TAG)                                                      \
    {MODKEY, KEY, view, {.ui = 1 << TAG}},                                     \
        {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},             \
        {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                      \
        {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
    {                                                                          \
        .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                   \
    }

/* commands */
static char dmenumon[2] =
    "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
    "rofi",  "-no-lazy-grab", "-show",  "drun",
    "-modi", "drun",          "-theme", ".config/rofi/launchers/misc/blurry",
    NULL};
static const char *termcmd[] = {
    "alacritty", NULL}; /*你可以在这里添加要执行的终端默认为alacritty*/
static const char *raisevolume[] = {"amixer", "-q",     "set", "Master",
                                    "5%+",    "unmute", NULL};
static const char *lowervolume[] = {"amixer", "-q",     "set", "Master",
                                    "5%-",    "unmute", NULL};
static const char *mutevolume[] = {"amixer", "-q",     "set",
                                   "Master", "toggle", NULL};
static const char *lightup[] = {"xbacklight", "-inc", "10", NULL};
static const char *lightdown[] = {"xbacklight", "-dec", "5", NULL};
static const char *flameshotcmd[] = {"flameshot", "gui", NULL};
static const char *rangercmd[] = {"alacritty", "-e", "ranger", NULL};
static Key keys[] = {
    /* modifier                     key        function        argument */
    {MODKEY | ShiftMask, XK_p, spawn, {.v = flameshotcmd}},
    {MODKEY | ShiftMask, XK_Return, spawn, {.v = termcmd}},
    {MODKEY, XK_r, spawn, {.v = rangercmd}},
    {MODKEY, XK_Left, spawn, {.v = lightup}},
    {MODKEY, XK_Right, spawn, {.v = lightdown}},
    {MODKEY, XK_p, spawn, {.v = dmenucmd}},
    {MODKEY, XK_b, togglebar, {0}},
    {MODKEY, XK_j, focusstack, {.i = +1}},
    {MODKEY, XK_k, focusstack, {.i = -1}},
    {MODKEY, XK_i, incnmaster, {.i = +1}},
    {MODKEY, XK_d, incnmaster, {.i = -1}},
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY, XK_Return, zoom, {0}},
    {MODKEY, XK_Tab, view, {0}},
    {MODKEY | ShiftMask, XK_c, killclient, {0}},
    {MODKEY, XK_t, setlayout, {.v = &layouts[0]}},
    {MODKEY, XK_f, setlayout, {.v = &layouts[1]}},
    {MODKEY, XK_m, setlayout, {.v = &layouts[2]}},
    {MODKEY, XK_space, setlayout, {0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY, XK_0, view, {.ui = ~0}},
    {MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
    {MODKEY, XK_comma, focusmon, {.i = -1}},
    {MODKEY, XK_period, focusmon, {.i = +1}},
    {MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
    {MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8){MODKEY | ShiftMask, XK_q, quit, {0}},
    {0, XF86XK_AudioRaiseVolume, spawn, {.v = raisevolume}},
    {0, XF86XK_AudioLowerVolume, spawn, {.v = lowervolume}},
    {0, XF86XK_AudioMute, spawn, {.v = mutevolume}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, {.v = termcmd}},
    {ClkClientWin, MODKEY, Button1, movemouse, {0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
};
