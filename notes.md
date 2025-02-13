# Debugging

## break on return value
```
breakpoint set --name mi_malloc --command 'finish' --command 'e `$x0 == 0x20000010000ull ? __builtin_debugtrap(), 243 : 0`' --command 'continue'
```

# Interposing
Symbols are C form and do not have leading underscore prefix.

## Remaining work

## @exit handling
`pthread_exit` not currently hooked.

### @fork handling
```c
#pragma mark - atfork libSystem integration

void _pthread_atfork_prepare_handlers(void);
void _pthread_atfork_prepare(void);
void _pthread_atfork_parent(void);
void _pthread_atfork_parent_handlers(void);
void _pthread_atfork_child(void);
void _pthread_atfork_child_handlers(void);
void _pthread_fork_prepare(void);
void _pthread_fork_parent(void);
void _pthread_fork_child(void);
void _pthread_fork_child_postinit(void);
```

## C

### Interposed
```
malloc           -> mi_malloc
calloc           -> mi_calloc
free             -> mi_free
realloc          -> mi_realloc
reallocf         -> mi_reallocf
reallocarray     -> mi_reallocarray
reallocarrayf    -> mi_reallocarrayf
valloc           -> mi_valloc
vfree            -> mi_free
aligned_alloc    -> mi_aligned_alloc
posix_memalign   -> mi_posix_memalign
malloc_good_size -> mi_good_size
malloc_size      -> mi_good_size
```

### All `libsystem_malloc.dylib` exports
```
__mach_stack_logging_copy_uniquing_table
__mach_stack_logging_enumerate_records
__mach_stack_logging_frames_for_uniqued_stack
__mach_stack_logging_get_frames
__mach_stack_logging_get_frames_for_stackid
__mach_stack_logging_set_file_path
__mach_stack_logging_shared_memory_address
__mach_stack_logging_stackid_for_vm_region
__mach_stack_logging_start_reading
__mach_stack_logging_stop_reading
__mach_stack_logging_uniquing_table_copy_from_serialized
__mach_stack_logging_uniquing_table_read_stack
__mach_stack_logging_uniquing_table_release
__mach_stack_logging_uniquing_table_retain
__mach_stack_logging_uniquing_table_serialize
__mach_stack_logging_uniquing_table_sizeof
__malloc_init
__malloc_late_init
_malloc_fork_child
_malloc_fork_parent
_malloc_fork_prepare
_malloc_no_asl_log
_os_cpu_number_override
aligned_alloc
calloc
free
mag_set_thread_index
malloc
malloc_check_counter
malloc_check_each
malloc_check_start
malloc_claimed_address
malloc_create_legacy_default_zone
malloc_create_zone
malloc_debug
malloc_default_purgeable_zone
malloc_default_zone
malloc_destroy_zone
malloc_engaged_nano
malloc_enter_process_memory_limit_warn_mode
malloc_error
malloc_freezedry
malloc_get_all_zones
malloc_get_thread_options
malloc_get_zone_name
malloc_good_size
malloc_jumpstart
malloc_logger
malloc_make_nonpurgeable
malloc_make_purgeable
malloc_memory_event_handler
malloc_memorypressure_mask_default_4libdispatch
malloc_memorypressure_mask_msl_4libdispatch
malloc_num_zones
malloc_num_zones_allocated
malloc_printf
malloc_register_stack_logger
malloc_sanitizer_get_functions
malloc_sanitizer_is_enabled
malloc_sanitizer_set_functions
malloc_set_thread_options
malloc_set_zone_name
malloc_singlethreaded
malloc_size
malloc_type_aligned_alloc
malloc_type_calloc
malloc_type_free
malloc_type_malloc
malloc_type_posix_memalign
malloc_type_realloc
malloc_type_valloc
malloc_type_zone_calloc
malloc_type_zone_free
malloc_type_zone_malloc
malloc_type_zone_memalign
malloc_type_zone_realloc
malloc_type_zone_valloc
malloc_zero_on_free_disable
malloc_zone_batch_free
malloc_zone_batch_malloc
malloc_zone_calloc
malloc_zone_check
malloc_zone_claimed_address
malloc_zone_disable_discharge_checking
malloc_zone_discharge
malloc_zone_enable_discharge_checking
malloc_zone_enumerate_discharged_pointers
malloc_zone_free
malloc_zone_from_ptr
malloc_zone_log
malloc_zone_malloc
malloc_zone_memalign
malloc_zone_pressure_relief
malloc_zone_print
malloc_zone_print_ptr_info
malloc_zone_realloc
malloc_zone_register
malloc_zone_statistics
malloc_zone_unregister
malloc_zone_valloc
malloc_zones
mstats
pgm_diagnose_fault_from_crash_reporter
pgm_extract_report_from_corpse
posix_memalign
realloc
reallocarray$DARWIN_EXTSN
reallocarrayf$DARWIN_EXTSN
sanitizer_diagnose_fault_from_crash_reporter
scalable_zone_info
scalable_zone_statistics
set_malloc_singlethreaded
stack_logging_enable_logging
szone_check_counter
szone_check_modulo
szone_check_start
tiny_print_region_free_list
turn_off_stack_logging
turn_on_stack_logging
valloc
vfree
xzm_ptr_lookup_4test
zeroify_scalable_zone
```

## C++

### Interposed
```
_ZdlPv -> mi_free
_Znwm  -> mi_malloc
```

### Relevant `libc++abi.dylib` exports

```
_ZdaPv                              -> operator delete[](void*)
_ZdaPvRKSt9nothrow_t                -> operator delete[](void*, std::nothrow_t const&)
_ZdaPvSt11align_val_t               -> operator delete[](void*, std::align_val_t)
_ZdaPvSt11align_val_tRKSt9nothrow_t -> operator delete[](void*, std::align_val_t, std::nothrow_t const&)
_ZdaPvm                             -> operator delete[](void*, unsigned long)
_ZdaPvmSt11align_val_t              -> operator delete[](void*, unsigned long, std::align_val_t)
_ZdlPv                              -> operator delete(void*)
_ZdlPvRKSt9nothrow_t                -> operator delete(void*, std::nothrow_t const&)
_ZdlPvSt11align_val_t               -> operator delete(void*, std::align_val_t)
_ZdlPvSt11align_val_tRKSt9nothrow_t -> operator delete(void*, std::align_val_t, std::nothrow_t const&)
_ZdlPvm                             -> operator delete(void*, unsigned long)
_ZdlPvmSt11align_val_t              -> operator delete(void*, unsigned long, std::align_val_t)
_Znam                               -> operator new[](unsigned long)
_ZnamRKSt9nothrow_t                 -> operator new[](unsigned long, std::nothrow_t const&)
_ZnamSt11align_val_t                -> operator new[](unsigned long, std::align_val_t)
_ZnamSt11align_val_tRKSt9nothrow_t  -> operator new[](unsigned long, std::align_val_t, std::nothrow_t const&)
_ZnamSt19__type_descriptor_t        -> operator new[](unsigned long, std::__type_descriptor_t)
_Znwm                               -> operator new(unsigned long)
_ZnwmRKSt9nothrow_t                 -> operator new(unsigned long, std::nothrow_t const&)
_ZnwmSt11align_val_t                -> operator new(unsigned long, std::align_val_t)
_ZnwmSt11align_val_tRKSt9nothrow_t  -> operator new(unsigned long, std::align_val_t, std::nothrow_t const&)
_ZnwmSt19__type_descriptor_t        -> operator new(unsigned long, std::__type_descriptor_t)
```
