dnl
dnl Bash specific tests
dnl
dnl Some derived from PDKSH 5.1.3 autoconf tests
dnl
dnl
dnl Check if dup2() does not clear the close on exec flag
dnl
AC_DEFUN(BASH_DUP2_CLOEXEC_CHECK,
[AC_MSG_CHECKING(if dup2 fails to clear the close-on-exec flag)
AC_CACHE_VAL(bash_cv_dup2_broken,
[AC_TRY_RUN([
#include <sys/types.h>
#include <fcntl.h>
main()
{
  int fd1, fd2, fl;
  fd1 = open("/dev/null", 2);
  if (fcntl(fd1, 2, 1) < 0)
    exit(1);
  fd2 = dup2(fd1, 1);
  if (fd2 < 0)
    exit(2);
  fl = fcntl(fd2, 1, 0);
  /* fl will be 1 if dup2 did not reset the close-on-exec flag. */
  exit(fl != 1);
}
], bash_cv_dup2_broken=yes, bash_cv_dup2_broken=no,
    [AC_MSG_WARN(cannot check dup2 if cross compiling -- defaulting to no)
     bash_cv_dup2_broken=no])
])
AC_MSG_RESULT($bash_cv_dup2_broken)
if test $bash_cv_dup2_broken = yes; then
AC_DEFINE(DUP2_BROKEN)
fi
])

dnl Check type of signal routines (posix, 4.2bsd, 4.1bsd or v7)
AC_DEFUN(BASH_SIGNAL_CHECK,
[AC_REQUIRE([AC_TYPE_SIGNAL])
AC_MSG_CHECKING(for type of signal functions)
AC_CACHE_VAL(bash_cv_signal_vintage,
[
  AC_TRY_LINK([#include <signal.h>],[
    sigset_t ss;
    struct sigaction sa;
    sigemptyset(&ss); sigsuspend(&ss);
    sigaction(SIGINT, &sa, (struct sigaction *) 0);
    sigprocmask(SIG_BLOCK, &ss, (sigset_t *) 0);
  ], bash_cv_signal_vintage=posix,
  [
    AC_TRY_LINK([#include <signal.h>], [
	int mask = sigmask(SIGINT);
	sigsetmask(mask); sigblock(mask); sigpause(mask);
    ], bash_cv_signal_vintage=4.2bsd,
    [
      AC_TRY_LINK([
	#include <signal.h>
	RETSIGTYPE foo() { }], [
		int mask = sigmask(SIGINT);
		sigset(SIGINT, foo); sigrelse(SIGINT);
		sighold(SIGINT); sigpause(SIGINT);
        ], bash_cv_signal_vintage=svr3, bash_cv_signal_vintage=v7
    )]
  )]
)
])
AC_MSG_RESULT($bash_cv_signal_vintage)
if test "$bash_cv_signal_vintage" = posix; then
AC_DEFINE(HAVE_POSIX_SIGNALS)
elif test "$bash_cv_signal_vintage" = "4.2bsd"; then
AC_DEFINE(HAVE_BSD_SIGNALS)
elif test "$bash_cv_signal_vintage" = svr3; then
AC_DEFINE(HAVE_USG_SIGHOLD)
fi
])

dnl Check if the pgrp of setpgrp() can't be the pid of a zombie process.
AC_DEFUN(BASH_PGRP_SYNC,
[AC_REQUIRE([AC_FUNC_GETPGRP])
AC_MSG_CHECKING(whether pgrps need synchronization)
AC_CACHE_VAL(bash_cv_pgrp_pipe,
[AC_TRY_RUN([
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
main()
{
# ifdef GETPGRP_VOID
#  define getpgID()	getpgrp()
# else
#  define getpgID()	getpgrp(0)
#  define setpgid(x,y)	setpgrp(x,y)
# endif
	int pid1, pid2, fds[2];
	int status;
	char ok;

	switch (pid1 = fork()) {
	  case -1:
	    exit(1);
	  case 0:
	    setpgid(0, getpid());
	    exit(0);
	}
	setpgid(pid1, pid1);

	sleep(2);	/* let first child die */

	if (pipe(fds) < 0)
	  exit(2);

	switch (pid2 = fork()) {
	  case -1:
	    exit(3);
	  case 0:
	    setpgid(0, pid1);
	    ok = getpgID() == pid1;
	    write(fds[1], &ok, 1);
	    exit(0);
	}
	setpgid(pid2, pid1);

	close(fds[1]);
	if (read(fds[0], &ok, 1) != 1)
	  exit(4);
	wait(&status);
	wait(&status);
	exit(ok ? 0 : 5);
}
], bash_cv_pgrp_pipe=no,bash_cv_pgrp_pipe=yes,
   [AC_MSG_WARN(cannot check pgrp synchronization if cross compiling -- defaulting to no)
    bash_cv_pgrp_pipe=no])
])
AC_MSG_RESULT($bash_cv_pgrp_pipe)
if test $bash_cv_pgrp_pipe = yes; then
AC_DEFINE(PGRP_PIPE)
fi
])

dnl
dnl check for typedef'd symbols in header files, but allow the caller to
dnl specify the include files to be checked in addition to the default
dnl 
dnl BASH_CHECK_TYPE(TYPE, HEADERS, DEFAULT[, VALUE-IF-FOUND])
AC_DEFUN(BASH_CHECK_TYPE,
[AC_REQUIRE([AC_HEADER_STDC])dnl
AC_MSG_CHECKING(for $1)
AC_CACHE_VAL(bash_cv_type_$1,
[AC_EGREP_CPP($1, [#include <sys/types.h>
#if STDC_HEADERS
#include <stdlib.h>
#endif
$2
], bash_cv_type_$1=yes, bash_cv_type_$1=no)])
AC_MSG_RESULT($bash_cv_type_$1)
ifelse($#, 4, [if test $bash_cv_type_$1 = yes; then
	AC_DEFINE($4)
	fi])
if test $bash_cv_type_$1 = no; then
  AC_DEFINE($1, $3)
fi
])

