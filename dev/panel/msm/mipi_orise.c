/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdint.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <sys/types.h>
#include <err.h>
#include <reg.h>
#include <debug.h>
#include <target/display.h>
#include <platform/iomap.h>

#define WVGA_MIPI_FB_WIDTH            480
#define WVGA_MIPI_FB_HEIGHT           800

#define TRULY_PANEL_FRAME_RATE        60
#define TRULY_PANEL_NUM_OF_LANES      2
#define TRULY_PANEL_LANE_SWAP         0
#define TRULY_PANEL_T_CLK_PRE         0x04
#define TRULY_PANEL_T_CLK_POST        0x18
#define TRULY_PANEL_BPP               24
#define TRULY_PANEL_CLK_RATE          343848960

// exit_sleep
static char disp_on0[2] = {
	0x11, 0x00
};
// race_unknown_cmd
static char disp_on1[2] = {
	0xff, 0x78
};
// race_set_pixel_format
static char disp_on2[2] = {
	0x3A, 0x70
};
// race_ctrl_display
static char disp_on3[2] = {
	0x53, 0x2C
};
// race_cabc_on
static char disp_on4[2] = {
	0x55, 0x01
};
// race_set_pixel_format
static char disp_on5[2] = {
	0x3A, 0x70
};
// display_on
static char disp_on6[2] = {
	0x29, 0x00
};

static struct mipi_dsi_cmd orise_panel_video_mode_cmds[] = {
	{sizeof(disp_on0), (char *)disp_on0},
	{sizeof(disp_on1), (char *)disp_on1},
	{sizeof(disp_on2), (char *)disp_on2},
	{sizeof(disp_on3), (char *)disp_on3},
	{sizeof(disp_on4), (char *)disp_on4},
	{sizeof(disp_on5), (char *)disp_on5},
	{sizeof(disp_on6), (char *)disp_on6}
};

int mipi_orise_config(void *pdata)
{
	int ret = NO_ERROR;
	/* 2 Lanes -- Enables Data Lane0, 1 */
	unsigned char lane_en = 3;
	unsigned long low_pwr_stop_mode = 0;
	unsigned char pulse_mode_hsa_he = 0;

	/* Needed or else will have blank line at top of display */
	unsigned char eof_bllp_pwr = 0x8;

	unsigned char interleav = 0;
	unsigned long ctl_base = 0x600;

	struct lcdc_panel_info *lcdc = NULL;
	struct msm_panel_info *pinfo = (struct msm_panel_info *) pdata;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return ERR_INVALID_ARGS;

	ret = mipi_dsi_video_mode_config((pinfo->xres + lcdc->xres_pad),
		(pinfo->yres + lcdc->yres_pad),
		(pinfo->xres),
		(pinfo->yres),
		(lcdc->h_front_porch),
		(lcdc->h_back_porch + lcdc->h_pulse_width),
		(lcdc->v_front_porch),
		(lcdc->v_back_porch + lcdc->v_pulse_width),
		(lcdc->h_pulse_width),
		(lcdc->v_pulse_width),
		pinfo->mipi.dst_format,
		pinfo->mipi.traffic_mode,
		lane_en,
		low_pwr_stop_mode,
		eof_bllp_pwr,
		interleav);
	return ret;
}

int mipi_orise_on(void)
{
	int ret = NO_ERROR;
	return ret;
}

int mipi_orise_off(void)
{
	int ret = NO_ERROR;
	return ret;
}

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* DSIPHY_REGULATOR_CTRL */
	.regulator = {0x02, 0x08, 0x05, 0x00, 0x20},
	/* DSIPHY_CTRL */
	.ctrl = {0x5F, 0x00, 0x00, 0x10},
	/* DSIPHY_STRENGTH_CTRL */
	.strength = {0xFF, 0x00, 0x06, 0x00},
	/* DSIPHY_TIMING_CTRL */
	.timing = {0x67, 0x16, 0x0D, 0x00, 0x38, 0x3C, 0x12, 0x19, 0x18, 0x03, 0x04, 0xA0},
	/* DSIPHY_PLL_CTRL */
	.pll = {0x01, 0x25, 0x30, 0xC2, 0x00, 0x30, 0x0C, 0x62, 0x41, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01},
};


void mipi_orise_init(struct msm_panel_info *pinfo)
{
	dprintf(INFO, "mipi_orise driver - created by gabixdev\n");

	if (!pinfo) {
		dprintf(CRITICAL, "[mipi_orise] Oops... panel is null?\n");
		return;
	}

	pinfo->xres = 480;
	pinfo->yres = 800;
	pinfo->lcdc.h_back_porch = MIPI_HSYNC_BACK_PORCH_DCLK;
	pinfo->lcdc.h_front_porch = 45;
	pinfo->lcdc.h_pulse_width = 4;
	pinfo->lcdc.v_back_porch = MIPI_VSYNC_BACK_PORCH_LINES;
	pinfo->lcdc.v_front_porch = 14;
	pinfo->lcdc.v_pulse_width = 1;
	pinfo->mipi.num_of_lanes = TRULY_PANEL_NUM_OF_LANES;
	pinfo->mipi.frame_rate = TRULY_PANEL_FRAME_RATE;

	pinfo->type = MIPI_VIDEO_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = TRULY_PANEL_BPP;
	pinfo->clk_rate = TRULY_PANEL_CLK_RATE;

	pinfo->mipi.mode = DSI_VIDEO_MODE;
	pinfo->mipi.traffic_mode = 1;
	pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo->mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo->mipi.tx_eot_append = TRUE;

	pinfo->mipi.lane_swap = TRULY_PANEL_LANE_SWAP;
	pinfo->mipi.t_clk_post = TRULY_PANEL_T_CLK_POST;
	pinfo->mipi.t_clk_pre = TRULY_PANEL_T_CLK_PRE;
	pinfo->mipi.panel_cmds = orise_panel_video_mode_cmds;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(orise_panel_video_mode_cmds);

	pinfo->on = mipi_orise_on;
	pinfo->off = mipi_orise_off;
	pinfo->config = mipi_orise_config;

	dprintf(INFO, "[mipi_orise] ok.\n");

	return;
}
