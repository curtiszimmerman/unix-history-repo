/*-
 * Copyright (c) 2000 Katsurajima Naoto <raven@katsurajima.seya.yokohama.jp>
 * Copyright (c) 2001 Cameron Grant <cg@freebsd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHERIN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THEPOSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <dev/sound/pcm/sound.h>
#include <dev/sound/pcm/ac97.h>
#include <dev/sound/pci/ich.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>

SND_DECLARE_FILE("$FreeBSD$");

/* -------------------------------------------------------------------- */

#define ICH_TIMEOUT 1000 /* semaphore timeout polling count */
#define ICH_DTBL_LENGTH 32
#define ICH_DEFAULT_BUFSZ 16384
#define ICH_MAX_BUFSZ 65536

#define INTEL_VENDORID	0x8086
#define SIS_VENDORID	0x1039
#define NVIDIA_VENDORID	0x10de
#define AMD_VENDORID	0x1022

#define INTEL_82440MX	0x7195
#define INTEL_82801AA	0x2415
#define INTEL_82801AB	0x2425
#define INTEL_82801BA	0x2445
#define INTEL_82801CA	0x2485
#define INTEL_82801DB	0x24c5	/* ICH4 needs special handling */
#define INTEL_82801EB	0x24d5	/* ICH5 needs to be treated as ICH4 */
#define INTEL_6300ESB	0x25a6	/* 6300ESB needs to be treated as ICH4 */
#define INTEL_82801FB	0x266e	/* ICH6 needs to be treated as ICH4 */
#define INTEL_82801GB	0x27de	/* ICH7 needs to be treated as ICH4 */
#define SIS_7012	0x7012	/* SiS 7012 needs special handling */
#define NVIDIA_NFORCE	0x01b1
#define NVIDIA_NFORCE2	0x006a
#define NVIDIA_NFORCE2_400	0x008a
#define NVIDIA_NFORCE3	0x00da
#define NVIDIA_NFORCE3_250	0x00ea
#define NVIDIA_NFORCE4	0x0059
#define NVIDIA_NFORCE_410_MCP	0x026b
#define AMD_768		0x7445
#define AMD_8111	0x746d

#define ICH_LOCK(sc)		snd_mtxlock((sc)->ich_lock)
#define ICH_UNLOCK(sc)		snd_mtxunlock((sc)->ich_lock)
#define ICH_LOCK_ASSERT(sc)	snd_mtxassert((sc)->ich_lock)

static const struct ich_type {
        uint16_t	vendor;
        uint16_t	devid;
	uint32_t	options;
#define PROBE_LOW	0x01
        char		*name;
} ich_devs[] = {
	{ INTEL_VENDORID,	INTEL_82440MX,	0,
		"Intel 440MX" },
	{ INTEL_VENDORID,	INTEL_82801AA,	0,
		"Intel ICH (82801AA)" },
	{ INTEL_VENDORID,	INTEL_82801AB,	0,
		"Intel ICH (82801AB)" },
	{ INTEL_VENDORID,	INTEL_82801BA,	0,
		"Intel ICH2 (82801BA)" },
	{ INTEL_VENDORID,	INTEL_82801CA,	0,
		"Intel ICH3 (82801CA)" },
	{ INTEL_VENDORID,	INTEL_82801DB,	PROBE_LOW,
		"Intel ICH4 (82801DB)" },
	{ INTEL_VENDORID,	INTEL_82801EB,	PROBE_LOW,
		"Intel ICH5 (82801EB)" },
	{ INTEL_VENDORID,	INTEL_6300ESB,	PROBE_LOW,
		"Intel 6300ESB" },
	{ INTEL_VENDORID,	INTEL_82801FB,	PROBE_LOW,
		"Intel ICH6 (82801FB)" },
	{ INTEL_VENDORID,	INTEL_82801GB,	PROBE_LOW,
		"Intel ICH7 (82801GB)" },
	{ SIS_VENDORID,		SIS_7012,	0,
		"SiS 7012" },
	{ NVIDIA_VENDORID,	NVIDIA_NFORCE,	0,
		"nVidia nForce" },
	{ NVIDIA_VENDORID,	NVIDIA_NFORCE2,	0,
		"nVidia nForce2" },
	{ NVIDIA_VENDORID,	NVIDIA_NFORCE2_400,	0,
		"nVidia nForce2 400" },
	{ NVIDIA_VENDORID,	NVIDIA_NFORCE3,	0,
		"nVidia nForce3" },
	{ NVIDIA_VENDORID,	NVIDIA_NFORCE3_250,	0,
		"nVidia nForce3 250" },
	{ NVIDIA_VENDORID,	NVIDIA_NFORCE4,	0,
		"nVidia nForce4" },
	{ NVIDIA_VENDORID,	NVIDIA_NFORCE_410_MCP,	0,
		"nVidia nForce 410 MCP" },
	{ AMD_VENDORID,		AMD_768,	0,
		"AMD-768" },
	{ AMD_VENDORID,		AMD_8111,	0,
		"AMD-8111" }
};