dnl
dnl Type of struct rlimit fields: some systems (OSF/1, NetBSD, RISC/os 5.0)
dnl have a rlim_t, others (4.4BSD based systems) use quad_t, others use
dnl long and still others use int (HP-UX 9.01, SunOS 4.1.3).  To simplify
dnl matters, this just checks for rlim_t, quad_t, or long.
dnl
AC_DEFUN(BASH_RLIMIT_TYPE,
[AC_MSG_CHECKING(for size and type of struct rlimit fields)
AC_CACHE_VAL(bash_cv_type_rlimit,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/resource.h>],
[rlim_t xxx;], bash_cv_type_rlimit=rlim_t,[
AC_TRY_RUN([
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
main()
{
#ifdef HAVE_QUAD_T
  struct rlimit rl;
  if (sizeof(rl.rlim_cur) == sizeof(quad_t))
    exit(0);
#endif
  exit(1);
}], bash_cv_type_rlimit=quad_t, bash_cv_type_rlimit=long,
        [AC_MSG_WARN(cannot check quad_t if cross compiling -- defaulting to long)
         bash_cv_type_rlimit=long])])
])
AC_MSG_RESULT($bash_cv_type_rlimit)
if test $bash_cv_type_rlimit = quad_t; then
AC_DEFINE(RLIMTYPE, quad_t)
elif test $bash_cv_type_rlimit = rlim_t; then
AC_DEFINE(RLIMTYPE, rlim_t)
fi
])

dnl
dnl Check for sys_siglist[] or _sys_siglist[]
dnl
AC_DEFUN(BASH_DECL_UNDER_SYS_SIGLIST,
[AC_MSG_CHECKING([for _sys_siglist in signal.h or unistd.h])
AC_CACHE_VAL(bash_cv_decl_under_sys_siglist,
[AC_TRY_COMPILE([
#include <sys/types.h>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif], [ char *msg = _sys_siglist[2]; ],
  bash_cv_decl_under_sys_siglist=yes, bash_cv_decl_under_sys_siglist=no,
  [AC_MSG_WARN(cannot check for _sys_siglist[] if cross compiling -- defaulting to no)])])dnl
AC_MSG_RESULT($bash_cv_decl_under_sys_siglist)
if test $bash_cv_decl_under_sys_siglist = yes; then
AC_DEFINE(UNDER_SYS_SIGLIST_DECLARED)
fi
])

AC_DEFUN(BASH_UNDER_SYS_SIGLIST,
[AC_REQUIRE([BASH_DECL_UNDER_SYS_SIGLIST])
AC_MSG_CHECKING([for _sys_siglist in system C library])
AC_CACHE_VAL(bash_cv_under_sys_siglist,
[AC_TRY_RUN([
#include <sys/types.h>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifndef UNDER_SYS_SIGLIST_DECLARED
extern char *_sys_siglist[];
#endif
main()
{
char *msg = (char *)_sys_siglist[2];
exit(msg == 0);
}],
	bash_cv_under_sys_siglist=yes, bash_cv_under_sys_siglist=no,
	[AC_MSG_WARN(cannot check for _sys_siglist[] if cross compiling -- defaulting to no)
	 bash_cv_under_sys_siglist=no])])
AC_MSG_RESULT($bash_cv_under_sys_siglist)
if test $bash_cv_under_sys_siglist = yes; then
AC_DEFINE(HAVE_UNDER_SYS_SIGLIST)
fi
])

AC_DEFUN(BASH_SYS_SIGLIST,
[AC_REQUIRE([AC_DECL_SYS_SIGLIST])
AC_MSG_CHECKING([for sys_siglist in system C library])
AC_CACHE_VAL(bash_cv_sys_siglist,
[AC_TRY_RUN([
#include <sys/types.h>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifndef SYS_SIGLIST_DECLARED
extern char *sys_siglist[];
#endif
main()
{
char *msg = sys_siglist[2];
exit(msg == 0);
}],
	bash_cv_sys_siglist=yes, bash_cv_sys_siglist=no,
	[AC_MSG_WARN(cannot check for sys_siglist if cross compiling -- defaulting to no)
	 bash_cv_sys_siglist=no])])
AC_MSG_RESULT($bash_cv_sys_siglist)
if test $bash_cv_sys_siglist = yes; then
AC_DEFINE(HAVE_SYS_SIGLIST)
fi
])

dnl Check for sys_errlist[] and sys_nerr, check for declaration
AC_DEFUN(BASH_SYS_ERRLIST,
[AC_MSG_CHECKING([for sys_errlist and sys_nerr])
AC_CACHE_VAL(bash_cv_sys_errlist,
[AC_TRY_LINK([#include <errno.h>],
[extern char *sys_errlist[];
 extern int sys_nerr;
 char *msg = sys_errlist[sys_nerr - 1];],
    bash_cv_sys_errlist=yes, bash_cv_sys_errlist=no)])dnl
AC_MSG_RESULT($bash_cv_sys_errlist)
if test $bash_cv_sys_errlist = yes; then
AC_DEFINE(HAVE_SYS_ERRLIST)
fi
])

dnl Check to see if opendir will open non-directories (not a nice thing)
AC_DEFUN(BASH_FUNC_OPENDIR_CHECK,
[AC_REQUIRE([AC_HEADER_DIRENT])dnl
AC_MSG_CHECKING(if opendir() opens non-directories)
AC_CACHE_VAL(bash_cv_opendir_not_robust,
[AC_TRY_RUN([
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */
#if defined(HAVE_DIRENT_H)
# include <dirent.h>
#else
# define dirent direct
# ifdef HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif /* SYSNDIR */
# ifdef HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif /* SYSDIR */
# ifdef HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif /* HAVE_DIRENT_H */
main()
{
DIR *dir;
int fd, err;
err = mkdir("/tmp/bash-aclocal", 0700);
if (err < 0) {
  perror("mkdir");
  exit(1);
}
unlink("/tmp/bash-aclocal/not_a_directory");
fd = open("/tmp/bash-aclocal/not_a_directory", O_WRONLY|O_CREAT|O_EXCL, 0666);
write(fd, "\n", 1);
close(fd);
dir = opendir("/tmp/bash-aclocal/not_a_directory");
unlink("/tmp/bash-aclocal/not_a_directory");
rmdir("/tmp/bash-aclocal");
exit (dir == 0);
}], bash_cv_opendir_not_robust=yes,bash_cv_opendir_not_robust=no,
    [AC_MSG_WARN(cannot check opendir if cross compiling -- defaulting to no)
     bash_cv_opendir_not_robust=no]
)])
AC_MSG_RESULT($bash_cv_opendir_not_robust)
if test $bash_cv_opendir_not_robust = yes; then
AC_DEFINE(OPENDIR_NOT_ROBUST)
fi
])

dnl
AC_DEFUN(BASH_TYPE_SIGHANDLER,
[AC_MSG_CHECKING([whether signal handlers are of type void])
AC_CACHE_VAL(bash_cv_void_sighandler,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <signal.h>
#ifdef signal
#undef signal
#endif
#ifdef __cplusplus
extern "C"
#endif
void (*signal ()) ();],
[int i;], bash_cv_void_sighandler=yes, bash_cv_void_sighandler=no)])dnl
AC_MSG_RESULT($bash_cv_void_sighandler)
if test $bash_cv_void_sighandler = yes; then
AC_DEFINE(VOID_SIGHANDLER)
fi
])

