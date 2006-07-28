/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * $FreeBSD$
 * created from FreeBSD: src/sys/kern/syscalls.master,v 1.221 2006/07/28 19:05:28 jhb Exp 
 */

#include "opt_compat.h"

#include <bsm/audit_kevents.h>
#include <sys/param.h>
#include <sys/sysent.h>
#include <sys/sysproto.h>

#define AS(name) (sizeof(struct name) / sizeof(register_t))

#ifdef COMPAT_43
#define compat(n, name) n, (sy_call_t *)__CONCAT(o,name)
#else
#define compat(n, name) 0, (sy_call_t *)nosys
#endif

#ifdef COMPAT_FREEBSD4
#define compat4(n, name) n, (sy_call_t *)__CONCAT(freebsd4_,name)
#else
#define compat4(n, name) 0, (sy_call_t *)nosys
#endif

/* The casts are bogus but will do for now. */
struct sysent sysent[] = {
	{ 0, (sy_call_t *)nosys, AUE_NULL },		/* 0 = syscall */
	{ AS(sys_exit_args), (sy_call_t *)sys_exit, AUE_EXIT },	/* 1 = exit */
	{ 0, (sy_call_t *)fork, AUE_FORK },		/* 2 = fork */
	{ AS(read_args), (sy_call_t *)read, AUE_NULL },	/* 3 = read */
	{ AS(write_args), (sy_call_t *)write, AUE_NULL },	/* 4 = write */
	{ AS(open_args), (sy_call_t *)open, AUE_OPEN_RWTC },	/* 5 = open */
	{ AS(close_args), (sy_call_t *)close, AUE_CLOSE },	/* 6 = close */
	{ AS(wait_args), (sy_call_t *)wait4, AUE_WAIT4 },	/* 7 = wait4 */
	{ compat(AS(ocreat_args),creat), AUE_O_CREAT },	/* 8 = old creat */
	{ AS(link_args), (sy_call_t *)link, AUE_LINK },	/* 9 = link */
	{ AS(unlink_args), (sy_call_t *)unlink, AUE_UNLINK },	/* 10 = unlink */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 11 = obsolete execv */
	{ AS(chdir_args), (sy_call_t *)chdir, AUE_CHDIR },	/* 12 = chdir */
	{ AS(fchdir_args), (sy_call_t *)fchdir, AUE_FCHDIR },	/* 13 = fchdir */
	{ AS(mknod_args), (sy_call_t *)mknod, AUE_MKNOD },	/* 14 = mknod */
	{ AS(chmod_args), (sy_call_t *)chmod, AUE_CHMOD },	/* 15 = chmod */
	{ AS(chown_args), (sy_call_t *)chown, AUE_CHOWN },	/* 16 = chown */
	{ AS(obreak_args), (sy_call_t *)obreak, AUE_NULL },	/* 17 = break */
	{ compat4(AS(freebsd4_getfsstat_args),getfsstat), AUE_GETFSSTAT },	/* 18 = old getfsstat */
	{ compat(AS(olseek_args),lseek), AUE_LSEEK },	/* 19 = old lseek */
	{ 0, (sy_call_t *)getpid, AUE_GETPID },		/* 20 = getpid */
	{ AS(mount_args), (sy_call_t *)mount, AUE_MOUNT },	/* 21 = mount */
	{ AS(unmount_args), (sy_call_t *)unmount, AUE_UMOUNT },	/* 22 = unmount */
	{ AS(setuid_args), (sy_call_t *)setuid, AUE_SETUID },	/* 23 = setuid */
	{ 0, (sy_call_t *)getuid, AUE_GETUID },		/* 24 = getuid */
	{ 0, (sy_call_t *)geteuid, AUE_GETEUID },		/* 25 = geteuid */
	{ AS(ptrace_args), (sy_call_t *)ptrace, AUE_PTRACE },	/* 26 = ptrace */
	{ AS(recvmsg_args), (sy_call_t *)recvmsg, AUE_RECVMSG },	/* 27 = recvmsg */
	{ AS(sendmsg_args), (sy_call_t *)sendmsg, AUE_SENDMSG },	/* 28 = sendmsg */
	{ AS(recvfrom_args), (sy_call_t *)recvfrom, AUE_RECVFROM },	/* 29 = recvfrom */
	{ AS(accept_args), (sy_call_t *)accept, AUE_ACCEPT },	/* 30 = accept */
	{ AS(getpeername_args), (sy_call_t *)getpeername, AUE_GETPEERNAME },	/* 31 = getpeername */
	{ AS(getsockname_args), (sy_call_t *)getsockname, AUE_GETSOCKNAME },	/* 32 = getsockname */
	{ AS(access_args), (sy_call_t *)access, AUE_ACCESS },	/* 33 = access */
	{ AS(chflags_args), (sy_call_t *)chflags, AUE_CHFLAGS },	/* 34 = chflags */
	{ AS(fchflags_args), (sy_call_t *)fchflags, AUE_FCHFLAGS },	/* 35 = fchflags */
	{ 0, (sy_call_t *)sync, AUE_SYNC },		/* 36 = sync */
	{ AS(kill_args), (sy_call_t *)kill, AUE_KILL },	/* 37 = kill */
	{ compat(AS(ostat_args),stat), AUE_STAT },	/* 38 = old stat */
	{ 0, (sy_call_t *)getppid, AUE_GETPPID },		/* 39 = getppid */
	{ compat(AS(olstat_args),lstat), AUE_LSTAT },	/* 40 = old lstat */
	{ AS(dup_args), (sy_call_t *)dup, AUE_DUP },	/* 41 = dup */
	{ 0, (sy_call_t *)pipe, AUE_PIPE },		/* 42 = pipe */
	{ 0, (sy_call_t *)getegid, AUE_GETEGID },		/* 43 = getegid */
	{ AS(profil_args), (sy_call_t *)profil, AUE_PROFILE },	/* 44 = profil */
	{ AS(ktrace_args), (sy_call_t *)ktrace, AUE_KTRACE },	/* 45 = ktrace */
	{ compat(AS(osigaction_args),sigaction), AUE_SIGACTION },	/* 46 = old sigaction */
	{ 0, (sy_call_t *)getgid, AUE_GETGID },		/* 47 = getgid */
	{ compat(AS(osigprocmask_args),sigprocmask), AUE_SIGPROCMASK },	/* 48 = old sigprocmask */
	{ AS(getlogin_args), (sy_call_t *)getlogin, AUE_GETLOGIN },	/* 49 = getlogin */
	{ AS(setlogin_args), (sy_call_t *)setlogin, AUE_SETLOGIN },	/* 50 = setlogin */
	{ AS(acct_args), (sy_call_t *)acct, AUE_ACCT },	/* 51 = acct */
	{ compat(0,sigpending), AUE_SIGPENDING },		/* 52 = old sigpending */
	{ AS(sigaltstack_args), (sy_call_t *)sigaltstack, AUE_SIGPENDING },	/* 53 = sigaltstack */
	{ AS(ioctl_args), (sy_call_t *)ioctl, AUE_IOCTL },	/* 54 = ioctl */
	{ AS(reboot_args), (sy_call_t *)reboot, AUE_REBOOT },	/* 55 = reboot */
	{ AS(revoke_args), (sy_call_t *)revoke, AUE_REVOKE },	/* 56 = revoke */
	{ AS(symlink_args), (sy_call_t *)symlink, AUE_SYMLINK },	/* 57 = symlink */
	{ AS(readlink_args), (sy_call_t *)readlink, AUE_READLINK },	/* 58 = readlink */
	{ AS(execve_args), (sy_call_t *)execve, AUE_EXECVE },	/* 59 = execve */
	{ AS(umask_args), (sy_call_t *)umask, AUE_UMASK },	/* 60 = umask */
	{ AS(chroot_args), (sy_call_t *)chroot, AUE_CHROOT },	/* 61 = chroot */
	{ compat(AS(ofstat_args),fstat), AUE_FSTAT },	/* 62 = old fstat */
	{ compat(AS(getkerninfo_args),getkerninfo), AUE_NULL },	/* 63 = old getkerninfo */
	{ compat(0,getpagesize), AUE_O_GETPAGESIZE },	/* 64 = old getpagesize */
	{ AS(msync_args), (sy_call_t *)msync, AUE_MSYNC },	/* 65 = msync */
	{ 0, (sy_call_t *)vfork, AUE_VFORK },		/* 66 = vfork */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 67 = obsolete vread */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 68 = obsolete vwrite */
	{ AS(sbrk_args), (sy_call_t *)sbrk, AUE_SBRK },	/* 69 = sbrk */
	{ AS(sstk_args), (sy_call_t *)sstk, AUE_SSTK },	/* 70 = sstk */
	{ compat(AS(ommap_args),mmap), AUE_MMAP },	/* 71 = old mmap */
	{ AS(ovadvise_args), (sy_call_t *)ovadvise, AUE_O_VADVISE },	/* 72 = vadvise */
	{ AS(munmap_args), (sy_call_t *)munmap, AUE_MUNMAP },	/* 73 = munmap */
	{ AS(mprotect_args), (sy_call_t *)mprotect, AUE_MPROTECT },	/* 74 = mprotect */
	{ AS(madvise_args), (sy_call_t *)madvise, AUE_MADVISE },	/* 75 = madvise */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 76 = obsolete vhangup */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 77 = obsolete vlimit */
	{ AS(mincore_args), (sy_call_t *)mincore, AUE_MINCORE },	/* 78 = mincore */
	{ AS(getgroups_args), (sy_call_t *)getgroups, AUE_GETGROUPS },	/* 79 = getgroups */
	{ AS(setgroups_args), (sy_call_t *)setgroups, AUE_SETGROUPS },	/* 80 = setgroups */
	{ 0, (sy_call_t *)getpgrp, AUE_GETPGRP },		/* 81 = getpgrp */
	{ AS(setpgid_args), (sy_call_t *)setpgid, AUE_SETPGRP },	/* 82 = setpgid */
	{ AS(setitimer_args), (sy_call_t *)setitimer, AUE_SETITIMER },	/* 83 = setitimer */
	{ compat(0,wait), AUE_WAIT4 },			/* 84 = old wait */
	{ AS(swapon_args), (sy_call_t *)swapon, AUE_SWAPON },	/* 85 = swapon */
	{ AS(getitimer_args), (sy_call_t *)getitimer, AUE_GETITIMER },	/* 86 = getitimer */
	{ compat(AS(gethostname_args),gethostname), AUE_SYSCTL },	/* 87 = old gethostname */
	{ compat(AS(sethostname_args),sethostname), AUE_SYSCTL },	/* 88 = old sethostname */
	{ 0, (sy_call_t *)getdtablesize, AUE_GETDTABLESIZE },	/* 89 = getdtablesize */
	{ AS(dup2_args), (sy_call_t *)dup2, AUE_DUP2 },	/* 90 = dup2 */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 91 = getdopt */
	{ AS(fcntl_args), (sy_call_t *)fcntl, AUE_FCNTL },	/* 92 = fcntl */
	{ AS(select_args), (sy_call_t *)select, AUE_SELECT },	/* 93 = select */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 94 = setdopt */
	{ AS(fsync_args), (sy_call_t *)fsync, AUE_FSYNC },	/* 95 = fsync */
	{ AS(setpriority_args), (sy_call_t *)setpriority, AUE_SETPRIORITY },	/* 96 = setpriority */
	{ AS(socket_args), (sy_call_t *)socket, AUE_SOCKET },	/* 97 = socket */
	{ AS(connect_args), (sy_call_t *)connect, AUE_CONNECT },	/* 98 = connect */
	{ compat(AS(accept_args),accept), AUE_ACCEPT },	/* 99 = old accept */
	{ AS(getpriority_args), (sy_call_t *)getpriority, AUE_GETPRIORITY },	/* 100 = getpriority */
	{ compat(AS(osend_args),send), AUE_SEND },	/* 101 = old send */
	{ compat(AS(orecv_args),recv), AUE_RECV },	/* 102 = old recv */
	{ compat(AS(osigreturn_args),sigreturn), AUE_SIGRETURN },	/* 103 = old sigreturn */
	{ AS(bind_args), (sy_call_t *)bind, AUE_BIND },	/* 104 = bind */
	{ AS(setsockopt_args), (sy_call_t *)setsockopt, AUE_SETSOCKOPT },	/* 105 = setsockopt */
	{ AS(listen_args), (sy_call_t *)listen, AUE_LISTEN },	/* 106 = listen */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 107 = obsolete vtimes */
	{ compat(AS(osigvec_args),sigvec), AUE_NULL },	/* 108 = old sigvec */
	{ compat(AS(osigblock_args),sigblock), AUE_NULL },	/* 109 = old sigblock */
	{ compat(AS(osigsetmask_args),sigsetmask), AUE_NULL },	/* 110 = old sigsetmask */
	{ compat(AS(osigsuspend_args),sigsuspend), AUE_NULL },	/* 111 = old sigsuspend */
	{ compat(AS(osigstack_args),sigstack), AUE_NULL },	/* 112 = old sigstack */
	{ compat(AS(orecvmsg_args),recvmsg), AUE_RECVMSG },	/* 113 = old recvmsg */
	{ compat(AS(osendmsg_args),sendmsg), AUE_SENDMSG },	/* 114 = old sendmsg */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 115 = obsolete vtrace */
	{ AS(gettimeofday_args), (sy_call_t *)gettimeofday, AUE_GETTIMEOFDAY },	/* 116 = gettimeofday */
	{ AS(getrusage_args), (sy_call_t *)getrusage, AUE_GETRUSAGE },	/* 117 = getrusage */
	{ AS(getsockopt_args), (sy_call_t *)getsockopt, AUE_GETSOCKOPT },	/* 118 = getsockopt */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 119 = resuba */
	{ AS(readv_args), (sy_call_t *)readv, AUE_READV },	/* 120 = readv */
	{ AS(writev_args), (sy_call_t *)writev, AUE_WRITEV },	/* 121 = writev */
	{ AS(settimeofday_args), (sy_call_t *)settimeofday, AUE_SETTIMEOFDAY },	/* 122 = settimeofday */
	{ AS(fchown_args), (sy_call_t *)fchown, AUE_FCHOWN },	/* 123 = fchown */
	{ AS(fchmod_args), (sy_call_t *)fchmod, AUE_FCHMOD },	/* 124 = fchmod */
	{ compat(AS(recvfrom_args),recvfrom), AUE_RECVFROM },	/* 125 = old recvfrom */
	{ AS(setreuid_args), (sy_call_t *)setreuid, AUE_SETREUID },	/* 126 = setreuid */
	{ AS(setregid_args), (sy_call_t *)setregid, AUE_SETREGID },	/* 127 = setregid */
	{ AS(rename_args), (sy_call_t *)rename, AUE_RENAME },	/* 128 = rename */
	{ compat(AS(otruncate_args),truncate), AUE_TRUNCATE },	/* 129 = old truncate */
	{ compat(AS(oftruncate_args),ftruncate), AUE_FTRUNCATE },	/* 130 = old ftruncate */
	{ AS(flock_args), (sy_call_t *)flock, AUE_FLOCK },	/* 131 = flock */
	{ AS(mkfifo_args), (sy_call_t *)mkfifo, AUE_MKFIFO },	/* 132 = mkfifo */
	{ AS(sendto_args), (sy_call_t *)sendto, AUE_SENDTO },	/* 133 = sendto */
	{ AS(shutdown_args), (sy_call_t *)shutdown, AUE_SHUTDOWN },	/* 134 = shutdown */
	{ AS(socketpair_args), (sy_call_t *)socketpair, AUE_SOCKETPAIR },	/* 135 = socketpair */
	{ AS(mkdir_args), (sy_call_t *)mkdir, AUE_MKDIR },	/* 136 = mkdir */
	{ AS(rmdir_args), (sy_call_t *)rmdir, AUE_RMDIR },	/* 137 = rmdir */
	{ AS(utimes_args), (sy_call_t *)utimes, AUE_UTIMES },	/* 138 = utimes */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 139 = obsolete 4.2 sigreturn */
	{ AS(adjtime_args), (sy_call_t *)adjtime, AUE_ADJTIME },	/* 140 = adjtime */
	{ compat(AS(ogetpeername_args),getpeername), AUE_GETPEERNAME },	/* 141 = old getpeername */
	{ compat(0,gethostid), AUE_SYSCTL },		/* 142 = old gethostid */
	{ compat(AS(osethostid_args),sethostid), AUE_SYSCTL },	/* 143 = old sethostid */
	{ compat(AS(ogetrlimit_args),getrlimit), AUE_GETRLIMIT },	/* 144 = old getrlimit */
	{ compat(AS(osetrlimit_args),setrlimit), AUE_SETRLIMIT },	/* 145 = old setrlimit */
	{ compat(AS(okillpg_args),killpg), AUE_KILLPG },	/* 146 = old killpg */
	{ 0, (sy_call_t *)setsid, AUE_SETSID },		/* 147 = setsid */
	{ AS(quotactl_args), (sy_call_t *)quotactl, AUE_QUOTACTL },	/* 148 = quotactl */
	{ compat(0,quota), AUE_O_QUOTA },			/* 149 = old quota */
	{ compat(AS(getsockname_args),getsockname), AUE_GETSOCKNAME },	/* 150 = old getsockname */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 151 = sem_lock */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 152 = sem_wakeup */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 153 = asyncdaemon */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 154 = nosys */
	{ AS(nfssvc_args), (sy_call_t *)nosys, AUE_NULL },	/* 155 = nfssvc */
	{ compat(AS(ogetdirentries_args),getdirentries), AUE_GETDIRENTRIES },	/* 156 = old getdirentries */
	{ compat4(AS(freebsd4_statfs_args),statfs), AUE_STATFS },	/* 157 = old statfs */
	{ compat4(AS(freebsd4_fstatfs_args),fstatfs), AUE_FSTATFS },	/* 158 = old fstatfs */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 159 = nosys */
	{ AS(lgetfh_args), (sy_call_t *)lgetfh, AUE_LGETFH },	/* 160 = lgetfh */
	{ AS(getfh_args), (sy_call_t *)getfh, AUE_NFS_GETFH },	/* 161 = getfh */
	{ AS(getdomainname_args), (sy_call_t *)getdomainname, AUE_SYSCTL },	/* 162 = getdomainname */
	{ AS(setdomainname_args), (sy_call_t *)setdomainname, AUE_SYSCTL },	/* 163 = setdomainname */
	{ AS(uname_args), (sy_call_t *)uname, AUE_NULL },	/* 164 = uname */
	{ AS(sysarch_args), (sy_call_t *)sysarch, AUE_NULL },	/* 165 = sysarch */
	{ AS(rtprio_args), (sy_call_t *)rtprio, AUE_NULL },	/* 166 = rtprio */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 167 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 168 = nosys */
	{ AS(semsys_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 169 = semsys */
	{ AS(msgsys_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 170 = msgsys */
	{ AS(shmsys_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 171 = shmsys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 172 = nosys */
	{ AS(pread_args), (sy_call_t *)pread, AUE_PREAD },	/* 173 = pread */
	{ AS(pwrite_args), (sy_call_t *)pwrite, AUE_PWRITE },	/* 174 = pwrite */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 175 = nosys */
	{ AS(ntp_adjtime_args), (sy_call_t *)ntp_adjtime, AUE_ADJTIME },	/* 176 = ntp_adjtime */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 177 = sfork */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 178 = getdescriptor */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 179 = setdescriptor */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 180 = nosys */
	{ AS(setgid_args), (sy_call_t *)setgid, AUE_SETGID },	/* 181 = setgid */
	{ AS(setegid_args), (sy_call_t *)setegid, AUE_SETEGID },	/* 182 = setegid */
	{ AS(seteuid_args), (sy_call_t *)seteuid, AUE_SETEUID },	/* 183 = seteuid */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 184 = lfs_bmapv */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 185 = lfs_markv */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 186 = lfs_segclean */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 187 = lfs_segwait */
	{ AS(stat_args), (sy_call_t *)stat, AUE_STAT },	/* 188 = stat */
	{ AS(fstat_args), (sy_call_t *)fstat, AUE_FSTAT },	/* 189 = fstat */
	{ AS(lstat_args), (sy_call_t *)lstat, AUE_LSTAT },	/* 190 = lstat */
	{ AS(pathconf_args), (sy_call_t *)pathconf, AUE_PATHCONF },	/* 191 = pathconf */
	{ AS(fpathconf_args), (sy_call_t *)fpathconf, AUE_FPATHCONF },	/* 192 = fpathconf */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 193 = nosys */
	{ AS(__getrlimit_args), (sy_call_t *)getrlimit, AUE_GETRLIMIT },	/* 194 = getrlimit */
	{ AS(__setrlimit_args), (sy_call_t *)setrlimit, AUE_SETRLIMIT },	/* 195 = setrlimit */
	{ AS(getdirentries_args), (sy_call_t *)getdirentries, AUE_GETDIRENTRIES },	/* 196 = getdirentries */
	{ AS(mmap_args), (sy_call_t *)mmap, AUE_MMAP },	/* 197 = mmap */
	{ 0, (sy_call_t *)nosys, AUE_NULL },		/* 198 = __syscall */
	{ AS(lseek_args), (sy_call_t *)lseek, AUE_LSEEK },	/* 199 = lseek */
	{ AS(truncate_args), (sy_call_t *)truncate, AUE_TRUNCATE },	/* 200 = truncate */
	{ AS(ftruncate_args), (sy_call_t *)ftruncate, AUE_FTRUNCATE },	/* 201 = ftruncate */
	{ AS(sysctl_args), (sy_call_t *)__sysctl, AUE_SYSCTL },	/* 202 = __sysctl */
	{ AS(mlock_args), (sy_call_t *)mlock, AUE_MLOCK },	/* 203 = mlock */
	{ AS(munlock_args), (sy_call_t *)munlock, AUE_MUNLOCK },	/* 204 = munlock */
	{ AS(undelete_args), (sy_call_t *)undelete, AUE_UNDELETE },	/* 205 = undelete */
	{ AS(futimes_args), (sy_call_t *)futimes, AUE_FUTIMES },	/* 206 = futimes */
	{ AS(getpgid_args), (sy_call_t *)getpgid, AUE_GETPGID },	/* 207 = getpgid */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 208 = newreboot */
	{ AS(poll_args), (sy_call_t *)poll, AUE_POLL },	/* 209 = poll */
	{ AS(nosys_args), (sy_call_t *)lkmnosys, AUE_NULL },	/* 210 = lkmnosys */
	{ AS(nosys_args), (sy_call_t *)lkmnosys, AUE_NULL },	/* 211 = lkmnosys */
	{ AS(nosys_args), (sy_call_t *)lkmnosys, AUE_NULL },	/* 212 = lkmnosys */
	{ AS(nosys_args), (sy_call_t *)lkmnosys, AUE_NULL },	/* 213 = lkmnosys */
	{ AS(nosys_args), (sy_call_t *)lkmnosys, AUE_NULL },	/* 214 = lkmnosys */
	{ AS(nosys_args), (sy_call_t *)lkmnosys, AUE_NULL },	/* 215 = lkmnosys */
	{ AS(nosys_args), (sy_call_t *)lkmnosys, AUE_NULL },	/* 216 = lkmnosys */
	{ AS(nosys_args), (sy_call_t *)lkmnosys, AUE_NULL },	/* 217 = lkmnosys */
	{ AS(nosys_args), (sy_call_t *)lkmnosys, AUE_NULL },	/* 218 = lkmnosys */
	{ AS(nosys_args), (sy_call_t *)lkmnosys, AUE_NULL },	/* 219 = lkmnosys */
	{ AS(__semctl_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 220 = __semctl */
	{ AS(semget_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 221 = semget */
	{ AS(semop_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 222 = semop */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 223 = semconfig */
	{ AS(msgctl_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 224 = msgctl */
	{ AS(msgget_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 225 = msgget */
	{ AS(msgsnd_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 226 = msgsnd */
	{ AS(msgrcv_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 227 = msgrcv */
	{ AS(shmat_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 228 = shmat */
	{ AS(shmctl_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 229 = shmctl */
	{ AS(shmdt_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 230 = shmdt */
	{ AS(shmget_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 231 = shmget */
	{ AS(clock_gettime_args), (sy_call_t *)clock_gettime, AUE_NULL },	/* 232 = clock_gettime */
	{ AS(clock_settime_args), (sy_call_t *)clock_settime, AUE_NULL },	/* 233 = clock_settime */
	{ AS(clock_getres_args), (sy_call_t *)clock_getres, AUE_NULL },	/* 234 = clock_getres */
	{ AS(ktimer_create_args), (sy_call_t *)ktimer_create, AUE_NULL },	/* 235 = ktimer_create */
	{ AS(ktimer_delete_args), (sy_call_t *)ktimer_delete, AUE_NULL },	/* 236 = ktimer_delete */
	{ AS(ktimer_settime_args), (sy_call_t *)ktimer_settime, AUE_NULL },	/* 237 = ktimer_settime */
	{ AS(ktimer_gettime_args), (sy_call_t *)ktimer_gettime, AUE_NULL },	/* 238 = ktimer_gettime */
	{ AS(ktimer_getoverrun_args), (sy_call_t *)ktimer_getoverrun, AUE_NULL },	/* 239 = ktimer_getoverrun */
	{ AS(nanosleep_args), (sy_call_t *)nanosleep, AUE_NULL },	/* 240 = nanosleep */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 241 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 242 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 243 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 244 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 245 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 246 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 247 = nosys */
	{ AS(ntp_gettime_args), (sy_call_t *)ntp_gettime, AUE_NULL },	/* 248 = ntp_gettime */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 249 = nosys */
	{ AS(minherit_args), (sy_call_t *)minherit, AUE_MINHERIT },	/* 250 = minherit */
	{ AS(rfork_args), (sy_call_t *)rfork, AUE_RFORK },	/* 251 = rfork */
	{ AS(openbsd_poll_args), (sy_call_t *)openbsd_poll, AUE_POLL },	/* 252 = openbsd_poll */
	{ 0, (sy_call_t *)issetugid, AUE_ISSETUGID },	/* 253 = issetugid */
	{ AS(lchown_args), (sy_call_t *)lchown, AUE_LCHOWN },	/* 254 = lchown */
	{ AS(aio_read_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 255 = aio_read */
	{ AS(aio_write_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 256 = aio_write */
	{ AS(lio_listio_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 257 = lio_listio */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 258 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 259 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 260 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 261 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 262 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 263 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 264 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 265 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 266 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 267 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 268 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 269 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 270 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 271 = nosys */
	{ AS(getdents_args), (sy_call_t *)getdents, AUE_O_GETDENTS },	/* 272 = getdents */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 273 = nosys */
	{ AS(lchmod_args), (sy_call_t *)lchmod, AUE_LCHMOD },	/* 274 = lchmod */
	{ AS(lchown_args), (sy_call_t *)lchown, AUE_LCHOWN },	/* 275 = netbsd_lchown */
	{ AS(lutimes_args), (sy_call_t *)lutimes, AUE_LUTIMES },	/* 276 = lutimes */
	{ AS(msync_args), (sy_call_t *)msync, AUE_MSYNC },	/* 277 = netbsd_msync */
	{ AS(nstat_args), (sy_call_t *)nstat, AUE_STAT },	/* 278 = nstat */
	{ AS(nfstat_args), (sy_call_t *)nfstat, AUE_FSTAT },	/* 279 = nfstat */
	{ AS(nlstat_args), (sy_call_t *)nlstat, AUE_LSTAT },	/* 280 = nlstat */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 281 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 282 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 283 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 284 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 285 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 286 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 287 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 288 = nosys */
	{ AS(preadv_args), (sy_call_t *)preadv, AUE_NULL },	/* 289 = preadv */
	{ AS(pwritev_args), (sy_call_t *)pwritev, AUE_NULL },	/* 290 = pwritev */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 291 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 292 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 293 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 294 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 295 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 296 = nosys */
	{ compat4(AS(freebsd4_fhstatfs_args),fhstatfs), AUE_FHSTATFS },	/* 297 = old fhstatfs */
	{ AS(fhopen_args), (sy_call_t *)fhopen, AUE_FHOPEN },	/* 298 = fhopen */
	{ AS(fhstat_args), (sy_call_t *)fhstat, AUE_FHSTAT },	/* 299 = fhstat */
	{ AS(modnext_args), (sy_call_t *)modnext, AUE_NULL },	/* 300 = modnext */
	{ AS(modstat_args), (sy_call_t *)modstat, AUE_NULL },	/* 301 = modstat */
	{ AS(modfnext_args), (sy_call_t *)modfnext, AUE_NULL },	/* 302 = modfnext */
	{ AS(modfind_args), (sy_call_t *)modfind, AUE_NULL },	/* 303 = modfind */
	{ AS(kldload_args), (sy_call_t *)kldload, AUE_MODLOAD },	/* 304 = kldload */
	{ AS(kldunload_args), (sy_call_t *)kldunload, AUE_MODUNLOAD },	/* 305 = kldunload */
	{ AS(kldfind_args), (sy_call_t *)kldfind, AUE_NULL },	/* 306 = kldfind */
	{ AS(kldnext_args), (sy_call_t *)kldnext, AUE_NULL },	/* 307 = kldnext */
	{ AS(kldstat_args), (sy_call_t *)kldstat, AUE_NULL },	/* 308 = kldstat */
	{ AS(kldfirstmod_args), (sy_call_t *)kldfirstmod, AUE_NULL },	/* 309 = kldfirstmod */
	{ AS(getsid_args), (sy_call_t *)getsid, AUE_GETSID },	/* 310 = getsid */
	{ AS(setresuid_args), (sy_call_t *)setresuid, AUE_SETRESUID },	/* 311 = setresuid */
	{ AS(setresgid_args), (sy_call_t *)setresgid, AUE_SETRESGID },	/* 312 = setresgid */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 313 = obsolete signanosleep */
	{ AS(aio_return_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 314 = aio_return */
	{ AS(aio_suspend_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 315 = aio_suspend */
	{ AS(aio_cancel_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 316 = aio_cancel */
	{ AS(aio_error_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 317 = aio_error */
	{ AS(oaio_read_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 318 = oaio_read */
	{ AS(oaio_write_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 319 = oaio_write */
	{ AS(olio_listio_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 320 = olio_listio */
	{ 0, (sy_call_t *)yield, AUE_NULL },		/* 321 = yield */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 322 = obsolete thr_sleep */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 323 = obsolete thr_wakeup */
	{ AS(mlockall_args), (sy_call_t *)mlockall, AUE_MLOCKALL },	/* 324 = mlockall */
	{ 0, (sy_call_t *)munlockall, AUE_MUNLOCKALL },	/* 325 = munlockall */
	{ AS(__getcwd_args), (sy_call_t *)__getcwd, AUE_NULL },	/* 326 = __getcwd */
	{ AS(sched_setparam_args), (sy_call_t *)sched_setparam, AUE_NULL },	/* 327 = sched_setparam */
	{ AS(sched_getparam_args), (sy_call_t *)sched_getparam, AUE_NULL },	/* 328 = sched_getparam */
	{ AS(sched_setscheduler_args), (sy_call_t *)sched_setscheduler, AUE_NULL },	/* 329 = sched_setscheduler */
	{ AS(sched_getscheduler_args), (sy_call_t *)sched_getscheduler, AUE_NULL },	/* 330 = sched_getscheduler */
	{ 0, (sy_call_t *)sched_yield, AUE_NULL },	/* 331 = sched_yield */
	{ AS(sched_get_priority_max_args), (sy_call_t *)sched_get_priority_max, AUE_NULL },	/* 332 = sched_get_priority_max */
	{ AS(sched_get_priority_min_args), (sy_call_t *)sched_get_priority_min, AUE_NULL },	/* 333 = sched_get_priority_min */
	{ AS(sched_rr_get_interval_args), (sy_call_t *)sched_rr_get_interval, AUE_NULL },	/* 334 = sched_rr_get_interval */
	{ AS(utrace_args), (sy_call_t *)utrace, AUE_NULL },	/* 335 = utrace */
	{ compat4(AS(freebsd4_sendfile_args),sendfile), AUE_SENDFILE },	/* 336 = old sendfile */
	{ AS(kldsym_args), (sy_call_t *)kldsym, AUE_NULL },	/* 337 = kldsym */
	{ AS(jail_args), (sy_call_t *)jail, AUE_JAIL },	/* 338 = jail */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 339 = pioctl */
	{ AS(sigprocmask_args), (sy_call_t *)sigprocmask, AUE_SIGPROCMASK },	/* 340 = sigprocmask */
	{ AS(sigsuspend_args), (sy_call_t *)sigsuspend, AUE_SIGSUSPEND },	/* 341 = sigsuspend */
	{ compat4(AS(freebsd4_sigaction_args),sigaction), AUE_SIGACTION },	/* 342 = old sigaction */
	{ AS(sigpending_args), (sy_call_t *)sigpending, AUE_SIGPENDING },	/* 343 = sigpending */
	{ compat4(AS(freebsd4_sigreturn_args),sigreturn), AUE_SIGRETURN },	/* 344 = old sigreturn */
	{ AS(sigtimedwait_args), (sy_call_t *)sigtimedwait, AUE_SIGWAIT },	/* 345 = sigtimedwait */
	{ AS(sigwaitinfo_args), (sy_call_t *)sigwaitinfo, AUE_NULL },	/* 346 = sigwaitinfo */
	{ AS(__acl_get_file_args), (sy_call_t *)__acl_get_file, AUE_NULL },	/* 347 = __acl_get_file */
	{ AS(__acl_set_file_args), (sy_call_t *)__acl_set_file, AUE_NULL },	/* 348 = __acl_set_file */
	{ AS(__acl_get_fd_args), (sy_call_t *)__acl_get_fd, AUE_NULL },	/* 349 = __acl_get_fd */
	{ AS(__acl_set_fd_args), (sy_call_t *)__acl_set_fd, AUE_NULL },	/* 350 = __acl_set_fd */
	{ AS(__acl_delete_file_args), (sy_call_t *)__acl_delete_file, AUE_NULL },	/* 351 = __acl_delete_file */
	{ AS(__acl_delete_fd_args), (sy_call_t *)__acl_delete_fd, AUE_NULL },	/* 352 = __acl_delete_fd */
	{ AS(__acl_aclcheck_file_args), (sy_call_t *)__acl_aclcheck_file, AUE_NULL },	/* 353 = __acl_aclcheck_file */
	{ AS(__acl_aclcheck_fd_args), (sy_call_t *)__acl_aclcheck_fd, AUE_NULL },	/* 354 = __acl_aclcheck_fd */
	{ AS(extattrctl_args), (sy_call_t *)extattrctl, AUE_EXTATTRCTL },	/* 355 = extattrctl */
	{ AS(extattr_set_file_args), (sy_call_t *)extattr_set_file, AUE_EXTATTR_SET_FILE },	/* 356 = extattr_set_file */
	{ AS(extattr_get_file_args), (sy_call_t *)extattr_get_file, AUE_EXTATTR_GET_FILE },	/* 357 = extattr_get_file */
	{ AS(extattr_delete_file_args), (sy_call_t *)extattr_delete_file, AUE_EXTATTR_DELETE_FILE },	/* 358 = extattr_delete_file */
	{ AS(aio_waitcomplete_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 359 = aio_waitcomplete */
	{ AS(getresuid_args), (sy_call_t *)getresuid, AUE_GETRESUID },	/* 360 = getresuid */
	{ AS(getresgid_args), (sy_call_t *)getresgid, AUE_GETRESGID },	/* 361 = getresgid */
	{ 0, (sy_call_t *)kqueue, AUE_NULL },		/* 362 = kqueue */
	{ AS(kevent_args), (sy_call_t *)kevent, AUE_NULL },	/* 363 = kevent */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 364 = __cap_get_proc */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 365 = __cap_set_proc */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 366 = __cap_get_fd */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 367 = __cap_get_file */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 368 = __cap_set_fd */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 369 = __cap_set_file */
	{ AS(nosys_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 370 = lkmressys */
	{ AS(extattr_set_fd_args), (sy_call_t *)extattr_set_fd, AUE_EXTATTR_SET_FD },	/* 371 = extattr_set_fd */
	{ AS(extattr_get_fd_args), (sy_call_t *)extattr_get_fd, AUE_EXTATTR_GET_FD },	/* 372 = extattr_get_fd */
	{ AS(extattr_delete_fd_args), (sy_call_t *)extattr_delete_fd, AUE_EXTATTR_DELETE_FD },	/* 373 = extattr_delete_fd */
	{ AS(__setugid_args), (sy_call_t *)__setugid, AUE_NULL },	/* 374 = __setugid */
	{ AS(nfsclnt_args), (sy_call_t *)nosys, AUE_NULL },	/* 375 = nfsclnt */
	{ AS(eaccess_args), (sy_call_t *)eaccess, AUE_EACCESS },	/* 376 = eaccess */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 377 = afs_syscall */
	{ AS(nmount_args), (sy_call_t *)nmount, AUE_NMOUNT },	/* 378 = nmount */
	{ 0, (sy_call_t *)kse_exit, AUE_NULL },		/* 379 = kse_exit */
	{ AS(kse_wakeup_args), (sy_call_t *)kse_wakeup, AUE_NULL },	/* 380 = kse_wakeup */
	{ AS(kse_create_args), (sy_call_t *)kse_create, AUE_NULL },	/* 381 = kse_create */
	{ AS(kse_thr_interrupt_args), (sy_call_t *)kse_thr_interrupt, AUE_NULL },	/* 382 = kse_thr_interrupt */
	{ AS(kse_release_args), (sy_call_t *)kse_release, AUE_NULL },	/* 383 = kse_release */
	{ AS(__mac_get_proc_args), (sy_call_t *)__mac_get_proc, AUE_NULL },	/* 384 = __mac_get_proc */
	{ AS(__mac_set_proc_args), (sy_call_t *)__mac_set_proc, AUE_NULL },	/* 385 = __mac_set_proc */
	{ AS(__mac_get_fd_args), (sy_call_t *)__mac_get_fd, AUE_NULL },	/* 386 = __mac_get_fd */
	{ AS(__mac_get_file_args), (sy_call_t *)__mac_get_file, AUE_NULL },	/* 387 = __mac_get_file */
	{ AS(__mac_set_fd_args), (sy_call_t *)__mac_set_fd, AUE_NULL },	/* 388 = __mac_set_fd */
	{ AS(__mac_set_file_args), (sy_call_t *)__mac_set_file, AUE_NULL },	/* 389 = __mac_set_file */
	{ AS(kenv_args), (sy_call_t *)kenv, AUE_NULL },	/* 390 = kenv */
	{ AS(lchflags_args), (sy_call_t *)lchflags, AUE_LCHFLAGS },	/* 391 = lchflags */
	{ AS(uuidgen_args), (sy_call_t *)uuidgen, AUE_NULL },	/* 392 = uuidgen */
	{ AS(sendfile_args), (sy_call_t *)sendfile, AUE_SENDFILE },	/* 393 = sendfile */
	{ AS(mac_syscall_args), (sy_call_t *)mac_syscall, AUE_NULL },	/* 394 = mac_syscall */
	{ AS(getfsstat_args), (sy_call_t *)getfsstat, AUE_GETFSSTAT },	/* 395 = getfsstat */
	{ AS(statfs_args), (sy_call_t *)statfs, AUE_STATFS },	/* 396 = statfs */
	{ AS(fstatfs_args), (sy_call_t *)fstatfs, AUE_FSTATFS },	/* 397 = fstatfs */
	{ AS(fhstatfs_args), (sy_call_t *)fhstatfs, AUE_NULL },	/* 398 = fhstatfs */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 399 = nosys */
	{ AS(ksem_close_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 400 = ksem_close */
	{ AS(ksem_post_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 401 = ksem_post */
	{ AS(ksem_wait_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 402 = ksem_wait */
	{ AS(ksem_trywait_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 403 = ksem_trywait */
	{ AS(ksem_init_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 404 = ksem_init */
	{ AS(ksem_open_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 405 = ksem_open */
	{ AS(ksem_unlink_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 406 = ksem_unlink */
	{ AS(ksem_getvalue_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 407 = ksem_getvalue */
	{ AS(ksem_destroy_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 408 = ksem_destroy */
	{ AS(__mac_get_pid_args), (sy_call_t *)__mac_get_pid, AUE_NULL },	/* 409 = __mac_get_pid */
	{ AS(__mac_get_link_args), (sy_call_t *)__mac_get_link, AUE_NULL },	/* 410 = __mac_get_link */
	{ AS(__mac_set_link_args), (sy_call_t *)__mac_set_link, AUE_NULL },	/* 411 = __mac_set_link */
	{ AS(extattr_set_link_args), (sy_call_t *)extattr_set_link, AUE_EXTATTR_SET_LINK },	/* 412 = extattr_set_link */
	{ AS(extattr_get_link_args), (sy_call_t *)extattr_get_link, AUE_EXTATTR_GET_LINK },	/* 413 = extattr_get_link */
	{ AS(extattr_delete_link_args), (sy_call_t *)extattr_delete_link, AUE_EXTATTR_DELETE_LINK },	/* 414 = extattr_delete_link */
	{ AS(__mac_execve_args), (sy_call_t *)__mac_execve, AUE_NULL },	/* 415 = __mac_execve */
	{ AS(sigaction_args), (sy_call_t *)sigaction, AUE_SIGACTION },	/* 416 = sigaction */
	{ AS(sigreturn_args), (sy_call_t *)sigreturn, AUE_SIGRETURN },	/* 417 = sigreturn */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 418 = __xstat */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 419 = __xfstat */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 420 = __xlstat */
	{ AS(getcontext_args), (sy_call_t *)getcontext, AUE_NULL },	/* 421 = getcontext */
	{ AS(setcontext_args), (sy_call_t *)setcontext, AUE_NULL },	/* 422 = setcontext */
	{ AS(swapcontext_args), (sy_call_t *)swapcontext, AUE_NULL },	/* 423 = swapcontext */
	{ AS(swapoff_args), (sy_call_t *)swapoff, AUE_SWAPOFF },	/* 424 = swapoff */
	{ AS(__acl_get_link_args), (sy_call_t *)__acl_get_link, AUE_NULL },	/* 425 = __acl_get_link */
	{ AS(__acl_set_link_args), (sy_call_t *)__acl_set_link, AUE_NULL },	/* 426 = __acl_set_link */
	{ AS(__acl_delete_link_args), (sy_call_t *)__acl_delete_link, AUE_NULL },	/* 427 = __acl_delete_link */
	{ AS(__acl_aclcheck_link_args), (sy_call_t *)__acl_aclcheck_link, AUE_NULL },	/* 428 = __acl_aclcheck_link */
	{ AS(sigwait_args), (sy_call_t *)sigwait, AUE_SIGWAIT },	/* 429 = sigwait */
	{ AS(thr_create_args), (sy_call_t *)thr_create, AUE_NULL },	/* 430 = thr_create */
	{ AS(thr_exit_args), (sy_call_t *)thr_exit, AUE_NULL },	/* 431 = thr_exit */
	{ AS(thr_self_args), (sy_call_t *)thr_self, AUE_NULL },	/* 432 = thr_self */
	{ AS(thr_kill_args), (sy_call_t *)thr_kill, AUE_NULL },	/* 433 = thr_kill */
	{ AS(_umtx_lock_args), (sy_call_t *)_umtx_lock, AUE_NULL },	/* 434 = _umtx_lock */
	{ AS(_umtx_unlock_args), (sy_call_t *)_umtx_unlock, AUE_NULL },	/* 435 = _umtx_unlock */
	{ AS(jail_attach_args), (sy_call_t *)jail_attach, AUE_NULL },	/* 436 = jail_attach */
	{ AS(extattr_list_fd_args), (sy_call_t *)extattr_list_fd, AUE_EXTATTR_LIST_FD },	/* 437 = extattr_list_fd */
	{ AS(extattr_list_file_args), (sy_call_t *)extattr_list_file, AUE_EXTATTR_LIST_FILE },	/* 438 = extattr_list_file */
	{ AS(extattr_list_link_args), (sy_call_t *)extattr_list_link, AUE_EXTATTR_LIST_LINK },	/* 439 = extattr_list_link */
	{ AS(kse_switchin_args), (sy_call_t *)kse_switchin, AUE_NULL },	/* 440 = kse_switchin */
	{ AS(ksem_timedwait_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 441 = ksem_timedwait */
	{ AS(thr_suspend_args), (sy_call_t *)thr_suspend, AUE_NULL },	/* 442 = thr_suspend */
	{ AS(thr_wake_args), (sy_call_t *)thr_wake, AUE_NULL },	/* 443 = thr_wake */
	{ AS(kldunloadf_args), (sy_call_t *)kldunloadf, AUE_NULL },	/* 444 = kldunloadf */
	{ AS(audit_args), (sy_call_t *)audit, AUE_AUDIT },	/* 445 = audit */
	{ AS(auditon_args), (sy_call_t *)auditon, AUE_AUDITON },	/* 446 = auditon */
	{ AS(getauid_args), (sy_call_t *)getauid, AUE_GETAUID },	/* 447 = getauid */
	{ AS(setauid_args), (sy_call_t *)setauid, AUE_SETAUID },	/* 448 = setauid */
	{ AS(getaudit_args), (sy_call_t *)getaudit, AUE_GETAUDIT },	/* 449 = getaudit */
	{ AS(setaudit_args), (sy_call_t *)setaudit, AUE_SETAUDIT },	/* 450 = setaudit */
	{ AS(getaudit_addr_args), (sy_call_t *)getaudit_addr, AUE_GETAUDIT_ADDR },	/* 451 = getaudit_addr */
	{ AS(setaudit_addr_args), (sy_call_t *)setaudit_addr, AUE_SETAUDIT_ADDR },	/* 452 = setaudit_addr */
	{ AS(auditctl_args), (sy_call_t *)auditctl, AUE_AUDITCTL },	/* 453 = auditctl */
	{ AS(_umtx_op_args), (sy_call_t *)_umtx_op, AUE_NULL },	/* 454 = _umtx_op */
	{ AS(thr_new_args), (sy_call_t *)thr_new, AUE_NULL },	/* 455 = thr_new */
	{ AS(sigqueue_args), (sy_call_t *)sigqueue, AUE_NULL },	/* 456 = sigqueue */
	{ AS(kmq_open_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 457 = kmq_open */
	{ AS(kmq_setattr_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 458 = kmq_setattr */
	{ AS(kmq_timedreceive_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 459 = kmq_timedreceive */
	{ AS(kmq_timedsend_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 460 = kmq_timedsend */
	{ AS(kmq_notify_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 461 = kmq_notify */
	{ AS(kmq_unlink_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 462 = kmq_unlink */
	{ AS(abort2_args), (sy_call_t *)abort2, AUE_NULL },	/* 463 = abort2 */
	{ AS(thr_set_name_args), (sy_call_t *)thr_set_name, AUE_NULL },	/* 464 = thr_set_name */
	{ AS(aio_fsync_args), (sy_call_t *)lkmressys, AUE_NULL },	/* 465 = aio_fsync */
	{ AS(thr_setscheduler_args), (sy_call_t *)thr_setscheduler, AUE_NULL },	/* 466 = thr_setscheduler */
	{ AS(thr_getscheduler_args), (sy_call_t *)thr_getscheduler, AUE_NULL },	/* 467 = thr_getscheduler */
	{ AS(thr_setschedparam_args), (sy_call_t *)thr_setschedparam, AUE_NULL },	/* 468 = thr_setschedparam */
};
