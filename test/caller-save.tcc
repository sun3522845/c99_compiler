
00000000 <function>:
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	81 ec 00 00 00 00    	sub    $0x0,%esp
   9:	b8 7b 00 00 00       	mov    $0x7b,%eax
   e:	e9 00 00 00 00       	jmp    13 <function+0x13>
  13:	c9                   	leave  
  14:	c3                   	ret    

00000015 <main>:
  15:	55                   	push   %ebp
  16:	89 e5                	mov    %esp,%ebp
  18:	81 ec 0c 00 00 00    	sub    $0xc,%esp
  1e:	8b 05 00 00 00 00    	mov    0x0,%eax
  24:	89 c1                	mov    %eax,%ecx
  26:	83 c0 01             	add    $0x1,%eax
  29:	89 05 00 00 00 00    	mov    %eax,0x0
  2f:	8b 05 00 00 00 00    	mov    0x0,%eax
  35:	89 c2                	mov    %eax,%edx
  37:	83 c0 01             	add    $0x1,%eax
  3a:	89 05 00 00 00 00    	mov    %eax,0x0
  40:	8b 05 00 00 00 00    	mov    0x0,%eax
  46:	89 4d fc             	mov    %ecx,-0x4(%ebp)
  49:	89 c1                	mov    %eax,%ecx
  4b:	83 c0 01             	add    $0x1,%eax
  4e:	89 05 00 00 00 00    	mov    %eax,0x0
  54:	89 55 f8             	mov    %edx,-0x8(%ebp)
  57:	89 4d f4             	mov    %ecx,-0xc(%ebp)
  5a:	e8 fc ff ff ff       	call   5b <main+0x46>
  5f:	8b 4d f4             	mov    -0xc(%ebp),%ecx
  62:	01 c1                	add    %eax,%ecx
  64:	8b 45 f8             	mov    -0x8(%ebp),%eax
  67:	01 c8                	add    %ecx,%eax
  69:	8b 4d fc             	mov    -0x4(%ebp),%ecx
  6c:	01 c1                	add    %eax,%ecx
  6e:	89 c8                	mov    %ecx,%eax
  70:	e9 00 00 00 00       	jmp    75 <main+0x60>
  75:	c9                   	leave  
  76:	c3                   	ret    
