#!/usr/bin/perl -w
#
# Copyright (c) 2001  The FreeBSD Project
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# From src/usr.bin/gensetdefs/gensetdefs.c,v 1.6 2000/09/29 13:29:43 dfr
#
# $FreeBSD$

use strict;

my %logalign = (4 => 2, 8 => 3);
my %datadecl = (4 => ".long", 8 => ".quad");
my %sets = ();
my $pointersize = 0;
my $objdump = $ENV{'OBJDUMP'} || 'objdump';

# Run objdump over each object file to find all defined linker sets.
# Linker sets are sections with names starting with `.set.'
foreach my $objfile (@ARGV) {
    open(SECTION, "$objdump -h $objfile |") or die "objdump -h $objfile failed : $!";
    while (<SECTION>) {
        my ($index, $name, $size, $vma, $lma, $offset, $align) = split;

        next unless $name;

        if ($name =~ /^.set./) {
            $name =~ s/^.set.//;
	    $sets{$name} ||= 0;
            $sets{$name} += hex "0x$size";

	    # Assume the size of a pointer is equal to it's alignment
	    # and thus the alignment of the section.
            if ($pointersize < eval $align) {
            	$pointersize = eval $align;
            }
        }
    }
    close SECTION;
}

# Generate our list of set definitions
my @setdefs;
while (my ($name, $size) = each %sets) {
    my $elements = $size / $pointersize;
    push @setdefs, "DEFINE_SET($name, $elements);\n";
}

# Create setdefs.h
open(SETDEFS_H, "> setdefs.h") or die "Could not write to setdefs.h : $!";
print SETDEFS_H sort @setdefs;
close SETDEFS_H;

# Create setdef0.c
open(SETDEF0_C, "> setdef0.c") or die "Could not write to setdef0.c : $!";
print SETDEF0_C <<END;
/* This file is generated by $0, do not edit. */

#define DEFINE_SET(set, count)			\\
__asm__(".section .set." #set ",\\"aw\\"");	\\
__asm__(".globl " #set);			\\
__asm__(".type " #set ",\@object");		\\
__asm__(".p2align $logalign{$pointersize}");	\\
__asm__(#set ":");				\\
__asm__("$datadecl{$pointersize} " #count);	\\
__asm__(".previous")

#include "setdefs.h"
END
close SETDEF0_C;

# Create setdef1.c
open(SETDEF1_C, "> setdef1.c") or die "Could not write to setdef1.c : $!";
print SETDEF1_C <<END;
/* This file is generated by $0, do not edit. */

#define DEFINE_SET(set, count)			\\
__asm__(".section .set." #set ",\\"aw\\"");	\\
__asm__("$datadecl{$pointersize} 0");		\\
__asm__(".previous")

#include "setdefs.h"
END
close SETDEF1_C;

exit;
