/* pcresearch.c - searching subroutines using PCRE for grep.
   Copyright 2000, 2007, 2009-2016 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

/* Written August 1992 by Mike Haertel. */

#include <config.h>
#include "search.h"

#if HAVE_LIBPCRE
# include <pcre.h>

/* This must be at least 2; everything after that is for performance
   in pcre_exec.  */
enum { NSUB = 300 };

/* Compiled internal form of a Perl regular expression.  */
static pcre *cre;

/* Additional information about the pattern.  */
static pcre_extra *extra;

# ifndef PCRE_STUDY_JIT_COMPILE
#  define PCRE_STUDY_JIT_COMPILE 0
# endif

# if PCRE_STUDY_JIT_COMPILE
/* Maximum size of the JIT stack.  */
static int jit_stack_size;
# endif

/* Match the already-compiled PCRE pattern against the data in SUBJECT,
   of size SEARCH_BYTES and starting with offset SEARCH_OFFSET, with
   options OPTIONS, and storing resulting matches into SUB.  Return
   the (nonnegative) match location or a (negative) error number.  */
static int
jit_exec (char const *subject, int search_bytes, int search_offset,
          int options, int *sub)
{
  while (true)
    {
      int e = pcre_exec (cre, extra, subject, search_bytes, search_offset,
                         options, sub, NSUB);

# if PCRE_STUDY_JIT_COMPILE
      if (e == PCRE_ERROR_JIT_STACKLIMIT
          && 0 < jit_stack_size && jit_stack_size <= INT_MAX / 2)
        {
          int old_size = jit_stack_size;
          int new_size = jit_stack_size = old_size * 2;
          static pcre_jit_stack *jit_stack;
          if (jit_stack)
            pcre_jit_stack_free (jit_stack);
          jit_stack = pcre_jit_stack_alloc (old_size, new_size);
          if (!jit_stack)
            error (EXIT_TROUBLE, 0,
                   _("failed to allocate memory for the PCRE JIT stack"));
          pcre_assign_jit_stack (extra, NULL, jit_stack);
          continue;
        }
# endif

      return e;
    }
}

#endif

#if HAVE_LIBPCRE
/* Table, indexed by ! (flag & PCRE_NOTBOL), of whether the empty
   string matches when that flag is used.  */
static int empty_match[2];

static bool multibyte_locale;
#endif