/* buffer descriptor */
struct ich_desc {
	volatile u_int32_t buffer;
	volatile u_int32_t length;
};

struct sc_info;

/* channel registers */
struct sc_chinfo {
	u_int32_t num:8, run:1, run_save:1;
	u_int32_t blksz, blkcnt, spd;
	u_int32_t regbase, spdreg;
	u_int32_t imask;
	u_int32_t civ;

	struct snd_dbuf *buffer;
	struct pcm_channel *channel;
	struct sc_info *parent;

	struct ich_desc *dtbl;
	bus_addr_t desc_addr;
};

/* device private data */
struct sc_info {
	device_t dev;
	int hasvra, hasvrm, hasmic;
	unsigned int chnum, bufsz;
	int sample_size, swap_reg;

	struct resource *nambar, *nabmbar, *irq;
	int regtype, nambarid, nabmbarid, irqid;
	bus_space_tag_t nambart, nabmbart;
	bus_space_handle_t nambarh, nabmbarh;
	bus_dma_tag_t dmat;
	bus_dmamap_t dtmap;
	void *ih;

	struct ac97_info *codec;
	struct sc_chinfo ch[3];
	int ac97rate;
	struct ich_desc *dtbl;
	bus_addr_t desc_addr;
	struct intr_config_hook	intrhook;
	int use_intrhook;
	uint16_t vendor;
	uint16_t devid;
	uint32_t flags;
#define IGNORE_PCR	0x01
	struct mtx *ich_lock;
};

/* -------------------------------------------------------------------- */

static u_int32_t ich_fmt[] = {
	AFMT_STEREO | AFMT_S16_LE,
	0
};
static struct pcmchan_caps ich_vrcaps = {8000, 48000, ich_fmt, 0};
static struct pcmchan_caps ich_caps = {48000, 48000, ich_fmt, 0};

/* -------------------------------------------------------------------- */
/* Hardware */
static __inline u_int32_t
ich_rd(struct sc_info *sc, int regno, int size)
{
	switch (size) {
	case 1:
		return bus_space_read_1(sc->nabmbart, sc->nabmbarh, regno);
	case 2:
		return bus_space_read_2(sc->nabmbart, sc->nabmbarh, regno);
	case 4:
		return bus_space_read_4(sc->nabmbart, sc->nabmbarh, regno);
	default:
		return 0xffffffff;
	}
}

static __inline void
ich_wr(struct sc_info *sc, int regno, u_int32_t data, int size)
{
	switch (size) {
	case 1:
		bus_space_write_1(sc->nabmbart, sc->nabmbarh, regno, data);
		break;
	case 2:
		bus_space_write_2(sc->nabmbart, sc->nabmbarh, regno, data);
		break;
	case 4:
		bus_space_write_4(sc->nabmbart, sc->nabmbarh, regno, data);
		break;
	}
}

/* ac97 codec */
static int
ich_waitcd(void *devinfo)
{
	int i;
	u_int32_t data;
	struct sc_info *sc = (struct sc_info *)devinfo;

	for (i = 0; i < ICH_TIMEOUT; i++) {
		data = ich_rd(sc, ICH_REG_ACC_SEMA, 1);
		if ((data & 0x01) == 0)
			return 0;
		DELAY(1);
	}
	if ((sc->flags & IGNORE_PCR) != 0)
		return (0);
	device_printf(sc->dev, "CODEC semaphore timeout\n");
	return ETIMEDOUT;
}

static int
ich_rdcd(kobj_t obj, void *devinfo, int regno)
{
	struct sc_info *sc = (struct sc_info *)devinfo;

	regno &= 0xff;
	ich_waitcd(sc);

	return bus_space_read_2(sc->nambart, sc->nambarh, regno);
}

static int
ich_wrcd(kobj_t obj, void *devinfo, int regno, u_int16_t data)
{
	struct sc_info *sc = (struct sc_info *)devinfo;

	regno &= 0xff;
	ich_waitcd(sc);
	bus_space_write_2(sc->nambart, sc->nambarh, regno, data);

	return 0;
}

static kobj_method_t ich_ac97_methods[] = {
	KOBJMETHOD(ac97_read,		ich_rdcd),
	KOBJMETHOD(ac97_write,		ich_wrcd),
	{ 0, 0 }
};
AC97_DECLARE(ich_ac97);

/* -------------------------------------------------------------------- */
/* common routines */

