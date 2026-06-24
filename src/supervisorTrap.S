.extern _ZN5RiscV20handleSupervisorTrapEv
.align 4
.global _ZN5RiscV14supervisorTrapEv
.type _ZN5RiscV14supervisorTrapEv, @function
_ZN5RiscV14supervisorTrapEv:
    addi sp, sp, -256

    .irp index, 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call _ZN5RiscV20handleSupervisorTrapEv

    .irp index, 1,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr

    addi sp, sp, 256
    sret

.global _ZN5RiscV10popSppSpieEv
.type _ZN5RiscV10popSppSpieEv, @function
_ZN5RiscV10popSppSpieEv:
    li t0, 0x100
    csrc sstatus, t0
    csrw sepc, ra
    sret