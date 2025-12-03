# Win32k.sys Function Pointer Hook

A kernel driver that hooks win32k.sys function pointers by pattern scanning and modifying function dispatch tables.

## How It Works

### The Problem
I wanted to hook a win32k.sys syscall, but traditional inline hooking is complex and fragile. Looking at the disassembly in IDA, I noticed many syscall handlers follow a pattern:

![Default Syscall Handler](images/default%20syscall.png)

```asm
mov     rax, cs:qword_XXXXXX    ; Load function pointer
test    rax, rax                 ; Check if NULL
jz      short loc_exit           ; Skip if not implemented
call    rax                      ; Call the function
```

Instead of inline hooking the syscall itself, I realized I could just **replace the function pointer** in that qword. Much cleaner.

### The Journey

#### Step 1: Pattern Scanning
Win32k.sys is a session-space driver - it's not loaded in system process context. I had to:
1. Find a GUI process (explorer.exe) 
2. Attach to its context with `KeStackAttachProcess`
3. Pattern scan for the specific `mov rax, [rip+offset]` instruction

The pattern is the byte signature of that instruction sequence. I used IDA to extract the bytes and mask out the relative offsets.

#### Step 2: Extracting the Function Pointer
The instruction `48 8B 05 XX XX XX XX` is RIP-relative addressing. To get the actual qword address:
```cpp
INT32 offset = *(INT32*)(patternAddress + 3);  // Extract offset
PVOID* qwordPtr = (PVOID*)(patternAddress + 7 + offset);  // RIP + offset + instruction length
```

#### Step 3: The Hook
Initially, many functions had NULL pointers (unimplemented features). I switched to `NtMITAccessibilityTimerNotification` which had a real function pointer.

Hooking is simple:
1. Disable write protection (CR0 WP bit)
2. Replace the qword with my handler address
3. Re-enable write protection

![Driver Load Result](images/driver%20load%20result.png)

#### Step 4: Triggering the Hook
Created a user-mode app that:
1. Loads win32u.dll (syscall stubs)
2. Resolves `NtMITAccessibilityTimerNotification`
3. Calls it directly with test input

When the syscall enters the kernel and executes that `call rax`, it jumps to my hook handler instead.

![Hook Call Result](images/hook%20call%20result.png)

![Usermode Result](images/usermode%20result.png)

### Key Insights

**Session Space**: Win32k.sys exists per-session, not globally. Had to attach to the target process's context both for scanning AND for the hook to work in that process.

**NULL Pointers**: Many function pointers are NULL (optional/unimplemented features). The code checks before calling. I had to find an actually-implemented function.

**RIP-Relative Addressing**: x64 uses instruction-pointer-relative offsets. Can't just read the bytes directly - have to calculate the absolute address.

**Write Protection**: Kernel memory is write-protected. Must toggle CR0.WP bit to modify the qword.

## Project Structure

```
├── main.cpp                    # Driver entry, orchestrates everything
├── pattern_scan/
│   ├── pattern_scan.h
│   └── pattern_scan.cpp        # Scans .text section for byte patterns
├── kernel_utils/
│   ├── kernel_utils.h
│   └── kernel_utils.cpp        # Module enumeration, PE parsing
├── process_utils/
│   ├── process_utils.h
│   └── process_utils.cpp       # Find processes by name
└── hook_handler/
    ├── hook_handler.h
    └── hook_handler.cpp        # Hook installation/removal
```

Hooking function pointers is way cleaner than inline hooks when the target uses dispatch tables. Just find the pointer and swap it.