static void
ich_filldtbl(struct sc_chinfo *ch)
{
	struct sc_info *sc = ch->parent;
	u_int32_t base;
	int i;

	base = sndbuf_getbufaddr(ch->buffer);
	if (ch->blksz > sc->bufsz / ch->blkcnt)
		ch->blksz = sc->bufsz / ch->blkcnt;
	sndbuf_resize(ch->buffer, ch->blkcnt, ch->blksz);
	ch->blksz = sndbuf_getblksz(ch->buffer);

	for (i = 0; i < ICH_DTBL_LENGTH; i++) {
		ch->dtbl[i].buffer = base + (ch->blksz * (i % ch->blkcnt));
		ch->dtbl[i].length = ICH_BDC_IOC
				   | (ch->blksz / ch->parent->sample_size);
	}
}

static int
ich_resetchan(struct sc_info *sc, int num)
{
	int i, cr, regbase;

	if (num == 0)
		regbase = ICH_REG_PO_BASE;
	else if (num == 1)
		regbase = ICH_REG_PI_BASE;
	else if (num == 2)
		regbase = ICH_REG_MC_BASE;
	else
		return ENXIO;

	ich_wr(sc, regbase + ICH_REG_X_CR, 0, 1);
#if 1
	/* This may result in no sound output on NForce 2 MBs, see PR 73987 */
	DELAY(100);
#else
	(void)ich_rd(sc, regbase + ICH_REG_X_CR, 1);
#endif
	ich_wr(sc, regbase + ICH_REG_X_CR, ICH_X_CR_RR, 1);
	for (i = 0; i < ICH_TIMEOUT; i++) {
		cr = ich_rd(sc, regbase + ICH_REG_X_CR, 1);
		if (cr == 0)
			return 0;
	}

	device_printf(sc->dev, "cannot reset channel %d\n", num);
	return ENXIO;
}

/* -------------------------------------------------------------------- */
/* channel interface */

static void *
ichchan_init(kobj_t obj, void *devinfo, struct snd_dbuf *b, struct pcm_channel *c, int dir)
{
	struct sc_info *sc = devinfo;
	struct sc_chinfo *ch;
	unsigned int num;

	ICH_LOCK(sc);
	num = sc->chnum++;
	ch = &sc->ch[num];
	ch->num = num;
	ch->buffer = b;
	ch->channel = c;
	ch->parent = sc;
	ch->run = 0;
	ch->dtbl = sc->dtbl + (ch->num * ICH_DTBL_LENGTH);
	ch->desc_addr = sc->desc_addr + (ch->num * ICH_DTBL_LENGTH) *
		sizeof(struct ich_desc);
	ch->blkcnt = 2;
	ch->blksz = sc->bufsz / ch->blkcnt;

	switch(ch->num) {
	case 0: /* play */
		KASSERT(dir == PCMDIR_PLAY, ("wrong direction"));
		ch->regbase = ICH_REG_PO_BASE;
		ch->spdreg = sc->hasvra? AC97_REGEXT_FDACRATE : 0;
		ch->imask = ICH_GLOB_STA_POINT;
		break;

	case 1: /* record */
		KASSERT(dir == PCMDIR_REC, ("wrong direction"));
		ch->regbase = ICH_REG_PI_BASE;
		ch->spdreg = sc->hasvra? AC97_REGEXT_LADCRATE : 0;
		ch->imask = ICH_GLOB_STA_PIINT;
		break;

	case 2: /* mic */
		KASSERT(dir == PCMDIR_REC, ("wrong direction"));
		ch->regbase = ICH_REG_MC_BASE;
		ch->spdreg = sc->hasvrm? AC97_REGEXT_MADCRATE : 0;
		ch->imask = ICH_GLOB_STA_MINT;
		break;

	default:
		return NULL;
	}

	ICH_UNLOCK(sc);
	if (sndbuf_alloc(ch->buffer, sc->dmat, sc->bufsz) != 0)
		return NULL;

	ICH_LOCK(sc);
	ich_wr(sc, ch->regbase + ICH_REG_X_BDBAR, (u_int32_t)(ch->desc_addr), 4);
	ICH_UNLOCK(sc);

	return ch;
}

static int
ichchan_setformat(kobj_t obj, void *data, u_int32_t format)
{
	return 0;
}

static int
ichchan_setspeed(kobj_t obj, void *data, u_int32_t speed)
{
	struct sc_chinfo *ch = data;
	struct sc_info *sc = ch->parent;

	if (ch->spdreg) {
		int r, ac97rate;

		ICH_LOCK(sc);
		if (sc->ac97rate <= 32000 || sc->ac97rate >= 64000)
			sc->ac97rate = 48000;
		ac97rate = sc->ac97rate;
		ICH_UNLOCK(sc);
		r = (speed * 48000) / ac97rate;
		/*
		 * Cast the return value of ac97_setrate() to u_int so that
		 * the math don't overflow into the negative range.
		 */
		ch->spd = ((u_int)ac97_setrate(sc->codec, ch->spdreg, r) *
				ac97rate) / 48000;
	} else {
		ch->spd = 48000;
	}
	return ch->spd;
}

