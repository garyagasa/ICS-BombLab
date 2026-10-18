
./bomb:     file format elf64-x86-64


Disassembly of section .init:

Disassembly of section .plt:

Disassembly of section .plt.sec:

Disassembly of section .text:

0000000000402cc0 <phase_00>:
  402cc0:	endbr64
  402cc4:	push   %rbp
  402cc5:	mov    %rsp,%rbp
  402cc8:	push   %r12
  402cca:	mov    %rdi,%r12
  402ccd:	push   %rbx
  402cce:	mov    %rsi,%rbx
  402cd1:	mov    $0x2,%esi
  402cd6:	sub    $0x10,%rsp
  402cda:	call   402be0 <valid_u32_fields>
  402cdf:	test   %al,%al
  402ce1:	jne    402cf0 <phase_00+0x30>
  402ce3:	add    $0x10,%rsp
  402ce7:	xor    %eax,%eax
  402ce9:	pop    %rbx
  402cea:	pop    %r12
  402cec:	pop    %rbp
  402ced:	ret
  402cee:	xchg   %ax,%ax
  402cf0:	xor    %eax,%eax
  402cf2:	lea    -0x14(%rbp),%rcx
  402cf6:	lea    -0x18(%rbp),%rdx
  402cfa:	mov    $0x40317f,%esi
  402cff:	mov    %r12,%rdi
  402d02:	call   4011e0 <__isoc99_sscanf@plt>
  402d07:	cmp    $0x2,%eax
  402d0a:	jne    402ce3 <phase_00+0x23>
  402d0c:	shr    $0x5,%rbx
  402d10:	movzbl %bl,%edi
  402d13:	xor    $0x2a,%edi
  402d16:	cmp    %edi,-0x18(%rbp)
  402d19:	jne    402ce3 <phase_00+0x23>
  402d1b:	call   402cb0 <tutorial_mix>
  402d20:	cmp    %eax,-0x14(%rbp)
  402d23:	sete   %al
  402d26:	add    $0x10,%rsp
  402d2a:	pop    %rbx
  402d2b:	pop    %r12
  402d2d:	pop    %rbp
  402d2e:	ret

Disassembly of section .fini:
