000000b2 <spill_short>:
  b2:	55                   	push   %ebp
  b3:	89 e5                	mov    %esp,%ebp
  b5:	81 ec 08 00 00 00    	sub    $0x8,%esp

                                a++, output %ecx
  bb:	8b 05 00 00 00 00    	mov    0x0,%eax
  c1:	89 c1                	mov    %eax,%ecx
  c3:	83 c0 01             	add    $0x1,%eax
  c6:	89 05 00 00 00 00    	mov    %eax,0x0

                                b++, output %edx
  cc:	8b 05 00 00 00 00    	mov    0x0,%eax
  d2:	89 c2                	mov    %eax,%edx
  d4:	83 c0 01             	add    $0x1,%eax
  d7:	89 05 00 00 00 00    	mov    %eax,0x0

                                c++, grab %ecx, output %ecx
  dd:	8b 05 00 00 00 00    	mov    0x0,%eax
  e3:	89 4d fc             	mov    %ecx,-0x4(%ebp)
  e6:	89 c1                	mov    %eax,%ecx
  e8:	83 c0 01             	add    $0x1,%eax
  eb:	89 05 00 00 00 00    	mov    %eax,0x0

                                d++, grab %edx, output %edx
  f1:	8b 05 00 00 00 00    	mov    0x0,%eax
  f7:	89 55 f8             	mov    %edx,-0x8(%ebp)
  fa:	89 c2                	mov    %eax,%edx
  fc:	83 c0 01             	add    $0x1,%eax
  ff:	89 05 00 00 00 00    	mov    %eax,0x0

                                c+d
 105:	01 d1                	add    %edx,%ecx

                                restore b
 107:	8b 45 f8             	mov    -0x8(%ebp),%eax

                                b+(c+d)
 10a:	01 c8                	add    %ecx,%eax

                                restore a
 10c:	8b 4d fc             	mov    -0x4(%ebp),%ecx

                                a+(b+(c+d))
 10f:	01 c1                	add    %eax,%ecx

 111:	89 0d 00 00 00 00    	mov    %ecx,0x0
 117:	c9                   	leave  
 118:	c3                   	ret    