static int
ichchan_setblocksize(kobj_t obj, void *data, u_int32_t blocksize)
{
	struct sc_chinfo *ch = data;
	struct sc_info *sc = ch->parent;

	ch->blksz = blocksize;
	ich_filldtbl(ch);
	ICH_LOCK(sc);
	ich_wr(sc, ch->regbase + ICH_REG_X_LVI, ch->blkcnt - 1, 1);
	ICH_UNLOCK(sc);

	return ch->blksz;
}

static int
ichchan_trigger(kobj_t obj, void *data, int go)
{
	struct sc_chinfo *ch = data;
	struct sc_info *sc = ch->parent;

	switch (go) {
	case PCMTRIG_START:
		ch->run = 1;
		ICH_LOCK(sc);
		ich_wr(sc, ch->regbase + ICH_REG_X_BDBAR, (u_int32_t)(ch->desc_addr), 4);
		ich_wr(sc, ch->regbase + ICH_REG_X_CR, ICH_X_CR_RPBM | ICH_X_CR_LVBIE | ICH_X_CR_IOCE, 1);
		ICH_UNLOCK(sc);
		break;

	case PCMTRIG_ABORT:
		ICH_LOCK(sc);
		ich_resetchan(sc, ch->num);
		ICH_UNLOCK(sc);
		ch->run = 0;
		break;
	}
	return 0;
}

static int
ichchan_getptr(kobj_t obj, void *data)
{
	struct sc_chinfo *ch = data;
	struct sc_info *sc = ch->parent;
      	u_int32_t pos;

	ICH_LOCK(sc);
	ch->civ = ich_rd(sc, ch->regbase + ICH_REG_X_CIV, 1) % ch->blkcnt;
	ICH_UNLOCK(sc);

	pos = ch->civ * ch->blksz;

	return pos;
}

static struct pcmchan_caps *
ichchan_getcaps(kobj_t obj, void *data)
{
	struct sc_chinfo *ch = data;

	return ch->spdreg? &ich_vrcaps : &ich_caps;
}

static kobj_method_t ichchan_methods[] = {
	KOBJMETHOD(channel_init,		ichchan_init),
	KOBJMETHOD(channel_setformat,		ichchan_setformat),
	KOBJMETHOD(channel_setspeed,		ichchan_setspeed),
	KOBJMETHOD(channel_setblocksize,	ichchan_setblocksize),
	KOBJMETHOD(channel_trigger,		ichchan_trigger),
	KOBJMETHOD(channel_getptr,		ichchan_getptr),
	KOBJMETHOD(channel_getcaps,		ichchan_getcaps),
	{ 0, 0 }
};
CHANNEL_DECLARE(ichchan);

/* -------------------------------------------------------------------- */
/* The interrupt handler */

static void
ich_intr(void *p)
{
	struct sc_info *sc = (struct sc_info *)p;
	struct sc_chinfo *ch;
	u_int32_t cbi, lbi, lvi, st, gs;
	int i;

	ICH_LOCK(sc);
	gs = ich_rd(sc, ICH_REG_GLOB_STA, 4) & ICH_GLOB_STA_IMASK;
	if (gs & (ICH_GLOB_STA_PRES | ICH_GLOB_STA_SRES)) {
		/* Clear resume interrupt(s) - nothing doing with them */
		ich_wr(sc, ICH_REG_GLOB_STA, gs, 4);
	}
	gs &= ~(ICH_GLOB_STA_PRES | ICH_GLOB_STA_SRES);

	for (i = 0; i < 3; i++) {
		ch = &sc->ch[i];
		if ((ch->imask & gs) == 0)
			continue;
		gs &= ~ch->imask;
		st = ich_rd(sc, ch->regbase +
				(sc->swap_reg ? ICH_REG_X_PICB : ICH_REG_X_SR),
			    2);
		st &= ICH_X_SR_FIFOE | ICH_X_SR_BCIS | ICH_X_SR_LVBCI;
		if (st & (ICH_X_SR_BCIS | ICH_X_SR_LVBCI)) {
				/* block complete - update buffer */
			if (ch->run) {
				ICH_UNLOCK(sc);
				chn_intr(ch->channel);
				ICH_LOCK(sc);
			}
			lvi = ich_rd(sc, ch->regbase + ICH_REG_X_LVI, 1);
			cbi = ch->civ % ch->blkcnt;
			if (cbi == 0)
				cbi = ch->blkcnt - 1;
			else
				cbi--;
			lbi = lvi % ch->blkcnt;
			if (cbi >= lbi)
				lvi += cbi - lbi;
			else
				lvi += cbi + ch->blkcnt - lbi;
			lvi %= ICH_DTBL_LENGTH;
			ich_wr(sc, ch->regbase + ICH_REG_X_LVI, lvi, 1);

		}
		/* clear status bit */
		ich_wr(sc, ch->regbase +
			   (sc->swap_reg ? ICH_REG_X_PICB : ICH_REG_X_SR),
		       st, 2);
	}
	ICH_UNLOCK(sc);
	if (gs != 0) {
		device_printf(sc->dev,
			      "Unhandled interrupt, gs_intr = %x\n", gs);
	}
}