void
Pcompile (char const *pattern, size_t size)
{
#if !HAVE_LIBPCRE
  error (EXIT_TROUBLE, 0, "%s",
         _("support for the -P option is not compiled into "
           "this --disable-perl-regexp binary"));
#else
  int e;
  char const *ep;
  static char const wprefix[] = "(?<!\\w)(?:";
  static char const wsuffix[] = ")(?!\\w)";
  static char const xprefix[] = "^(?:";
  static char const xsuffix[] = ")$";
  int fix_len_max = MAX (sizeof wprefix - 1 + sizeof wsuffix - 1,
                         sizeof xprefix - 1 + sizeof xsuffix - 1);
  char *re = xnmalloc (4, size + (fix_len_max + 4 - 1) / 4);
  int flags = (PCRE_MULTILINE
               | (match_icase ? PCRE_CASELESS : 0));
  char const *patlim = pattern + size;
  char *n = re;
  char const *p;
  char const *pnul;

  if (1 < MB_CUR_MAX)
    {
      if (! using_utf8 ())
        error (EXIT_TROUBLE, 0,
               _("-P supports only unibyte and UTF-8 locales"));
      multibyte_locale = true;
      flags |= PCRE_UTF8;
    }

  /* FIXME: Remove these restrictions.  */
  if (memchr (pattern, '\n', size))
    error (EXIT_TROUBLE, 0, _("the -P option only supports a single pattern"));
  if (! eolbyte)
    {
      bool escaped = false;
      bool after_unescaped_left_bracket = false;
      for (p = pattern; *p; p++)
        if (escaped)
          escaped = after_unescaped_left_bracket = false;
        else
          {
            if (*p == '$' || (*p == '^' && !after_unescaped_left_bracket))
              error (EXIT_TROUBLE, 0,
                     _("unescaped ^ or $ not supported with -Pz"));
            escaped = *p == '\\';
            after_unescaped_left_bracket = *p == '[';
          }
    }

  *n = '\0';
  if (match_words)
    strcpy (n, wprefix);
  if (match_lines)
    strcpy (n, xprefix);
  n += strlen (n);

  /* The PCRE interface doesn't allow NUL bytes in the pattern, so
     replace each NUL byte in the pattern with the four characters
     "\000", removing a preceding backslash if there are an odd
     number of backslashes before the NUL.  */
  for (p = pattern; (pnul = memchr (p, '\0', patlim - p)); p = pnul + 1)
    {
      memcpy (n, p, pnul - p);
      n += pnul - p;
      for (p = pnul; pattern < p && p[-1] == '\\'; p--)
        continue;
      n -= (pnul - p) & 1;
      strcpy (n, "\\000");
      n += 4;
    }

  memcpy (n, p, patlim - p);
  n += patlim - p;
  *n = '\0';
  if (match_words)
    strcpy (n, wsuffix);
  if (match_lines)
    strcpy (n, xsuffix);

  cre = pcre_compile (re, flags, &ep, &e, pcre_maketables ());
  if (!cre)
    error (EXIT_TROUBLE, 0, "%s", ep);

  extra = pcre_study (cre, PCRE_STUDY_JIT_COMPILE, &ep);
  if (ep)
    error (EXIT_TROUBLE, 0, "%s", ep);

# if PCRE_STUDY_JIT_COMPILE
  if (pcre_fullinfo (cre, extra, PCRE_INFO_JIT, &e))
    error (EXIT_TROUBLE, 0, _("internal error (should never happen)"));

  /* The PCRE documentation says that a 32 KiB stack is the default.  */
  if (e)
    jit_stack_size = 32 << 10;
# endif

  free (re);

  int sub[NSUB];
  empty_match[false] = pcre_exec (cre, extra, "", 0, 0,
                                  PCRE_NOTBOL, sub, NSUB);
  empty_match[true] = pcre_exec (cre, extra, "", 0, 0, 0, sub, NSUB);
#endif /* HAVE_LIBPCRE */
}