dnl
dnl A signed 16-bit integer quantity
dnl
AC_DEFUN(BASH_TYPE_BITS16_T,
[
if test "$ac_cv_sizeof_short" = 2; then
  AC_CHECK_TYPE(bits16_t, short)
elif test "$ac_cv_sizeof_char" = 2; then
  AC_CHECK_TYPE(bits16_t, char)
else
  AC_CHECK_TYPE(bits16_t, short)
fi
])

dnl
dnl An unsigned 16-bit integer quantity
dnl
AC_DEFUN(BASH_TYPE_U_BITS16_T,
[
if test "$ac_cv_sizeof_short" = 2; then
  AC_CHECK_TYPE(u_bits16_t, unsigned short)
elif test "$ac_cv_sizeof_char" = 2; then
  AC_CHECK_TYPE(u_bits16_t, unsigned char)
else
  AC_CHECK_TYPE(u_bits16_t, unsigned short)
fi
])

dnl
dnl A signed 32-bit integer quantity
dnl
AC_DEFUN(BASH_TYPE_BITS32_T,
[
if test "$ac_cv_sizeof_int" = 4; then
  AC_CHECK_TYPE(bits32_t, int)
elif test "$ac_cv_sizeof_long" = 4; then
  AC_CHECK_TYPE(bits32_t, long)
else
  AC_CHECK_TYPE(bits32_t, int)
fi
])

dnl
dnl An unsigned 32-bit integer quantity
dnl
AC_DEFUN(BASH_TYPE_U_BITS32_T,
[
if test "$ac_cv_sizeof_int" = 4; then
  AC_CHECK_TYPE(u_bits32_t, unsigned int)
elif test "$ac_cv_sizeof_long" = 4; then
  AC_CHECK_TYPE(u_bits32_t, unsigned long)
else
  AC_CHECK_TYPE(u_bits32_t, unsigned int)
fi
])

AC_DEFUN(BASH_TYPE_PTRDIFF_T,
[
if test "$ac_cv_sizeof_int" = "$ac_cv_sizeof_char_p"; then
  AC_CHECK_TYPE(ptrdiff_t, int)
elif test "$ac_cv_sizeof_long" = "$ac_cv_sizeof_char_p"; then
  AC_CHECK_TYPE(ptrdiff_t, long)
else
  AC_CHECK_TYPE(ptrdiff_t, int)
fi
])

dnl
dnl A signed 64-bit quantity
dnl
AC_DEFUN(BASH_TYPE_BITS64_T,
[
if test "$ac_sv_sizeof_char_p" = 8; then
  AC_CHECK_TYPE(bits64_t, char *)
elif test "$ac_cv_sizeof_double" = 8; then
  AC_CHECK_TYPE(bits64_t, double)
elif test "$ac_cv_sizeof_long" = 8; then
  AC_CHECK_TYPE(bits64_t, long)
else
  AC_CHECK_TYPE(bits64_t, double)
fi
])

AC_DEFUN(BASH_FUNC_STRSIGNAL,
[AC_MSG_CHECKING([for the existence of strsignal])
AC_CACHE_VAL(bash_cv_have_strsignal,
[AC_TRY_LINK([#include <sys/types.h>
#include <signal.h>],
[char *s = (char *)strsignal(2);],
 bash_cv_have_strsignal=yes, bash_cv_have_strsignal=no)])
AC_MSG_RESULT($bash_cv_have_strsignal)
if test $bash_cv_have_strsignal = yes; then
AC_DEFINE(HAVE_STRSIGNAL)
fi
])

AC_DEFUN(BASH_FUNC_LSTAT,
[dnl Cannot use AC_CHECK_FUNCS(lstat) because Linux defines lstat() as an
dnl inline function in <sys/stat.h>.
AC_CACHE_CHECK([for lstat], bash_cv_func_lstat,
[AC_TRY_LINK([
#include <sys/types.h>
#include <sys/stat.h>
],[ lstat(".",(struct stat *)0); ],
bash_cv_func_lstat=yes, bash_cv_func_lstat=no)])
if test $bash_cv_func_lstat = yes; then
  AC_DEFINE(HAVE_LSTAT)
fi
])

AC_DEFUN(BASH_FUNC_INET_ATON,
[
AC_CACHE_CHECK([for inet_aton], bash_cv_func_inet_aton,
[AC_TRY_LINK([
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
struct in_addr ap;], [ inet_aton("127.0.0.1", &ap); ],
bash_cv_func_inet_aton=yes, bash_cv_func_inet_aton=no)])
if test $bash_cv_func_inet_aton = yes; then
  AC_DEFINE(HAVE_INET_ATON)
fi
])

AC_DEFUN(BASH_STRUCT_TERMIOS_LDISC,
[AC_MSG_CHECKING([for a c_line member of struct termios])
AC_CACHE_VAL(bash_cv_termios_ldisc,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <termios.h>],[struct termios t; int i; i = t.c_line;],
  bash_cv_termios_ldisc=yes, bash_cv_termios_ldisc=no)])dnl
AC_MSG_RESULT($bash_cv_termios_ldisc)
if test $bash_cv_termios_ldisc = yes; then
AC_DEFINE(TERMIOS_LDISC)
fi
])

AC_DEFUN(BASH_STRUCT_TERMIO_LDISC,
[AC_MSG_CHECKING([for a c_line member of struct termio])
AC_CACHE_VAL(bash_cv_termio_ldisc,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <termio.h>],[struct termio t; int i; i = t.c_line;],
  bash_cv_termio_ldisc=yes, bash_cv_termio_ldisc=no)])dnl
AC_MSG_RESULT($bash_cv_termio_ldisc)
if test $bash_cv_termio_ldisc = yes; then
AC_DEFINE(TERMIO_LDISC)
fi
])

AC_DEFUN(BASH_FUNC_GETENV,
[AC_MSG_CHECKING(to see if getenv can be redefined)
AC_CACHE_VAL(bash_cv_getenv_redef,
[AC_TRY_RUN([
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#ifndef __STDC__
#  ifndef const
#    define const
#  endif
#endif
char *
getenv (name)
#if defined (__linux__) || defined (__bsdi__) || defined (convex)
     const char *name;
#else
     char const *name;
#endif /* !__linux__ && !__bsdi__ && !convex */
{
return "42";
}
main()
{
char *s;
/* The next allows this program to run, but does not allow bash to link
   when it redefines getenv.  I'm not really interested in figuring out
   why not. */
#if defined (NeXT)
exit(1);
#endif
s = getenv("ABCDE");
exit(s == 0);	/* force optimizer to leave getenv in */
}
], bash_cv_getenv_redef=yes, bash_cv_getenv_redef=no,
   [AC_MSG_WARN(cannot check getenv redefinition if cross compiling -- defaulting to yes)
    bash_cv_getenv_redef=yes]
)])
AC_MSG_RESULT($bash_cv_getenv_redef)
if test $bash_cv_getenv_redef = yes; then
AC_DEFINE(CAN_REDEFINE_GETENV)
fi
])