/* ------------------------------------------------------------------------- */
/* Sysctl to control ac97 speed (some boards appear to end up using
 * XTAL_IN rather than BIT_CLK for link timing).
 */

static int
ich_initsys(struct sc_info* sc)
{
#ifdef SND_DYNSYSCTL
	SYSCTL_ADD_INT(snd_sysctl_tree(sc->dev),
		       SYSCTL_CHILDREN(snd_sysctl_tree_top(sc->dev)),
		       OID_AUTO, "ac97rate", CTLFLAG_RW,
		       &sc->ac97rate, 48000,
		       "AC97 link rate (default = 48000)");
#endif /* SND_DYNSYSCTL */
	return 0;
}

/* -------------------------------------------------------------------- */
/* Calibrate card to determine the clock source.  The source maybe a
 * function of the ac97 codec initialization code (to be investigated).
 */

static
void ich_calibrate(void *arg)
{
	struct sc_info *sc;
	struct sc_chinfo *ch;
	struct timeval t1, t2;
	u_int8_t ociv, nciv;
	u_int32_t wait_us, actual_48k_rate, bytes;

	sc = (struct sc_info *)arg;
	ch = &sc->ch[1];

	if (sc->use_intrhook)
		config_intrhook_disestablish(&sc->intrhook);

	/*
	 * Grab audio from input for fixed interval and compare how
	 * much we actually get with what we expect.  Interval needs
	 * to be sufficiently short that no interrupts are
	 * generated.
	 */

	KASSERT(ch->regbase == ICH_REG_PI_BASE, ("wrong direction"));

	bytes = sndbuf_getsize(ch->buffer) / 2;
	ichchan_setblocksize(0, ch, bytes);

	/*
	 * our data format is stereo, 16 bit so each sample is 4 bytes.
	 * assuming we get 48000 samples per second, we get 192000 bytes/sec.
	 * we're going to start recording with interrupts disabled and measure
	 * the time taken for one block to complete.  we know the block size,
	 * we know the time in microseconds, we calculate the sample rate:
	 *
	 * actual_rate [bps] = bytes / (time [s] * 4)
	 * actual_rate [bps] = (bytes * 1000000) / (time [us] * 4)
	 * actual_rate [Hz] = (bytes * 250000) / time [us]
	 */

	/* prepare */
	ociv = ich_rd(sc, ch->regbase + ICH_REG_X_CIV, 1);
	nciv = ociv;
	ich_wr(sc, ch->regbase + ICH_REG_X_BDBAR, (u_int32_t)(ch->desc_addr), 4);

	/* start */
	microtime(&t1);
	ich_wr(sc, ch->regbase + ICH_REG_X_CR, ICH_X_CR_RPBM, 1);

	/* wait */
	while (nciv == ociv) {
		microtime(&t2);
		if (t2.tv_sec - t1.tv_sec > 1)
			break;
		nciv = ich_rd(sc, ch->regbase + ICH_REG_X_CIV, 1);
	}
	microtime(&t2);

	/* stop */
	ich_wr(sc, ch->regbase + ICH_REG_X_CR, 0, 1);

	/* reset */
	DELAY(100);
	ich_wr(sc, ch->regbase + ICH_REG_X_CR, ICH_X_CR_RR, 1);

	/* turn time delta into us */
	wait_us = ((t2.tv_sec - t1.tv_sec) * 1000000) + t2.tv_usec - t1.tv_usec;

	if (nciv == ociv) {
		device_printf(sc->dev, "ac97 link rate calibration timed out after %d us\n", wait_us);
		return;
	}

	actual_48k_rate = (bytes * 250000) / wait_us;

	if (actual_48k_rate < 47500 || actual_48k_rate > 48500) {
		sc->ac97rate = actual_48k_rate;
	} else {
		sc->ac97rate = 48000;
	}

	if (bootverbose || sc->ac97rate != 48000) {
		device_printf(sc->dev, "measured ac97 link rate at %d Hz", actual_48k_rate);
		if (sc->ac97rate != actual_48k_rate)
			printf(", will use %d Hz", sc->ac97rate);
	 	printf("\n");
	}

	return;
}

/* -------------------------------------------------------------------- */
/* Probe and attach the card */

static void
ich_setmap(void *arg, bus_dma_segment_t *segs, int nseg, int error)
{
	struct sc_info *sc = (struct sc_info *)arg;
	sc->desc_addr = segs->ds_addr;
	return;
}

