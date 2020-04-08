#include <int80.h>
#include <system.h>
#include <timet.h>
#include <rtc.h>
#include <keyboard.h>
#include <screen.h>
#include <video_vm.h>
#include <sound.h>

enum sys_call_codes {SYS_SYSTEM = 0, SYS_TICKS_CODE, SYS_RTC_CODE, SYS_READ_CODE, SYS_SCREEN_CODE, SYS_VIDEO_CODE, SYS_SOUND_CODE};

uint64_t int80_handler(void * sysCallCode, void * arg1, void * arg2, void * arg3, void * arg4, void * arg5, void * arg6) {
    
    switch ((uint64_t) sysCallCode) {

        case SYS_SYSTEM: //syscall System
            return sys_system(arg1, arg2, arg3, arg4);

        case SYS_TICKS_CODE: //syscall Ticks
            return sys_timet(arg1, arg2, arg3);

        case SYS_RTC_CODE: //syscall RTC
            return sys_rtc(arg1);

        case SYS_READ_CODE: //syscall Read
            return sys_read(arg1);

        case SYS_SCREEN_CODE: //syscall Screen
            return sys_screen(arg1, arg2, arg3, arg4);

        case SYS_VIDEO_CODE: //syscall Video
            return sys_video(arg1, arg2, arg3, arg4, arg5);

        case SYS_SOUND_CODE: //syscall Sound
            return sys_sound(arg1, arg2, arg3);
        
    }
    return 0;

}