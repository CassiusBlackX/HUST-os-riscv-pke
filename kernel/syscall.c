/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "string.h"
#include "process.h"
#include "util/functions.h"
#include "elf.h"

#include "spike_interface/spike_utils.h"

extern elf_symbol elf_symbols[64];
extern char elf_symbol_names[64][32];
extern unsigned elf_symbol_num;

//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char* buf, size_t n) {
  sprint(buf);
  return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code) {
  sprint("User exit with code:%d.\n", code);
  // in lab1, PKE considers only one app (one process). 
  // therefore, shutdown the system when the app calls exit()
  shutdown(code);
}

/// search for func name
ssize_t find_func_name(uint64 fp) {
  fp = *(uint64*)(fp - 8);
  for (int i = 0; i < elf_symbol_num; i++) {
    if (fp >= elf_symbols[i].value && fp < elf_symbols[i].value + elf_symbols[i].size) {
      sprint("%s\n", elf_symbol_names[i]);
      if (strcmp(elf_symbol_names[i], "main") == 0) return 1;
      else return 0;
    }
  }
  return 0;
}

/// print backtrace
ssize_t sys_user_print_backtrace(uint64 n) {
  uint64 user_sp = current->trapframe->regs.sp + 16;
  uint64 current_p = user_sp;
  for (int i = 0; i < n;) {
    uint64 current_ra = *(uint64*)(current_p + 8);
    uint64 current_fp = *(uint64*)current_p;
    if (current_ra == 0) {
      // reaching the end of user stack
      sprint("reach the bottom of the stack\n");
      break;
    }
    if (current_fp == 0) {
      current_p += 16;
      continue;
    }
    if (elf_print_name(current_ra, &i, n) == EL_ERR) {
      panic("backtrace %d: ra=%p fp=%p\n", i, current_ra, current_fp);
    }
    current_p += 16;
  }
  return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7) {
  switch (a0) {
    case SYS_user_print:
      return sys_user_print((const char*)a1, a2);
    case SYS_user_exit:
      return sys_user_exit(a1);
    case SYS_user_print_backtrace:
      return sys_user_print_backtrace(a1);
    default:
      panic("Unknown syscall %ld \n", a0);
  }
}