static int
ich_init(struct sc_info *sc)
{
	u_int32_t stat;

	ich_wr(sc, ICH_REG_GLOB_CNT, ICH_GLOB_CTL_COLD, 4);
	DELAY(600000);
	stat = ich_rd(sc, ICH_REG_GLOB_STA, 4);

	if ((stat & ICH_GLOB_STA_PCR) == 0) {
		/* ICH4/ICH5 may fail when busmastering is enabled. Continue */
		if (sc->vendor == INTEL_VENDORID && (
		    sc->devid == INTEL_82801DB || sc->devid == INTEL_82801EB ||
		    sc->devid == INTEL_6300ESB || sc->devid == INTEL_82801FB ||
		    sc->devid == INTEL_82801GB)) {
			sc->flags |= IGNORE_PCR;
			device_printf(sc->dev, "primary codec not ready!\n");
		}
	}

#if 0
	ich_wr(sc, ICH_REG_GLOB_CNT, ICH_GLOB_CTL_COLD | ICH_GLOB_CTL_PRES, 4);
#else
	ich_wr(sc, ICH_REG_GLOB_CNT, ICH_GLOB_CTL_COLD, 4);
#endif

	if (ich_resetchan(sc, 0) || ich_resetchan(sc, 1))
		return ENXIO;
	if (sc->hasmic && ich_resetchan(sc, 2))
		return ENXIO;

	return 0;
}

static int
ich_pci_probe(device_t dev)
{
	int i;
	uint16_t devid, vendor;

	vendor = pci_get_vendor(dev);
	devid = pci_get_device(dev);
	for (i = 0; i < sizeof(ich_devs)/sizeof(ich_devs[0]); i++) {
		if (vendor == ich_devs[i].vendor &&
				devid == ich_devs[i].devid) {
			device_set_desc(dev, ich_devs[i].name);
			/* allow a better driver to override us */
			if ((ich_devs[i].options & PROBE_LOW) != 0)
				return (BUS_PROBE_LOW_PRIORITY);
			return (BUS_PROBE_DEFAULT);
		}
	}
	return (ENXIO);
}

