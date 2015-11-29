00000000 <signed_div>:
   9:	8b 45 f8             	mov    -0x8(%ebp),%eax
   c:	8b 4d f4             	mov    -0xc(%ebp),%ecx
   f:	99                   	cltd   
  10:	f7 f9                	idiv   %ecx
  12:	89 45 fc             	mov    %eax,-0x4(%ebp)

00000017 <unsigend_div>:
  20:	8b 45 f8             	mov    -0x8(%ebp),%eax
  23:	8b 4d f4             	mov    -0xc(%ebp),%ecx
  26:	31 d2                	xor    %edx,%edx
  28:	f7 f1                	div    %ecx
  2a:	89 45 fc             	mov    %eax,-0x4(%ebp)

0000002f <signed_mul>:
  38:	8b 45 f8             	mov    -0x8(%ebp),%eax
  3b:	8b 4d f4             	mov    -0xc(%ebp),%ecx
  3e:	0f af c1             	imul   %ecx,%eax
  41:	89 45 fc             	mov    %eax,-0x4(%ebp)

00000046 <unsigned_mul>:
  4f:	8b 45 f8             	mov    -0x8(%ebp),%eax
  52:	8b 4d f4             	mov    -0xc(%ebp),%ecx
  55:	0f af c1             	imul   %ecx,%eax
  58:	89 45 fc             	mov    %eax,-0x4(%ebp)
