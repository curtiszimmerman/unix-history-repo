/*-
 * Copyright (c) 1994 S�ren Schmidt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer 
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software withough specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/exec.h>
#include <sys/conf.h>
#include <sys/sysent.h>
#include <sys/lkm.h>
#include <sys/imgact_elf.h>
#include "i386/linux/linux.h"

extern const struct execsw linux_execsw;

MOD_EXEC(linux, -1, &linux_execsw);

extern Elf32_Brandinfo *linux_brandlist[];

static int
linux_load(struct lkm_table *lkmtp, int cmd)
{
	Elf32_Brandinfo **brandinfo;
	int error;

	error = 0;

	for (brandinfo = &linux_brandlist[0]; *brandinfo != NULL; ++brandinfo)
		if (elf_insert_brand_entry(*brandinfo) < 0)
			error = 1;

	if (error)
		uprintf("Could not install ELF interpreter entry\n");
	/* uprintf("Linux emulator installed\n"); XXX - shut up, you! */
	return 0;
}

static int
linux_unload(struct lkm_table *lkmtp, int cmd)
{
	Elf32_Brandinfo **brandinfo;
	int error;

	error = 0;

	for (brandinfo = &linux_brandlist[0]; *brandinfo != NULL; ++brandinfo)
		if (elf_remove_brand_entry(*brandinfo) < 0)
			error = 1;

	if (error)
		uprintf("Could not deinstall ELF interpreter entry\n");
	uprintf("Linux emulator removed\n");
	return 0;
}

int
linux_mod(struct lkm_table *lkmtp, int cmd, int ver)
{
	MOD_DISPATCH(linux,lkmtp, cmd, ver,
		linux_load, linux_unload, lkm_nullcmd);
}