size_t
Pexecute (char *buf, size_t size, size_t *match_size,
          char const *start_ptr)
{
#if !HAVE_LIBPCRE
  /* We can't get here, because Pcompile would have been called earlier.  */
  error (EXIT_TROUBLE, 0, _("internal error"));
  return -1;
#else
  int sub[NSUB];
  char const *p = start_ptr ? start_ptr : buf;
  bool bol = p[-1] == eolbyte;
  char const *line_start = buf;
  int e = PCRE_ERROR_NOMATCH;
  char const *line_end;

  /* The search address to pass to pcre_exec.  This is the start of
     the buffer, or just past the most-recently discovered encoding
     error.  */
  char const *subject = buf;

  /* If the input is unibyte or is free of encoding errors a multiline search is
     typically more efficient.  Otherwise, a single-line search is
     typically faster, so that pcre_exec doesn't waste time validating
     the entire input buffer.  */
  bool multiline = true;
  if (multibyte_locale)
    {
      multiline = ! buf_has_encoding_errors (buf, size - 1);
      buf[size - 1] = eolbyte;
    }

  for (; p < buf + size; p = line_start = line_end + 1)
    {
      bool too_big;

      if (multiline)
        {
          size_t pcre_size_max = MIN (INT_MAX, SIZE_MAX - 1);
          size_t scan_size = MIN (pcre_size_max + 1, buf + size - p);
          line_end = memrchr (p, eolbyte, scan_size);
          too_big = ! line_end;
        }
      else
        {
          line_end = memchr (p, eolbyte, buf + size - p);
          too_big = INT_MAX < line_end - p;
        }

      if (too_big)
        error (EXIT_TROUBLE, 0, _("exceeded PCRE's line length limit"));

      for (;;)
        {
          /* Skip past bytes that are easily determined to be encoding
             errors, treating them as data that cannot match.  This is
             faster than having pcre_exec check them.  */
          while (mbclen_cache[to_uchar (*p)] == (size_t) -1)
            {
              p++;
              subject = p;
              bol = false;
            }

          int search_offset = p - subject;

          /* Check for an empty match; this is faster than letting
             pcre_exec do it.  */
          if (p == line_end)
            {
              sub[0] = sub[1] = search_offset;
              e = empty_match[bol];
              break;
            }

          int options = 0;
          if (!bol)
            options |= PCRE_NOTBOL;
          if (multiline)
            options |= PCRE_NO_UTF8_CHECK;

          e = jit_exec (subject, line_end - subject, search_offset,
                        options, sub);
          if (e != PCRE_ERROR_BADUTF8)
            {
              if (0 < e && multiline && sub[1] - sub[0] != 0)
                {
                  char const *nl = memchr (subject + sub[0], eolbyte,
                                           sub[1] - sub[0]);
                  if (nl)
                    {
                      /* This match crosses a line boundary; reject it.  */
                      p = subject + sub[0];
                      line_end = nl;
                      continue;
                    }
                }
              break;
            }
          int valid_bytes = sub[0];

          if (search_offset <= valid_bytes)
            {
              /* Try to match the string before the encoding error.  */
              if (valid_bytes == 0)
                {
                  /* Handle the empty-match case specially, for speed.
                     This optimization is valid if VALID_BYTES is zero,
                     which means SEARCH_OFFSET is also zero.  */
                  sub[1] = 0;
                  e = empty_match[bol];
                }
              else
                e = jit_exec (subject, valid_bytes, search_offset,
                              options | PCRE_NO_UTF8_CHECK | PCRE_NOTEOL, sub);

              if (e != PCRE_ERROR_NOMATCH)
                break;

              /* Treat the encoding error as data that cannot match.  */
              p = subject + valid_bytes + 1;
              bol = false;
            }

          subject += valid_bytes + 1;
        }

      if (e != PCRE_ERROR_NOMATCH)
        break;
      bol = true;
    }

  if (e <= 0)
    {
      switch (e)
        {
        case PCRE_ERROR_NOMATCH:
          break;

        case PCRE_ERROR_NOMEMORY:
          error (EXIT_TROUBLE, 0, _("memory exhausted"));

# if PCRE_STUDY_JIT_COMPILE
        case PCRE_ERROR_JIT_STACKLIMIT:
          error (EXIT_TROUBLE, 0, _("exhausted PCRE JIT stack"));
# endif

        case PCRE_ERROR_MATCHLIMIT:
          error (EXIT_TROUBLE, 0, _("exceeded PCRE's backtracking limit"));

        default:
          /* For now, we lump all remaining PCRE failures into this basket.
             If anyone cares to provide sample grep usage that can trigger
             particular PCRE errors, we can add to the list (above) of more
             detailed diagnostics.  */
          error (EXIT_TROUBLE, 0, _("internal PCRE error: %d"), e);
        }

      return -1;
    }
  else
    {
      char const *matchbeg = subject + sub[0];
      char const *matchend = subject + sub[1];
      char const *beg;
      char const *end;
      if (start_ptr)
        {
          beg = matchbeg;
          end = matchend;
        }
      else if (multiline)
        {
          char const *prev_nl = memrchr (line_start - 1, eolbyte,
                                         matchbeg - (line_start - 1));
          char const *next_nl = memchr (matchend, eolbyte,
                                        line_end + 1 - matchend);
          beg = prev_nl + 1;
          end = next_nl + 1;
        }
      else
        {
          beg = line_start;
          end = line_end + 1;
        }
      *match_size = end - beg;
      return beg - buf;
    }
#endif
}