AC_DEFUN(BASH_FUNC_PRINTF,
[AC_MSG_CHECKING(for declaration of printf in <stdio.h>)
AC_CACHE_VAL(bash_cv_printf_declared,
[AC_TRY_RUN([
#include <stdio.h>
#ifdef __STDC__
typedef int (*_bashfunc)(const char *, ...);
#else
typedef int (*_bashfunc)();
#endif
main()
{
_bashfunc pf;
pf = (_bashfunc) printf;
exit(pf == 0);
}
], bash_cv_printf_declared=yes, bash_cv_printf_declared=no,
   [AC_MSG_WARN(cannot check printf declaration if cross compiling -- defaulting to yes)
    bash_cv_printf_declared=yes]
)])
AC_MSG_RESULT($bash_cv_printf_declared)
if test $bash_cv_printf_declared = yes; then
AC_DEFINE(PRINTF_DECLARED)
fi
])

AC_DEFUN(BASH_FUNC_ULIMIT_MAXFDS,
[AC_MSG_CHECKING(whether ulimit can substitute for getdtablesize)
AC_CACHE_VAL(bash_cv_ulimit_maxfds,
[AC_TRY_RUN([
main()
{
long maxfds = ulimit(4, 0L);
exit (maxfds == -1L);
}
], bash_cv_ulimit_maxfds=yes, bash_cv_ulimit_maxfds=no,
   [AC_MSG_WARN(cannot check ulimit if cross compiling -- defaulting to no)
    bash_cv_ulimit_maxfds=no]
)])
AC_MSG_RESULT($bash_cv_ulimit_maxfds)
if test $bash_cv_ulimit_maxfds = yes; then
AC_DEFINE(ULIMIT_MAXFDS)
fi
])

AC_DEFUN(BASH_CHECK_LIB_TERMCAP,
[
if test "X$bash_cv_termcap_lib" = "X"; then
_bash_needmsg=yes
else
AC_MSG_CHECKING(which library has the termcap functions)
_bash_needmsg=
fi
AC_CACHE_VAL(bash_cv_termcap_lib,
[AC_CHECK_LIB(termcap, tgetent, bash_cv_termcap_lib=libtermcap,
    [AC_CHECK_LIB(curses, tgetent, bash_cv_termcap_lib=libcurses,
	[AC_CHECK_LIB(ncurses, tgetent, bash_cv_termcap_lib=libncurses,
	    bash_cv_termcap_lib=gnutermcap)])])])
if test "X$_bash_needmsg" = "Xyes"; then
AC_MSG_CHECKING(which library has the termcap functions)
fi
AC_MSG_RESULT(using $bash_cv_termcap_lib)
if test $bash_cv_termcap_lib = gnutermcap && test -z "$prefer_curses"; then
LDFLAGS="$LDFLAGS -L./lib/termcap"
TERMCAP_LIB="./lib/termcap/libtermcap.a"
TERMCAP_DEP="./lib/termcap/libtermcap.a"
elif test $bash_cv_termcap_lib = libtermcap && test -z "$prefer_curses"; then
TERMCAP_LIB=-ltermcap
TERMCAP_DEP=
elif test $bash_cv_termcap_lib = libncurses; then
TERMCAP_LIB=-lncurses
TERMCAP_DEP=
else
TERMCAP_LIB=-lcurses
TERMCAP_DEP=
fi
])

AC_DEFUN(BASH_FUNC_GETCWD,
[AC_MSG_CHECKING([if getcwd() calls popen()])
AC_CACHE_VAL(bash_cv_getcwd_calls_popen,
[AC_TRY_RUN([
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef __STDC__
#ifndef const
#define const
#endif
#endif

int popen_called;

FILE *
popen(command, type)
     const char *command;
     const char *type;
{
	popen_called = 1;
	return (FILE *)NULL;
}

FILE *_popen(command, type)
     const char *command;
     const char *type;
{
  return (popen (command, type));
}

int
pclose(stream)
FILE *stream;
{
	return 0;
}

int
_pclose(stream)
FILE *stream;
{
	return 0;
}

main()
{
	char	lbuf[32];
	popen_called = 0;
	getcwd(lbuf, 32);
	exit (popen_called);
}
], bash_cv_getcwd_calls_popen=no, bash_cv_getcwd_calls_popen=yes,
   [AC_MSG_WARN(cannot check whether getcwd calls popen if cross compiling -- defaulting to no)
    bash_cv_getcwd_calls_popen=no]
)])
AC_MSG_RESULT($bash_cv_getcwd_calls_popen)
if test $bash_cv_getcwd_calls_popen = yes; then
AC_DEFINE(GETCWD_BROKEN)
fi
])

AC_DEFUN(BASH_STRUCT_DIRENT_D_INO,
[AC_REQUIRE([AC_HEADER_DIRENT])
AC_MSG_CHECKING(if struct dirent has a d_ino member)
AC_CACHE_VAL(bash_cv_dirent_has_dino,
[AC_TRY_COMPILE([
#include <stdio.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */
#if defined(HAVE_DIRENT_H)
# include <dirent.h>
#else
# define dirent direct
# ifdef HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif /* SYSNDIR */
# ifdef HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif /* SYSDIR */
# ifdef HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif /* HAVE_DIRENT_H */
],[
struct dirent d; int z; z = d.d_ino;
], bash_cv_dirent_has_dino=yes, bash_cv_dirent_has_dino=no)])
AC_MSG_RESULT($bash_cv_dirent_has_dino)
if test $bash_cv_dirent_has_dino = yes; then
AC_DEFINE(STRUCT_DIRENT_HAS_D_INO)
fi
])