static int
ich_pci_attach(device_t dev)
{
	uint32_t		subdev;
	u_int16_t		extcaps;
	uint16_t		devid, vendor;
	struct sc_info 		*sc;
	char 			status[SND_STATUSLEN];

	if ((sc = malloc(sizeof(*sc), M_DEVBUF, M_NOWAIT | M_ZERO)) == NULL) {
		device_printf(dev, "cannot allocate softc\n");
		return ENXIO;
	}

	sc->ich_lock = snd_mtxcreate(device_get_nameunit(dev), "sound softc");
	sc->dev = dev;

	vendor = sc->vendor = pci_get_vendor(dev);
	devid = sc->devid = pci_get_device(dev);
	subdev = (pci_get_subdevice(dev) << 16) | pci_get_subvendor(dev);
	/*
	 * The SiS 7012 register set isn't quite like the standard ich.
	 * There really should be a general "quirks" mechanism.
	 */
	if (vendor == SIS_VENDORID && devid == SIS_7012) {
		sc->swap_reg = 1;
		sc->sample_size = 1;
	} else {
		sc->swap_reg = 0;
		sc->sample_size = 2;
	}

	/*
	 * Enable bus master. On ich4/5 this may prevent the detection of
	 * the primary codec becoming ready in ich_init().
	 */
	pci_enable_busmaster(dev);

	/*
	 * By default, ich4 has NAMBAR and NABMBAR i/o spaces as
	 * read-only.  Need to enable "legacy support", by poking into
	 * pci config space.  The driver should use MMBAR and MBBAR,
	 * but doing so will mess things up here.  ich4 has enough new
	 * features it warrants it's own driver. 
	 */
	if (vendor == INTEL_VENDORID && (devid == INTEL_82801DB ||
	    devid == INTEL_82801EB || devid == INTEL_6300ESB ||
	    devid == INTEL_82801FB || devid == INTEL_82801GB)) {
		sc->nambarid = PCIR_MMBAR;
		sc->nabmbarid = PCIR_MBBAR;
		sc->regtype = SYS_RES_MEMORY;
		pci_write_config(dev, PCIR_ICH_LEGACY, ICH_LEGACY_ENABLE, 1);
	} else {
		sc->nambarid = PCIR_NAMBAR;
		sc->nabmbarid = PCIR_NABMBAR;
		sc->regtype = SYS_RES_IOPORT;
	}

	sc->nambar = bus_alloc_resource_any(dev, sc->regtype, 
		&sc->nambarid, RF_ACTIVE);
	sc->nabmbar = bus_alloc_resource_any(dev, sc->regtype, 
		&sc->nabmbarid, RF_ACTIVE);

	if (!sc->nambar || !sc->nabmbar) {
		device_printf(dev, "unable to map IO port space\n");
		goto bad;
	}

	sc->nambart = rman_get_bustag(sc->nambar);
	sc->nambarh = rman_get_bushandle(sc->nambar);
	sc->nabmbart = rman_get_bustag(sc->nabmbar);
	sc->nabmbarh = rman_get_bushandle(sc->nabmbar);

	sc->bufsz = pcm_getbuffersize(dev, 4096, ICH_DEFAULT_BUFSZ, ICH_MAX_BUFSZ);
	if (bus_dma_tag_create(NULL, 8, 0, BUS_SPACE_MAXADDR_32BIT, BUS_SPACE_MAXADDR,
			       NULL, NULL, sc->bufsz, 1, 0x3ffff, 0,
			       NULL, NULL, &sc->dmat) != 0) {
		device_printf(dev, "unable to create dma tag\n");
		goto bad;
	}

	sc->irqid = 0;
	sc->irq = bus_alloc_resource_any(dev, SYS_RES_IRQ, &sc->irqid,
		RF_ACTIVE | RF_SHAREABLE);
	if (!sc->irq || snd_setup_intr(dev, sc->irq, INTR_MPSAFE, ich_intr, sc, &sc->ih)) {
		device_printf(dev, "unable to map interrupt\n");
		goto bad;
	}

	if (ich_init(sc)) {
		device_printf(dev, "unable to initialize the card\n");
		goto bad;
	}

	if (bus_dmamem_alloc(sc->dmat, (void **)&sc->dtbl,
		    BUS_DMA_NOWAIT, &sc->dtmap))
		goto bad;

	if (bus_dmamap_load(sc->dmat, sc->dtmap, sc->dtbl,
		    sizeof(struct ich_desc) * ICH_DTBL_LENGTH * 3,
		    ich_setmap, sc, 0))
		goto bad;

	sc->codec = AC97_CREATE(dev, sc, ich_ac97);
	if (sc->codec == NULL)
		goto bad;

	/*
	 * Turn on inverted external amplifier sense flags for few
	 * 'special' boards.
	 */
	switch (subdev) {
	case 0x202f161f:	/* Gateway 7326GZ */
	case 0x203a161f:	/* Gateway 4028GZ */
	case 0x204c161f:	/* Kvazar-Micro Senator 3592XT */
	case 0x8144104d:	/* Sony VAIO PCG-TR* */
	case 0x8197104d:	/* Sony S1XP */
	case 0x81c0104d:	/* Sony VAIO type T */
	case 0x81c5104d:	/* Sony VAIO VGN B1VP/B1XP */
	case 0x3089103c:	/* Compaq Presario B3800 */
		ac97_setflags(sc->codec, ac97_getflags(sc->codec) | AC97_F_EAPD_INV);
		break;
	default:
		break;
	}

	mixer_init(dev, ac97_getmixerclass(), sc->codec);

	/* check and set VRA function */
	extcaps = ac97_getextcaps(sc->codec);
	sc->hasvra = extcaps & AC97_EXTCAP_VRA;
	sc->hasvrm = extcaps & AC97_EXTCAP_VRM;
	sc->hasmic = ac97_getcaps(sc->codec) & AC97_CAP_MICCHANNEL;
	ac97_setextmode(sc->codec, sc->hasvra | sc->hasvrm);

	if (pcm_register(dev, sc, 1, sc->hasmic? 2 : 1))
		goto bad;

	pcm_addchan(dev, PCMDIR_PLAY, &ichchan_class, sc);		/* play */
	pcm_addchan(dev, PCMDIR_REC, &ichchan_class, sc);		/* record */
	if (sc->hasmic)
		pcm_addchan(dev, PCMDIR_REC, &ichchan_class, sc);	/* record mic */

	snprintf(status, SND_STATUSLEN, "at io 0x%lx, 0x%lx irq %ld bufsz %u %s",
		 rman_get_start(sc->nambar), rman_get_start(sc->nabmbar), rman_get_start(sc->irq), sc->bufsz,PCM_KLDSTRING(snd_ich));

	pcm_setstatus(dev, status);

	ich_initsys(sc);

	sc->intrhook.ich_func = ich_calibrate;
	sc->intrhook.ich_arg = sc;
	sc->use_intrhook = 1;
	if (config_intrhook_establish(&sc->intrhook) != 0) {
		device_printf(dev, "Cannot establish calibration hook, will calibrate now\n");
		sc->use_intrhook = 0;
		ich_calibrate(sc);
	}

	return 0;

bad:
	if (sc->codec)
		ac97_destroy(sc->codec);
	if (sc->ih)
		bus_teardown_intr(dev, sc->irq, sc->ih);
	if (sc->irq)
		bus_release_resource(dev, SYS_RES_IRQ, sc->irqid, sc->irq);
	if (sc->nambar)
		bus_release_resource(dev, sc->regtype,
		    sc->nambarid, sc->nambar);
	if (sc->nabmbar)
		bus_release_resource(dev, sc->regtype,
		    sc->nabmbarid, sc->nabmbar);
	if (sc->dtmap)
		bus_dmamap_unload(sc->dmat, sc->dtmap);
	if (sc->dmat)
		bus_dma_tag_destroy(sc->dmat);
	if (sc->ich_lock)
		snd_mtxfree(sc->ich_lock);
	free(sc, M_DEVBUF);
	return ENXIO;
}