AC_DEFUN(BASH_STRUCT_DIRENT_D_FILENO,
[AC_REQUIRE([AC_HEADER_DIRENT])
AC_MSG_CHECKING(if struct dirent has a d_fileno member)
AC_CACHE_VAL(bash_cv_dirent_has_d_fileno,
[AC_TRY_COMPILE([
#include <stdio.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */
#if defined(HAVE_DIRENT_H)
# include <dirent.h>
#else
# define dirent direct
# ifdef HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif /* SYSNDIR */
# ifdef HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif /* SYSDIR */
# ifdef HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif /* HAVE_DIRENT_H */
],[
struct dirent d; int z; z = d.d_fileno;
], bash_cv_dirent_has_d_fileno=yes, bash_cv_dirent_has_d_fileno=no)])
AC_MSG_RESULT($bash_cv_dirent_has_d_fileno)
if test $bash_cv_dirent_has_d_fileno = yes; then
AC_DEFINE(STRUCT_DIRENT_HAS_D_FILENO)
fi
])

AC_DEFUN(BASH_REINSTALL_SIGHANDLERS,
[AC_REQUIRE([AC_TYPE_SIGNAL])
AC_REQUIRE([BASH_SIGNAL_CHECK])
AC_MSG_CHECKING([if signal handlers must be reinstalled when invoked])
AC_CACHE_VAL(bash_cv_must_reinstall_sighandlers,
[AC_TRY_RUN([
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

typedef RETSIGTYPE sigfunc();

int nsigint;

#ifdef HAVE_POSIX_SIGNALS
sigfunc *
set_signal_handler(sig, handler)
     int sig;
     sigfunc *handler;
{
  struct sigaction act, oact;
  act.sa_handler = handler;
  act.sa_flags = 0;
  sigemptyset (&act.sa_mask);
  sigemptyset (&oact.sa_mask);
  sigaction (sig, &act, &oact);
  return (oact.sa_handler);
}
#else
#define set_signal_handler(s, h) signal(s, h)
#endif

RETSIGTYPE
sigint(s)
int s;
{
  nsigint++;
}

main()
{
	nsigint = 0;
	set_signal_handler(SIGINT, sigint);
	kill((int)getpid(), SIGINT);
	kill((int)getpid(), SIGINT);
	exit(nsigint != 2);
}
], bash_cv_must_reinstall_sighandlers=no, bash_cv_must_reinstall_sighandlers=yes,
   [AC_MSG_WARN(cannot check signal handling if cross compiling -- defaulting to no)
    bash_cv_must_reinstall_sighandlers=no]
)])
AC_MSG_RESULT($bash_cv_must_reinstall_sighandlers)
if test $bash_cv_must_reinstall_sighandlers = yes; then
AC_DEFINE(MUST_REINSTALL_SIGHANDLERS)
fi
])

AC_DEFUN(BASH_FUNC_SBRK_DECLARED,
[AC_MSG_CHECKING(for declaration of sbrk in <unistd.h>)
AC_CACHE_VAL(bash_cv_sbrk_declared,
[AC_EGREP_HEADER(sbrk, unistd.h,
 bash_cv_sbrk_declared=yes, bash_cv_sbrk_declared=no)])
AC_MSG_RESULT($bash_cv_sbrk_declared)
if test $bash_cv_sbrk_declared = yes; then
AC_DEFINE(SBRK_DECLARED)
fi
])

dnl check that some necessary job control definitions are present
AC_DEFUN(BASH_JOB_CONTROL_MISSING,
[AC_REQUIRE([BASH_SIGNAL_CHECK])
AC_MSG_CHECKING(for presence of necessary job control definitions)
AC_CACHE_VAL(bash_cv_job_control_missing,
[AC_TRY_RUN([
#include <sys/types.h>
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <signal.h>

/* Add more tests in here as appropriate. */
main()
{
/* signal type */
#if !defined (HAVE_POSIX_SIGNALS) && !defined (HAVE_BSD_SIGNALS)
exit(1);
#endif

/* signals and tty control. */
#if !defined (SIGTSTP) || !defined (SIGSTOP) || !defined (SIGCONT)
exit (1);
#endif

/* process control */
#if !defined (WNOHANG) || !defined (WUNTRACED) 
exit(1);
#endif

/* Posix systems have tcgetpgrp and waitpid. */
#if defined (_POSIX_VERSION) && !defined (HAVE_TCGETPGRP)
exit(1);
#endif

#if defined (_POSIX_VERSION) && !defined (HAVE_WAITPID)
exit(1);
#endif

/* Other systems have TIOCSPGRP/TIOCGPRGP and wait3. */
#if !defined (_POSIX_VERSION) && !defined (HAVE_WAIT3)
exit(1);
#endif

exit(0);
}], bash_cv_job_control_missing=present, bash_cv_job_control_missing=missing,
    [AC_MSG_WARN(cannot check job control if cross-compiling -- defaulting to missing)
     bash_cv_job_control_missing=missing]
)])
AC_MSG_RESULT($bash_cv_job_control_missing)
if test $bash_cv_job_control_missing = missing; then
AC_DEFINE(JOB_CONTROL_MISSING)
fi
])

dnl check whether named pipes are present
dnl this requires a previous check for mkfifo, but that is awkward to specify
AC_DEFUN(BASH_SYS_NAMED_PIPES,
[AC_MSG_CHECKING(for presence of named pipes)
AC_CACHE_VAL(bash_cv_sys_named_pipes,
[AC_TRY_RUN([
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* Add more tests in here as appropriate. */
main()
{
int fd, err;

#if defined (HAVE_MKFIFO)
exit (0);
#endif

#if !defined (S_IFIFO) && (defined (_POSIX_VERSION) && !defined (S_ISFIFO))
exit (1);
#endif

#if defined (NeXT)
exit (1);
#endif
err = mkdir("/tmp/bash-aclocal", 0700);
if (err < 0) {
  perror ("mkdir");
  exit(1);
}
fd = mknod ("/tmp/bash-aclocal/sh-np-autoconf", 0666 | S_IFIFO, 0);
if (fd == -1) {
  rmdir ("/tmp/bash-aclocal");
  exit (1);
}
close(fd);
unlink ("/tmp/bash-aclocal/sh-np-autoconf");
rmdir ("/tmp/bash-aclocal");
exit(0);
}], bash_cv_sys_named_pipes=present, bash_cv_sys_named_pipes=missing,
    [AC_MSG_WARN(cannot check for named pipes if cross-compiling -- defaulting to missing)
     bash_cv_sys_named_pipes=missing]
)])
AC_MSG_RESULT($bash_cv_sys_named_pipes)
if test $bash_cv_sys_named_pipes = missing; then
AC_DEFINE(NAMED_PIPES_MISSING)
fi
])

AC_DEFUN(BASH_FUNC_POSIX_SETJMP,
[AC_REQUIRE([BASH_SIGNAL_CHECK])
AC_MSG_CHECKING(for presence of POSIX-style sigsetjmp/siglongjmp)
AC_CACHE_VAL(bash_cv_func_sigsetjmp,
[AC_TRY_RUN([
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <signal.h>
#include <setjmp.h>

main()
{
#if !defined (_POSIX_VERSION) || !defined (HAVE_POSIX_SIGNALS)
exit (1);
#else

int code;
sigset_t set, oset;
sigjmp_buf xx;

/* get the mask */
sigemptyset(&set);
sigemptyset(&oset);
sigprocmask(SIG_BLOCK, (sigset_t *)NULL, &set);
sigprocmask(SIG_BLOCK, (sigset_t *)NULL, &oset);

/* save it */
code = sigsetjmp(xx, 1);
if (code)
  exit(0);	/* could get sigmask and compare to oset here. */

/* change it */
sigaddset(&set, SIGINT);
sigprocmask(SIG_BLOCK, &set, (sigset_t *)NULL);

/* and siglongjmp */
siglongjmp(xx, 10);
exit(1);
#endif
}], bash_cv_func_sigsetjmp=present, bash_cv_func_sigsetjmp=missing,
    [AC_MSG_WARN(cannot check for sigsetjmp/siglongjmp if cross-compiling -- defaulting to missing)
     bash_cv_func_sigsetjmp=missing]
)])
AC_MSG_RESULT($bash_cv_func_sigsetjmp)
if test $bash_cv_func_sigsetjmp = present; then
AC_DEFINE(HAVE_POSIX_SIGSETJMP)
fi
])

AC_DEFUN(BASH_HAVE_TIOCGWINSZ,
[AC_MSG_CHECKING(for TIOCGWINSZ in sys/ioctl.h)
AC_CACHE_VAL(bash_cv_tiocgwinsz_in_ioctl,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/ioctl.h>], [int x = TIOCGWINSZ;],
  bash_cv_tiocgwinsz_in_ioctl=yes,bash_cv_tiocgwinsz_in_ioctl=no)])
AC_MSG_RESULT($bash_cv_tiocgwinsz_in_ioctl)
if test $bash_cv_tiocgwinsz_in_ioctl = yes; then   
AC_DEFINE(GWINSZ_IN_SYS_IOCTL)
fi
])

AC_DEFUN(BASH_STRUCT_WINSIZE,
[AC_MSG_CHECKING(for struct winsize in sys/ioctl.h and termios.h)
AC_CACHE_VAL(bash_cv_struct_winsize_header,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/ioctl.h>], [struct winsize x;],
  bash_cv_struct_winsize_header=ioctl_h,
  [AC_TRY_COMPILE([#include <sys/types.h>
#include <termios.h>], [struct winsize x;],
  bash_cv_struct_winsize_header=termios_h, bash_cv_struct_winsize_header=other)
])])
if test $bash_cv_struct_winsize_header = ioctl_h; then
  AC_MSG_RESULT(sys/ioctl.h)
  AC_DEFINE(STRUCT_WINSIZE_IN_SYS_IOCTL)
elif test $bash_cv_struct_winsize_header = termios_h; then
  AC_MSG_RESULT(termios.h)
  AC_DEFINE(STRUCT_WINSIZE_IN_TERMIOS)
else
  AC_MSG_RESULT(not found)
fi
])

AC_DEFUN(BASH_HAVE_TIOCSTAT,
[AC_MSG_CHECKING(for TIOCSTAT in sys/ioctl.h)
AC_CACHE_VAL(bash_cv_tiocstat_in_ioctl,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/ioctl.h>], [int x = TIOCSTAT;],
  bash_cv_tiocstat_in_ioctl=yes,bash_cv_tiocstat_in_ioctl=no)])
AC_MSG_RESULT($bash_cv_tiocstat_in_ioctl)
if test $bash_cv_tiocstat_in_ioctl = yes; then   
AC_DEFINE(TIOCSTAT_IN_SYS_IOCTL)
fi
])

AC_DEFUN(BASH_HAVE_FIONREAD,
[AC_MSG_CHECKING(for FIONREAD in sys/ioctl.h)
AC_CACHE_VAL(bash_cv_fionread_in_ioctl,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/ioctl.h>], [int x = FIONREAD;],
  bash_cv_fionread_in_ioctl=yes,bash_cv_fionread_in_ioctl=no)])
AC_MSG_RESULT($bash_cv_fionread_in_ioctl)
if test $bash_cv_fionread_in_ioctl = yes; then   
AC_DEFINE(FIONREAD_IN_SYS_IOCTL)
fi
])

dnl
dnl See if speed_t is declared in <sys/types.h>.  Some versions of linux
dnl require a definition of speed_t each time <termcap.h> is included,
dnl but you can only get speed_t if you include <termios.h> (on some
dnl versions) or <sys/types.h> (on others).
dnl
AC_DEFUN(BASH_MISC_SPEED_T,
[AC_MSG_CHECKING(for speed_t in sys/types.h)
AC_CACHE_VAL(bash_cv_speed_t_in_sys_types,
[AC_TRY_COMPILE([#include <sys/types.h>], [speed_t x;],
  bash_cv_speed_t_in_sys_types=yes,bash_cv_speed_t_in_sys_types=no)])
AC_MSG_RESULT($bash_cv_speed_t_in_sys_types)
if test $bash_cv_speed_t_in_sys_types = yes; then   
AC_DEFINE(SPEED_T_IN_SYS_TYPES)
fi
])

AC_DEFUN(BASH_CHECK_GETPW_FUNCS,
[AC_MSG_CHECKING(whether programs are able to redeclare getpw functions)
AC_CACHE_VAL(bash_cv_can_redecl_getpw,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <pwd.h>
extern struct passwd *getpwent();
extern struct passwd *getpwuid();
extern struct passwd *getpwnam();],
[struct passwd *z; z = getpwent(); z = getpwuid(0); z = getpwnam("root");],
  bash_cv_can_redecl_getpw=yes,bash_cv_can_redecl_getpw=no)])
AC_MSG_RESULT($bash_cv_can_redecl_getpw)
if test $bash_cv_can_redecl_getpw = no; then
AC_DEFINE(HAVE_GETPW_DECLS)
fi
])

AC_DEFUN(BASH_CHECK_DEV_FD,
[AC_MSG_CHECKING(whether /dev/fd is available)
AC_CACHE_VAL(bash_cv_dev_fd,
[if test -d /dev/fd  && test -r /dev/fd/0; then
   bash_cv_dev_fd=standard
 elif test -d /proc/self/fd && test -r /proc/self/fd/0; then
   bash_cv_dev_fd=whacky
 else
   bash_cv_dev_fd=absent
 fi
])
AC_MSG_RESULT($bash_cv_dev_fd)
if test $bash_cv_dev_fd = "standard"; then
  AC_DEFINE(HAVE_DEV_FD)
  AC_DEFINE(DEV_FD_PREFIX, "/dev/fd/")
elif test $bash_cv_dev_fd = "whacky"; then
  AC_DEFINE(HAVE_DEV_FD)
  AC_DEFINE(DEV_FD_PREFIX, "/proc/self/fd/")
fi
])