static int
ich_pci_detach(device_t dev)
{
	struct sc_info *sc;
	int r;

	r = pcm_unregister(dev);
	if (r)
		return r;
	sc = pcm_getdevinfo(dev);

	bus_teardown_intr(dev, sc->irq, sc->ih);
	bus_release_resource(dev, SYS_RES_IRQ, sc->irqid, sc->irq);
	bus_release_resource(dev, sc->regtype, sc->nambarid, sc->nambar);
	bus_release_resource(dev, sc->regtype, sc->nabmbarid, sc->nabmbar);
	bus_dmamap_unload(sc->dmat, sc->dtmap);
	bus_dma_tag_destroy(sc->dmat);
	snd_mtxfree(sc->ich_lock);
	free(sc, M_DEVBUF);
	return 0;
}

static void
ich_pci_codec_reset(struct sc_info *sc)
{
	int i;
	uint32_t control;

	control = ich_rd(sc, ICH_REG_GLOB_CNT, 4); 
	control &= ~(ICH_GLOB_CTL_SHUT);
	control |= (control & ICH_GLOB_CTL_COLD) ?
		    ICH_GLOB_CTL_WARM : ICH_GLOB_CTL_COLD;
	ich_wr(sc, ICH_REG_GLOB_CNT, control, 4);

	for (i = 500000; i; i--) {
	     	if (ich_rd(sc, ICH_REG_GLOB_STA, 4) & ICH_GLOB_STA_PCR)
			break;		/*		or ICH_SCR? */
		DELAY(1);
	}

	if (i <= 0)
		printf("%s: time out\n", __func__);
}

static int
ich_pci_suspend(device_t dev)
{
	struct sc_info *sc;
	int i;

	sc = pcm_getdevinfo(dev);
	ICH_LOCK(sc);
	for (i = 0 ; i < 3; i++) {
		sc->ch[i].run_save = sc->ch[i].run;
		if (sc->ch[i].run) {
			ICH_UNLOCK(sc);
			ichchan_trigger(0, &sc->ch[i], PCMTRIG_ABORT);
			ICH_LOCK(sc);
		}
	}
	ICH_UNLOCK(sc);
	return 0;
}

static int
ich_pci_resume(device_t dev)
{
	struct sc_info *sc;
	int i;

	sc = pcm_getdevinfo(dev);

	if (sc->regtype == SYS_RES_IOPORT)
		pci_enable_io(dev, SYS_RES_IOPORT);
	else
		pci_enable_io(dev, SYS_RES_MEMORY);
	pci_enable_busmaster(dev);

	ICH_LOCK(sc);
	/* Reinit audio device */
    	if (ich_init(sc) == -1) {
		device_printf(dev, "unable to reinitialize the card\n");
		ICH_UNLOCK(sc);
		return ENXIO;
	}
	/* Reinit mixer */
	ich_pci_codec_reset(sc);
	ICH_UNLOCK(sc);
	ac97_setextmode(sc->codec, sc->hasvra | sc->hasvrm);
    	if (mixer_reinit(dev) == -1) {
		device_printf(dev, "unable to reinitialize the mixer\n");
		return ENXIO;
	}
	/* Re-start DMA engines */
	for (i = 0 ; i < 3; i++) {
		struct sc_chinfo *ch = &sc->ch[i];
		if (sc->ch[i].run_save) {
			ichchan_setblocksize(0, ch, ch->blksz);
			ichchan_setspeed(0, ch, ch->spd);
			ichchan_trigger(0, ch, PCMTRIG_START);
		}
	}
	return 0;
}

static device_method_t ich_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		ich_pci_probe),
	DEVMETHOD(device_attach,	ich_pci_attach),
	DEVMETHOD(device_detach,	ich_pci_detach),
	DEVMETHOD(device_suspend, 	ich_pci_suspend),
	DEVMETHOD(device_resume,	ich_pci_resume),
	{ 0, 0 }
};

static driver_t ich_driver = {
	"pcm",
	ich_methods,
	PCM_SOFTC_SIZE,
};

DRIVER_MODULE(snd_ich, pci, ich_driver, pcm_devclass, 0, 0);
MODULE_DEPEND(snd_ich, sound, SOUND_MINVER, SOUND_PREFVER, SOUND_MAXVER);
MODULE_VERSION(snd_ich, 1);