AC_DEFUN(BASH_CHECK_DEV_STDIN,
[AC_MSG_CHECKING(whether /dev/stdin stdout stderr are available)
AC_CACHE_VAL(bash_cv_dev_stdin,
[if test -d /dev/fd && test -r /dev/stdin; then
   bash_cv_dev_stdin=present
 elif test -d /proc/self/fd && test -r /dev/stdin; then
   bash_cv_dev_stdin=present
 else
   bash_cv_dev_stdin=absent
 fi
])
AC_MSG_RESULT($bash_cv_dev_stdin)
if test $bash_cv_dev_stdin = "present"; then
  AC_DEFINE(HAVE_DEV_STDIN)
fi
])

dnl
dnl Check for the presence of getpeername in libsocket.
dnl If libsocket is present, check for libnsl and add it to LIBS if
dnl it's there, since most systems with libsocket require linking
dnl with libnsl as well.  This should only be called if getpeername
dnl was not found in libc.
dnl
dnl NOTE: IF WE FIND GETPEERNAME, WE ASSUME THAT WE HAVE BIND/CONNECT
dnl	  AS WELL
dnl
AC_DEFUN(BASH_CHECK_SOCKLIB,
[
if test "X$bash_cv_have_socklib" = "X"; then
_bash_needmsg=
else
AC_MSG_CHECKING(for socket library)
_bash_needmsg=yes
fi
AC_CACHE_VAL(bash_cv_have_socklib,
[AC_CHECK_LIB(socket, getpeername,
        bash_cv_have_socklib=yes, bash_cv_have_socklib=no, -lnsl)])
if test "X$_bash_needmsg" = Xyes; then
  AC_MSG_RESULT($bash_cv_have_socklib)
  _bash_needmsg=
fi
if test $bash_cv_have_socklib = yes; then
  # check for libnsl, add it to LIBS if present
  if test "X$bash_cv_have_libnsl" = "X"; then
    _bash_needmsg=
  else
    AC_MSG_CHECKING(for libnsl)
    _bash_needmsg=yes
  fi
  AC_CACHE_VAL(bash_cv_have_libnsl,
	   [AC_CHECK_LIB(nsl, t_open,
		 bash_cv_have_libnsl=yes, bash_cv_have_libnsl=no)])
  if test "X$_bash_needmsg" = Xyes; then
    AC_MSG_RESULT($bash_cv_have_libnsl)
    _bash_needmsg=
  fi
  if test $bash_cv_have_libnsl = yes; then
    LIBS="-lsocket -lnsl $LIBS"
  else
    LIBS="-lsocket $LIBS"
  fi
  AC_DEFINE(HAVE_LIBSOCKET)
  AC_DEFINE(HAVE_GETPEERNAME)
fi
])

dnl
dnl This needs BASH_CHECK_SOCKLIB, but since that's not called on every
dnl system, we can't use AC_PREREQ
dnl
AC_DEFUN(BASH_FUNC_GETHOSTBYNAME,
[if test "X$bash_cv_have_gethostbyname" = "X"; then
_bash_needmsg=yes
else
AC_MSG_CHECKING(for gethostbyname in socket library)
_bash_needmsg=
fi
AC_CACHE_VAL(bash_cv_have_gethostbyname,
[AC_TRY_LINK([#include <netdb.h>],
[ struct hostent *hp;
  hp = gethostbyname("localhost");
], bash_cv_have_gethostbyname=yes, bash_cv_have_gethostbyname=no)]
)
if test "X$_bash_needmsg" = Xyes; then
    AC_MSG_CHECKING(for gethostbyname in socket library)
fi
AC_MSG_RESULT($bash_cv_have_gethostbyname)
if test "$bash_cv_have_gethostbyname" = yes; then
AC_DEFINE(HAVE_GETHOSTBYNAME)
fi
])

AC_DEFUN(BASH_DEFAULT_MAIL_DIR,
[AC_MSG_CHECKING(for default mail directory)
AC_CACHE_VAL(bash_cv_mail_dir,
[if test -d /var/mail; then
   bash_cv_mail_dir=/var/mail
 elif test -d /usr/mail; then
   bash_cv_mail_dir=/usr/mail
 elif test -d /var/spool/mail; then
   bash_cv_mail_dir=/var/spool/mail
 elif test -d /usr/spool/mail; then
   bash_cv_mail_dir=/usr/spool/mail
 else
   bash_cv_mail_dir=unknown
 fi
])
AC_MSG_RESULT($bash_cv_mail_dir)
if test $bash_cv_mail_dir = "/var/mail"; then
   AC_DEFINE(DEFAULT_MAIL_DIRECTORY, "/var/mail")
elif test $bash_cv_mail_dir = "/usr/mail"; then
   AC_DEFINE(DEFAULT_MAIL_DIRECTORY, "/usr/mail")
elif test $bash_cv_mail_dir = "/var/spool/mail"; then
   AC_DEFINE(DEFAULT_MAIL_DIRECTORY, "/var/spool/mail")
elif test $bash_cv_mail_dir = "/usr/spool/mail"; then
   AC_DEFINE(DEFAULT_MAIL_DIRECTORY, "/usr/spool/mail")
else
   AC_DEFINE(DEFAULT_MAIL_DIRECTORY, "unknown")
fi
])

dnl
dnl Check if HPUX needs _KERNEL defined for RLIMIT_* definitions
dnl
AC_DEFUN(BASH_KERNEL_RLIMIT_CHECK,
[AC_MSG_CHECKING([whether $host_os needs _KERNEL for RLIMIT defines])
AC_CACHE_VAL(bash_cv_kernel_rlimit,
[AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/resource.h>
],
[
  int f;
  f = RLIMIT_DATA;
], bash_cv_kernel_rlimit=no,
[AC_TRY_COMPILE([
#include <sys/types.h>
#define _KERNEL
#include <sys/resource.h>
#undef _KERNEL
],
[
	int f;
        f = RLIMIT_DATA;
], bash_cv_kernel_rlimit=yes, bash_cv_kernel_rlimit=no)]
)])
AC_MSG_RESULT($bash_cv_kernel_rlimit)
if test $bash_cv_kernel_rlimit = yes; then
AC_DEFINE(RLIMIT_NEEDS_KERNEL)
fi
])

AC_DEFUN(BASH_FUNC_STRCOLL,
[
AC_MSG_CHECKING(whether or not strcoll and strcmp differ)
AC_CACHE_VAL(bash_cv_func_strcoll_broken,
[AC_TRY_RUN([
#include <stdio.h>
#if defined (HAVE_LOCALE_H)
#include <locale.h>
#endif

main(c, v)
int     c;
char    *v[];
{
        int     r1, r2;
        char    *deflocale, *defcoll;

#ifdef HAVE_SETLOCALE
        deflocale = setlocale(LC_ALL, "");
	defcoll = setlocale(LC_COLLATE, "");
#endif

#ifdef HAVE_STRCOLL
	/* These two values are taken from tests/glob-test. */
        r1 = strcoll("abd", "aXd");
#else
	r1 = 0;
#endif
        r2 = strcmp("abd", "aXd");

	/* These two should both be greater than 0.  It is permissible for
	   a system to return different values, as long as the sign is the
	   same. */

        /* Exit with 1 (failure) if these two values are both > 0, since
	   this tests whether strcoll(3) is broken with respect to strcmp(3)
	   in the default locale. */
	exit (r1 > 0 && r2 > 0);
}
], bash_cv_func_strcoll_broken=yes, bash_cv_func_strcoll_broken=no,
   [AC_MSG_WARN(cannot check strcoll if cross compiling -- defaulting to no)
    bash_cv_func_strcoll_broken=no]
)])
AC_MSG_RESULT($bash_cv_func_strcoll_broken)
if test $bash_cv_func_strcoll_broken = yes; then
AC_DEFINE(STRCOLL_BROKEN)
fi
])

dnl
dnl If available, use support for large files unless the user specified
dnl one of the CPPFLAGS, LDFLAGS, or LIBS variables (<eggert@twinsun.com>
dnl via GNU patch 2.5)
dnl
AC_DEFUN(BASH_LARGE_FILE_SUPPORT,
[AC_MSG_CHECKING(whether large file support needs explicit enabling)
ac_getconfs=''
ac_result=yes
ac_set=''
ac_shellvars='CPPFLAGS LDFLAGS LIBS'
for ac_shellvar in $ac_shellvars; do
  case $ac_shellvar in
  CPPFLAGS) ac_lfsvar=LFS_CFLAGS ac_lfs64var=LFS64_CFLAGS ;;
  *) ac_lfsvar=LFS_$ac_shellvar ac_lfs64var=LFS64_$ac_shellvar ;;
  esac
  eval test '"${'$ac_shellvar'+set}"' = set && ac_set=$ac_shellvar
  (getconf $ac_lfsvar) >/dev/null 2>&1 || { ac_result=no; break; }
  ac_getconf=`getconf $ac_lfsvar`
  ac_getconf64=`getconf $ac_lfs64var`
  ac_getconfs=$ac_getconfs$ac_getconf\ $ac_getconf64
  eval ac_test_$ac_shellvar="\$ac_getconf\ \$ac_getconf64"
done
case "$ac_result$ac_getconfs" in
yes) ac_result=no ;;
esac
case "$ac_result$ac_set" in
yes?*) ac_result="yes, but $ac_set is already set, so use its settings"
esac
AC_MSG_RESULT($ac_result)
case $ac_result in
yes)
  for ac_shellvar in $ac_shellvars; do
    eval $ac_shellvar=\$ac_test_$ac_shellvar
  done ;;
esac
])

dnl
dnl AC_SYS_RESTARTABLE_SYSCALLS tests only for restarted system calls
dnl after a signal handler has been installed with signal().  Since
dnl Bash uses sigaction() if it is available, we need to check whether
dnl or not a signal handler installed with sigaction and SA_RESTART
dnl causes system calls to be restarted after the signal is caught
dnl
AC_DEFUN(BASH_SYS_RESTARTABLE_SYSCALLS,
[AC_REQUIRE([BASH_SIGNAL_CHECK])
AC_CACHE_CHECK(for restartable system calls with posix sigaction,
bash_cv_sys_restartable_syscalls,
[AC_TRY_RUN(
[/* Exit 0 (true) if wait returns something other than -1,
   i.e. the pid of the child, which means that wait was restarted
   after getting the signal.  */
#include <sys/types.h>
#include <signal.h>
static int caught = 0;
void ucatch (isig) int isig; { caught = 1; }
main ()
{
#if !defined (_POSIX_VERSION) || !defined (HAVE_POSIX_SIGNALS)
  exit (1);
#else
  struct sigaction act, oact;
  int i, status;

  act.sa_handler = ucatch;
  /* Might want to add SA_RESTART here, but bash's set_signal_handler
     does not. */
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);
  sigemptyset(&oact.sa_mask);
  i = fork ();
  /* A possible race condition here, but in practice it never happens. */
  if (i == 0) { sleep (3); kill (getppid (), SIGINT); sleep (3); exit (0); }
  sigaction(SIGINT, &act, &oact);
  status = wait(&i);
  if (status == -1) wait(&i);
  exit (status == -1);
#endif
}
], bash_cv_sys_restartable_syscalls=yes, bash_cv_sys_restartable_syscalls=no,
   AC_MSG_WARN(cannot check restartable syscalls if cross compiling))
])
if test $bash_cv_sys_restartable_syscalls = yes; then
  AC_DEFINE(HAVE_RESTARTABLE_SYSCALLS)
fi
])
dnl
dnl Check for 64-bit off_t -- used for malloc alignment
dnl
dnl C does not allow duplicate case labels, so the compile will fail if
dnl sizeof(off_t) is > 4.
dnl
AC_DEFUN(BASH_CHECK_OFF_T_64,
[AC_CACHE_CHECK(for 64-bit off_t, bash_cv_off_t_64,
AC_TRY_COMPILE([
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
],[
switch (0) case 0: case (sizeof (off_t) <= 4):;
], bash_cv_off_t_64=no, bash_cv_off_t_64=yes))
if test $bash_cv_off_t_64 = yes; then
        AC_DEFINE(HAVE_OFF_T_64)
fi])

AC_DEFUN(BASH_STRUCT_TIMEVAL,
[AC_MSG_CHECKING(for struct timeval in sys/time.h and time.h)
AC_CACHE_VAL(bash_cv_struct_timeval,
[
AC_EGREP_HEADER(struct timeval, sys/time.h,
		bash_cv_struct_timeval=yes,
		AC_EGREP_HEADER(struct timeval, time.h,
			bash_cv_struct_timeval=yes,
			bash_cv_struct_timeval=no))
])
AC_MSG_RESULT($bash_cv_struct_timeval)
if test $bash_cv_struct_timeval = yes; then
  AC_DEFINE(HAVE_TIMEVAL)
fi
])
